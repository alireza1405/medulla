#define IMU_RAW_DATA_CMD 0xC1

void IMUReadRaw(USART_t *usart, char *data);
void initIMU(PORT_t *port, USART_t *usart);
void IMUStopContinuous(USART_t *usart);
void IMUStartContinuous(USART_t *usart);
void IMUModePresetActive(USART_t *usart);
void IMUBaudSet(USART_t *usart, int baud);
void IMUGetContinuousPacket(USART_t *usart, char *data);


