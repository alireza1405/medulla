// Devin Koepl, Kevin Kemper

#ifndef FUNCS_H_UCONTROLLER
#define FUNCS_H_UCONTROLLER

// Medulla IDs
#define MEDULLA_FCA_ID 				0xFC

// Command bytes.
// During normal running, the controller should switch between sending the two run mode bytes
// as its command byte at each timestep.  We can implement watchdog timers to detect if the
// control computer is operating correctly.  CMD_BAD is a place-holder so the value is never used.
#define CMD_BAD						0
#define CMD_RESTART					(1<<0)
#define CMD_DISABLE					(1<<1)
#define CMD_RUN						(1<<2)
#define CMD_RESET					(1<<3)
#define CMD_RUN_TOGGLE_bm			(1<<6)
#define CMD_RUN_OK_bm				(1<<7)


// Status bits.
//   If any of the bits in the status byte are set, there is an error in the uController.
//   The following #defines describe the bit locations of each possible error.
#define STATUS_LIMITSW				(1<<7)	// A limit switch was hit.
#define STATUS_TCOVF				(1<<6)	// The step timer rolled over.
#define STATUS_BADPWM				(1<<5)	// The PWM command was out of range.
#define STATUS_ENC					(1<<4)	// There was a problem wit1h the encoders.
#define STATUS_BADCMD				(1<<3)	// The command received was invalid, or the toggle bit was wrong.
#define STATUS_DISABLED				(1<<2)	// Everything is OK, but the uController is in a disabled state
#define STATUS_DANGER				(1<<1)	// The uC is taking control of the motor
#define STATUS_BADMOTOR				(1<<0)	// The motor didn't move as expected

/*
 * Sensor values:
*/
#define TRANS_ANGLE 				enc16[0]
#define LEG_SEG_ANGLE				enc16[1]
#define ROTOR_ANGLE					enc16[2]
#define LEG_COURSE_ANGLE			enc16[3]


#define MOTOR_TORQUE				motor

// Medulla A sensors:
#define MIN_LEG_SEG_COURSE_COUNT	6098
#define MAX_LEG_SEG_COURSE_COUNT	9144

//TODO: the angles are total junk
#define MIN_LEG_SEG_COUNT			7080
#define MAX_LEG_SEG_COUNT			21594
#define MIN_LEG_SEG_ANGLE			3.70
#define MAX_LEG_SEG_ANGLE			1.47

#define MIN_TRAN_COUNT			2388
#define MAX_TRAN_COUNT			15560
#define MIN_TRAN_ANGLE			1.5707
#define MAX_TRAN_ANGLE			3.6


// These are the rough ranges of motion for the zero force leg segments.
//  E.g. they are the leg angle counts when the transmission is at the limits
//  and the spring is uncompressed.
#define MIN_TRAN_SEG_COUNT		6290
#define MAX_TRAN_SEG_COUNT		8945


// Need to set these during sensor calibration if you want the spring deflections
// to be computed accurately.  Add these offsets to the transmission angle.
//#define TRAN_OFF_ANGLE			-0.028043331476324
#define TRAN_OFF_ANGLE			0.



// For motors
#define MTR_DIR_bm					(1<<15)
#define MTR_MIN_TRQ					-19.3
#define MTR_MAX_TRQ  				19.3

#define MTR_MIN_CNT					100
#define MTR_MAX_CNT 				19900
#define MTR_ZERO_CNT				10000

// This should only be just enough torque to hold the leg up.
#define AMPS_OPEN					1.1
#define PWM_OPEN					((uint16_t)(MTR_ZERO_CNT + (AMPS_OPEN / (MTR_MAX_TRQ/(MTR_ZERO_CNT))) ))


// For handling rollover of encoders
#define ROLLOVER13BIT_THRESHOLD		4096
#define MAX_13BIT					8192
#define ROLLOVER16BIT_THRESHOLD		32768
#define MAX_16BIT					65536

#define SEC_PER_CNT					125E-9

// For GCC running on a 32-bit machine to minimize the size in memory of these structs, the smallest
// values must be last.  This is important for compatability with the avr-gcc compiled code.
typedef struct 
{
	uint16_t	motor;

	uint8_t		command;
} uControllerInput;

typedef struct
{
	uint32_t 	enc32;

	uint16_t	enc16[4];

	uint16_t 	timestep;

	uint8_t		id;
	uint8_t		status;	
	
	uint8_t		therm1;
	uint8_t		therm2;
	uint8_t		therm3;
} uControllerOutput;

#endif // FUNCS_H_UCONTROLLER
