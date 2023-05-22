CC=gcc
CFLAGS=-Wall -Wextra -std=gnu11 -g -Og
BUILD_PATH=build

ifdef OS
	FixPath = $(subst /,\,$1)
	REM = REM
else
	FixPath = $1
	REM = #
endif

ifdef __COMMENT__
	$(call FixPath,build)
endif

.PHONY: all
all: clean build run

.PHONY: clean
clean:
ifdef OS
	@if exist $(BUILD_PATH) rmdir /s /q $(BUILD_PATH)
else
	@rm -rf $(BUILD_PATH)
endif

.PHONY: build
build:
ifdef OS
	@if not exist $(BUILD_PATH) mkdir $(BUILD_PATH)
else
	@mkdir -p $(BUILD_PATH)
endif
	@$(CC) main.c uart_commands.c uart_port.c $(CFLAGS) -o build/imx_controller

.PHONY: run
run:
ifdef OS
	@cd build && imx_controller.exe
else
	@cd build && ./imx_controller
endif
