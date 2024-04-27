#include <stdlib.h>

#include "oled.h"
#include "state.h"
#include "utils.h"
#include "quantum.h"

char blackRow[] = {
    0x00,
    0x00,
    0x00,
    0x00,
};
char whiteRow[] = {
    0xff,
    0xff,
    0xff,
    0xff,
};
char borderedRow[] = {0b10000000, 0x00, 0x00, 0b00000001};

oled_restore_block header_msg_restore;
oled_block         current_str_block;
dims               current_drawbox;
dims               header_msg_pos;

// saveBlock saves the block at `dims` on the OLED into a newly allocated char** and returns it
char **saveBlock(dims d) {
    oled_buffer_reader_t reader;
    char               **saveBlock = malloc(sizeof(char *) * d.h);

    int start_pos = d.y * 32 + d.x;

    // iterate each row
    reader = oled_read_raw(start_pos);
    for (int i = 0; i < d.h; i++) {
        saveBlock[i] = malloc(sizeof(char) * d.w);
        memcpy(saveBlock[i + d.y], reader.current_element, d.w);
    }

    return saveBlock;
}

// displayBlock displays d.w px of d.h rows of block to d.x,d.y on the OLED
void displayBlock(oled_block blk) {
    oled_set_cursor(blk.dims.x, blk.dims.y);

    for (int i = 0; i < blk.dims.h; i++) {
        oled_write_raw(blk.block[i], blk.dims.w);

        blk.block++;
    }
}

// clearDisplay blacks out the display in d.w x d.h from d.x,dy.y
void clearDisplay(dims d) {
    oled_set_cursor(d.x, d.y);

    for (int i = 0; i < d.h; i++) {
        oled_write_raw(blackRow, d.w);
        oled_set_cursor(d.x, d.y + i);
    }
}

// displayBox is like clearDisplay, but it draws a box of dims too.
// (this isn't sophisticated enough to not break with a "box" of 1px height)
void displayBox(dims d) {
    oled_set_cursor(d.x, d.y);

    for (int i = 0; i < d.h; i++) {
        if (i == 0) {
            oled_write_raw(whiteRow, d.w);
        } else if (i == d.h - 1) {
            oled_write_raw(whiteRow, d.w);
        } else {
            oled_write_raw(borderedRow, d.w);
        }
        oled_set_cursor(d.x, d.y + i);
    }
}

// cpBlock copies a box specified by srcDims from src to the position @ dst_x,dst_y
void cpBlock(char **src, char **dest, dims srcDims, int dst_x, int dst_y) {
    // increment rows to src & dst row
    src += srcDims.y;
    dest += dst_y;

    for (int i = 0; i < srcDims.h; i += 32) {
        memcpy(dest + dst_x, src + srcDims.x, srcDims.w);

        src++;
        dest++;
    }
}

// freeBlock frees all memory associated with a char** (block)
void freeBlock(oled_block blk) {
    for (int i = 0; i < blk.dims.h; i++) {
        free(blk.block[i]);
    }
    free(blk.block);
}

// renderStr takes a vertical single column font sheet with 0px padding and uses CHAR_W & CHAR_H & ASCII char addresses to pull each
// character from the font sheet to render string s and return it as a bitmap
char **renderStr(char **fontSheet, strg s) {
    // alloc str block
    int    rows  = ((CHAR_H + 1) * s.len);
    char **block = malloc(sizeof(char *) * rows);
    for (int i = 0; i < rows; i++) {
        block[i] = malloc(sizeof(char) * CHAR_W);
    }

    // for each character, copy (0,'row_addr_char') CHAR_W x CHAR_H block to the `block` @ (0, i*(CHAR_H+1))
    //                                   ,-- y axis addr of target fontsheet char (fontsheet starts @ ASCII 31)
    for (int i = 0; i < s.len; i++) { //,|----------------,
        dims charDims = (dims){.x = 0, .y = (CHAR_H * s.str[i - 31]), .w = CHAR_W, .h = CHAR_H + 1};
        cpBlock(fontSheet, block, charDims, 0, (CHAR_H + 1) * i);
    }

    return block;
}

// animTxtFrame is called to animate a frame.
//
// TODO:
// + this should be using a string in state to be currently displaying
// + when the above string is altered a flag needs to be set to let us know that we need to immediately start
//   re-rendering, after which it can be unset
// + it'd be nice to program a fade out animation as well but we'll see how expensive it is.
// + we'll need an additional fn that restores from the saved block once we're done, it should set msgDisplaying to false.
void animTxtFrame(int frameId) {
    strg headerMsg = str_wrap("LAYER");

    // render the string bitmap into the box for each frame (except on stretch frame 3, clear box each time. May want to make this more efficient)
    switch (frameId) {
        case 0:
            // for first frame do initial calculations and string rendering
            printf("what");
            int strWidth  = CHAR_W;
            int strHeight = (CHAR_H + 1) * headerMsg.len;

            //   CHAR_H+1px pad--,/''''''''\_,----each char-,    /'--- extra 2px at box's horizontal edges
            int  drawboxHeight = ((CHAR_H + 1) * headerMsg.len) + 2 + 22; // <--- 22px of animation space,
            dims drawboxDims   = (dims){
                  .x = 2,
                  .y = (128 - drawboxHeight) - 2,
                  .w = CHAR_W + 2,
                  .h = drawboxHeight,
            };

            current_drawbox   = drawboxDims;
            current_str_block = (oled_block){
                .block = renderStr((char **)fontSheet, headerMsg),                                                   //
                .dims  = (dims)(dims){.w = strWidth, .h = strHeight, .x = drawboxDims.x + 2, .y = drawboxDims.y + 2} //
            };

            // if a msg was already displaying clear the current header msg dims
            //
            // else need to also keep track of what animation frame we're on currently, we want to disappear the text
            // and re-display the saved block @ the same frame.
            if (state.msg_displaying) {
                clearDisplay(current_str_block.dims);
            } else {
                header_msg_restore.block = saveBlock(drawboxDims);
                displayBox(current_drawbox);
            }

            displayBlock(current_str_block);
            break;
        case 1:
            clearDisplay(current_str_block.dims);
            current_str_block.dims.y += 1; // move 1 px down
            displayBlock(current_str_block);
            break;
        case 2:
            clearDisplay(current_str_block.dims);
            current_str_block.dims.y += 3; // accelerate to 3 px down
            displayBlock(current_str_block);
            break;
        case 3:
            // this is a stretch frame, don't clear this time & redraw several times
            current_str_block.dims.y += 5;
            displayBlock(current_str_block);

            current_str_block.dims.y += 4;
            displayBlock(current_str_block);

            current_str_block.dims.y += 3;
            displayBlock(current_str_block);

            current_str_block.dims.y += 2;
            displayBlock(current_str_block);
            break;
        case 4:
            // need to exapnd header_msg_dims temporarily to capture the entire stretch frame
            current_str_block.dims.y -= 14;
            current_str_block.dims.h += 14;
            clearDisplay(current_str_block.dims);
            current_str_block.dims.y += 14 + 2; // readjust dimensions + hard decceleration, only move 2 more frames
            current_str_block.dims.h -= 14;

            displayBlock(current_str_block);
            break;
        case 5:
            clearDisplay(current_str_block.dims);
            current_str_block.dims.y += 1; // halt (+1)
            displayBlock(current_str_block);
            break;
        case 6:
            clearDisplay(current_str_block.dims);
            current_str_block.dims.y -= 2; // snap back (-2)
            displayBlock(current_str_block);
            break;
    };

    freeBlock(current_str_block);
}
