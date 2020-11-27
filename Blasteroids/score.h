//
//  score.h
//  Blasteroids
//
//  Created by linxucc on 2020/11/21.
//  Copyright © 2020 lin. All rights reserved.
//

#ifndef score_h
#define score_h

#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include "common.h"


typedef struct {
    int score;
    ALLEGRO_COLOR color;
    ALLEGRO_FONT* font;
    float x,y;
    float size_factor;
} Score;

int score_init(Score* s);

int score_draw(Score* s);

int score_change(Score* s, int s_delta);

int score_destroy(Score* s);


#endif /* score_h */

