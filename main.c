#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "uart_port.h"
#include "configuration.h"

#define Read_Buffer_Count 10240

const int Commands_Count = 2;
const char *Commands[] = { "add 111 9 -20", "add 111 a 9" };

static char read_buffer[1024];
static configuration_t configuration;

static void command_send_command(char *input);
static void command_change_uart_name(void);
static void print_help(void);
static void wait_for_any_key(void);

int main(void)
{
	printf("i.MX Cortex-M7 controller.\r\n\r\n");

	configuration_initialize(&configuration);
	configuration_load(&configuration);

	char input[1024];
	while (true)
	{
		print_help();

		printf("Your choice: ");
		int input_variables_count = scanf("%s", input);
		printf("\r\n");
		if (input_variables_count == 1)
		{
			if (strcmp(input, "u") == 0)
			{
				command_change_uart_name();
			}
			else if (strcmp(input, "q") == 0)
			{
				printf("Bye.\r\n");
				return 0;
			}
			else
			{
				command_send_command(input);
			}
		}

		printf("\r\n");

		wait_for_any_key();
	}
}

static void command_send_command(char *input)
{
	char *end;
	int cmd_ix = (int)strtol(input, &end, 10);
	if (input != end && cmd_ix > 0 && cmd_ix <= Commands_Count)
	{
		printf("> %s\r\n", Commands[(int)cmd_ix - 1]);

		int read_count;
		bool timeout;
		int read_time_us;
		bool receive_ok = send_and_receive_to_uart(configuration.uart_port_name, Commands[(int)cmd_ix - 1],
				strlen(Commands[(int)cmd_ix - 1]), read_buffer, Read_Buffer_Count, &read_count,
				&timeout, &read_time_us);
		if (receive_ok)
		{
			printf("< %s", read_buffer);
			printf("(+%.1f ms)\r\n", read_time_us / 1000.0);
		}
		else if (!receive_ok && timeout)
		{
			printf("< (read timeout +%.1f ms)\r\n", read_time_us / 1000.0);
		}
	}
}

static void command_change_uart_name(void)
{
	char input[1024];
	printf("UART name: ");
	if (scanf("%s", input) > 0)
	{
		configuration_set_uart_name(&configuration, input);
		configuration_save(&configuration);
	}
}

static void print_help(void)
{
	printf("Press:\r\n");
	for (int i = 0; i < Commands_Count; i++)
	{
		printf("[%d] Send \"%s\"\r\n", i + 1, Commands[i]);
	}
	printf("[u] Set UART name - current: %s\r\n", configuration.uart_port_name);
	printf("[q] Quit program.\r\n");
}

static void wait_for_any_key(void)
{
#ifdef __unix__
	printf("Press ENTER to continue...");
	while (getchar() != '\n') ;
	getchar();
#else
	system("pause");
#endif
}

/*

	Compile and run:
make
	or in Visual Studio Code with extension "Makefile Tools":
Ctrl+Shift+B

gcc main.c uart_commands.c uart_port.c configuration.c -Wall -Wextra -std=gnu11 -g -Og -o build/imx_controller

	Connected Virtual Serial Ports with echo on Linux
socat -d -d pty,raw,echo=0 pty,raw,echo=0
cat < /dev/pts/1
echo "Test" > /dev/pts/2
# socat PTY,link=/dev/ttyS10 PTY,link=/dev/ttyS11

	tests:
// char result[1024] = { 0 };

// char s1[] = "add 123 a 432\r\n";
// if (parse_line(s1, result))
// 	printf("%s\r\n", result);

// char s2[] = "subtract 1234 4 30\r\n";
// if (parse_line(s2, result))
// 	printf("%s\r\n", result);

*/
