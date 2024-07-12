#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <protocol.h>

#include <stdint.h>

void client_connect(uint16_t port);

RequestKind debug_control(void);

#endif // __CLIENT_H__
