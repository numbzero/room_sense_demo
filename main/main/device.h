#ifndef DEVICE_H
#define DEVICE_H

typedef struct  device_s {
    int         id;
    char        name[64];
    char        mac_addr[32];
    char        token[512];
    char        otp[32];
}               device_t;

device_t        esp32_device;

#endif
