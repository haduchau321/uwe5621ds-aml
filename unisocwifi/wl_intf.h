/*
 * Copyright (C) 2015 Spreadtrum Communications Inc.
 *
 * Authors	:
 * Keguang Zhang <keguang.zhang@spreadtrum.com>
 * Jingxiang Li <Jingxiang.li@spreadtrum.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __WL_INTF_H__
#define __WL_INTF_H__

#include <linux/types.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/cpufreq.h>
#include "wl_core.h"
#include <wcn_bus.h>

#define HW_TYPE_SDIO 0
#define HW_TYPE_PCIE 1
#define HW_TYPE_SIPC 2
#define HW_TYPE_USB 3

#define SDIO_RX_CMD_PORT	22
#define SDIO_RX_PKT_LOG_PORT	23
/*use port 24 because fifo_len = 8*/
#define SDIO_RX_DATA_PORT	24
#define SDIO_TX_CMD_PORT	8
/*use port 10 because fifo_len = 8*/
#define SDIO_TX_DATA_PORT	10

#define PCIE_RX_CMD_PORT	22
#define PCIE_RX_DATA_PORT	23
#define PCIE_TX_CMD_PORT	2
#define PCIE_TX_DATA_PORT	3

#define USB_RX_CMD_PORT	20
#define USB_RX_PKT_LOG_PORT	21
#define USB_RX_DATA_PORT	22
#define USB_TX_CMD_PORT	4
#define USB_TX_DATA_PORT	6

#define MSDU_DSCR_RSVD	5

#define DEL_LUT_INDEX 0
#define ADD_LUT_INDEX 1
#define UPD_LUT_INDEX 2

#define BOOST_TXNUM_LEVEL	16
#define BOOST_RXNUM_LEVEL	16

#ifdef SPRDWL_TX_SELF
#include <linux/pm.h>
struct sprdwl_tx_buf {
    unsigned char   *base;
    unsigned short  buf_len;
    unsigned short  curpos;
    int change_size;
};
#endif

#define MAX_CHN_NUM 16
struct sprdwl_intf_ops {
    unsigned int max_num;
    void *intf;
    struct mchn_ops_t *hif_ops;
};

struct sdiohal_puh {
    unsigned int pad:6;
    unsigned int check_sum:1;
    unsigned int len:16;
    unsigned int eof:1;
    unsigned int subtype:4;
    unsigned int type:4;
}; /* 32bits public header */

#endif /* __WL_INTF_H__ */
