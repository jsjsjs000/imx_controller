#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "configuration.h"

const char *Config_Filename = "configuration.cfg";

bool configuration_load(configuration_t *cfg)
{
	int f = open(Config_Filename, O_RDONLY);
	if (f < 0)
	{
		return false;
	}

	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	int read_count = read(f, buffer, sizeof(buffer));
	if (read_count < 0)
	{
		close(f);
		return false;
	}

	if (read_count > 0)
	{
		char *s = calloc(1, read_count + 1);
		strncpy(s, buffer, read_count);
		cfg->uart_port_name = s;
	}

	close(f);
	return true;
}

bool configuration_save(configuration_t *cfg)
{
	//int f = open(Config_Filename, O_RDWR);//O_CREAT | O_TRUNC | O_RDWR); // $$
	int f = creat(Config_Filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (f < 0)
	{
		return false;
	}

	if (write(f, cfg->uart_port_name, strlen(cfg->uart_port_name)) < 0)
	{
		close(f);
		return false;
	}

	close(f);
	return true;
}
