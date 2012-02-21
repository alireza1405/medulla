// Kit Morton
//
//	atrias_leg.c
//	This program reads sensors and controls a motor for half of an ATRIAS 2.0
//	leg.
////////////////////////////////////////////////////////////////////////////////

#include <util/delay.h>

#include "medulla_controller.h"

/*
  These two structs define the data that is transferred between the computer
  and the medulla over the ethercat bus. Instead of defining these structs here
  it is more useful to define them somewhere that both the medulla build environment
  and the computer's build environment can find them. This way, as long as the
  compliers pack the structs the same way, data transfer should be transparent
*/
typedef struct
{
	uint8_t stateCommand;
} MedullaInput;

typedef struct
{
	uint8_t currentState;
} MedullaOutput;

// We define instances of the structs so we can use them in the program
MedullaInput in;
MedullaOutput out;

/*
  This is simply for keeping track of what the previous state was so we can
  do a printf when we switch states.
*/
MedullaState prevState;

/*
  This is where execution starts. All the main function needs to do is to call
  the medulla_run function to start off the medulla controller library.
*/
int main(void) {
	/*
	  Pointers to the input and output structs are passed to the medulla_run function.
	  This allows the medulla controller library to automatically pass the structs to
	  the computer over the ethercat bus.
	*/
	medulla_run((void*)(&in),(void*)(&out));
	
	// Since medulla_run contains the event loop, we will never return from main, but
	// we have to return something to be C compliant.
	return 0;
}

/*
  This function gets called during initilization of the medulla controller
  library and is a place to initilize any custom hardware before the event
  loop starts
*/
void init(void) {
	// Initilize the RGB leds on the medulla
	PORTC.DIRSET = 0b111;
	PORTC.OUTSET = 0b111;
	
	// Printf prints string messages to the debug header on the medulla at 115200 baud.
	printf("Initilizing\n");
	
	// Initilize the previous state variable
	prevState = INIT;
}
	
/*
  The updateInput funcion gets called by the medulla controller with the intension that
  this is where sensors will be read
*/
void updateInput(void) {
	printf("Updating");
}

/*
  This function is a helper function for the medulla controller. It takes in the current state
  of the medulla controller state machine and puts it into the output struct. This function needs
  to be implemented for medulla_controller to compile, but it is not neccesary that the function
  does anything.
*/
void setState(MedullaState state) {
	out.currentState = state;
}

/*
  This function returns the commanded state from the computer. It is implemented as a function so
  that the state command variable passed from the computer to the medulla can contain any desired values.
  This function is supposed to interpret the command from the computer and return a MedullaState.
*/
MedullaState getState(void) {
	return in.stateCommand;
}

/*
  The timerOverflow function gets called when the watchdog timer does not get fed. The medulla controller
  automatically asserts the e-stop output, this function should safely stop any hardware outputs and 
  put the medulla into a safe state.
*/
void timerOverflow(void) {
	printf("Overflow\n");

	PORTC.OUTCLR = 0b111;
	PORTC.OUTSET = 0b101;
	
	while(1) {		
		// If we tripped the watchdog timer, then we probably got stuck in a loop
		// so now that we tripped the estop, just stay in an infinite loop and wait
		// to get reset manually. We probably shouldn't recover from this.
	}
}

// **** State Machine ****

MedullaState state_idle(void) {
	if (prevState != IDLE)
		printf("IDLE\n");
	prevState = IDLE;
	
	// Update status LEDs
	PORTC.OUTCLR = 0b101;
	PORTC.OUTSET = 0b010;
	
	// Reset watchdog timer
	resetTimer(WATCHDOG_TIMER);
	
	return IDLE;;
}

MedullaState state_init(void) {
	if (prevState != INIT)
		printf("INIT\n");
	prevState = INIT;
	
	// Change indicator LED
	PORTC.OUTCLR = 0b001;
	PORTC.OUTSET = 0b110;
	
	// Reset the watchdog timer
	resetTimer(WATCHDOG_TIMER);

	return INIT;
}

MedullaState state_run(void) {
	if (prevState != RUN)
		printf("RUN\n");
	prevState = RUN;

	// Set indicator LEDs
	PORTC.OUTCLR = 0b011;
	PORTC.OUTSET = 0b100;
		
	// Reset watchdog timer
	resetTimer(WATCHDOG_TIMER);
	return RUN;
}

void state_stop(void) {	
	if (prevState != STOP)
		printf("STOP\n");
	prevState = STOP;
	
	// Update Status LEDs
	PORTC.OUTCLR = 0b001;
	PORTC.OUTSET = 0b110;
	
	// Reset Watchdog Timer
	resetTimer(WATCHDOG_TIMER);
}

MedullaState state_error_damping(void) {
	if (prevState != ERROR_DAMPING)
		printf("ERROR DAMPING\n");
	prevState = ERROR_DAMPING;
	
	// Update Status LEDs
	PORTC.OUTCLR = 0b010;
	PORTC.OUTSET = 0b101;
	
	// Reset Watchdog Timer
	resetTimer(WATCHDOG_TIMER);
	
	return ERROR_DAMPING;
}

MedullaState state_error(void) {
	if (prevState != ERROR)
		printf("ERROR\n");
	prevState = ERROR;
	
	// Reset watchdog timer
	resetTimer(WATCHDOG_TIMER);
	
	// Update Status LEDs
	PORTC.OUTCLR = 0b110;
	PORTC.OUTSET = 0b001;

	return ERROR;
}
