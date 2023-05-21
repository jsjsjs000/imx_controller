#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "uart_commands.h"

const int commands_count = 2;
const char *commands[] = { "add 111 9 -20", "add 111 a 9" };

void print_help(void);
void wait_for_any_key(void);

int main(void)
{
	printf("i.MX Cortex-M7 controller.\r\n\r\n");

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

			}
			else if (strcmp(input, "q") == 0)
			{
				printf("Bye.\r\n");
				return 0;
			}
			else
			{
				char *end;
				int cmd_ix = (int)strtol(input, &end, 10);
				if (input != end && cmd_ix > 0 && cmd_ix <= commands_count)
				{
					printf("> %s\r\n", commands[(int)cmd_ix - 1]);
					printf("< 120 (+10 ms)\r\n");
					//printf("< (+20 ms - timeout - no answer)\r\n");
				}
			}
		}

		printf("\r\n");

		wait_for_any_key();
	}

	// char result[1024] = { 0 };

	// char s1[] = "add 123 a 432\r\n";
	// if (parse_line(s1, result))
	// 	printf("%s\r\n", result);

	// char s2[] = "subtract 1234 4 30\r\n";
	// if (parse_line(s2, result))
	// 	printf("%s\r\n", result);

	return 0;
}

void print_help(void)
{
	printf("Press:\r\n");
	for (int i = 0; i < commands_count; i++)
	{
		printf("[%d] Send \"%s\"\r\n", i + 1, commands[i]);
	}
	printf("[u] Set UART name - current: /dev/ttyUSB1\r\n");
	printf("[q] Quit program.\r\n");
}

void wait_for_any_key(void)
{
#ifdef __unix__
	system("read");
#else
	system("pause");
#endif
}

/*

	to do:
- time counting
- port name configuration to config file

	Compile and run:
make
	or in Visual Studio Code with extension "Makefile Tools":
Ctrl+Shift+B

gcc main.c uart_commands.c -Wall -Wextra -std=gnu11 -g -Og -o build/imx_controller

*/
