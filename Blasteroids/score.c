//
//  score.c
//  Blasteroids
//
//  Created by linxucc on 2020/11/21.
//  Copyright © 2020 lin. All rights reserved.
//


#include "score.h"

int score_init(Score* s)
{
    s->score = SCORE_STEP;  // initialy, you have 100, it's better looking than 0.
    s->x = SCORE_POSITION_X;
    s->y = SCORE_POSITION_Y;
    s->size_factor = SCORE_SCALE;
    s->font = al_create_builtin_font();
    s->color = (al_map_rgb(238, 130, 177));
    return 0;
}

int score_draw(Score* s)
{
    // draw code goes here.
    // set transform, where to put the score
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    // which comes first which comes late matters.
    al_scale_transform(&transform, s->size_factor, s->size_factor);   // scale-x, scale-y
    al_translate_transform(&transform, s->x, s->y);   // move to position-x,y
    al_use_transform(&transform);
    al_draw_textf(s->font, s->color, s->x, s->y, 0, "%d", s->score);
    return 0;
}

/*
 Change the current score
 s_dalta could be - or +, it should be passed clearly by caller.
 return is the newly updated score value, if it's <= 0 after this change, 0 will be returned, otherwise, the updated score will be returned.
 */
int score_change(Score* s, int s_delta)
{
    return s->score + s_delta > 0 ? s->score + s_delta : 0;
}

int score_destroy(Score* s)
{
    //free(s->font);    // don't know why but this will be freed before this...
    free(s);
    return 0;
}
