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
    uint8_t x_pos_offset; // to adjust position for number 1-4
    // uint8_t y_pos_offset;
} Number;