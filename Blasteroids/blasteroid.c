/**
 This is the main program of blasteroid game.
 The entry point and main event loop goes here.
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "common.h"
#include "blast.h"
#include "spaceship.h"
#include "lifecounter.h"
#include "score.h"
#include "asteroids.h"
#include "collision.h"
#include "level.h"

/*
 "Our Allegro Instance"
 
 This thing is to wrap everything used from Allegro into one single place.
 It lets you easily pass it around to other functions so that our main function could remain slim, without the risk of making all of Allegro related things into global variables.
 
 */
typedef struct {
    ALLEGRO_TIMER* timer;
    ALLEGRO_EVENT_QUEUE* queue;
    ALLEGRO_DISPLAY* disp;
    ALLEGRO_BITMAP* buffer;
    ALLEGRO_FONT* font;
    ALLEGRO_EVENT event;
    bool done;
    bool redraw;
    unsigned char key[ALLEGRO_KEY_MAX];
} OUR_AL_INSTANCE;

// allegro related initialization
int our_al_instance_init(OUR_AL_INSTANCE* al)
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    // Timer, ticks at 1/FPS second.
    al->timer = al_create_timer(1.0 / GAME_FPS);
    must_init(al->timer, "timer");
    // Event queue
    al->queue = al_create_event_queue();
    must_init(al->queue, "queue");
    // Display
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    al->disp = al_create_display(DISP_WIDTH, DISP_HEIGHT);
    must_init(al->disp, "display");
    // Draw Buffer for scale
    al->buffer = al_create_bitmap(BUFFER_WIDTH, BUFFER_HEIGHT);
    must_init(al->buffer, "bitmap buffer");
    // Font, use the built-in one.
    al->font = al_create_builtin_font();
    must_init(al->font, "font");
    // Primitives for draw everything.
    must_init(al_init_primitives_addon(), "primitives");
    // Wire up events.
    al_register_event_source(al->queue, al_get_keyboard_event_source());
    al_register_event_source(al->queue, al_get_display_event_source(al->disp));
    al_register_event_source(al->queue, al_get_timer_event_source(al->timer));
    
    // event doesn't need init.
    
    // Key status array, Allegro tutorial's keyboard handling.
    memset(al->key, 0, sizeof(al->key));
    
    // Exit flag
    al->done = false;
    // Draw flag
    al->redraw = true;
    
    return 0;
}

int our_al_instance_destroy(OUR_AL_INSTANCE* al)
{
    al_destroy_font(al->font);
    al_destroy_bitmap(al->buffer);
    al_destroy_display(al->disp);
    al_destroy_timer(al->timer);
    al_destroy_event_queue(al->queue);
    free(al);
    return 0;
}

/*
 Game Instance
 
 Game instance is to wrap everything we have around, as a game instance, so that we could pass it around, move game related logic out from main.
 It also give you a clear mind what's in our game.
 
 */

typedef struct {
    Spaceship* ship;    // the ship
    LifeCounter* life_counter;
    Score* score;
    BlastCluster* all_blasts;   // all the blasts
    Level* level;       // level 1, 2, 3...
    AsteroidCluster* all_asteroids;    // all the asteroids.
} GAME_INSTANCE;

int game_instance_init(GAME_INSTANCE* game)
{
    game->ship = malloc(sizeof(Spaceship));
    spaceship_init(game->ship, BUFFER_WIDTH/2.0, BUFFER_HEIGHT/2.0);  // space ship takes 2 float position to init, we put it on the center of the screen.
    
    game->life_counter = malloc(sizeof(LifeCounter));
    lifecounter_init(game->life_counter);
    
    game->score = malloc(sizeof(Score));
    // init score, params: &s, position_x, position_y, size_scale_factor
    score_init(game->score);
    
    // blasts
    game->all_blasts = malloc(sizeof(BlastCluster));
    blastcluster_init(game->all_blasts);
    
    // level
    game->level = malloc(sizeof(Level));
    level_first_init(game->level);    // level 1 as started.
    
    // Asteroids
    game->all_asteroids = malloc(sizeof(AsteroidCluster));
    asteroid_cluster_init(game->all_asteroids);
    //it's a special case, when the game just launch, we have to init level 1's asteroids.
    asteroid_cluster_add_some_random_asteroids(game->all_asteroids, game->level->asteroid_total);
    
    return 0;
}

int game_instance_destroy(GAME_INSTANCE* game)
{
    spaceship_destroy(game->ship);
    lifecounter_destroy(game->life_counter);
    score_destroy(game->score);
    blastcluster_destroy(game->all_blasts);
    asteroid_cluster_destroy(game->all_asteroids);
    level_destroy(game->level);
    free(game);
    return 0;
}

