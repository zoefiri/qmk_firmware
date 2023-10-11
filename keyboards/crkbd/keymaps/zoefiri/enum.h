#pragma once

#ifndef ENUM_H
#define ENUM_H

#include "quantum.h"

enum my_keycodes {
  PGM_00 = SAFE_RANGE,
  PGM_01, PGM_02, PGM_03, PGM_04, PGM_05, PGM_06,
  PGM_07, PGM_08, PGM_09, PGM_10, PGM_11, PGM_12,
  PGM_13, PGM_14, PGM_15, PGM_16, PGM_17, PGM_18,
  PGM_19, PGM_20, PGM_21, PGM_22, PGM_23, PGM_24,
  PGM_25, PGM_26, PGM_27, PGM_28, PGM_29, PGM_30,
  PGM_31, PGM_32, PGM_33, PGM_34, PGM_35, PGM_36,
  PGM_37, PGM_38, PGM_39, PGM_40, PGM_41,
  SV_PGM, XX_PGM,DEL_PGM, PG_PGM, TO_PGM, CST_DBG
};

enum modes {
    NORMAL,
    PGM_PG_SELECTING,
    PGM_PG_TARGETING
};


// tapdance keycodes
enum td_keycodes {
  ZRGB_CYC // cycle RGB hue but do it in increasingly slow steps
};

#endif
