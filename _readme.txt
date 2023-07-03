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
