#include "quantum.h"
#include "rgb_matrix.h"
#include "timer.h"
#include "transactions.h"
#include "state.h"

uint16_t last_frame = 0;

uint16_t rbuff;
uint16_t gbuff;
uint16_t bbuff;

uint8_t x = 0;

/*!md {{{

# starplan!


# complex randomly scalable values

### rscalable_t:
 * **base:** _int_ - the starting initial value
 * **deviation:** _int_ - % val may deviate by
 * **direction:** _enum_ -
     * ASCEND
     * DESCEND
     * CENTERED
 * **halfdev:** _int_ - half the deviation, this is explicitly held here to make CENTERED direction scalables marginally more performant.

### range_rscalable_t:
 * **min:** _rscalable_t_
 * **max:** _rscalable_t_

### fXY:
_a simple pair of floats_
 * **x:** _float_
 * **y:** _float_


# the properties used to define the animation

## Ok so the plan here is that for each "cycle" I decide cycle duration and "intensity", this intensity is randomly distributed across x "stars"
### starsettings_t
 * **starcount:    .** *range_rscalable_t* - dictates count of strars in a cycle

 * **duration:     .** *range_rscalable_t* - decided each cycle, this will be lightly randomized likely in a range that bumps as WPM rises.

 * **directionality:** *int* - 1/2/3/4 to progress stars in < / V / ^ / > directions


 *
 #### star properties
 * **intensity:  .** range_*rscalable_t* - total intensity factor is decided each cycle and directly correlates value, but inversely correlates saturation, both of these correlations are statically bounded.
    * _function:_ this is applied at a given point by multiplying all RGB channels by *intensity / 2* and then inverting the scaling by _inv_intensity = 1 / intensity_,
    multiply _intensity_ and _intensity_inv_ correspondingly by the greatest and smallest channels of the value-adjusted color.

 * **volatility: .** *rscalable_t* - factor that determines "sparking" emission effects from a star. This is distributed across all stars in a cycle, the amount
    of volatiles at any given moment is determined by volatility, with more volatiles their strength decreases and speed increases towards the end of the stream.
 * **volatiles:  .**  *fXY[volatility]* - holds a list of float pairs representing current volatile's strength, and when it will end.
    * _function:_ the volatile's trajectory is characterized by its index in **volatiles** (scaled) plus the start of the cycle's timestamp modulated over 4.0,
    a volatile's position in its trajectory is determiend by _pos = FXy[volatile].Y - now() / FXy[volatile].Y_ so relative to the x,y of a star's center
    it x-pos for the horizontal position of the volatile and y - √(-pos * trajectory) for the vertical.

 * **size:       .** *rscalable_t* - factor that determines how large a star is
    * _function:_ a pixel at a x,y has its initial strength determined by *(starpos_x - x)² + (starpos_y - y)² / size²*, this function is
    hereforth refered to as the captureFunc(size). If the result is `< 1` the pixel shows at the star's full strength.
 * **luminance:  .** *rscalable_t* - factor that determines how far ambient light emission from a star reaches
    * _function:_ if the initial captureFunc() returns a value `> 1` it's ran again with a circle of `luminance` size. Except this time the strength
       of a pixel is determined by a square root function using ** as domain (only if `> 1`). This function is one that starts at

 * **hue:        .** *rscalable_t* - hue each star will use in a cycle

## these are properties of the individual stars that are generated each cycle
### starprops_t
 * individual stars:
 * **pos:    .** _int_ - the posititon of this star's origin
 * **duration:** _int_ - the amount of time it takes for this star to traverse the grid
 * **spawned:.** *int* - the timestamp for when this star was initially spawned
 * **...:    .** _int_ - star contains a prop for each of the star properties in starsettings, they're generated from the random scalable type.


 *scales a random integer to rscalable_t specifications and returns it*

## rand_scale_range(range_rscalable_t) -> int:
 *scales a random integer in the range defined by the randomly scaled min/max rscalables*

## mutate_rscalable(*rscalable_t, rscalable_t) -> int:
 *arg0 is the mutatee rscalable, arg1 is the rscalable to use to mutate it.*

## mutate_range_rscalable(*range_rscalable_t, range_rscalable_t) -> int:
 *arg0 is the mutatee range_rscalable, arg1 is the range_rscalable to use to mutate it.*

## mutate_rscalable_by_range(*rscalable_t, range_rscalable_t) -> int:
 *arg0 is the mutatee rscalable, arg1 is the range_rscalable to use to mutate it.*

}}}*/

