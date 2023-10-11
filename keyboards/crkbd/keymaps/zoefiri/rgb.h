#pragma once

#ifndef RGB_H
#define RGB_H

#include "state.h"

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
    float x;
    float y;
} xy_float;

typedef struct {
    int intensity;
    int volatility;
    int luminance;
    int hue;

    int pos;
    int duration;
    int spawned;
} star_t;

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
    color_t  color;
    xy_float pos;
} pixel_t;

typedef struct {
    pixel_t* pixels;
    int      len;
} pixels_t;

#define PHYS_PIXELS_COUNT 48

#endif
