#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "configuration.h"
#include "uart_communication.h"
#include "uart_port.h"

static char read_buffer[1024];

int8_t led_r = LED_COMMAND_LIGHT_25;
int8_t led_g = LED_COMMAND_OFF;
int8_t led_b = LED_COMMAND_OFF;
enum led_mode_t led_mode = LED_MODE_AUTO;

char communication_log1[1024] = { 0 };
char communication_log2[1024 + 128] = { 0 };
char communication_log3[1024] = { 0 };

void command_send_leds_states(int8_t r, int8_t g, int8_t b)
{
	char s[64];
	sprintf(s, "leds %d %d %d", r, g, b);
	command_send_command(s, true);
}

void command_send_leds_mode(enum led_mode_t led_mode)
{
	char s[64];
	sprintf(s, "leds_mode %d", (int)led_mode);
	command_send_command(s, true);
}

void command_send_command(char *command, bool log)
{
	if (log)
	{
		sprintf(communication_log1, "> %s\r\n", command);
	}

	int read_count;
	bool timeout;
	int read_time_us;
	bool receive_ok = send_and_receive_to_uart(command, strlen(command), read_buffer,
			READ_BUFFER_COUNT, &read_count, &timeout, &read_time_us);
	if (log)
	{
		if (receive_ok)
		{
			sprintf(communication_log2, "< %s", read_buffer);
			sprintf(communication_log3, "(+%.1f ms)\r\n\r\n", read_time_us / 1000.0);
		}
		else if (!receive_ok && timeout)
		{
			sprintf(communication_log2, "< (read timeout +%.1f ms)\r\n", read_time_us / 1000.0);
			sprintf(communication_log3, "                                              \r\n");
		}
	}
}
