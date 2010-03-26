#ifndef _ECACHED_SETTINGS_H_
#define _ECACHED_SETTINGS_H_


#define	DEFAULT_PORT		12345
#define	DEFAULT_ADDRESS		"0.0.0.0"
#define	DEFAULT_MAX_MEM		128
#define	DEFAULT_MAX_CONN	1000
#define DEFAULT_VERBOSITY	1
#define DEFAULT_PID_FILE	"/var/run/ecached.pid"
#define DEFAULT_THREADS		false


typedef struct ecached_settings {
    int			port;
    char		*address;
    uint64_t		max_mem;
    uint32_t		max_conn;
    uint8_t		verbosity;
    char		*pid_file;
    bool		threads;
} *ecached_settings_t;


#endif /* !_ECACHED_SETTINGS_H_ */
