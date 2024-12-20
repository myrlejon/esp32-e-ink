// define number structs and positions
#include "esp_log.h"
#include "def.h"

uint8_t default_x_pos = 20;
uint8_t default_y_pos = 10;

Number seven_segments = {
    .segments = {
        .a = {.x_pos = 80, .y_pos = 35, .width = 5, .height = 30 },
        .b = {.x_pos = 55, .y_pos = 65, .width = 25, .height = 5 },
        .c = {.x_pos = 25, .y_pos = 65, .width = 25, .height = 5 },
        .d = {.x_pos = 20, .y_pos = 35, .width = 5, .height = 30 },
        .e = {.x_pos = 25, .y_pos = 30,  .width = 25, .height = 5 },
        .f = {.x_pos = 55, .y_pos = 30,  .width = 25, .height = 5 },
        .g = {.x_pos = 50, .y_pos = 35, .width = 5, .height = 30 },
    } 
};

uint8_t active_segments[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
};

void draw_number(int number, int position) {

    int y_offset = 0;

    switch (position)
    {
        // do nothing, default value
        case 1:
            break;
        case 2:
            y_offset = 45;
            break;
        case 3:
            y_offset = 100;
            break;
        case 4: 
            y_offset = 145;
            break;
        default:
            break;
    }

    Segment *segment = &seven_segments.segments.a; // this makes it possible to loop through a-g
    for (int i = 0; i < sizeof(active_segments[0]); i++)
    {
        if (active_segments[number][i] == 1){

            draw_rect(segment[i].x_pos, segment[i].y_pos + y_offset, segment[i].width, segment[i].height);
        }
    }
};