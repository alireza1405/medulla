// Kevin Kemper
//
////////////////////////////////////////////////////////////////////////////////

#define DEBUG

#define ENABLE_LIMITS
#define ENABLE_TC_STEP


// delay.h needs to know how fast the clk is rolling along at
#define F_CPU 32000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "../libs/menial_io.h"

#include "../libs/uart.h" 
#include "../libs/ecat.h"
#include "../libs/clock.h"


#include "../libs/limitSW.h"
#include "../libs/timer.h"
#include "../libs/led.h"



// state machine states
#define STATE_READY		0
#define STATE_START		1
#define STATE_RUN		2
#define STATE_ERROR		4
#define STATE_RESET		5




// map the 0th usart on port E to the RS232 keyword for readability
#ifdef DEBUG
#define RS232	USARTE0
#endif



#ifdef DEBUG
void		printMenu();
void		printState(uint8_t state);
static int	uart_putchar(char c, FILE *stream);
#endif

// global vars...
#ifdef DEBUG
static FILE mystdout = FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);
#endif


////////////////////////////////////////////////////////////////////////////////

int main(void) {

	uint8_t		i;
	uint8_t		tmp8;
	uint16_t	tmp16;

	uint16_t	tmp_cnt16 	= 0;
	
	uControllerInput	in;
	uControllerOutput	out;
	

	uint16_t		ssi[4];
	uint8_t			status;
	uint8_t			toggle = CMD_RUN_TOGGLE_bm;
	
	// rs232 input data
	#ifdef DEBUG
	uint8_t			rs_data;
	#endif
	
	// eCat vars - Don't touch unless you know what you're doing!
	uint8_t			data[512];

	uint8_t			al_status	= 0;
	uint16_t		al_event	= 0;
	
	uint16_t		SM0_addr	= 0;
	uint16_t		SM1_addr	= 0;
	uint16_t		SM2_addr	= 0;
	uint16_t		SM3_addr	= 0;
	
	uint16_t		SM2_len		= 0;
	uint16_t		SM3_len		= 0;

	////////////////////////////////////////////////////////////////////////////
	// init stuffs
	CLK.PSCTRL = 0x00;															// no division on peripheral clock
	Config32MHzClock();


	cli();																		// Disable interrupts

	_delay_ms(1000);															// Wait a sec to let everything rattle out and settle down
	
	#ifdef DEBUG
	stdout = &mystdout;															// Attach uart_putchar to the stdout for printf
	initUART(&PORTE,&RS232,1152);												// RS232 uart 115.2kb
	#endif


	initLED();
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;												// Enable midlevel interrupts
	sei();																		// Global enable interrupts
	
	led_solid_red();
	

	initTimer();
	
	#ifdef ENABLE_LIMITS
	initLimitSW();
	#endif

	initeCAT();

	
	// init the output values
	out.id				= 0xFC;
	out.status			= 0;
	out.enc32			= 0;
	out.enc16[0]		= 0;
	out.enc16[1]		= 0;
	out.enc16[2]		= 0;
	out.enc16[3]		= 0;
	out.timestep		= 0;
	out.therm1			= 0;
	out.therm2			= 0;
	out.therm3			= 0;
	
	in.motor			= 0;
	in.command			= CMD_DISABLE;

//	PORTC.DIRSET = 0x07;
//	PORTC.OUT = 0x05;

	// init the global struct
	global_flags.status		= 0;
	global_flags.limits		= 0;
	global_flags.state		= STATE_ERROR;
	global_flags.error_cnt	= 0;
	
	_delay_ms(100);

	////////////////////////////////////////////////////////////////////////////
	// init eCat stuff

	#ifdef DEBUG
	printf("\t\tmedulla.1.5: Grizzle Test : %s\n\n",__DATE__);	// print what program we're running...
	#endif
	

	while(!(PORT_ECAT.IN) & (1<<ECAT_EEPROM_LOADED));							// The EEPROM_LOADED signal indicates that the SPI interface is operational.
	
	#ifdef DEBUG
	printf("eeprom loaded\n");
	#endif
	
	
	al_event |= readAddr(AL_STATUS, &al_status, 1);								// check to see what the et1100 is doing so we're on the same page
	
	if (al_status == AL_STATUS_OP_bm) {											// if the eCAT is in OP, grab and store the SM addresses:


		al_event |= readAddr(SM0_BASE, data, 2);
		SM0_addr = *((uint16_t*)(data+SM_PHY_ADDR));

		al_event |= readAddr(SM1_BASE, data, 2);
		SM1_addr = *((uint16_t*)(data+SM_PHY_ADDR));
	
		al_event |= readAddr(SM2_BASE, data, 4);
		SM2_addr	= *((uint16_t*)(data+SM_PHY_ADDR));
		SM2_len		= *((uint16_t*)(data+SM_LENGTH));

		al_event |= readAddr(SM3_BASE, data, 4);
		SM3_addr	= *((uint16_t*)(data+SM_PHY_ADDR));
		SM3_len		= *((uint16_t*)(data+SM_LENGTH));

		#ifdef DEBUG
		printf("SM2_len: 0x%X, SM3_len: 0x%X\n",SM2_len,SM3_len);
		#endif
	}
	
	
	// Enable the interrupts
	PMIC.CTRL |= PMIC_HILVLEN_bm;
//	PMIC.CTRL |= PMIC_MEDLVLEN_bm;
	PMIC.CTRL |= PMIC_LOLVLEN_bm;


	limitDis();
	


	while(1) {

//XXX: this is a hack but should be ok now that we have a step timer...
		_delay_us(8);



		#if 1
		if (RS232.STATUS&USART_RXCIF_bm) {										// do we have something from the RS232?
			rs_data = RS232.DATA;

			switch (rs_data) {
		
				case '~':
					global_flags.state = STATE_RESET;
					break;
		
				case 'v':
					printf("\t\tmedulla.1.5: Grizzle Test : %s\n\n",__DATE__);
					break;
		
				case 'o':
					printf("Out:\n");
					printf("out.status:			0x%.2X\n", out.status);
					printf("out.timestep:			%u\n", out.timestep);
//					printf("out.TRANS_ANGLE:		%u\n", out.TRANS_ANGLE);
//					printf("out.LEG_SEG_ANGLE:		%u\n", out.LEG_SEG_ANGLE);
//					printf("out.ROTOR_ANGLE:		%u\n", out.ROTOR_ANGLE);
//					printf("out.LEG_COURSE_ANGLE:		%u\n", out.LEG_COURSE_ANGLE);
					break;

				case 'g':
					printf("global_flags.state: %u\n",global_flags.state);
					break;

				case 'i':
					printf("In size: %u\n",sizeof(in));
					printf("in.command:	%u\n", in.command);
					printf("in.motor:	%u\n", in.motor);
					break;


				case 'l':
					printf("lim: 0x%.2X\n", (uint8_t)(~PORT_LIMIT.IN) );
					break;
					
				case '0':
				case '1':
				case '2':
				case '3':
					if (rs_data == '0') {
						printf("SM0: ");
						al_event |= readAddr(SM0_BASE, data, 8);
					}
					else if (rs_data == '1') {
						printf("SM1: ");
						al_event |= readAddr(SM1_BASE, data, 8);
					}
					else if (rs_data == '2') {
						printf("SM2: ");
						al_event |= readAddr(SM2_BASE, data, 8);
					}
					else if (rs_data == '3') {
						printf("SM3: ");
						al_event |= readAddr(SM3_BASE, data, 8);
					}

					printf("	Physical Start Address:	0x%.4X\n",*((uint16_t*)(data+SM_PHY_ADDR)));
					printf("	Length: 0x%.4X/%d\n",*((uint16_t*)(data+SM_LENGTH)),*((uint16_t*)(data+SM_LENGTH)));
					printf("	Control Register: 0x%.2X\n",*(data+SM_CONTROL));
					printf("	Status Register: 0x%.2X\n",*(data+SM_STATUS));
					printf("	Activate: 0x%.2X\n",*(data+SM_ACTIVE));
					printf("	PDI Control: 0x%.2X\n",*(data+SM_PDI_CTRL));

					break;
					
				case 'w':
				
					if (al_status != AL_STATUS_OP_bm) { printf("eCAT not in OP... so no write for you!\n"); break;}
					
//					printf("\tWriting %d to SM3 at 0x%X\n",ssi[0],SM3_addr);
//					al_event |= writeAddr(SM3_addr, dummy, SM3_len);
					
					break;

				case 'R':
					if (al_status != AL_STATUS_OP_bm) { printf("eCAT not in OP... so no read for you!\n"); break;}


					al_event = readAddr(SM0_addr, data, (uint16_t)512);

					
					printf("Read from SM0 at 0x%X:\n",SM0_addr);
					
					for(i=0;i<16;i++) {
						printf("0x%.2X\t", *(data+i));
					}
					printf("\n");
					break;
					
				case 'r':
					if (al_status != AL_STATUS_OP_bm) { printf("eCAT not in OP... so no read for you!\n"); break;}

//					if (SM2_len <= 8)
//						al_event = readAddr(SM2_addr, data, SM2_len);
//					else
//						al_event = readAddr(SM2_addr, data, 8);
					
					printf("Read from SM2 at 0x%X:\n",SM2_addr);
					
//					printf("Ch1: 0x%.2X Ch2: 0x%.4X",in.ch1, in.ch2);
					printf("\n");

					break;
					
				default:
//					printMenu();
					break;
			
			}
		}
		#endif


		// update timestamp
		out.timestep			= TC_STEP.CNT;


		////////////////////////////////////////////////////////////////////////
		switch (global_flags.state) {

////////////////////////////////////////////////////////////////////////////////
			case STATE_READY :

				_delay_us(100);

				led_solid_red();
	
//XXX			
				if ((al_status != AL_STATUS_OP_bm)) {
					global_flags.state = STATE_ERROR;
				}
				else {

					global_flags.state = STATE_START;//XXX STATE_START
					
					// reset stuff
					global_flags.status		= 0;
					global_flags.error_cnt	= 0;
					

					#ifdef DEBUG
					printf("START\n");
					#endif
				}
					
				break;
				
////////////////////////////////////////////////////////////////////////////////
			case STATE_START :
			
				led_solid_orange();
				
				
				limitEn();
				
				global_flags.status		= 0;
				out.status				= 0;
						
				global_flags.state = STATE_RUN;
				tmp_cnt16 = 0;
				
				
				#ifdef DEBUG
				printf("RUN\n");
				#endif

				
				break;
			
////////////////////////////////////////////////////////////////////////////////
			case STATE_RUN :
				

				if ((al_status == AL_STATUS_OP_bm)) {							// Stuff to do when eCAT is in OP mode
					
					// Deal with commands from the master
					if ( ( in.command & (~CMD_RUN_TOGGLE_bm)) == CMD_RUN ) {
					
						#ifdef ENABLE_TC_STEP
						tc_Start();
						#endif
						
						out.status		&= ~STATUS_DISABLED;
						
	
						// Play with the LED colors
//						tmp16 = (((uint16_t*)&out.LEG_SEG_ANGLE)[1]);
//						printf("%u\n",tmp16);
//						tmp16 = tmp16-LEG_MIN16;
//						tmp16 = tmp16*5 + TRAN_MIN;	// translate the leg angle into the same space as the transmission position
//						tmp16 = ((int16_t)(tran_pos - tmp16)<0)?(tmp16-tran_pos):(tran_pos-tmp16);
//						tmp16 = 0x0000;
//						printf("\t%u\n",tmp16);
						solidLED();
						SetDutyR( 0x00FF );
						SetDutyG( 0x7FFF );
						SetDutyB( 0x00FF );
						
						
					}
					else if (in.command == CMD_DISABLE) {
						
						out.status |= STATUS_DISABLED;
						
						led_solid_white();

					}
					else {														//XXX if we get any other cmd then we should report bad_cmd and move to error?
//						global_flags.status |= STATUS_BADCMD;
//						out.status |= STATUS_BADCMD;
					}

					
				}
				else {															// Stuff to do when eCAT is NOT in OP mode
					
					tc_Stop();
					
					led_solid_purple();
					
				}


				break;

//////////////////////////////////
			case STATE_RESET :
			
			
				#ifdef DEBUG
				printf("\n\nRESET: GOING DOWN!!!\n\n");
				#endif
				
				_delay_ms(10);
					
				CCP			= CCP_IOREG_gc;
				RST.CTRL	= RST_SWRST_bm;
				break;

//////////////////////////////////
			case STATE_ERROR :
			default :

				limitDis();
				tc_Stop();

				// set the color based on the type of problem
				if ( out.status == STATUS_BADMOTOR )
					led_blink_dark_purple();
				else if ( out.status == STATUS_LIMITSW)
					led_blink_orange();
				else if ( out.status == STATUS_TCOVF )
					led_blink_blue();
				else
					led_blink_red();

				if ((al_status == AL_STATUS_OP_bm)) {								// Stuff to do when eCAT is in OP mode
					
					// Deal with commands from the master
					if (in.command == CMD_RESTART) {
						global_flags.state	= STATE_READY;
						
						#ifdef DEBUG
						printf("READY\n");
						#endif
					}
				}

				break;
				
		}



		////////////////////////////////////////////////////////////////////////
		// Manage any status changes caused by interrupts
		if (global_flags.status != 0) {
			
			if (global_flags.status & STATUS_LIMITSW) {
				printf("Limit SW: %X\n",global_flags.limits);
				global_flags.state		= STATE_ERROR;
			}
					
			if (global_flags.status & STATUS_TCOVF) {
				printf("TOV\n");
				global_flags.state		= STATE_ERROR;
			}
			
			if (global_flags.status & STATUS_BADCMD) {
				printf("Bad cmd: 0x%.2X\n", in.command);
				global_flags.state		= STATE_ERROR;
			}

			out.status			   |= global_flags.status;
			global_flags.status		= 0;
		}
		
		
		
		
		////////////////////////////////////////////////////////////////////////
		// Manage EtherCAT stuff

		if (al_status == AL_STATUS_OP_bm) {										// if we're in OP mode, update the txpdo on the ET1100
			al_event |= writeAddr(SM3_addr, &out  , SM3_len);
		}
		else {																	// else keep updating al_event
			al_event |= eCATnop();
		}
		
		if (al_event & 0x01) {													// if al_control needs checking...
			#if 0
			printf("AL_Event Request:	0x%2X\n",al_event);
			#endif

			al_event = readAddr(AL_CONTROL, &al_status, 1);
			writeAddr(AL_STATUS, &al_status, 1);

			#if 0
			printf("\t");
			printState(al_status);
			printf("\n");
			#endif
			
			if (al_status == AL_STATUS_OP_bm) {									// store SM addresses:
				
				al_event |= readAddr(SM0_BASE, data, 2);
				SM0_addr = *((uint16_t*)(data+SM_PHY_ADDR));

				al_event |= readAddr(SM1_BASE, data, 2);
				SM1_addr = *((uint16_t*)(data+SM_PHY_ADDR));
				
				al_event |= readAddr(SM2_BASE, data, 4);
				SM2_addr	= *((uint16_t*)(data+SM_PHY_ADDR));
				SM2_len		= *((uint16_t*)(data+SM_LENGTH));

				al_event |= readAddr(SM3_BASE, data, 4);
				SM3_addr	= *((uint16_t*)(data+SM_PHY_ADDR));
				SM3_len		= *((uint16_t*)(data+SM_LENGTH));		

//				printf("SM0 addr: 0x%X\n",SM0_addr);
//				printf("SM1 addr: 0x%X\n",SM1_addr);
//				printf("SM2 addr: 0x%X\n",SM2_addr);				
//				printf("SM3 addr: 0x%X\n",SM3_addr);
			}

			
		}
		
		if (al_event & 0xFF00) {												// if there's a SM interrupt pending...

			if (al_event & 0x0400) {											// SM2...
				al_event |= readAddr(SM2_BASE+SM_STATUS, data, 1);
				
				if (*data & 0x01) {												// stuff wrtten to the buffer!
					al_event = readAddr(SM2_addr, &in, SM2_len);


					if (in.command == CMD_RESET) {
						#ifdef DEBUG
						printf("CMD_RESET\n");
						#endif
						global_flags.state = STATE_RESET;
					}
					else if (in.command == CMD_RESTART) {
					
						#ifdef ENABLE_TC_STEP
						tc_Stop();
						#endif
						
						#ifdef DEBUG
						printf("CMD_RESTART\n");
						#endif

						toggle			= CMD_RUN_TOGGLE_bm;

					}
					else if (in.command == CMD_DISABLE) {
					
						#ifdef ENABLE_TC_STEP
						tc_Stop();
						#endif

//						out.status		|= STATUS_DISABLED;
						toggle			= CMD_RUN_TOGGLE_bm;


					}
					else {
//						#ifdef DEBUG
//						printf("CMD_BAD: 0x%.2X\n", in.command);
//						#endif
						global_flags.status |= STATUS_BADCMD;
						out.status |= STATUS_BADCMD;
					}
					
					// reset the time step counter
					#ifdef ENABLE_TC_STEP
					TC_STEP.CNT = 0;
					#endif
					
				}
				
			}
			else if (al_event & 0x0800) {										// SM3...
				al_event |= readAddr(SM3_BASE+SM_STATUS, data, 1);
			}
			else {
				//ignore
			}
		
		}
		
		#ifndef ENABLE_TC_STEP
		TC_STEP.CNT = 0;
		#endif
	
	} // end while
} // end main

