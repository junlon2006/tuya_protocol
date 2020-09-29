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
#include <stdbool.h>

#define TAG         "tuya_stack"
#define PACKED      __attribute__ ((packed))

typedef struct {
    uint8_t  sync1;          //同步头，SYNC_WORD1
    uint8_t  sync2;          //同步头，SYNC_WORD2
    uint8_t  version;        //版本号
    uint8_t  cmd;            //控制命令
    uint8_t  payload_h_8_len;//payload长度高8位
    uint8_t  payload_l_8_len;//payload长度低8位
    uint8_t  payload[0];     //payload，注最后一位为校验和
} PACKED tuya_protocol_packet_t;

typedef struct {
    send_data on_send;
    user_data on_user;
    bool      init;
} tuya_business_t;

static tuya_protocol_hooks_t g_hooks    = {.malloc_fn = malloc, .free_fn = free, .realloc_fn = realloc};
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
    g_business.init    = true;
    LOGT(TAG, "tuya stack init success");
    return 0;
}

static uint16_t _get_payload_len(tuya_protocol_packet_t *packet) {
    return ((uint16_t)packet->payload_h_8_len << 8) + (uint16_t)packet->payload_l_8_len;
}

static void _crc_calc(tuya_protocol_packet_t *packet) {
    uint16_t crc_idx = _get_payload_len(packet);
    uint16_t crc_payload_len = crc_idx + sizeof(tuya_protocol_packet_t);
    uint8_t *p = (uint8_t *)packet;
    int i;

    packet->payload[crc_idx] = 0;
    for (i = 0; i < crc_payload_len; i++) {
        packet->payload[crc_idx] += p[i];
    }
}

static bool _is_crc_valid(tuya_protocol_packet_t *packet) {
    uint16_t crc_idx = _get_payload_len(packet);
    uint8_t crc = packet->payload[crc_idx];
    _crc_calc(packet);
    return (crc == packet->payload[crc_idx]);
}

static void _one_pcb_done(uint8_t *pcb) {
    tuya_protocol_packet_t *packet = (tuya_protocol_packet_t *)pcb;
    if (!_is_crc_valid(packet)) {
        LOGW(TAG, "crc invalid");
        return;
    }

    if (NULL == g_business.on_user) {
        LOGE(TAG, "not register user hook");
        return;
    }

    g_business.on_user(packet->payload, _get_payload_len(packet), packet->cmd);
}

#define SYNC_WORD1                  (0x55)
#define SYNC_WORD2                  (0xaa)
#define DEFAULT_PROTOCOL_BUF_SIZE   (16)
#define LAYOUT_SYNC_IDX             (2)
#define LAYOUT_PAYLOAD_LEN_HIGH_IDX (4)
#define LAYOUT_PAYLOAD_LEN_LOW_IDX  (5)
#define REST_STACK()                do { length = 1; idx = 0;} while (0)
static void _protocol_buffer_generate_byte_by_byte(unsigned char recv_c) {
    static uint8_t *protocol_packet_buffer = NULL;
    static uint16_t protocol_packet_buffer_len = DEFAULT_PROTOCOL_BUF_SIZE;
    static uint16_t idx = 0;
    static uint16_t length = 1; //crc存储在payload最后一位，故length最少为1字节

    if (NULL == protocol_packet_buffer) {
        protocol_packet_buffer = g_hooks.malloc_fn(protocol_packet_buffer_len);
    }

    if (idx >= protocol_packet_buffer_len) {
        //每次追加DEFAULT_PROTOCOL_BUF_SIZE，保留最长帧，永不GC，因为payload不可能超过256字节，小内存常驻
        protocol_packet_buffer_len += DEFAULT_PROTOCOL_BUF_SIZE;
        protocol_packet_buffer = g_hooks.realloc_fn(protocol_packet_buffer, protocol_packet_buffer_len);
    }

    if (idx < LAYOUT_SYNC_IDX) {
        if (idx == 0 && recv_c == SYNC_WORD1) { //同步头1
            protocol_packet_buffer[idx++] = recv_c;
        } else if (idx == 1 && recv_c == SYNC_WORD2) { //同步头2
            protocol_packet_buffer[idx++] = recv_c;
        } else {
            REST_STACK();
        }
        return;
    }

    if (LAYOUT_PAYLOAD_LEN_HIGH_IDX == idx) {
        length += ((unsigned short)recv_c) << 8;
        if (length > 257) {
            LOGE(TAG, "payload too long, must be error");
            REST_STACK();
            return;
        }
        goto L_HEADER;
    }

    if (LAYOUT_PAYLOAD_LEN_LOW_IDX == idx) {
        length += recv_c;
        LOGT(TAG, "length=%d", length);
        goto L_HEADER;
    }

L_HEADER:
    if (idx < sizeof(tuya_protocol_packet_t)) {
        protocol_packet_buffer[idx++] = recv_c;
        goto L_END;
    }

    if (sizeof(tuya_protocol_packet_t) <= idx && 0 < length) {
        protocol_packet_buffer[idx++] = recv_c;
        length--;
    }

L_END:
    if (sizeof(tuya_protocol_packet_t) <= idx && 0 == length) {
        _one_pcb_done(protocol_packet_buffer);
        REST_STACK();
    }
}

void TuyaProtocolStackInput(uint8_t *data, uint16_t bytes_len) {
    int i;
    if (!g_business.init) return;
    for (i = 0; i < bytes_len; i++) {
        _protocol_buffer_generate_byte_by_byte(data[i]);
    }
}

int TuyaProtocolStackOutput(uint8_t cmd, uint8_t version, uint8_t *payload, uint16_t payload_len) {
    uint16_t len = sizeof(tuya_protocol_packet_t) + payload_len + 1;
    tuya_protocol_packet_t *packet = g_hooks.malloc_fn(len);
    if (NULL == packet) {
        LOGE(TAG, "alloc memory failed");
        return -1;
    }

    if (payload_len > 256) {
        LOGE(TAG, "payload too long, cannot > 256 bytes");
        return -1;
    }

    packet->sync1           = SYNC_WORD1;
    packet->sync2           = SYNC_WORD2;
    packet->version         = version;
    packet->cmd             = cmd;
    packet->payload_h_8_len = (payload_len >> 8) & 0xff;
    packet->payload_l_8_len = payload_len & 0xff;
    if (payload && payload_len > 0) {
        memcpy(packet->payload, payload, payload_len);
    }
    _crc_calc(packet);

    if (NULL == g_business.on_send) {
        g_hooks.free_fn(packet);
        LOGE(TAG, "send hook not register");
        return -1;
    }

    //donot handler multi-thread send
    g_business.on_send((uint8_t *)packet, len);
    g_hooks.free_fn(packet);
    return 0;
}
