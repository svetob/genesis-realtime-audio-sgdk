#include <genesis.h>
#include "resources.h"
#include "ui.h"

#include <audiofx/echo8.h>
#include <audiofx/filter_lp8.h>
#include <pcm_stream/pcm_stream8.h>

#include <test/log.h>
#include <test/timer.h>

#define PCM_PLAYBACK_RATE 13300
#define ECHO_BUFFER_SIZE  4096

static const char *tabNames[] = {"STREAM", "FILTER", " ECHO ", "REVERB", "DRIVE ", "OUTPUT"};
static const char *filterTypeNames[] = {"1-POLE 6db", "2-POLE 12db", "2-P RESONANT 12db"};
static u8 tabParamCnt[] = {0, 3, 2, 0, 0, 0};

// ===========================
// PRIVATE
// ===========================

#define PARAM_ROW_FILTER_ENABLED 1
#define PARAM_ROW_FILTER_TYPE    2
#define PARAM_ROW_FILTER_FREQ    3
#define PARAM_ROW_FILTER_Q       4

#define PARAM_ROW_ECHO_ENABLED   1
#define PARAM_ROW_ECHO_DELAY     2
#define PARAM_ROW_ECHO_FEEDBACK  3

static u8 currentTab = 0;
static u8 currentRow = 0;
static s8 tabRollerOffset = 0;

static char uicharbuf[10];

static bool redrawParams;

static void writeParamU16(u16 val, char *unit, u16 x, u16 y, u8 minLen)
{
    intToStr(val, uicharbuf, minLen);

    u8 len = 0;
    char *charAt = uicharbuf;
    bool sawFirstDigit = false;
    while (*charAt != NULL) {
        if (*charAt == '0' && sawFirstDigit == false) {
            *charAt = ' ';
        } else {
            sawFirstDigit = true;
        }
        len++;
        charAt++;
    }

    VDP_drawText(uicharbuf, x, y);
    VDP_drawText(unit, x + len + 1, y);
}

static void changeParam(bool pressed, bool inc)
{
    // --- Filter ---
    if (currentTab == TAB_FILTER) {
        // Enabled
        if (currentRow == PARAM_ROW_FILTER_ENABLED && pressed) {
            param_filter_enabled = !param_filter_enabled;
        }

        // Type
        if (currentRow == PARAM_ROW_FILTER_TYPE && pressed) {
            if (!inc && param_filter_type > 0) {
                param_filter_type--;
            }
            if (inc && param_filter_type < 2) {
                param_filter_type++;
            }
        }

        // Freq
        if (currentRow == PARAM_ROW_FILTER_FREQ && !pressed) {
            if (inc) {
                if (param_filter_freq < 1000) {
                    param_filter_freq += 10;
                } else if (param_filter_freq < (PCM_PLAYBACK_RATE >> 1)) {
                    param_filter_freq += 50;
                }
            } else {
                if (param_filter_freq > 1000) {
                    param_filter_freq -= 50;
                } else if (param_filter_freq > 40) {
                    param_filter_freq -= 10;
                }
            }
        }

        // Resonance (Q)
        if (currentRow == PARAM_ROW_FILTER_Q && !pressed) {
            if (inc && param_filter_q < 65000) {
                param_filter_q += 400;
            }
            if (!inc && param_filter_q > 1000) {
                param_filter_q -= 400;
            }
        }

        filter_params_updated = true;
    }

    if (currentTab == TAB_ECHO) {
        if (currentRow == 1 && pressed) {
            // Enabled
            param_echo_enabled = !param_echo_enabled;
        }

        if (currentRow == 2 && !pressed) {
            // Freq
            if (inc) {
                if (param_echo_delay < ECHO_BUFFER_SIZE) {
                    param_echo_delay += 64;
                }
            } else {
                if (param_echo_delay > 256) {
                    param_echo_delay -= 64;
                }
            }
        }

        echo_params_updated = true;
    }

    redrawParams = true;
}

static void handleInputHeld(u16 joy)
{
    if (currentRow == 0) {
        return;
    }

    if (joy & BUTTON_LEFT) {
        changeParam(false, false);
    }

    if (joy & BUTTON_RIGHT) {
        changeParam(false, true);
    }
}

