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
#ifndef TUYA_PROTOCOL_INC_TUYA_PROTOCOL_H_
#define TUYA_PROTOCOL_INC_TUYA_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_protocol_cmd_type.h"
#include <stdint.h>

typedef void (*send_data)(uint8_t *data, uint32_t bytes_len);
typedef void (*user_data)(uint8_t *data, uint32_t bytes_len, uint8_t cmd);

typedef struct {
    void* (*malloc_fn)(unsigned long size);             /**< malloc hook */
    void  (*free_fn)(void *ptr);                        /**< free hook */
    void* (*realloc_fn)(void *ptr, unsigned long size); /**< realloc hook */
} tuya_protocol_hooks_t;

/**
 * @brief     协议栈依赖的可移植函数，需要根据系统实际情况，进行注册
 * @param[in] hooks 注册函数指针集结构体
 * @return    void
 */
void TuyaProtocolStackRegisterHooks(tuya_protocol_hooks_t *hooks);

/**
 * @brief     协议栈初始化函数
 * @param[in] send_handler 函数指针，用于注册发送函数到协议栈中
 * @param[in] user_handler  协议栈解析出数据后，回调给应用层
 * @return    错误码，0代表成功，-1代表失败
 */
int TuyaProtocolStackInit(send_data send_handler, user_data user_handler);

/**
 * @brief     协议栈入口函数
 * @param[in] data 接收到的数据buffer
 * @param[in] bytes_len 接收到的数据长度
 * @return    void
 */
void TuyaProtocolStackInput(uint8_t *data, uint16_t bytes_len);

/**
 * @brief                 协议栈发送消息函数
 * @param[in] cmd         发送消息的类型，该类型是全局唯一的，标识一个消息类型
 * @param[in] version     版本号
 * @param[in] payload     cmd对应的消息包含的参数，如没有参数则设置为NULL
 * @param[in] payload_len cmd对应的消息参数的长度，如果没有参数设置为0
 * @return                错误码，0代表成功，-1代表失败
 */
int TuyaProtocolStackOutput(uint8_t cmd, uint8_t version, uint8_t *payload, uint16_t payload_len);

#ifdef __cplusplus
}
#endif
#endif // TUYA_PROTOCOL_INC_TUYA_PROTOCOL_H_