pixels_t render_pixels;

// int rand_scaled(rscalable_t rscalable) {
//     // generate the deviation
//     int dev = rand() % rscalable.deviation;
//
//     // TODO
//     return 0;
// }

#define COLR(hexcode) \
    { (0xff0000 & hexcode) / 0x10000, (0x00ff00 & hexcode) / 0x100, 0x0000ff & hexcode }

#define SET_COLR(target, hexcode)              \
    target.r = (0xff0000 & hexcode) / 0x10000; \
    target.g = (0x00ff00 & hexcode) / 0x100;   \
    target.b = 0x0000ff & hexcode;

color_t mult_color(color_t color, float mult) {
    rbuff = color.r * mult;
    gbuff = color.g * mult;
    bbuff = color.b * mult;
    color.r *= rbuff > 0xff ? 0xff : rbuff;
    color.g *= gbuff > 0xff ? 0xff : gbuff;
    color.b *= bbuff > 0xff ? 0xff : bbuff;
    return color;
}

// alternative to mult_color to directly operate on a color, for very frequent color multiplication where copying colors isn't needed.
#define MULT_COLOR(color, mult)                 \
    {                                           \
        rbuff = color.r * mult;                 \
        gbuff = color.g * mult;                 \
        bbuff = color.b * mult;                 \
        color.r *= rbuff > 0xff ? 0xff : rbuff; \
        color.g *= gbuff > 0xff ? 0xff : gbuff; \
        color.b *= bbuff > 0xff ? 0xff : bbuff; \
    }

#define OR_COLORS(color, or_color) \
    {                              \
        color.r |= or_color.r;     \
        color.g |= or_color.g;     \
        color.b |= or_color.b;     \
    }

// draw a pixel at x,y with specified color, value multiplier, and position.
#define DRAW_PIXEL(grid, x, y, color, mult, pos) \
    { grid[(int)x][y] = position_adjust_color(mult_color(color, mult), pos); }

typedef union {
} colun_t;

// this defines the "extra" luminance the 2nd LED will have from the "spill-over" as with a position between idx 0 and 1
int next_lumboost;

// this might need to be changed so that we have a "backing" buffer of an extra 12 columns for each row
// at the moment the concept for this is that it takes the x,y a pixel belongs at and then aliases it across the two physical LEDs according to floating point position it's supposed to be @, so
// presently this is the last thing we call to draw our canvas since beyond this point doing anything but the aliasing stuff will probably fuck with the canvas
// color_t* position_adjust_color(float position, color_t* color) {
//     // intensity on x value corresponding floor of position
//     float intensity = (int)(position + 1) - position;
//     MULT_COLOR(color, intensity);
// }

