
typedef struct configuration_t
{
	char *uart_port_name;
} configuration_t;

extern bool configuration_load(configuration_t *cfg);
extern bool configuration_save(configuration_t *cfg);
