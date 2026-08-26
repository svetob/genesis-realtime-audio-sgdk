#ifndef __TESTUI_H
#define __TESTUI_H

#include <genesis.h>
#include <audiofx/filter_lp8.h>

typedef enum {
    TAB_STREAM = 0,
    TAB_FILTER,
    TAB_ECHO,
    TAB_REVERB,
    TAB_DRIVE,
    TAB_OUTPUT,
} UITab;

#define UI_TABS 6

extern bool param_filter_enabled;
extern FilterLPType param_filter_type;
extern u16 param_filter_freq;
extern u16 param_filter_q;
extern bool filter_params_updated;

extern bool param_echo_enabled;
extern u16 param_echo_delay;
extern u8 param_echo_feedback;
extern bool echo_params_updated;

void toggleVGM();
void playSoundSnare();
void playSoundSweep();
void resetStream();
void updateParams();
void updateStream(bool renderNext);
void startStream();

void runUI();

#endif
