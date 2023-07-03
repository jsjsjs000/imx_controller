#pragma once

enum command_t { COMMAND_NONE, COMMAND_SEND_LEDS_STATES, COMMAND_SEND_LEDS_MODE };
extern enum command_t command;

extern bool uart_thread_initialize(void);
extern bool uart_thread_destroy(void);
extern void enqueue_command_send_leds_states(int8_t r, int8_t g, int8_t b);
extern void enqueue_command_send_leds_mode(enum led_mode_t led_mode);
