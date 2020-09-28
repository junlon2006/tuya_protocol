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
 * Description : main.c
 * Author      : junlon2006@163.com
 * Date        : 2020.09.28
 *
 **************************************************************************/
#include "tuya_protocol.h"
#include "uni_log.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define TAG "main"

static void _send_data(uint8_t *data, uint32_t bytes_len) {
    int i;
    LOGR(TAG, "send data. ");
    for (i = 0; i < bytes_len; i++) {
        LOGR(TAG, "%0x,", data[i]);
    }
    LOGR(TAG, "\n");
}

static void _user_data(uint8_t *data, uint32_t bytes_len, uint8_t cmd) {
    int i;
    LOGR(TAG, "recv cmd[%0x] ", cmd);
    for (i = 0; i < bytes_len; i++) {
        LOGR(TAG, "%0x,", data[i]);
    }
    LOGR(TAG, "\n");
}


int main() {
    tuya_protocol_hooks_t hooks = {.malloc_fn = malloc, .free_fn = free, .realloc_fn = realloc};
    TuyaProtocolStackRegisterHooks(&hooks);
    TuyaProtocolStackInit(_send_data, _user_data);

    while (1) {
        //推送协议
        uint8_t heartbeat[2] = {0x01, 0x03};
        TuyaProtocolStackOutput(HEART_BEAT, 3, heartbeat, sizeof(heartbeat));

        //协议解析
        uint8_t recv_bytes[] = {0x55, 0xaa, 0x3, 0x0, 0x0, 0x2, 0x1, 0x3, 0x8};
        TuyaProtocolStackInput(recv_bytes, sizeof(recv_bytes));
        usleep(1000 * 1);
    }

    return 0;
}