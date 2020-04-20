#pragma once
#include "periph/uart.h"
#include "thread.h"

enum MB_FC {
  MB_FC_NONE = 0,                  /*!< null operator */
  MB_FC_READ_COILS = 1,            /*!< FCT=1 -> read coils or digital outputs */
  MB_FC_READ_DISCRETE_INPUT = 2,   /*!< FCT=2 -> read digital inputs */
  MB_FC_READ_REGISTERS = 3,        /*!< FCT=3 -> read registers or analog outputs */
  MB_FC_READ_INPUT_REGISTER = 4,   /*!< FCT=4 -> read analog inputs */
  MB_FC_WRITE_COIL = 5,            /*!< FCT=5 -> write single coil or output */
  MB_FC_WRITE_REGISTER = 6,        /*!< FCT=6 -> write single register */
  MB_FC_WRITE_MULTIPLE_COILS = 15, /*!< FCT=15 -> write multiple coils or outputs */
  MB_FC_WRITE_REGISTERS = 16       /*!< FCT=16 -> write multiple registers */
};

//! Максимальный размер пакета протокола Modbus RTU.
#define MODBUS_RTU_PACKET_SIZE_MAX 256

//! Размер контрольной суммы протокола Modbus RTU.
#define MODBUS_RTU_CRC_SIZE 2

//! Размер полей сообщения протокола Modbus RTU.
#define MODBUS_RTU_FIELDS_SIZE (sizeof(modbus_rtu_id_t) + sizeof(modbus_rtu_func_t))
//! Размер полей и контрольной суммы сообщения протокола Modbus RTU.
#define MODBUS_RTU_FIELDS_CRC_SIZE (MODBUS_RTU_CRC_SIZE + MODBUS_RTU_FIELDS_SIZE)
//! Максимальный размер данных протокола Modbus RTU.
#define MODBUS_RTU_DATA_SIZE_MAX (MODBUS_RTU_PACKET_SIZE_MAX - MODBUS_RTU_FIELDS_CRC_SIZE)
//! Максимальный размер данных и контрольной суммы протокола Modbus RTU.
#define MODBUS_RTU_DATA_CRC_SIZE_MAX (MODBUS_RTU_DATA_SIZE_MAX + MODBUS_RTU_CRC_SIZE)

typedef struct {
  uint8_t id;     // ID of slave, on response not use
  uint8_t func;   // function code
  uint16_t addr;  // Starting Address
  uint16_t count; // Quantity of Registers
  uint16_t *regs; // Registers
} modbus_rtu_message_t;

typedef struct {
  uart_t uart;
  uint32_t baudrate;
  gpio_t pin_tx;          // if 0 disable
  unsigned pin_tx_enable; // state when enabled
  uint8_t id;             // 0 - master, otherwice slave
  uint32_t timeout;       // while slave start response; us
  uint8_t _buffer[MODBUS_RTU_PACKET_SIZE_MAX];
  uint8_t _size_buffer;
  uint32_t _rx_timeout;       // beetwen byte; us
  modbus_rtu_message_t *_msg; // now process
  kernel_pid_t _pid;          // who get messege
} modbus_rtu_t;

int modbus_rtu_init(modbus_rtu_t *modbus);
int modbus_rtu_send_request(modbus_rtu_t *modbus, modbus_rtu_message_t *message);
int modbus_rtu_poll(modbus_rtu_t *modbus, modbus_rtu_message_t *message);
int modbus_rtu_send_response(modbus_rtu_t *modbus, modbus_rtu_message_t *message);