// this takes a list of pixels at positions with a global offset and renders them to a physically
// representative gird of RGB vals. Returns a list of the xy it set on
void render_pixels_to_grid(pixels_t pixels, color_t* phys, int phys_len, float offset) {
    float x_base;
    float y_base;
    float x_nxt;
    float y_nxt;

    for (int i = 0; i < pixels.len; i++) {
        // calculate the % strength that aliases into x and/or y +1 and the % that displays at the base x or y
        float x = ((int)(pixels.pixels[i].pos.x + offset) % 12) + (pixels.pixels[i].pos.x - (int)pixels.pixels[i].pos.x);

        float y = pixels.pixels[i].pos.y;

        x_nxt  = x - (int)x;
        y_nxt  = y - (int)y;
        x_base = 1 - x_nxt;
        y_base = 1 - y_nxt;

        printf("%d calc'd float params: x: %f, y: \n%f, x_nxt: %f, y_nxt: %f, x_base: %f, y_base: %f\n|\n", i, x, y, x_nxt, y_nxt, x_base, y_base);

        // render base pixel, bitwise overlapping the grid,
        // then render onto the following adjacent pixels in (x), (y), and (x,y).
        color_t base_color = mult_color(mult_color(pixels.pixels[i].color, x_base), y_base);
        int     base_index = (int)x + (int)y * 12;
        printf("idx: %d, colr: %d.%d.%d   (base)\n", base_index, base_color.r, base_color.g, base_color.b);

        OR_COLORS(phys[base_index], base_color);

        if (x <= 12) {
            color_t nxt_x_color = mult_color(mult_color(pixels.pixels[i].color, x_nxt), y_base);
            int     index       = (1 + (int)x) + (int)y * 12;
            printf("idx: %d, colr: %d.%d.%d   (nxt x)", index, nxt_x_color.r, nxt_x_color.g, nxt_x_color.b);

            OR_COLORS(phys[index], nxt_x_color);
        }

        if (y <= 3) {
            color_t nxt_y_color = mult_color(mult_color(pixels.pixels[i].color, x_base), y_nxt);
            int     index       = (int)x + ((int)y + 1) * 12;
            printf("idx: %d, colr: %d.%d.%d   (nxt y)", index, nxt_y_color.r, nxt_y_color.g, nxt_y_color.b);

            OR_COLORS(phys[index], nxt_y_color);
        }

        if (x <= 12 && y <= 3) {
            color_t nxt_xy_color = mult_color(mult_color(pixels.pixels[i].color, x_nxt), y_nxt);
            int     index        = (int)x + 1 + ((int)y + 1) * 12;
            printf("idx: %d, colr: %d.%d.%d   (nxt xy)\n", index, nxt_xy_color.r, nxt_xy_color.g, nxt_xy_color.b);

            OR_COLORS(phys[index], nxt_xy_color);
        }
        printf("\n\n");
    }
}

// this takes a physical pixel grid and clears the indexes on it corresponding to xy coordinates in state.last_frame_enabled coord list
void clr_previous_frame(color_t frame[PHYS_PIXELS_COUNT]) {
    // for (int i = 0; i < state.last_frame_enabled.len; i++) {
    //     SET_COLR(last_frame[state.last_frame_enabled.xy[i].x + state.last_frame_enabled.xy[i].y * 12], 0x000000);
    // }
    for (int i = 0; i < PHYS_PIXELS_COUNT; i++) {
        SET_COLR(frame[i], 0x000000);
    }
}

// this takes a physical pixel grid and clears the indexes on it corresponding to xy coordinates in state.last_frame_enabled coord list
void clr_previous_frame_xy(color_t frame[12][4]) {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 4; j++) {
            SET_COLR(frame[i][j], 0x000000);
        }
    }
}

// void display_frame() {
//     for(int i=0; i < 4; i++) {
//         rgb_matrix_set_color((*state.phys_pixels), frame[x][y].r, frame[x][y].g, frame[x][y].b);
//     }
// }

#define SET_XY_COLOR(x, y, color) rgb_matrix_set_color(((*state.yx_led_map)[y][x]), color.r, color.g, color.b)

#define ASSIGN_XY_COLOR(x, y, color) rgb_matrix_set_color(((*state.yx_led_map)[y][x]), color.r, color.g, color.b)

#define RENDER_XY_COLOR_TO_GRID(x, y, color) state.phys_pixels[*state.yx_led_map)[y][x])] = color