/* Process keyboard event so that our spaceship could move properly */
static void process_keyboard_events_the_right_way(OUR_AL_INSTANCE **al, GAME_INSTANCE *game) {
    // UP, DOWN, LEFT, RIGHT, SPACE, for spaceship action.
    if (game->level->game_status < GAME_OVER) {
        if((*al)->key[ALLEGRO_KEY_UP]) {
            spaceship_accelerate(game->ship);
        }
        if((*al)->key[ALLEGRO_KEY_DOWN]) {
            spaceship_decelerate(game->ship);
        }
        if((*al)->key[ALLEGRO_KEY_LEFT]) {
            spaceship_turn_left(game->ship);
        }
        if((*al)->key[ALLEGRO_KEY_RIGHT]) {
            spaceship_turn_right(game->ship);
        }
        if((*al)->key[ALLEGRO_KEY_SPACE]) {
            // press space, add a blast
            blastcluster_add_blast(game->all_blasts, game->ship);
            // -- not good -- only respond to fire after level start
//            if (game->level->game_status > LEVEL_START) {
//                blastcluster_add_blast(game->bc, game->ship);
//            }
        }
    }
    
    // ESC to quit game.
    if((*al)->key[ALLEGRO_KEY_ESCAPE])
        (*al)->done = true;
    
    // Refresh keyboard stuff "the right way", it's from allegro's tutorial.
    for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
        (*al)->key[i] &= KEY_SEEN;
}

static void game_move_to_next_level(GAME_INSTANCE *game) {
    level_go_next(game->level);
    // Re-prepare the asteroids of next level.
    asteroid_cluster_add_some_random_asteroids(game->all_asteroids, game->level->asteroid_total);
    // No need to center the ship, it feels bad.
    //        spaceship_reset_to_center((*game)->ship);
    // Life ++ ???
    lifecounter_add_one_life(game->life_counter);
}

static void run_main_game_logic(GAME_INSTANCE *game) {
    if (ship_crash_detection(game->all_asteroids, game->ship))
    {
        if (life_after_die_once(game->life_counter)==0) {
            // if life == 0, game over
            game->level->game_status = GAME_OVER;
        }
        // this will set the invicible time.
        spaceship_just_hit(game->ship);
    }
    
    // Run through all the asteroids and blasts, see if any got hit, the result score increment will be returned by asteroid_hit_detection().
    game->score->score += asteroid_hit_detection(game->all_asteroids, game->all_blasts);
}

static bool check_if_wins(GAME_INSTANCE *game) {
    if (!(game->all_asteroids->first_node)) {
        game->level->game_status = LEVEL_WIN;
        return true;
    }
    else return false;
}

/* Run game logic to determined what's happened */
static void run_game_logic(GAME_INSTANCE *game) {
    /*
     By default, the main game logic (detects if spaceship got hit, blast hit asteroids...) should always run.
     Unless it's GAME_OVER which should do nothing, or NEXT_LEVEL which is a intermediate state to change game instance to next level. (prepare new asteroids, etc...)
     
     When and only when IN_GAME_PLAY, should always check if now level wins.
     In other states, which is mostly for overlay text to display properly for a specific seconds, just count down the countdown ticker.
     
     */
    switch (game->level->game_status) {
        case GAME_OVER: break;
        case NEXT_LEVEL:
            // Move to next level.
            game_move_to_next_level(game);
            break;
            
        case IN_GAME_PLAY:
            // If IN_GAME_PLAY check if win, then tick (nothing will happen), then hit detection.
            // After run through all the asteroids, and no asteroid is left, you win this level, tell LEVEL you are win. (move to next level thigns happens there)
            check_if_wins(game); // fall through
        case NEW_LEVEL_NUMBER:
        case LEVEL_START:
        case LEVEL_WIN:
            // NEW_LEVEL_NUMBER, LEVEL_START, LEVEL_WIN: tick(count down) then check hit.
            level_tick(game->level);
            // fall through
        default:
            // unless GAME_OVER or switch to NEXT_LEVEL, game logic should always run.
            // Spaceship crash detection, crash has a high priority than other collisions.
            run_main_game_logic(game);
            break;
    }
}

/* Update all moving elements on the screen */
static void update_and_move(GAME_INSTANCE *game) {
    // -- Not good -- All moving elements only get moved when IN_GAME_PLAY, otherwise it should be in a pause.
    if (1) {
        blastcluster_update(game->all_blasts);  // blasters move
        asteroid_cluster_update(game->all_asteroids);  // asteroids move
        spaceship_update(game->ship);   // spaceship move
    }
}

