#include "uart.h"
#include "imu.h"
#include "avr_compiler.h"

//Based upon Information from Single Byte Command Protocol Datasheet
//for 3DM-GX3-25

void initIMU(PORT_t *port, USART_t *usart){
	//Same as regular UART, needs 115200 baud
	initUART(port, usart, 4608);
}

void IMUReadRaw(USART_t *usart, char *data){
	int i;
	//Send command byte
	UARTWriteChar(usart, IMU_RAW_DATA_CMD);
	//Read all 31 bytes
	for(i=0;i<31;i++){
		data[i] = UARTReadChar(usart);
	}
}

//See datasheet Stop Continuous Mode command
void IMUStopContinuous(USART_t *usart){
	UARTWriteChar(usart, 0xFA);
	UARTWriteChar(usart, 0x75);
	UARTWriteChar(usart, 0xB4);
}

//Configures Continuous Mode for Acceleration, Angular Rate, and Orientation
//Output then begins Continous Mode
void IMUStartContinuous(USART_t *usart){
	//See continous mode Preset
	UARTWriteChar(usart, 0xD6);
	UARTWriteChar(usart, 0xC6);
	UARTWriteChar(usart, 0x6B);
	UARTWriteChar(usart, 0xC8); //Active mode setting

	//See datasheet Mode command
	UARTWriteChar(usart, 0xD4);
	UARTWriteChar(usart, 0xA3);
	UARTWriteChar(usart, 0x47);
	UARTWriteChar(usart, 0x02); //Active mode setting

}

//Sets power on to default to active mode, not continuous mode
//see datasheet Mode Preset Command
void IMUModePresetActive(USART_t *usart){
	UARTWriteChar(usart, 0xD5);
	UARTWriteChar(usart, 0xBA);
	UARTWriteChar(usart, 0x89);
	UARTWriteChar(usart, 0x01); //Active mode setting
}

//Changes Baurdrate to 460800 (4608) or 115200 (1152), probably will only be used to set default
//once from 115200, keep in mind that when changing baudrates, init command
//must also be changed to reflect the new baud setting
void IMUBaudSet(USART_t *usart, int baud){
	int i;
	UARTWriteChar(usart, 0xD9);
	UARTWriteChar(usart, 0xC3);
	UARTWriteChar(usart, 0x55);
	UARTWriteChar(usart, 0x01);//Sets To 1 and only uart output
	UARTWriteChar(usart, 0x02);//Sets this to write these Settings to nonvolatile memory, making them defaults (IMPORTANT)
	
//	if(baud == 4608){
	//460800 baudsetting over 4 bytes, 0b00000000/00000111/00001000/00000000
	//0x00, 0x07, 0x08, 0x00
		UARTWriteChar(usart, 0x00);
		UARTWriteChar(usart, 0x07);
		UARTWriteChar(usart, 0x08);
		UARTWriteChar(usart, 0x00);
//	}

	UARTWriteChar(usart, 0b00000010);
	UARTWriteChar(usart, 0);

	//Eat the response (maybe should verify this actually, but too lazy righ
	//now
	for(i=0;i<10;i++){
		UARTReadChar(usart);
	}
}

//For reading continous packets, will sync with next incoming packet (C1 starts
//the packet) then put the next 30 bytes into data to be passed along and parsed
//elsewhere
void IMUGetContinuousPacket(USART_t *usart, char *data){
	int i;
	while(UARTReadChar(usart) != 0xC1);
	for(i=0;i<30;i++){
		data[i] = UARTReadChar(usart);
	}
}
