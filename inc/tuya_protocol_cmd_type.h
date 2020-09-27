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
 * Description : tuya_protocol_cmd_type.h
 * Author      : junlon2006@163.com
 * Date        : 2020.09.27
 *
 **************************************************************************/
#ifndef TUYA_PROTOCOL_INC_TUYA_PROTOCOL_CMD_TYPE_H_
#define TUYA_PROTOCOL_INC_TUYA_PROTOCOL_CMD_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HEART_BEAT         = 0x00,
    PRODUCT_VERSION    = 0x01,
    WORK_MODE          = 0x02,
    NET_STATUS         = 0x03,
    NET_CONFIG         = 0x04,
    NET_CONFIG_MODE    = 0x05,
    DP_DOWNLOAD        = 0x06,
    DP_UPLOAD          = 0x07,
    DP_QUERY           = 0x08,
    START_OTA_MODE     = 0x0a,
    OTA_PACKET         = 0x0b,
    GREENWICH_TIME     = 0x0c,
    LOCAL_TIME         = 0x1c,
    SCAN_WIFI          = 0x0e,
    MEMORY_STATUS      = 0x0f,
    WEATHER_ABILITY_ON = 0x20,
    WEATHER_UPDATE     = 0x21,
    REPORT             = 0x22,
    WIFI_DBM           = 0x24,
    DISABLE_HEARTBEAT  = 0x25,
    WIFI_STATUS        = 0x2b,
    MAC_ADDRESS        = 0x2d,
    MIC_MUTE_SET       = 0x61,
    SPEAKER_VOLUME     = 0x62,
} tuya_cmd_word_t;

#ifdef __cplusplus
}
#endif
#endif // TUYA_PROTOCOL_INC_TUYA_PROTOCOL_CMD_TYPE_H_