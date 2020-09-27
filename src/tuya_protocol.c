/**************************************************************************
 * Copyright (C) 2020-2020  Junlon2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **************************************************************************
 *
 * Description : tuya_protocol.h
 * Author      : junlon2006@163.com
 * Date        : 2020.09.27
 *
 **************************************************************************/
#include "tuya_protocol.h"
#include "uni_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAG "tuya_stack"

#define SYNC_WORD1  0x55
#define SYNC_WORD2  0xaa
#define PACKED      __attribute__ ((packed))

typedef struct {
    uint8_t  sync1;      //同步头，SYNC_WORD1
    uint8_t  sync2;      //同步头，SYNC_WORD2
    uint8_t  version;    //版本号
    uint8_t  cmd;        //控制命令
    uint16_t payload_len;//payload长度
    uint8_t  payload[0]; //payload，注最后一位为校验和
} PACKED tuya_protocol_packet_t;

typedef struct {
    send_data on_send;
    user_data on_user;
} tuya_business_t;

static tuya_protocol_hooks_t g_hooks = {.malloc_fn = malloc, .free_fn = free};
static tuya_business_t       g_business = {0};

void TuyaProtocolStackRegisterHooks(tuya_protocol_hooks_t *hooks) {
    if (NULL == hooks) {
        return;
    }

    g_hooks.malloc_fn = hooks->malloc_fn;
    g_hooks.free_fn   = hooks->free_fn;
}

int TuyaProtocolStackInit(send_data send_handler, user_data user_handler) {
    g_business.on_send = send_handler;
    g_business.on_user = user_handler;
    return 0;
}

static void _protocol_buffer_generate_byte_by_byte(unsigned char recv_c) {
    
}

void TuyaProtocolStackInput(uint8_t *data, uint32_t bytes_len) {
    int i;
    for (i = 0; i < bytes_len; i++) {
        _protocol_buffer_generate_byte_by_byte(data[i]);
    }
}

int TuyaProtocolStackOutput(uint8_t cmd, uint8_t *payload, uint32_t payload_len) {
    return 0;
}

int main() {
    return 0;
}