////////////////////////////////////////////////////////////////////////////////
// Helper functions...




#ifdef DEBUG
// print the options available to the user
void printMenu () {
	printf("\n");
	printf("Current state: %u\n", global_flags.state);
	printf("Menu Options:\n");
	printf("~: Software Reset\n");
	printf("o: Out values\n");
	printf("\n");
	printf("0: SyncManager0 info\n");
	printf("1: SyncManager1 info\n");
	printf("2: SyncManager2 info\n");
	printf("3: SyncManager3 info\n");
	printf("\n");
	printf("R: Read from SM0\n");
	printf("r: Read from SM2\n");
	printf("\n");
}


// translate the byte code "state" into something meaningfull for printing
void printState(uint8_t state) {

	switch (state) {
		case AL_STATUS_INIT_bm:
			printf("init");
			break;
		case AL_STATUS_PREOP_bm:
			printf("pre-op");
			break;
		case AL_STATUS_SAFEOP_bm:
			printf("safe op");
			break;
		case AL_STATUS_OP_bm:
			printf("op");
			break;
		default:
			printf("OMG!!!");
	}
}

// function to attach to stdout to the usart
static int uart_putchar (char c, FILE *stream) {

    if (c == '\n')
        uart_putchar('\r', stream);
 
    // Wait for the transmit buffer to be empty
    while ( !( RS232.STATUS & USART_DREIF_bm) );
 
    // Put our character into the transmit buffer
    RS232.DATA = c; 
 
    return 0;
}
#endif

