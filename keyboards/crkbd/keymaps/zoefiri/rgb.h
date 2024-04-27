#pragma once

#ifndef RGB_H
#    define RGB_H

#    include "state.h"

#    define PHYS_PIXELS_COUNT 48

#    define FADE_ACCURACY 16

#    define MAX_STARS 5

#    define TICK_RATE 75

typedef enum { ASCEND, DESCEND, CENTERED } rscale_direction_t;

// represents RNG that scales int base upwards, downwards, or equally probably either by % represented by deviation
typedef struct {
    uint16_t           base;
    uint16_t           deviation;
    rscale_direction_t direction;

    uint16_t halfdev;
} rscalable_t;

// represents RNG that is bounded by two rscalable random values
typedef struct {
    rscalable_t min;
    rscalable_t max;
} range_rscalable_t;

typedef struct {
    int skipped;
} rgb_state_t;

// star props
//
typedef struct {
    double x;
    double y;
} xy_double;

typedef struct {
    double intensity;
    double volatility;
    double luminance;
    int    hue;

    double x_pos;
    double y_pos;

    double velocity;
    bool    spawned;
} star_t;

typedef struct {
    int     starcount;
    int     last_despawned;
    star_t *stars;
} starfield_t;

// rgb color shit
//
typedef union {
    uint8_t color[3];
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
} color_t;

typedef struct {
    color_t   color;
    xy_double pos;
} pixel_t;

typedef struct {
    pixel_t *pixels;
    int      len;
} pixels_t;

#endif
