SRC += pgm.c
SRC += sync.c
SRC += rgb.c
SRC += tapdance.c
SRC += moonscrape.c
SRC += oled.c

# ergo features
STENO_ENABLE     = no
TAP_DANCE_ENABLE = yes
MOUSEKEY_ENABLE  = no  # Mouse keys

# devtools & etc.
CONSOLE_ENABLE   = yes
BOOTMAGIC_ENABLE = yes
CONVERT_TO       = blok
PRINTF_LIB       = $(PRINTF_LIB_FLOAT)
# LTO_ENABLE     = yes # enable this when actually building FW

# RGB
RGB_MATRIX_ENABLE = yes
RGBLIGHT_ENABLE   = no     # Enables QMK's RGB code
WS2812_DRIVER     = vendor # Use the RP2040's PIO interface

# enable split
SPLIT_KEYBOARD = yes

# fun :D
MIDI_ENABLE = no
