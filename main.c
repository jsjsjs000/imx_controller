#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart_commands.h"

void print_help(void);

int main(void)
{
	printf("i.MX Cortex-M7 controller.\r\n\r\n");

	char input[1024];
	while (true)
	{
		print_help();
		// char ch = getchar();
		// fflush(stdin);
		// if (ch == '1')
		// 	printf("a");
		// else if (ch == '2')
		// 	printf("b");

		int x = scanf("%s", input);
		printf("x %d\r\n", x);
		if (x == 1)
		{
			printf("%s", input);
		}
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
	printf("[1] Send \"add 111 9 -20\"\r\n");
	printf("[2] Send \"add 111 a 9\"\r\n");
}

/*

	Compile and run:
make
	or in Visual Studio Code with extension "Makefile Tools":
Ctrl+Shift+B

*/