// void render_grid(pixelsPhys_t pixels, uint8_t led_min, uint8_t led_max) {
//     for (int i = 0; i < pixels.len; i++) {
//         RGB_MATRIX_INDICATOR_SET_COLOR(i, state.rgbTicker, state.rgbTicker | 0b00001111, i * 9);
//     }
// }

// next up: we need to write a function that renders a circle to a x,y of a given size and draws bloom around it.

// int render_star(star_t star, color_t* grid[12][4]) {
//     uint16_t now = timer_read();
//     // probably do smth special if we're @ duration..
//     float x = ((now % star.duration) / (float)star.duration) * 12.0;
//     int   y = star.pos;
//
//     // calculate bloom
//     color_t white = COLR(0xffffff);
//     // star center
//     grid[(int)x][y] = position_adjust_color(mult_color(white, 0.9), x);
//     // star "head"
//     grid[x + 1][y] = mult_color(white, 0.5);
//     // star sidetrails
//     grid[x][y - 1] = mult_color(white, 0.3) color[x][y + 1] = mult_color(white, 0.3);
//     // star "head" sides
//     grid[x + 1][y - 1] = mult_color(white, 0.3) color[x + 1][y + 1] = mult_color(white, 0.3);
//     // star trail sides
//     grid[x - 1][y - 1] = mult_color(white, 0.2) color[x - 1][y + 1] = mult_color(white, 0.2) color[x - 2][y - 1] = mult_color(white, 0.09) color[x - 2][y + 1] = mult_color(white, 0.09);
//     // star trail
//     grid[x - 1][y] = mult_color(white, 0.3) color[x - 2][y] = mult_color(white, 0.2) color[x - 3][y] = mult_color(white, 0.1) color[x - 4][y] = mult_color(white, 0.05);
// }

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // if(timer_elapsed32(last_frame) > 500) {
    //     // do stuff
    //     x = rand()%9;
    // }

    // if (timer_elapsed(last_frame) > 10) {
    //     if (led_min == 0) {
    //         state.rgbTicker = (state.rgbTicker + 1) % 252;
    //
    //         if (state.rgbTicker == 1) {
    //             render_pixels.len    = 1;
    //             render_pixels.pixels = malloc(sizeof(pixel_t) * render_pixels.len);
    //
    //             color_t test;
    //             SET_COLR(test, 0xffff00);
    //             printf("coltest %d %d %d\n", test.r, test.g, test.b);
    //
    //             SET_COLR(render_pixels.pixels[0].color, 0xffff00);
    //             render_pixels.pixels[0].pos.x = 0.0;
    //             render_pixels.pixels[0].pos.y = 0.0;
    //         }
    //
    //         clr_previous_frame(state.phys_pixels);
    //         render_pixels_to_grid(render_pixels, state.phys_pixels, PHYS_PIXELS_COUNT, state.rgbTicker / 12.0);
    //     }

    if(timer_elapsed32(last_frame) < 500) {
        return true;
    }
    if (led_min == 0 && !state.isSister) {
        clr_previous_frame(state.phys_pixels);
        for (int i=0; i < 48; i++) {
            int i_scale = i + 1;
            color_t color;
            color.r       = 12.70 * abs(i_scale - 21);
            color.g       = i_scale;
            color.b       = 12.70 * (-22 + i_scale);

            color.r       = i < 20 ? color.r : 0;
            color.g       = i < 20 ? color.g * (i) : color.g * (41 - i);
            color.b       = i > 20 ? color.b : 0;

            state.phys_pixels[(*state.yx_led_map)[i/12][i%12]] = color;
        }
    }
    for (uint8_t i = led_min; i < led_max; i++) {
        // printf("min: %d   max: %d\n", led_min, led_max);
        // printf("RGB_MATRIX_INDICATOR_SET_COLOR(%d, %d, %d, %d)\n", i, state.phys_pixels[i].r, state.phys_pixels[i].g, state.phys_pixels[i].b);
        // RGB_MATRIX_INDICATOR_SET_COLOR(i, state.phys_pixels[i].r, state.phys_pixels[i].g, state.phys_pixels[i].b);
        // printf("would have set led %d (%d, %d) = %d to #%x.%x.%x ::: %d %d %d %d - %d %d %d %d - %d %d %d %d - %d %d %d %d :::::: %d\n", i, i%6, i/6, (*state.yx_led_map)[i/12][i%12], color.r, color.g, color.b,
        //         ((*state.yx_led_map)[0][0]), ((*state.yx_led_map)[0][1]), ((*state.yx_led_map)[0][2]), ((*state.yx_led_map)[0][3]),
        //         ((*state.yx_led_map)[1][0]), ((*state.yx_led_map)[1][1]), ((*state.yx_led_map)[1][2]), ((*state.yx_led_map)[1][3]),
        //         ((*state.yx_led_map)[2][0]), ((*state.yx_led_map)[2][1]), ((*state.yx_led_map)[2][2]), ((*state.yx_led_map)[2][3]),
        //         ((*state.yx_led_map)[3][0]), ((*state.yx_led_map)[3][1]), ((*state.yx_led_map)[3][2]), ((*state.yx_led_map)[3][3]),
        //         state.isSister
        //       );

        color_t color = state.phys_pixels[i];
        rgb_matrix_set_color(i, color.r, color.g, color.b);
    }


    // }
    // if (state.rgbTicker == 251) {
    //     state.y_state = (state.y_state + 1) % 4;
    // }
    last_frame = timer_read32();
    return false;
}

