#include <stdio.h>
#include <stdlib.h>

#define b1 0b00000001
#define b2 0x00000010
#define b3 0x00000100
#define b4 0x00001000
#define b5 0x00010000
#define b6 0x00100000
#define b7 0x01000000
#define b8 0x10000000

#define Nb1 0b11111110
#define Nb2 0x11111101
#define Nb3 0x11111011
#define Nb4 0x11110111
#define Nb5 0x11101111
#define Nb6 0x11011111
#define Nb7 0x10111111
#define Nb8 0x01111111

uint8_t bmask[] = {b1, b2, b3, b4, b5, b6, b7, b8};
uint8_t Nbmask[] = {Nb1, Nb2, Nb3, Nb4, Nb5, Nb6, Nb7, Nb8};

typedef struct {
    uint8_t x;
    uint8_t y;
} coord_t;

typedef struct {
    coord_t* ON;
    coord_t* OFF;
    uint16_t on_len;
    uint16_t off_len;
} frame_t;

typedef uint8_t rawframe_t[(32/8)*128];

typedef struct {
    frame_t*   frames;
    rawframe_t last_frame;

    uint16_t speed;
    uint16_t len;
} anim_t;

frame_t frames_compilebuffer;

void oled_spinup() {
    frames_compilebuffer.ON  = malloc(sizeof(coord_t) * 4096);
    frames_compilebuffer.OFF = malloc(sizeof(coord_t) * 4096);
    frames_compilebuffer.on_len = 4096;
    frames_compilebuffer.off_len = 4096;
}

void progress_to_frame(rawframe_t* frame, frame_t n_frame) {
    uint8_t *frame_vertchunk;

    for(int i=0; i<n_frame.on_len; i++) {
        //                  px column --,       px line (8px high)  --,
        //                              V                             V
        frame_vertchunk = &(*frame[n_frame.ON[i].x + 128*(n_frame.ON[i].y/8)]);

        // set bit of framechunk corresponding to y%8 ON
        //
        *frame_vertchunk |= (*frame_vertchunk & bmask[n_frame.ON[i].y%8]);
    }

    for(int i=0; i<n_frame.off_len; i++) {
        //                  px column --,       px line (8px high)  --,
        //                              V                             V
        frame_vertchunk = &(*frame[n_frame.OFF[i].x + 128*(n_frame.OFF[i].y/8)]);

        // set bit of framechunk corresponding to y%8 OFF
        //
        *frame_vertchunk |= (*frame_vertchunk & Nbmask[n_frame.ON[i].y%8]);
    }
}

void draw_box(int x, int y, int w, int h) {
    int t_y = 0;
    for (int t_y = 0; t_y < h; t_y++) {
        if(x == 0) {
            for (int t_x = 0; t_x < w; t_x++) {

            }
        }
    }
}
