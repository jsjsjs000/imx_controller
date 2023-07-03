#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>
#include "uart_commands.h"
#include "uart_communication.h"
#include "uart_port.h"
#include "uart_thread.h"

enum command_t command = COMMAND_NONE;
int8_t send_led_r;
int8_t send_led_g;
int8_t send_led_b;
enum led_mode_t send_led_mode;

static pthread_t uart_thread;
static volatile bool thread_exit = false;
static void* uart_thread_loop(void *data);

bool uart_thread_initialize(void)
{
	if (pthread_create(&uart_thread, NULL, uart_thread_loop, "uart_thread"))
	{
		printf("Error: Can't create uart thread.\n");
		return false;
	}

	return true;
}

bool uart_thread_destroy(void)
{
	thread_exit = true;
	if (pthread_cancel(uart_thread))
	{
		printf("Error: Can't destroy uart thread.\n");
		return false;
	}

	return true;
}

static bool send_command_led_status(void)
{
	const char *command = "leds_status";
	int read_count;
	bool timeout;
	int read_time_us;
	char read_buffer[1024];
	if (!send_and_receive_to_uart(command, strlen(command), read_buffer,
			READ_BUFFER_COUNT, &read_count, &timeout, &read_time_us))
	{
		return false;
	}

	char result[512] = { 0 };
	if (!parse_line(read_buffer, result))
	{
		return false;
	}

	return true;
}

void* uart_thread_loop(void *data __attribute__((unused)))
{
	while (!thread_exit)
	{
		if (command == COMMAND_SEND_LEDS_STATES)
		{
			command_send_leds_states(send_led_r, send_led_g, send_led_b);
			command = COMMAND_NONE;
		}
		else if (command == COMMAND_SEND_LEDS_MODE)
		{
			command_send_leds_mode(send_led_mode);
			command = COMMAND_NONE;
		}
		else
		{
			send_command_led_status();
		}

		usleep(30 * 1000); /// sleep 30 ms
	}

	return NULL;
}

void enqueue_command_send_leds_states(int8_t r, int8_t g, int8_t b)
{
	send_led_r = r;
	send_led_g = g;
	send_led_b = b;
	command = COMMAND_SEND_LEDS_STATES;
}

void enqueue_command_send_leds_mode(enum led_mode_t led_mode)
{
	send_led_mode = led_mode;
	command = COMMAND_SEND_LEDS_MODE;
}
