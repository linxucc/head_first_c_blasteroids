//
//  common.c
//  Blasteroids
//
//  Created by linxucc on 2020/11/25.
//  Copyright © 2020 lin. All rights reserved.
//

#include "common.h"

int random_between(int lo, int hi)
{
    return lo + (rand() % (hi - lo));
}

float random_between_f(float lo, float hi)
{
    return lo + ((float)rand() / (float)RAND_MAX) * (hi - lo);
}

int go_error(char* err_msg)
{
    printf("Error: %s", err_msg);
    exit(1);
}

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

void draw_text_center_overlay(ALLEGRO_FONT* font, float pos_x, float pos_y, ALLEGRO_COLOR font_color, ALLEGRO_COLOR background_color, float scale, char* text_to_draw)
{
    
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    // which comes first which comes late matters.
    al_scale_transform(&transform, scale, scale);   // scale-x, scale-y
    al_translate_transform(&transform, 0, 0);   // move to position-x,y
    al_use_transform(&transform);
    al_draw_filled_rectangle(0,  0,  BUFFER_WIDTH,  BUFFER_HEIGHT, background_color);
    al_translate_transform(&transform, pos_x, pos_y);
    al_use_transform(&transform);
    al_draw_text(font, font_color, 0, 0, ALLEGRO_ALIGN_CENTRE, text_to_draw);
}

void draw_level_center_overlay(ALLEGRO_FONT* font, float pos_x, float pos_y, ALLEGRO_COLOR font_color, ALLEGRO_COLOR background_color, float scale, int level)
{
    
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    // which comes first which comes late matters.
    al_scale_transform(&transform, scale, scale);   // scale-x, scale-y
    al_translate_transform(&transform, 0, 0);   // move to position-x,y
    al_use_transform(&transform);
    al_draw_filled_rectangle(0,  0,  BUFFER_WIDTH,  BUFFER_HEIGHT, background_color);
    al_translate_transform(&transform, pos_x, pos_y);
    al_use_transform(&transform);
    al_draw_textf(font, font_color, 0, 0, ALLEGRO_ALIGN_CENTRE, "LEVEL %d", level);
}
