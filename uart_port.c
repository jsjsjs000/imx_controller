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

static const int Timeout = 50;

static bool uart_port_set_parameters(int f);

bool send_and_receive_to_uart(char *uart_port_name, const char *send, int send_count,
		char *read_buffer, int read_buffer_size, int *read_count, bool *timeout, int *read_time_us)
{
	*read_count = 0;
	*timeout = false;
	*read_time_us = 0;

	int f = open(uart_port_name, O_RDWR | O_NOCTTY);
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
		printf("Error set UART parameters.\r\n");
		perror(uart_port_name);
		close(f);
		return false;
	}

		// write to UART
	int wrote_count = 0;
	do
	{
		int wrote_count_ = write(f, &send[wrote_count], 1);
		if (wrote_count_ < 0)
		{
			printf("Can't write to %s", uart_port_name);
			perror(uart_port_name);
			close(f);
			return false;
		}
		wrote_count += wrote_count_;
	}
	while (wrote_count < send_count);
	
	if (write(f, "\r\n", 2) < 0)
	{
		printf("Can't write to %s", uart_port_name);
		perror(uart_port_name);
		close(f);
		return false;
	}

		// read from UART
	struct timeval start_tv, end_tv;
	gettimeofday(&start_tv, NULL);

	int read_available;
	do
	{
		ioctl(f, FIONREAD, &read_available);
		if (read_available > 0)
		{
			if (*read_count + read_available >= read_buffer_size)
			{
				printf("UART try read buffer overlow (%d bytes).", read_buffer_size);
				close(f);
				return false;
			}

			int read_count_ = read(f, &read_buffer[*read_count], read_available);
			if (read_count_ < 0)
			{
				printf("Can't read from %s", uart_port_name);
				perror(uart_port_name);
				close(f);
				return false;
			}
			else if (read_count_ > 0)
			{
				*read_count += read_count_;
			}
		}
		else
		{
			usleep(1000);
		}

		gettimeofday(&end_tv, NULL);
		*read_time_us = (end_tv.tv_sec - start_tv.tv_sec) * 1000000 +		// in microseconds
				(end_tv.tv_usec - start_tv.tv_usec);
		if (*read_time_us >= Timeout * 1000)
		{
			*timeout = true;
			break;
		}
	}
	while (read_buffer[*read_count - 1] != '\n');
	read_buffer[*read_count] = 0;

	gettimeofday(&end_tv, NULL);

	*read_time_us = (end_tv.tv_sec - start_tv.tv_sec) * 1000000 +		// in microseconds
			(end_tv.tv_usec - start_tv.tv_usec);

	close(f);
	return !(*timeout);
}

static bool uart_port_set_parameters(int f)
{
	struct termios tty;
	memset(&tty, 0, sizeof(tty));
	if (tcgetattr(f, &tty) != 0)
	{
		return false;
	}

	cfsetospeed(&tty, (speed_t)B115200);
	cfsetispeed(&tty, (speed_t)B115200);

	tty.c_cflag     &=  ~PARENB;            // Make 8n1
	tty.c_cflag     &=  ~CSTOPB;
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS8;

	tty.c_cflag     &=  ~CRTSCTS;           // no flow control
	tty.c_cc[VMIN]   =  0;
	tty.c_cc[VTIME]  =  0;
	tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

	cfmakeraw(&tty);

	tcflush(f, TCIFLUSH);
	if (tcsetattr(f, TCSANOW, &tty) != 0)
	{
		return false;
	}

	return true;
}

/*

	UART:
https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#getting-the-number-of-rx-bytes-available

*/