static void draw_everything_to_buffer(OUR_AL_INSTANCE *al, GAME_INSTANCE *game) {
    // set draw target to our buffer
    al_set_target_bitmap(al->buffer);
    // clear the screen to black.
    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    // always draw blasts, asteroids, life counter and score.
    blastcluster_draw(game->all_blasts);
    asteroid_cluster_draw(game->all_asteroids);  // draw asteroids.
    lifecounter_draw(game->life_counter);
    score_draw(game->score);
    
    // Draw the damm ship if it's not game over.
    if (game->level->game_status < GAME_OVER) {
        spaceship_draw(game->ship);
    }
    
    // draw overlay text (level X, win, game over...) if status indicates should draw.
    switch (game->level->game_status) {
        case IN_GAME_PLAY:
            break;
        case GAME_OVER:
            draw_text_center_overlay(al->font, BUFFER_WIDTH/2.0, BUFFER_HEIGHT/2.0-10, al_map_rgb(192, 57, 50), al_premul_rgba(255, 255, 255, 0), 10, "GAME OVER");
            break;
        case NEW_LEVEL_NUMBER:
            draw_level_center_overlay(al->font, BUFFER_WIDTH/2.0, BUFFER_HEIGHT/2.0-10, al_map_rgb(192, 57, 50), al_premul_rgba(255, 255, 255, 0), 10, game->level->level_number);
            break;
        case LEVEL_START:
            draw_text_center_overlay(al->font, BUFFER_WIDTH/2.0, BUFFER_HEIGHT/2.0-10, al_map_rgb(192, 57, 50), al_premul_rgba(255, 255, 255, 0), 10, "START");
            break;
        case LEVEL_WIN:
            draw_text_center_overlay(al->font, BUFFER_WIDTH/2.0, BUFFER_HEIGHT/2.0-10, al_map_rgb(192, 57, 50), al_premul_rgba(255, 255, 255, 0), 10, "YOU WIN!");
            break;
            
        default:
            break;
    }
     
    al_set_target_backbuffer(al->disp); // Set draw target back to screen.
}

static void draw_buffer_to_screen(OUR_AL_INSTANCE *al) {
    al_set_target_backbuffer(al->disp);
    al_draw_scaled_bitmap(al->buffer, 0, 0, BUFFER_WIDTH, BUFFER_HEIGHT, 0, 0, DISP_WIDTH, DISP_HEIGHT, 0);     // Scale happens here,
    al_flip_display();
}

int main(int argc, char **argv)
{
    // Init allegro
    OUR_AL_INSTANCE* al = malloc(sizeof(OUR_AL_INSTANCE));
    our_al_instance_init(al);
    
    // Init our game "Instance".
    GAME_INSTANCE* game = malloc(sizeof(GAME_INSTANCE));
    game_instance_init(game);
    
    // Main loop begin, start the timer.
    al_start_timer(al->timer);
    while(1)
    {
        al_wait_for_event(al->queue, &(al->event));

        /* Process the event */
        switch((al->event).type)
        {
            case ALLEGRO_EVENT_TIMER:
                /* Timer tick, process the processed keyboard events, then re-draw */
                // Translated keyboard events to move spaceship and open fire.
                process_keyboard_events_the_right_way(&al, game);
                al->redraw = true;  // re-draw each timer event.
                
                break;
            
            case ALLEGRO_EVENT_KEY_DOWN:
                /* Raw keyboard events input */
                al->key[al->event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                // key down, set it to 00000011, both seen, and released.
                // until next timer, assume it's still not released. Under this situation, if KEY_UP happens, 00000011 & 00000010 = 00000010,
                // so even if it's a very quick press-release, within the gap between 1/30 seconds, next draw will still recognize this key-press. Or a very quick key-press will be omitted if it happenly happened between two draw.
                // the release bit is purely for this situation, this bit is only reset to 0 in the draw event, so each key-press is garenteed to be drawn at least once.
                break;
                
            case ALLEGRO_EVENT_KEY_UP:
                /* Raw keyboard events input */
                al->key[al->event.keyboard.keycode] &= KEY_RELEASED;
                // key up situation, if the key-press has been drawn at least once, it's code will be 00000001, so & with 00000010 will end up be 00000000, which equals to false.
                // if key-press has not been drawn yet, it will be 00000011 & 00000010 = 00000010, which will still equals to true, so this key-press will get a draw
                // this whole system could be replaced by a series of if-else and 2 bool, but bitwise operation is much more concise and faster.
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                /* Event of user closed the game window */
                al->done = true;
                break;
        }

        /* ESC pressed, quit game */
        if(al->done)
            break;

        /* Run game logic and draw when timer ticks */
        if(al->redraw && al_is_event_queue_empty(al->queue))
        {
            /* Each frame/timer-tick, do these 4 things: */
            run_game_logic(game);  // 1. run game logic, collision detection, win/lose are judged here.
            update_and_move(game);  // 2. update every moving things, asteroids, blast, spaceship, all what they should be to next moment.
            draw_everything_to_buffer(al, game);    // 3. draw everything on the buffer, buffer is used to scale later, for better support of high resolution.
            draw_buffer_to_screen(al);  // 4. draw buffer to actual screen, with proper scale, everything just drawn shows on screen.
            al->redraw = false;     // do not re-draw untill next timer tick.
        }
    }

    our_al_instance_destroy(al);
    game_instance_destroy(game);

    return 0;
}