static void handleInput(u16 joy, u16 changed, u16 state)
{
    if (changed & state & BUTTON_UP) {
        if (currentRow > 0) {
            currentRow -= 1;
        }
        redrawParams = true;
    }

    if (changed & state & BUTTON_DOWN) {
        if (currentRow < tabParamCnt[currentTab]) {
            currentRow += 1;
        }
        redrawParams = true;
    }

    if (changed & state & BUTTON_LEFT) {
        if (currentRow == 0) {
            if (currentTab > 0) {
                currentTab--;
                tabRollerOffset += 10;
                redrawParams = true;
            }
        } else {
            changeParam(true, false);
        }
    }

    if (changed & state & BUTTON_RIGHT) {
        if (currentRow == 0) {
            if (currentTab < UI_TABS - 1) {
                currentTab++;
                tabRollerOffset -= 10;
                redrawParams = true;
            }
        } else {
            changeParam(true, true);
        }
    }

    if (changed & state & BUTTON_A) {
        playSoundSweep();
    }

    if (changed & state & BUTTON_B) {
        playSoundSnare();
    }

    if (changed & state & BUTTON_START) {
        toggleVGM();
    }

    if (changed & state & BUTTON_MODE) {
        resetStream();
    }
}

static void drawTabs()
{
    s16 startX = 17;
    s16 Y = 10;
    s16 offsetPerTab = 10;

    VDP_clearText(0, Y, 40);

    startX -= offsetPerTab * currentTab + tabRollerOffset;
    for (u8 i = 0; i < UI_TABS; i++) {
        if (startX > 2 && startX < 30) {
            VDP_drawText(tabNames[i], startX, Y);
        }
        startX += offsetPerTab;
    }

    if (currentRow == 0) {
        VDP_drawText("       < ", 0, 10);
        VDP_drawText(" >         ", 30, 10);
    }

    if (tabRollerOffset > 0) {
        tabRollerOffset -= 2;
    }
    if (tabRollerOffset < 0) {
        tabRollerOffset += 2;
    }
}

#define YTOP    13
#define YOFFSET 2
#define XNAME   2
#define XVALUE  22
#define YPOS(x) (YTOP + (YOFFSET * (x - 1)))

static void drawOptions()
{
    if (!redrawParams) {
        return;
    }

    VDP_clearTextArea(0, YTOP, 40, 20);

    if (currentRow > 0) {
        VDP_drawText(">", XVALUE - 2, YTOP + (YOFFSET * (currentRow - 1)));
    }

    if (currentTab == TAB_FILTER) {
        VDP_drawText("         ENABLED", XNAME, YPOS(PARAM_ROW_FILTER_ENABLED));
        VDP_drawText("     FILTER TYPE", XNAME, YPOS(PARAM_ROW_FILTER_TYPE));
        VDP_drawText("     CUTOFF FREQ", XNAME, YPOS(PARAM_ROW_FILTER_FREQ));
        VDP_drawText("   RESONANCE (Q)", XNAME, YPOS(PARAM_ROW_FILTER_Q));

        VDP_drawText(param_filter_enabled ? "ON" : "OFF", XVALUE, YPOS(PARAM_ROW_FILTER_ENABLED));
        VDP_drawText(filterTypeNames[param_filter_type], XVALUE, YPOS(PARAM_ROW_FILTER_TYPE));
        writeParamU16(param_filter_freq, "Hz", XVALUE, YPOS(PARAM_ROW_FILTER_FREQ), 4);
        writeParamU16(param_filter_q, NULL, XVALUE, YPOS(PARAM_ROW_FILTER_Q), 5);
    }

    if (currentTab == TAB_ECHO) {
        VDP_drawText("         ENABLED", XNAME, YPOS(PARAM_ROW_ECHO_ENABLED));
        VDP_drawText("           DELAY", XNAME, YPOS(PARAM_ROW_ECHO_DELAY));
        VDP_drawText("        FEEDBACK", XNAME, YPOS(PARAM_ROW_ECHO_FEEDBACK));

        VDP_drawText(param_echo_enabled ? "ON" : "OFF", XVALUE, YPOS(PARAM_ROW_ECHO_ENABLED));
        writeParamU16(param_echo_delay, "samples", XVALUE, YPOS(PARAM_ROW_ECHO_DELAY), 4);
        writeParamU16(param_filter_q, NULL, XVALUE, YPOS(PARAM_ROW_ECHO_FEEDBACK), 4);
    }

    redrawParams = false;
}

// ===========================
// PUBLIC
// ===========================

void runUI()
{
    VDP_clearPlane(BG_A, true);
    VDP_clearPlane(BG_B, true);
    VDP_clearPlane(WINDOW, true);

    VDP_drawText("PCM STREAM", 15, 1);

    JOY_setEventHandler(handleInput);
    Z80_loadDriver(Z80_DRIVER_XGM2, true);

    startStream();

    while (true) {
        SYS_doVBlankProcess();

        handleInputHeld(JOY_readJoypad(JOY_1));

        // UI render
        drawTabs();
        drawOptions();

        logNamedU16("SCANLINES", scanlines_avg, 1, 27, 3);

        // PCM Stream updates
        updateParams();
        updateStream(true);

        while (GET_VCOUNTER < 70) {
        }

        updateStream(false);

        while (GET_VCOUNTER < 140) {
        }

        updateStream(false);

        // Performance measurements
        scanlineTimerNextFrame();
    }
}