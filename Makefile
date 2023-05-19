CC=gcc
CFLAGS=-Wall -Wextra -std=gnu11 -g -Og

all: clean build run

clean:
	@rm -rf build

build:
	@mkdir -p build
	@$(CC) main.c uart_commands.c $(CFLAGS) -o build/imx_controller

run:
	@cd build && ./imx_controller
