#define IMU_RAW_DATA_CMD 0xC1
#define IMU_ORIENTATION_DATA_CMD 0xC8

#define IMU_RX_BUFFER_LENGTH 67
#define IMU_TX_BUFFER_LENGTH 12

typedef struct{
	uint8_t tx_buffer[IMU_TX_BUFFER_LENGTH];
	uint8_t rx_buffer[IMU_RX_BUFFER_LENGTH];
	uart_port_t uart_port;
}IMU_data_t;

void initIMU(PORT_t *port, USART_t *uart, IMU_data_t *imu_data);

void IMURequestOrientation(IMU_data_t * imu_data);

void IMUReceiveOrientation(IMU_data_t * imu_data);
