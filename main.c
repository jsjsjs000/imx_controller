#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "common.h"
#include "configuration.h"
#include "uart_communication.h"
#include "uart_port.h"
#include "uart_thread.h"

volatile bool state_changed = true;
configuration_t configuration;

// static void command_custom_packet(void);
// static void command_change_uart_name(void);
static void print_help(void);
// static void wait_for_any_key(void);
static char* i2c_get_led_status(uint8_t value);

int main(void)
{
	printf("i.MX Cortex-M7 controller.\r\n\r\n");

	configuration_initialize(&configuration);
	configuration_load(&configuration);

	if (!open_uart(configuration.uart_port_name))
		return 1;

	if (!uart_thread_initialize())
		return 1;

	printf("\033[s");

	while (true)
	{
		if (state_changed)
		{
				/// https://tldp.org/HOWTO/Bash-Prompt-HOWTO/x361.html
			printf("\033[u");
			print_help();
			fflush(stdout);
			state_changed = false;
		}

		int c = getch_non_blocking();
		if (c)
		{
			printf("\b      \r");
			state_changed = true;

			if (c == '1')
			{
				enqueue_command_send_leds_states(max(led_r - 1, 0), led_g, led_b);
			}
			else if (c == '2')
			{
				enqueue_command_send_leds_states(min(led_r + 1, LED_COMMAND_LIGHT_100), led_g, led_b);
			}
			else if (c == '3')
			{
				enqueue_command_send_leds_states(led_r, max(led_g - 1, 0), led_b);
			}
			else if (c == '4')
			{
				enqueue_command_send_leds_states(led_r, min(led_g + 1, LED_COMMAND_LIGHT_100), led_b);
			}
			else if (c == '5')
			{
				enqueue_command_send_leds_states(led_r, led_g, max(led_b - 1, 0));
			}
			else if (c == '6')
			{
				enqueue_command_send_leds_states(led_r, led_g, min(led_b + 1, LED_COMMAND_LIGHT_100));
			}
			else if (c == 'a')
			{
				enqueue_command_send_leds_mode((led_mode == LED_MODE_AUTO) ? LED_MODE_MANUAL : LED_MODE_AUTO);
			}
			// else if (c == 'p')
			// {
			// 	command_custom_packet();
			// }
			// else if (c == 'u')
			// {
			// 	command_change_uart_name();
			// }
			else if (c == 'q')
			{
				printf("\r\nBye.\r\n");
				close_uart();
				return 0;
			}
			// else
			// {
			//char input[1024];
			// 	command_send_command(input);
			// }
		}

		usleep(10 * 1000); /// sleep 10 ms
	}

	if (!uart_thread_destroy())
		return 1;

	return 0;
}

// static void command_send_command(char *input)
// {
// 	char *end;
// 	int cmd_ix = (int)strtol(input, &end, 10);
// 	if (input != end && cmd_ix > 0 && cmd_ix <= Commands_Count)
// 	{
// 		printf("> %s\r\n", Commands[(int)cmd_ix - 1]);

// 		int read_count;
// 		bool timeout;
// 		int read_time_us;
// 		bool receive_ok = send_and_receive_to_uart(configuration.uart_port_name, Commands[(int)cmd_ix - 1],
// 				strlen(Commands[(int)cmd_ix - 1]), read_buffer, READ_BUFFER_COUNT, &read_count,
// 				&timeout, &read_time_us);
// 		if (receive_ok)
// 		{
// 			printf("< %s", read_buffer);
// 			printf("(+%.1f ms)\r\n", read_time_us / 1000.0);
// 		}
// 		else if (!receive_ok && timeout)
// 		{
// 			printf("< (read timeout +%.1f ms)\r\n", read_time_us / 1000.0);
// 		}
// 	}
// }

// static void command_custom_packet(void)
// {
// 	printf("Send custom packet: ");
// 	while (getchar() != '\n') ;
	
// 	char *input = NULL;//[1024];
// 	size_t input_length;
// 	if (getline(&input, &input_length, stdin) > 0)
// 	{
// 		printf("> %s\r\n", input);

// 		int read_count;
// 		bool timeout;
// 		int read_time_us;
// 		bool receive_ok = send_and_receive_to_uart(configuration.uart_port_name, input,
// 				strlen(input), read_buffer, READ_BUFFER_COUNT, &read_count,
// 				&timeout, &read_time_us);
// 		if (receive_ok)
// 		{
// 			printf("< %s", read_buffer);
// 			printf("(+%.1f ms)\r\n", read_time_us / 1000.0);
// 		}
// 		else if (!receive_ok && timeout)
// 		{
// 			printf("< (read timeout +%.1f ms)\r\n", read_time_us / 1000.0);
// 		}
// 	}
// }

// static void command_change_uart_name(void)
// {
// 	char input[1024];
// 	printf("UART name: ");
// 	if (scanf("%s", input) > 0)
// 	{
// 		configuration_set_uart_name(&configuration, input);
// 		configuration_save(&configuration);
// 	}
// }

static void print_help(void)
{
	printf("[1/2] LED Red:   %s  \r\n", i2c_get_led_status(led_r));
	printf("[3/4] LED Green: %s  \r\n", i2c_get_led_status(led_g));
	printf("[5/6] LED Blue:  %s  \r\n", i2c_get_led_status(led_b));
	printf("[a]   Auto mode: %s  \r\n", (led_mode == LED_MODE_AUTO) ? "on " : "off");
	// $$ printf("[u]   Set UART name - current: %s\r\n", configuration.uart_port_name);
	printf("[q]   Quit program.\r\n");
	if (strlen(communication_log1) > 0)
	{
		printf("\r                                       \r\n");
		printf("                                         \r%s", communication_log1);
		printf("                                         \r%s", communication_log2);
		printf("                                         \r%s", communication_log3);
	}
	printf("Press suitable key: ");
}

static char* i2c_get_led_status(uint8_t value)
{
	switch (value)
	{
		case LED_COMMAND_OFF:       return "  0%";
		case LED_COMMAND_LIGHT_25:  return " 25%";
		case LED_COMMAND_LIGHT_50:  return " 50%";
		case LED_COMMAND_LIGHT_100: return "100%";
	}
	return "undef";
}

// static void wait_for_any_key(void)
// {
// #ifdef __unix__
// 	printf("Press ENTER to continue...");
// 	while (getchar() != '\n') ;
// 	getchar();
// #else
// 	system("pause");
// #endif
// }
