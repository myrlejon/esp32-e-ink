#include <stdint.h>

// active - 1 for being shown, 0 for hidden
typedef struct {
    uint8_t active;
    uint8_t x_pos;
    uint8_t y_pos;
    uint8_t width;
    uint8_t height;
} Segment;

typedef struct {
    Segment a, b, c, d, e, f, g;
} Segments;

typedef struct {
    Segments segments;
} Number;

Number zero = {
    .segments = {
        .a = { .active = 1, .x_pos = 10, .y_pos = 20, .width = 30, .height = 5 },
        .b = { .active = 1, .x_pos = 40, .y_pos = 25, .width = 5, .height = 25 },
        .c = { .active = 1, .x_pos = 40, .y_pos = 60, .width = 5, .height = 25 },
        .d = { .active = 1, .x_pos = 10, .y_pos = 85, .width = 30, .height = 5 },
        .e = { .active = 1, .x_pos = 5,  .y_pos = 60, .width = 5, .height = 25 },
        .f = { .active = 1, .x_pos = 5,  .y_pos = 25, .width = 5, .height = 25 },
        .g = { .active = 0, .x_pos = 10, .y_pos = 50, .width = 30, .height = 5 },
    }
};

struct Number_1 {
    
};

struct Number_2 {

};

struct Number_3 {

};

struct Number_4 {

};
