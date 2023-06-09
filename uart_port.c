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

static const int TIMEOUT_MS = 50;

static int fuart = 0;
static bool uart_port_set_parameters(int f);

bool open_uart(char *uart_port_name)
{
	fuart = open(uart_port_name, O_RDWR); // | O_NOCTTY
	if (fuart < 0)
	{
		printf("Can't open port %s\r\n", uart_port_name);
		perror(uart_port_name);
		printf("if you don't have permissions to %s type it in terminal:\r\n", uart_port_name);
		printf("sudo gpasswd --add ${USER} dialout\r\n");
		printf("and relogin.\r\n");
		return false;
	}

	if (!uart_port_set_parameters(fuart))
	{
		printf("Error set UART parameters.\r\n");
		perror(uart_port_name);
		close(fuart);
		return false;
	}

	return true;
}

void close_uart(void)
{
	close(fuart);
}

bool send_and_receive_to_uart(const char *send, int send_count, char *read_buffer,
		int read_buffer_size, int *read_count, bool *timeout, int *read_time_us)
{
	*read_count = 0;
	*timeout = false;
	*read_time_us = 0;

		// write to UART
	int wrote_count = 0;
	do
	{
		int wrote_count_ = write(fuart, &send[wrote_count], send_count);
		if (wrote_count_ < 0)
		{
			printf("Can't write to UART\r\n");
			perror(NULL);
			return false;
		}
		wrote_count += wrote_count_;
	}
	while (wrote_count < send_count);
	
	if (write(fuart, "\r\n", 2) < 0)
	{
		printf("Can't write to UART\r\n");
		perror(NULL);
		return false;
	}

		// read from UART
	struct timeval start_tv, end_tv;
	gettimeofday(&start_tv, NULL);

	// int read_available;
	do
	{
		// ioctl(f, FIONREAD, &read_available);
		// if (read_available > 0)
		{
			// if (*read_count + read_available >= read_buffer_size)
			// {
			// 	printf("UART try read buffer overlow (%d bytes).\r\n", read_buffer_size);
			// 	return false;
			// }

			int read_count_ = read(fuart, &read_buffer[*read_count], read_buffer_size - *read_count);
			if (read_count_ < 0)
			{
				printf("Can't read from UART\r\n");
				perror(NULL);
				return false;
			}
			else if (read_count_ > 0)
			{
				*read_count += read_count_;
				if (*read_count >= read_buffer_size)
				{
					printf("UART try read buffer overlow (%d bytes).\r\n", read_buffer_size);
					return false;
				}
			}
			else
			{
				usleep(1000);
			}
		}
		// else
		// {
		// 	usleep(1000);
		// }

		gettimeofday(&end_tv, NULL);
		*read_time_us = (end_tv.tv_sec - start_tv.tv_sec) * 1000000 +		// in microseconds
				(end_tv.tv_usec - start_tv.tv_usec);
		if (*read_time_us >= TIMEOUT_MS * 1000)
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

	tty.c_cflag &=  ~PARENB;            // Make 8n1
	tty.c_cflag &=  ~CSTOPB;
	tty.c_cflag &=  ~CSIZE;
	tty.c_cflag |=  CS8;

	tty.c_cflag     &=  ~CRTSCTS;           // no flow control
	tty.c_cc[VMIN]   =  0;
	tty.c_cc[VTIME]  =  0;
	tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO;    // Disable echo
  tty.c_lflag &= ~ECHOE;   // Disable erasure
  tty.c_lflag &= ~ECHONL;  // Disable new-line echo
  tty.c_lflag &= ~ISIG;    // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

	tcflush(f, TCIFLUSH);
	return (tcsetattr(f, TCSANOW, &tty) != 0) ? false : true;
}

/*

	UART:
https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#getting-the-number-of-rx-bytes-available

*/
