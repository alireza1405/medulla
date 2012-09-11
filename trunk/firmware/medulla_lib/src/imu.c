#include "uart.h"
#include "imu.h"

//Based upon Information from Single Byte Command Protocol Datasheet
//for 3DM-GX3-25

void init_IMU(PORT_t *port, USART_t *uart, IMU_data_t *imu_data, uint8_t *imu_data_buffer){

	//Same as regular UART, needs 460800 baud
	imu_data->uart_port = uart_init_port(port, uart, 0xD01B, imu_data->tx_buffer, IMU_TX_BUFFER_LENGTH, imu_data->rx_buffer, IMU_RX_BUFFER_LENGTH);

	uart_connect_port(&(imu_data->uart_port), 0);
	imu_data->imu_data_buffer = imu_data_buffer;
}

void IMU_request_orientation(IMU_data_t * imu_data){
	(imu_data->tx_buffer)[0] = IMU_ORIENTATION_DATA_CMD;
	uart_tx_data(&(imu_data->uart_port), imu_data->tx_buffer, 1);
}

bool IMU_received_orientation(IMU_data_t * imu_data){
	return (uart_received_bytes(&(imu_data->uart_port)) >= 67);
}

void IMU_process_orientation(IMU_data_t * imu_data){
	if (IMU_received_orientation(imu_data) == false)
		return;

	uart_rx_data(&(imu_data->uart_port),imu_data->imu_data_buffer,64);
}

