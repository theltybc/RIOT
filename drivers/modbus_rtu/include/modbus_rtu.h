/*
 * Copyright (C) 2020 Denis Litvinov <li.denis.iv@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_modbus ModBus
 * @ingroup     drivers
 * @brief       Modbus
 *
 *              To use function need defined corresponding macro:
 *              `MODBUS_RTU_USE_READ_COILS`,
 *              `MODBUS_RTU_USE_READ_DISCRETE_INPUT`,
 *              `MODBUS_RTU_USE_READ_REGISTERS`,
 *              `MODBUS_RTU_USE_READ_INPUT_REGISTER`,
 *              `MODBUS_RTU_USE_WRITE_COIL`,
 *              `MODBUS_RTU_USE_WRITE_REGISTER`,
 *              `MODBUS_RTU_USE_WRITE_COILS`,
 *              `MODBUS_RTU_USE_WRITE_REGISTERS`,
 *              or `MODBUS_RTU_USE_ALL`
 * 
 * @{
 * @file
 * @brief       Modbus interface definitions
 *
 * @author      Denis Litvinov <li.denis.iv@gmail.com>
 */

#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/uart.h"
#include "thread.h"
#include "mutex.h"

/**
 * @brief   Modbus functions
 */
enum {
    MB_FC_NONE                  = 0,    /**< null operator */
    MB_FC_READ_COILS            = 1,    /**< read coils or digital outputs */
    MB_FC_READ_DISCRETE_INPUT   = 2,    /**< read digital inputs */
    MB_FC_READ_REGISTERS        = 3,    /**< read registers or analog outputs */
    MB_FC_READ_INPUT_REGISTER   = 4,    /**< read analog inputs */
    MB_FC_WRITE_COIL            = 5,    /**< write single coil or output */
    MB_FC_WRITE_REGISTER        = 6,    /**< write single register */
    MB_FC_WRITE_COILS           = 15,   /**< write multiple coils or outputs */
    MB_FC_WRITE_REGISTERS       = 16    /**< write multiple registers */
};

#ifdef MODBUS_RTU_USE_ALL
#define MODBUS_RTU_USE_READ_COILS
#define MODBUS_RTU_USE_READ_DISCRETE_INPUT
#define MODBUS_RTU_USE_READ_REGISTERS
#define MODBUS_RTU_USE_READ_INPUT_REGISTER
#define MODBUS_RTU_USE_WRITE_COIL
#define MODBUS_RTU_USE_WRITE_REGISTER
#define MODBUS_RTU_USE_WRITE_COILS
#define MODBUS_RTU_USE_WRITE_REGISTERS
#endif /* MODBUS_RTU_USE_ALL */

/**
 * @brief   Modbus error
 */
enum {
    MB_ER_NONE              = 0,
    MB_ER_ILLEGAL_FUNCTION  = 1,
    MB_ER_ILLEGAL_ADDRESS,
    MB_ER_ILLEGAL_VALUE,
    MB_ER_SERVER_FAILURE,
    MB_ER_ACKNOWLEDGE,
    MB_ER_SERVER_BUSY,
    MB_ER_MEMORY_PARITY_ERROR,
    /* non standard */
    MB_ER_TIMEOUT   = 250,
    MB_ER_CRC       = 251,
    MB_ER_INVAL_ID  = 252
};

#define MODBUS_RTU_PACKET_SIZE_MAX 256 /**< max size packet */

/**
 * @brief   Modbus message structure.
 */
typedef struct {
    uint8_t id;         /**< ID of slave, on response not use */
    uint8_t func;       /**< function code */
    uint16_t addr;      /**< Starting Address */
    uint16_t count;     /**< Quantity of Registers or byte or bits */
    uint16_t *data;     /**< Registers or byte or bits*/
    uint16_t data_size; /**< size of data in byte*/
} modbus_rtu_message_t;

/**
 * @brief   Modbus structure.
 */
typedef struct {
    /**
     * @brief   RTS pin. GPIO_UNDEF if not use.
     */
    gpio_t pin_rts;
    /**
     * @brief   RTS signals when transmit.
     */
    int pin_rts_enable;
    /**
     * @brief   Before slave begin to response; usec.
     */
    uint32_t timeout;
    /**
     * @brief   Timeout beetwin dyte. **Must never be changed by the user.**
     * @internal
     */
    uint32_t rx_timeout;
    /**
     * @brief   pointer on current message. **Must never be changed by the user.**
     * @internal
     */
    modbus_rtu_message_t *msg;
    /**
     * @brief   Mutex for block buffer. **Must never be changed by the user.**
     * @internal
     */
    mutex_t mutex_buffer;
    /**
     * @brief   UART port.
     */
    uart_t uart;
    /**
     * @brief   PID thread. **Must never be changed by the user.**
     * @internal
     */
    kernel_pid_t pid;
    /**
     * @brief   ID of the device, 0 is reserved for master.
     */
    uint8_t id;
    /**
     * @brief   Buffer size. **Must never be changed by the user.**
     * @internal
     */
    uint8_t size_buffer;
    /**
     * @brief   Template buffer for receive message or send. **Must never be changed
     *          by the user.**
     * @internal
     */
    uint8_t buffer[MODBUS_RTU_PACKET_SIZE_MAX];
} modbus_rtu_t;

/**
 * @brief   Initialize the given modbus.
 *
 * Initialize fields of modbus and UART, RTS pin.
 *
 * @param[in] modbus    pointer modbus
 * @param[in] baudrate  desired baudrate in baud/s
 *
 * @return              0 on success, otherwise error
 */
int modbus_rtu_init(modbus_rtu_t *modbus, uint32_t baudrate);

/**
 * @brief   Send request to slave.
 *
 * Send request to slave, blocking and wait response
 * if necessary write in data field.
 *
 * @param[in] modbus    pointer modbus
 * @param[in] message   pointer modbus message
 *
 * @return              0 on success, otherwise error
 */
int modbus_rtu_send_request(modbus_rtu_t *modbus,
                            modbus_rtu_message_t *message);

/**
 * @brief   Wait request from master.
 *
 * Wait request from master, blocking.
 * Request will be written to message.
 * Use as slave only.
 *
 * @param[in] modbus    pointer to modbus
 * @param[in] message   pointer to modbus message
 *
 * @return              0 on success, otherwise error
 */
int modbus_rtu_poll(modbus_rtu_t *modbus, modbus_rtu_message_t *message);

/**
 * @brief   Send response to master.
 *
 * Response must be writeт to message.
 * Use only slave.
 * id, func, addr, count - must be set correctly, or not change if it been use in modbus_rtu_send_request
 *
 * @param[in] modbus    pointer modbus
 * @param[in] message   pointer modbus message
 *
 * @return              0 on success, otherwise error
 */
int modbus_rtu_send_response(modbus_rtu_t *modbus,
                             modbus_rtu_message_t *message);

/**
 * @brief   Copy number of bits from src to dst.
 *
 * @param[in] dst            destination
 * @param[in] start_bit_dst  the first bit will be copied to
 * @param[in] src            source
 * @param[in] start_bit_src  the first bit will be copied from
 * @param[in] number         number of bits
 */
void modbus_rtu_copy_bits(uint8_t *dst, uint16_t start_bit_dst,
                          const uint8_t *src, uint16_t start_bit_src,
                          uint16_t number);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_RTU_H */
/** @} */
