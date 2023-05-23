
typedef struct configuration_t
{
	char *uart_port_name;
} configuration_t;

extern void configuration_initialize(configuration_t *cfg);
extern bool configuration_load(configuration_t *cfg);
extern bool configuration_save(configuration_t *cfg);
extern void configuration_set_uart_name(configuration_t *cfg, char *uart_name);
