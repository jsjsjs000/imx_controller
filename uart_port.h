extern bool send_and_receive_to_uart(char *uart_port_name, const char *send, int send_count,
		char *read_buffer, int read_buffer_size, int *read_count, bool *timeout, int *read_time_us);
