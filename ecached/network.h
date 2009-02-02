#ifndef _ECACHED_NETWORK_H_
#define _ECACHED_NETWORK_H_


typedef enum {
    COMMAND_SET,
    COMMAND_ADD,
    COMMAND_REPLACE,
    COMMAND_APPEND,
    COMMAND_PREPEND,
    COMMAND_CAS,
    COMMAND_GET,
    COMMAND_GETS
} command_t;

typedef enum {
    CONNECTION_ACCEPTED,
    CONNECTION_PARSED_REQUEST,

} connection_state_t;


typedef struct network_data_bucket {
    int         fd;
    size_t      offset;
} network_data_bucket_t;

typedef struct network_data {
    size_t			size;
    uint16_t			refcnt;
    uint16_t			buckets;
    network_data_bucket_t	data[];
} network_data_t;


#endif _ECACHED_NETWORK_H_
