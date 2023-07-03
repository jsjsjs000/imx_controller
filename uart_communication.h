#pragma once

#define READ_BUFFER_COUNT 10240

#define LED_COMMAND_OFF       0
#define LED_COMMAND_LIGHT_25  1
#define LED_COMMAND_LIGHT_50  2
#define LED_COMMAND_LIGHT_100 3

enum led_mode_t { LED_MODE_AUTO, LED_MODE_MANUAL };

extern int8_t led_r;
extern int8_t led_g;
extern int8_t led_b;
extern enum led_mode_t led_mode;

extern char communication_log1[1024];
extern char communication_log2[1024 + 128];
extern char communication_log3[1024];

extern void command_send_leds_states(int8_t r, int8_t g, int8_t b);
extern void command_send_leds_mode(enum led_mode_t led_mode);
extern void command_send_command(char *command, bool log);
