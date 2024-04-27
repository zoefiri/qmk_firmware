#include "quantum.h"
#include "rgb_matrix.h"
#include "timer.h"
#include "transactions.h"
#include "state.h"
#include "sync.h"

uint32_t last_frame = 0;
uint32_t last_spawn = 0;

uint16_t rbuff;
uint16_t gbuff;
uint16_t bbuff;

uint8_t x = 0;

uint32_t x_step = 0;

int yx_led_map[4][12] = {{18, 17, 12, 11, 4, 3, 24, 25, 32, 33, 38, 39}, {19, 16, 13, 10, 5, 2, 23, 26, 31, 34, 37, 40}, {20, 15, 14, 9, 6, 1, 22, 27, 30, 35, 36, 41}, {-1, -1, -1, 8, 7, 0, 21, 28, 29, -1, -1, -1}};

star_t stars[MAX_STARS];

starfield_t starfield = {.stars = stars, .starcount = 0, .last_despawned = -1};

star_t single_star = {.intensity  = 0.10,
                      .volatility = 1.0,
                      .luminance  = 1.0,
                      .hue        = 255,

                      .x_pos    = 0.0,
                      .y_pos    = 0.0,
                      .velocity = 10.0,
                      .spawned  = 1};

typedef struct {
    int l;
    int f;
} doublerep;

doublerep to_doublerep(double double_arg) {
    double    partial = (double_arg - (uint32_t)double_arg);
    doublerep ret     = {.l = (uint32_t)double_arg, .f = (partial)*10000};
    return ret;
}

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
_a simple pair of doubles_
 * **x:** _double_
 * **y:** _double_


# the properties used to define the animation

## Ok so the plan here is that for each "cycle" I decide cycle velocity and "intensity", this intensity is randomly distributed across x "stars"
### starsettings_t
 * **starcount:    .** *range_rscalable_t* - dictates count of strars in a cycle

 * **velocity:     .** *range_rscalable_t* - decided each cycle, this will be lightly randomized likely in a range that bumps as WPM rises.

 * **directionality:** *int* - 1/2/3/4 to progress stars in < / V / ^ / > directions


 *
 #### star properties
 * **intensity:  .** range_*rscalable_t* - total intensity factor is decided each cycle and directly correlates value, but inversely correlates saturation, both of these correlations are statically bounded.
    * _function:_ this is applied at a given point by multiplying all RGB channels by *intensity / 2* and then inverting the scaling by _inv_intensity = 1 / intensity_,
    multiply _intensity_ and _intensity_inv_ correspondingly by the greatest and smallest channels of the value-adjusted color.

 * **volatility: .** *rscalable_t* - factor that determines "sparking" emission effects from a star. This is distributed across all stars in a cycle, the amount
    of volatiles at any given moment is determined by volatility, with more volatiles their strength decreases and speed increases towards the end of the stream.
 * **volatiles:  .**  *fXY[volatility]* - holds a list of double pairs representing current volatile's strength, and when it will end.
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
 * **velocity:** _int_ - the speed at which the star traverses the grid
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

#define GRID_XY(grid, x, y) grid[yx_led_map[y][x]]

#define SET_COLR(target, hexcode)              \
    target.r = (0xff0000 & hexcode) / 0x10000; \
    target.g = (0x00ff00 & hexcode) / 0x100;   \
    target.b = 0x0000ff & hexcode;

color_t white = {COLR(0xffff66)};

color_t mult_color(color_t color, double mult) {
    rbuff   = color.r * mult;
    gbuff   = color.g * mult;
    bbuff   = color.b * mult;
    color.r = rbuff > 0xff ? 0xff : rbuff;
    color.g = gbuff > 0xff ? 0xff : gbuff;
    color.b = bbuff > 0xff ? 0xff : bbuff;
    return color;
}

void add_to_color(uint8_t color[3], uint8_t color2[3]) {
    rbuff    = color[0] + color2[0];
    gbuff    = color[1] + color2[1];
    bbuff    = color[2] + color2[2];
    color[0] = rbuff > 0xff ? 0xff : rbuff;
    color[1] = gbuff > 0xff ? 0xff : gbuff;
    color[2] = bbuff > 0xff ? 0xff : bbuff;
}

// alternative to mult_color to directly operate on a color, for very frequent color multiplication where copying colors isn't needed.
#define MULT_COLOR(color, mult)                \
    {                                          \
        rbuff   = color.r * mult;              \
        gbuff   = color.g * mult;              \
        bbuff   = color.b * mult;              \
        color.r = rbuff > 0xff ? 0xff : rbuff; \
        color.g = gbuff > 0xff ? 0xff : gbuff; \
        color.b = bbuff > 0xff ? 0xff : bbuff; \
    }

