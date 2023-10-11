#pragma once

#ifndef PGM_H
#define PGM_H

#include "state.h"

void pgm_program(state_t *state, uint16_t keycode, keyrecord_t *record);

// macro stuff for generating a case for each PGM key
#define MACRO_PGM_CASE(PGM_INDEX)\
    case PGM_ ## PGM_INDEX : \
        if(record->event.pressed) {\
            register_code( user_config.pgm_layer[PGM_ ## PGM_INDEX - PGM_00]);\
        }\
        else {\
            unregister_code( user_config.pgm_layer[PGM_ ## PGM_INDEX - PGM_00] );\
        }\
        return false;

#define MACRO_PGM_CASE_TIMES_TEN(PGM_INDEX_LEADER)\
    MACRO_PGM_CASE(PGM_INDEX_LEADER ## 1)\
    MACRO_PGM_CASE(PGM_INDEX_LEADER ## 2)\
    MACRO_PGM_CASE(PGM_INDEX_LEADER ## 3)\
    MACRO_PGM_CASE(PGM_INDEX_LEADER ## 4)\
    MACRO_PGM_CASE(PGM_INDEX_LEADER ## 5)\
    MACRO_PGM_CASE(PGM_INDEX_LEADER ## 6)\
    MACRO_PGM_CASE(PGM_INDEX_LEADER ## 7)\
    MACRO_PGM_CASE(PGM_INDEX_LEADER ## 8)\
    MACRO_PGM_CASE(PGM_INDEX_LEADER ## 9)

#define MACRO_PGMS()\
    MACRO_PGM_CASE_TIMES_TEN(0)\
    MACRO_PGM_CASE_TIMES_TEN(1)\
    MACRO_PGM_CASE_TIMES_TEN(2)\
    MACRO_PGM_CASE_TIMES_TEN(3)\
    MACRO_PGM_CASE(40)\
    MACRO_PGM_CASE(41)

#endif
