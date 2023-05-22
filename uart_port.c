#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>

const int Read_Buffer_Count = 10240;

static char *uart_port_name = "/dev/ttyUSB1";

static bool uart_port_set_parameters(int f);

bool send_and_receive_to_uart(const char *send, int send_count)
{
	int f = open(uart_port_name, O_RDWR | O_NOCTTY); // $$ O_NONBLOCK
	if (f < 0)
	{
		printf("Can't open port %s", uart_port_name);
		perror(uart_port_name);
		printf("if you do'nt have permissions to %s type it in terminal:", uart_port_name);
		printf("sudo gpasswd --add ${USER} dialout");
		printf("and relogin.");
		return false;
	}

	if (!uart_port_set_parameters(f))
	{
		return false;
	}

	int wrote_count = 0;
	do
	{
		int wrote_count_ = write(f, &send[wrote_count], 1);
		if (wrote_count_ < 0)
		{
			printf("Can't write to %s", uart_port_name);
			perror(uart_port_name);
			return false;
		}
		wrote_count += wrote_count_;
	}
	while (wrote_count < send_count);
	
	if (write(f, "\r\n", 2) < 0)
	{
		printf("Can't write to %s", uart_port_name);
		perror(uart_port_name);
		return false;
	}

	long time_long;
	struct timeval start_tv, end_tv;
	gettimeofday(&start_tv, NULL);

	int read_count = 0;
	char read_buffer[Read_Buffer_Count];
	do
	{
		int read_count_ = read(f, &read_buffer[read_count], 1);
		if (read_count_ < 0)
		{
			printf("Can't read from %s", uart_port_name);
			perror(uart_port_name);
			return false;
		}
		else if (read_count_ > 0)
		{
			read_count += read_count_;
		}

		gettimeofday(&end_tv, NULL);
		time_long = (end_tv.tv_sec - start_tv.tv_sec) * 1000000 +		// in microseconds
				(end_tv.tv_usec - start_tv.tv_usec);
		if (time_long >= 50 * 1000)
		{
			break;
		}
	}
	while (read_buffer[read_count - 1] != '\n');
	read_buffer[read_count] = 0;

	gettimeofday(&end_tv, NULL);

printf("< %s", read_buffer);

	time_long = (end_tv.tv_sec - start_tv.tv_sec) * 1000000 +		// in microseconds
			(end_tv.tv_usec - start_tv.tv_usec);
	printf("(+%.1f ms)\r\n", time_long / 1000.0);

	close(f);
	return true;
}

static bool uart_port_set_parameters(int f)
{
	struct termios tty;
	memset(&tty, 0, sizeof(tty));
	if (tcgetattr(f, &tty) != 0)
	{
		printf("Error set UART parameters.\r\n");
		perror(uart_port_name);
		return false;
	}

	cfsetospeed(&tty, (speed_t)B115200);
	cfsetispeed(&tty, (speed_t)B115200);

	tty.c_cflag     &=  ~PARENB;            // Make 8n1
	tty.c_cflag     &=  ~CSTOPB;
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS8;

	tty.c_cflag     &=  ~CRTSCTS;           // no flow control
	tty.c_cc[VMIN]   =  1;                  // read doesn't block
	tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
	tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

	cfmakeraw(&tty);

	tcflush(f, TCIFLUSH);
	if (tcsetattr(f, TCSANOW, &tty) != 0)
	{
		printf("Error set UART parameters.\r\n");
		perror(uart_port_name);
		return false;
	}

	return true;
}
