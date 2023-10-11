#include "quantum.h"
#include "quantum/color.h"

RGB_MATRIX_EFFECT(starfire)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// e.g: A more complex effect, relying on external methods and state, with
// dedicated init and run methods
static uint8_t ticker;
static uint8_t r;
static void starfire_complex_init(effect_params_t* params) {
  ticker = 0;
  r = 0;
  srand(0);
}

static bool starfire_complex_run(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    ticker = (ticker+1) % 256;
    r = rand()%256;

    uint8_t i = ticker%42;
    printf("fugg %d %d %d %d\n", i, ticker, ~ticker, ticker & (ticker-(ticker*2)%256));
    rgb_matrix_set_color(i, 0xff, 0xff, 0xff);

    return rgb_matrix_check_finished_leds(led_max);
}

static bool starfire(effect_params_t* params) {
  if (params->init) starfire_complex_init(params);
  return starfire_complex_run(params);
}

#endif

