#include "uart.h"
#include "imu.h"
#include "avr_compiler.h"

//Based upon Information from Single Byte Command Protocol Datasheet
//for 3DM-GX3-25

void initIMU(PORT_t *port, USART_t *uart, IMU_data_t *imu_data){

	//Same as regular UART, needs 460800 baud
	imu_data->uart_port = uart_init_port(port, uart, 0xD01B, imu_data->tx_buffer, IMU_TX_BUFFER_LENGTH, imu_data->rx_buffer, IMU_RX_BUFFER_LENGTH);


	uart_connect_port(&(imu_data->uart_port), 0);
}

void IMURequestOrientation(IMU_data_t * imu_data){
	(imu_data->tx_buffer)[0] = IMU_ORIENTATION_DATA_CMD;
	uart_tx_data(&(imu_data->uart_port), imu_data->tx_buffer, 1);
}

void IMUReceiveOrientation(IMU_data_t * imu_data){
	uint8_t waiting_for = 67;
	while(waiting_for){
		waiting_for -= uart_rx_data(&(imu_data->uart_port), &(imu_data->current_data[67 - waiting_for]), waiting_for);
		}
}