// void set_rgb_state(state_t* state) {
// if (timer_elapsed32(last_frame) > 500) {
//     last_frame = timer_read32();
// } else {
//     return;
// }
//
// ticker = (ticker+1) % 256;
//
// int i = ticker%12;
// printf("fugg %d %d %d %d\n", i, ticker, ~ticker, ticker & (uint8_t)(last_frame%256));
// rgb_matrix_set_color(i, 0xff, 0xff, 0xff);

// switch(state->pgm_pgmkey) {
//     case 1:
//         // rgblight_setrgb(117, 157, 10);
//         break;
//
//     case 2:
//         rgblight_setrgb(127, 127, 0);
//         break;
//
//     default:
//         uprintf("switching rgb for layer: %d\n", state->main_active_layer);
//         switch(state->main_active_layer) {
//             case 0:
//                 rgblight_setrgb(107, 107, 40);
//                 break;
//             case 7:
//                 rgblight_setrgb(200, 55, 00);
//                 break;
//             default:
//                 rgblight_setrgb(255, 255, 255);
//                 break;
//         }
//         break;
// }
// }

led_config_t g_led_config = {{{18, 17, 12, 11, 4, 3},
                              {19, 16, 13, 10, 5, 2},
                              {20, 15, 14, 9, 6, 1},
                              {NO_LED, NO_LED, NO_LED, 8, 7, 0},

                              {39, 38, 33, 32, 25, 24},
                              {40, 37, 34, 31, 26, 23},
                              {41, 36, 35, 30, 27, 22},
                              {NO_LED, NO_LED, NO_LED, 29, 28, 21}},
                             {{95, 63}, {85, 39}, {85, 21}, {85, 4}, {68, 2}, {68, 19}, {68, 37}, {80, 58}, {60, 55}, {50, 35}, {50, 13}, {50, 0}, {33, 3}, {33, 20}, {33, 37}, {16, 42}, {16, 24}, {16, 7}, {0, 7}, {0, 24}, {0, 41}, {129, 63}, {139, 39}, {139, 21}, {139, 4}, {156, 2}, {156, 19}, {156, 37}, {144, 58}, {164, 55}, {174, 35}, {174, 13}, {174, 0}, {191, 3}, {191, 20}, {191, 37}, {208, 42}, {208, 24}, {208, 7}, {224, 7}, {224, 24}, {224, 41}},
                             {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}};


