#ifndef IMU_H
#define IMU_H

#include <stdbool.h>
#include "uart.h"

#define IMU_RAW_DATA_CMD 0xC1
#define IMU_ORIENTATION_DATA_CMD 0xC8

#define IMU_RX_BUFFER_LENGTH 67
#define IMU_TX_BUFFER_LENGTH 12

typedef struct{
	uint8_t tx_buffer[IMU_TX_BUFFER_LENGTH];
	uint8_t rx_buffer[IMU_RX_BUFFER_LENGTH];
	uart_port_t uart_port;
	uint8_t *imu_data_buffer;
}IMU_data_t;

void init_IMU(PORT_t *port, USART_t *uart, IMU_data_t *imu_data, uint8_t *imu_data_buffer);

void IMU_request_orientation(IMU_data_t * imu_data);

bool IMU_received_orientation(IMU_data_t * imu_data);

void IMU_process_orientation(IMU_data_t * imu_data);

#endif //IMU_H