// macro to quickly add together two colors
#define ADD_COLOR(color, color2)               \
    {                                          \
        rbuff   = color.r + color2.r;          \
        gbuff   = color.g + color2.g;          \
        bbuff   = color.b + color2.b;          \
        color.r = rbuff > 0xff ? 0xff : rbuff; \
        color.g = gbuff > 0xff ? 0xff : gbuff; \
        color.b = bbuff > 0xff ? 0xff : bbuff; \
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
// at the moment the concept for this is that it takes the x,y a pixel belongs at and then aliases it across the two physical LEDs according to doubleing point position it's supposed to be @, so
// presently this is the last thing we call to draw our canvas since beyond this point doing anything but the aliasing stuff will probably fuck with the canvas
// color_t* position_adjust_color(double position, color_t* color) {
//     // intensity on x value corresponding floor of position
//     double intensity = (int)(position + 1) - position;
//     MULT_COLOR(color, intensity);
// }

// this takes a list of pixels at positions with a global offset and renders them to a physically
// representative gird of RGB vals. Returns a list of the xy it set on
void render_pixels_to_grid(pixels_t pixels, color_t* phys, int phys_len, double offset) {
    double x_base;
    double y_base;
    double x_nxt;
    double y_nxt;

    for (int i = 0; i < pixels.len; i++) {
        // calculate the % strength that aliases into x and/or y +1 and the % that displays at the base x or y
        double x = ((int)(pixels.pixels[i].pos.x + offset) % 12) + (pixels.pixels[i].pos.x - (int)pixels.pixels[i].pos.x);

        double y = pixels.pixels[i].pos.y;

        x_nxt  = x - (int)x;
        y_nxt  = y - (int)y;
        x_base = 1 - x_nxt;
        y_base = 1 - y_nxt;

        printf("%d calc'd double params: x: %f, y: \n%f, x_nxt: %f, y_nxt: %f, x_base: %f, y_base: %f\n|\n", i, x, y, x_nxt, y_nxt, x_base, y_base);

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

#define SET_XY_COLOR(x, y, color) rgb_matrix_set_color(yx_led_map[y][x], color.r, color.g, color.b)

#define ASSIGN_XY_COLOR(x, y, color) rgb_matrix_set_color(yx_led_map[y][x]), color.r, color.g, color.b)

#define RENDER_XY_COLOR_TO_GRID(x, y, color) state.phys_pixels[yx_led_map[y][x])] = color

// void render_grid(pixelsPhys_t pixels, uint8_t led_min, uint8_t led_max) {
//     for (int i = 0; i < pixels.len; i++) {
//         RGB_MATRIX_INDICATOR_SET_COLOR(i, state.rgbTicker, state.rgbTicker | 0b00001111, i * 9);
//     }
// }

// next up: we need to write a function that renders a circle to a x,y of a given size and draws bloom around it.

void draw_pixel_aliased(double x, double y, color_t pixel, color_t grid[48]) {
    int x_int = x;
    int y_int = y;

    double overflow_x = x - x_int;
    double overflow_y = y - y_int;
    double base_x     = 1 - overflow_x;
    double base_y     = 1 - overflow_y;

    // color_t base_color = mult_color(pixel, (base_x + base_y)/2.0);

    // base
    add_to_color(GRID_XY(grid, x_int, y_int).color, mult_color(pixel, (base_x + base_y) / 2.0).color);

    // x overflow
    add_to_color(GRID_XY(grid, x_int + 1, y_int).color, mult_color(pixel, (overflow_x + base_y) / 2.0).color);

    // y overflow
    // add_to_color(GRID_XY(grid, x_int, y_int + 1).color, mult_color(pixel, (base_x + overflow_y) / 2.0).color);

    //
    // // xy overflow
    // ADD_COLOR(GRID_XY(grid, x_int + 1, y_int + 1), mult_color(pixel, (overflow_x + overflow_y) / 2.0));
    // tmp            = mult_color(pixel, (overflow_x + overflow_y) / 2.0);
    // doublerep_prnt = to_doublerep((overflow_x + overflow_y)/2.0);
    // uprintf("<%d.%04d> 0x%02x:%02x:%02x\n\n", doublerep_prnt.l, doublerep_prnt.f, tmp.r, tmp.g, tmp.b);
}

void render_particle(double x, double y, double radius, double bloom, double brightness, color_t grid[48]) {
    color_t base   = mult_color(white, brightness);
    color_t trails = mult_color(base, bloom);
    // color_t verts  = mult_color(trails, 0.10);

    // draw center of particle
    draw_pixel_aliased(x, y, base, grid);

    bool u_contacting = false;
    bool b_contacting = false;
    bool l_contacting = false;
    // bool ll_contacting = false;
    // bool r_contacting = false;

    // contacting upper edge
    if (y == 0) {
        u_contacting = true;
    } else if (y == 12 - 1) {
        // if it wasn't contacting the upper edge, check if it was contacting the bottom edge
        b_contacting = true;
    }

    // contacting left edge
    if (x == 0) {
        l_contacting = true;
    } else if (x == 1) {
        // ll_contacting = true;
    }
    // else if (x == 12-1) {
    //     // if it wasn't contacting the left edge, check if it was contacting the right edge
    //     r_contacting = true;
    // }

    // draw upper
    if (!u_contacting) {
        // draw_pixel_aliased(x, y - 1, verts, grid);
        // // draw upper_r
        // if(!r_contacting) {
        //     ADD_COLOR(GRID_XY(grid, x+1, y-1), corners);
        // }
    }

    // draw bottom
    if (!b_contacting) {
        // draw_pixel_aliased(x, y + 1, verts, grid);
        // // draw upper_r
        // if(!r_contacting) {
        //     ADD_COLOR(GRID_XY(grid, x+1, y+1), corners);
        // }
    }

    // draw left
    if (!l_contacting) {
        draw_pixel_aliased(x - 1, y, trails, grid);

        // // draw further left
        // if (!ll_contacting) {
        //     draw_pixel_aliased(x - 2, y, trails, grid);
        // }
    }

    // // draw right
    // if (!r_contacting) {
    //     ADD_COLOR(GRID_XY(grid, x+1, y), sides);
    // }
}

void render_star(star_t star, color_t grid[48]) {
    // uprintf("rendering star particle @ x %d.%d", xpos.l , xpos.f);
    render_particle(star.x_pos, star.y_pos, 1.0, star.intensity, star.luminance, grid);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // if starting this render cycle & this is the main board, for each star that is spawned move its position by:
    // velocity * x_step / FADE_ACCURACY
    // after this if a star's position is >= 12.0 (has reached end of board) despawn it, otherwise render.
    if (led_min == 0 && !state.isSister) {
        clr_previous_frame(state.phys_pixels);

        for (int i = 0; i < MAX_STARS; i++) {
            if (timer_elapsed32(last_frame) > 50 || starfield.stars[i].spawned) {
                //                                              ,|' runs from 0-12.0 in intervals of 1/(12*FADE_ACCURACY)
                //                                              '-----------,
                //                                                          :----------------------,
                starfield.stars[i].x_pos = (starfield.stars[i].velocity) * (x_step % (FADE_ACCURACY * 12)) / (float)FADE_ACCURACY;
                uprintf("star %d x_pos now @: %d.%d from x_step %ld\n", i, to_doublerep(starfield.stars[i].x_pos).l, to_doublerep(starfield.stars[i].x_pos).f, x_step);
                if (starfield.stars[i].x_pos >= 12.0) {
                    uprintf("despawning star %d!\n", i);
                    starfield.stars[i].spawned = false;
                    starfield.starcount--;
                } else {
                    render_star(starfield.stars[i], state.phys_pixels);
                }
            }
        }

        sync_boards(&state);
    }

    // render colors on each key from state.phys_pixels grid
    for (uint8_t i = led_min; i < led_max; i++) {
        if (!(i == 8 || i == 7 || i == 0 || i == 21 || i == 28 || i == 29)) {
            RGB_MATRIX_INDICATOR_SET_COLOR(i, state.phys_pixels[i].r, state.phys_pixels[i].g, state.phys_pixels[i].b);
        }
    }

    // if this is the main board and enough time has passed
    if (timer_elapsed32(last_frame) > 50 && !state.isSister) {
        x_step = x_step + 1;
        uprintf("x_step: %ld :: MAX_STARS: %d, starcount: %d\n", x_step, MAX_STARS, starfield.starcount);
        last_frame = timer_read32();

        // spawn new or init stars if necessary
        if (starfield.starcount < MAX_STARS) {
            uprintf("less than %d MAX_STARS! spawning some more.\n", MAX_STARS);
            for (int i = 0; i < MAX_STARS; i++) {
                if (!starfield.stars[i].spawned && timer_elapsed32(last_spawn) > 20000) {
                    uprintf("spawning star %d...\n", i);
                    last_spawn = timer_read32();
                    star_t new_star    = {.intensity  = 0.10,
                                          .volatility = 1.0,
                                          .luminance  = 1.0,
                                          .hue        = 255,

                                          .x_pos    = 0.0,
                                          .y_pos    = (rand() % (4 * 3)) / 3.0,
                                          .velocity = ((rand() % (1 * 20)) + 10.0) / 20.0,
                                          .spawned  = true};
                    starfield.stars[i] = new_star;
                    starfield.starcount++;
                }
            }
        }
    }
    return true;
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