// // commented to preserve fmt
// led_config_t g_led_config = {
//     {
//         {18, 17, 12, 11, 4, 3},
//         {19, 16, 13, 10, 5, 2},
//         {20, 15, 14, 9, 6, 1},
//         {NO_LED, NO_LED, NO_LED, 8, 7, 0},
//
//         {39, 38, 33, 32, 25, 24},
//         {40, 37, 34, 31, 26, 23},
//         {41, 36, 35, 30, 27, 22},
//         {NO_LED, NO_LED, NO_LED, 29, 28, 21}
//     },
//     {
//         {95, 63},  {85, 39},  {85, 21},  {85, 4},   {68, 2},  {68, 19},  {68, 37},
//         {80, 58},  {60, 55},  {50, 35},  {50, 13},  {50, 0},  {33, 3},   {33, 20},
//         {33, 37},  {16, 42},  {16, 24},  {16, 7},   {0, 7},   {0, 24},   {0, 41},
//         {129, 63}, {139, 39}, {139, 21}, {139, 4},  {156, 2}, {156, 19}, {156, 37},
//         {144, 58}, {164, 55}, {174, 35}, {174, 13}, {174, 0}, {191, 3},  {191, 20},
//         {191, 37}, {208, 42}, {208, 24}, {208, 7},  {224, 7}, {224, 24}, {224, 41}
//     },
//     {
//         4, 4, 4, 4, 4, 4, 4,
//         4, 4, 4, 4, 4, 4, 4,
//         4, 4, 4, 4, 4, 4, 4,
//         4, 4, 4, 4, 4, 4, 4,
//         4, 4, 4, 4, 4, 4, 4,
//         4, 4, 4, 4, 4, 4, 4,
//         4, 4, 4, 4, 4, 4, 4,
//         4, 4, 4, 4, 4
//     }
// };

// led_config_t g_led_config = { {
//     {  24,  23,  18,  17,  10,   9 },
//     {  25,  22,  19,  16,  11,   8 },
//     {  26,  21,  20,  15,  12,   7 },
//     { NO_LED, NO_LED, NO_LED,  14,  13,   6 },
//     {  51,  50,  45,  44,  37,  36 },
//     {  52,  49,  46,  43,  38,  35 },
//     {  53,  48,  47,  42,  39,  34 },
//     { NO_LED, NO_LED, NO_LED,  41,  40,  33 }
// }, {
//     {  85,  16 }, {  50,  13 }, {  16,  20 }, {  16,  38 }, {  50,  48 }, {  85,  52 }, {  95,  63 },
//     {  85,  39 }, {  85,  21 }, {  85,   4 }, {  68,   2 }, {  68,  19 }, {  68,  37 }, {  80,  58 },
//     {  60,  55 }, {  50,  35 }, {  50,  13 }, {  50,   0 }, {  33,   3 }, {  33,  20 }, {  33,  37 },
//     {  16,  42 }, {  16,  24 }, {  16,   7 }, {   0,   7 }, {   0,  24 }, {   0,  41 }, { 139,  16 },
//     { 174,  13 }, { 208,  20 }, { 208,  38 }, { 174,  48 }, { 139,  52 }, { 129,  63 }, { 139,  39 },
//     { 174,  35 }, { 174,  13 }, { 174,   0 }, { 191,   3 }, { 191,  20 }, { 191,  37 }, { 208,  42 },
//     { 208,  24 }, { 208,   7 }, { 224,   7 }, { 224,  24 }, { 224,  41 }
// }, {
//     4, 4, 4, 4, 4, 4, 4,
//     4, 4, 4, 4, 4, 4, 4,
//     4, 4, 4, 4, 4, 4, 4,
//     4, 4, 4, 4, 4, 4, 4,
//     4, 4, 4, 4, 4, 4, 4,
//     4, 4, 4, 4, 4, 4, 4,
//     4, 4, 4, 4, 4, 4, 4,
//     4, 4, 4, 4, 4
// } };
//
