SRC += pgm.c
SRC += sync.c
SRC += rgb.c
SRC += tapdance.c

STENO_ENABLE = no
MOUSEKEY_ENABLE = no     # Mouse keys
LTO_ENABLE      = yes
MIDI_ENABLE = no
CONSOLE_ENABLE = yes
BOOTMAGIC_ENABLE = yes
RGB_MATRIX_ENABLE = yes
RGBLIGHT_ENABLE = no # Enables QMK's RGB code
WS2812_DRIVER = vendor # Use the RP2040's PIO interface
TAP_DANCE_ENABLE = yes
# VIA_ENABLE      = yes    # Enable VIA
