#include <genesis.h>
#include "resources.h"
#include "ui.h"

#include <audiofx/echo.h>
#include <audiofx/filterlp.h>
#include <pcmstream/pcmstream.h>

#include <test/log.h>
#include <test/timer.h>

// ===========================
// CONST
// ===========================

#define PCM_PLAYBACK_RATE        13300
#define ECHO_BUFFER_SIZE         4096

#define PARAM_ROW_FILTER_ENABLED 1
#define PARAM_ROW_FILTER_TYPE    2
#define PARAM_ROW_FILTER_FREQ    3
#define PARAM_ROW_FILTER_Q       4

#define PARAM_ROW_ECHO_ENABLED   1
#define PARAM_ROW_ECHO_DELAY     2
#define PARAM_ROW_ECHO_FEEDBACK  3

#define PARAM_ROW_YTOP           13
#define PARAM_ROW_YOFFSET        2
#define PARAM_ROW_XNAME          2
#define PARAM_ROW_XVALUE         22
#define PARAM_ROW_YPOS(x)        (PARAM_ROW_YTOP + (PARAM_ROW_YOFFSET * (x - 1)))

static const char *tabNames[] = {"STREAM", "FILTER", " ECHO ", "REVERB", "DRIVE ", "OUTPUT"};
static const char *filterTypeNames[] = {"1-POLE 6db", "2-POLE 12db", "2-P RESONANT 12db"};
static u8 tabParamCnt[] = {0, 4, 3, 0, 0, 0};

// ===========================
// PRIVATE
// ===========================

static u8 currentTab = 0;
static u8 currentRow = 0;
static s8 tabRollerOffset = 0;

static bool redrawParams = false;
static u8 redrawParamRow = 0;

static char paramStrBuf[10];
static u8 paramToStrU16(u16 val, char *unit, u8 minLen)
{
    intToStr(val, paramStrBuf, minLen);

    u8 len = 0;
    char *charAt = paramStrBuf;
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
    return len;
}

inline static void writeParamValueU16(u8 row, u16 val, char *unit, u8 minLen)
{
    u8 len = paramToStrU16(val, unit, minLen);
    VDP_drawText(paramStrBuf, PARAM_ROW_XVALUE, PARAM_ROW_YPOS(row));
    VDP_drawText(unit, PARAM_ROW_XVALUE + len + 1, PARAM_ROW_YPOS(row));
}

inline static void writeParamValueText(char *val, u8 row)
{
    if (redrawParams || redrawParamRow == row) {
        VDP_drawText(val, PARAM_ROW_XVALUE, PARAM_ROW_YPOS(row));
    }
}

inline static void writeParamName(char *val, u8 row)
{
    if (redrawParams) {
        VDP_drawText(val, PARAM_ROW_XNAME, PARAM_ROW_YPOS(row));
    }
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

static void drawOptions()
{
    if (!redrawParams && !redrawParamRow) {
        return;
    }

    if (redrawParams) {
        VDP_clearTextArea(0, PARAM_ROW_YTOP, 40, 20);
    }

    if (currentRow > 0) {
        VDP_drawText(">", PARAM_ROW_XVALUE - 2,
                     PARAM_ROW_YTOP + (PARAM_ROW_YOFFSET * (currentRow - 1)));
    }

    if (currentTab == TAB_FILTER) {
        writeParamName("         ENABLED", PARAM_ROW_FILTER_ENABLED);
        writeParamName("     FILTER TYPE", PARAM_ROW_FILTER_TYPE);
        writeParamName("     CUTOFF FREQ", PARAM_ROW_FILTER_FREQ);
        writeParamName("   RESONANCE (Q)", PARAM_ROW_FILTER_Q);

        writeParamValueText(param_filter_enabled ? "ON" : "OFF", PARAM_ROW_FILTER_ENABLED);
        writeParamValueText((char *) filterTypeNames[param_filter_type], PARAM_ROW_FILTER_TYPE);
        writeParamValueU16(PARAM_ROW_FILTER_FREQ, param_filter_freq, "Hz", 4);
        writeParamValueU16(PARAM_ROW_FILTER_Q, param_filter_q, NULL, 5);
    }

    else if (currentTab == TAB_ECHO) {
        writeParamName("         ENABLED", PARAM_ROW_ECHO_ENABLED);
        writeParamName("           DELAY", PARAM_ROW_ECHO_DELAY);
        writeParamName("        FEEDBACK", PARAM_ROW_ECHO_FEEDBACK);

        writeParamValueText(param_echo_enabled ? "ON" : "OFF", PARAM_ROW_ECHO_ENABLED);
        writeParamValueU16(PARAM_ROW_ECHO_DELAY, param_echo_delay, "samples", 4);
        writeParamValueU16(PARAM_ROW_ECHO_FEEDBACK, param_filter_q, NULL, 5);
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
        scanlineTimerWait();

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