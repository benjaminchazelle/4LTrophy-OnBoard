//
//
//

#define GB_MODE_OFF    0x00
#define GB_MODE_BRUSH  0x01
#define GB_MODE_DCC    0x02
#define GB_MODE_STEPG_OFF  0x08
#define GB_MODE_STEPP_OFF  0x09
#define GB_MODE_STEPG_PWR  0x18
#define GB_MODE_STEPP_PWR  0x19
#define GB_MODE_STEP_MASK  0x0C

#define GB_ENDSTOP_OFF    0
#define GB_ENDSTOP_LOW    1
#define GB_ENDSTOP_HIGH   2

#define GB_MOVE_STOP   0   
#define GB_MOVE_A      1
#define GB_MOVE_B      2

#define GB_RMAP_NONE   0
#define GB_RAMP_005    1  
#define GB_RAMP_012    2
#define GB_RAMP_025    3
#define GB_RAMP_037    4
#define GB_RAMP_050    5
#define GB_RAMP_075    6
#define GB_RAMP_100    7
#define GB_RAMP_125    8
#define GB_RAMP_150    9
#define GB_RAMP_175   10
#define GB_RAMP_200   11
#define GB_RAMP_250   12
#define GB_RAMP_300   13
#define GB_RAMP_400   14
#define GB_RAMP_500   15

#define GB_STOP_OFF    0
#define GB_STOP_ON     1

#define GB_PIN_SAME    0
#define GB_PIN_INPUT   1
#define GB_PIN_OUTPUT  2
#define GB_PIN_ENDSTOP 3
#define GB_PIN_ADC     4
#define GB_PIN_DAC     5
#define GB_PIN_I2C     6

// default range of a DAC
#define GB_DAC_MAX (float)2.7
#define GB_DAC_MIN (float)0.7

#ifdef _WIN32
  #include "gertbot_winserial.h"
#else
  #include "gertbot_pi_uart.h"
#endif


typedef struct {
  float min,max;
} dac_cal_type;

extern dac_cal_type dac_calibrate[4][2];

//
// Setup uart for 57600 baud
// 1 start, 1 stop no parity
// The 'port' is a comport number under windows
// It is currently unused on the Raspberry-Pi 
//
// return 1 on success
// return 0 on error 
int  open_connection(int port);

//
// Stops all motors on all boards
// Stepper motors keep the stator powerd
//
void stop_all();

//
// Remove power from all output on all boards
//
void emergency_stop();

//
// Setup an output on a board
//
// For mode use one off:
// GB_MODE_OFF             0
// GB_MODE_BRUSH           1
// GB_MODE_DCC             2
// GB_MODE_STEP_GRAY_OFF   8
// GB_MODE_STEP_PULSE_OFF  9
// GB_MODE_STEP_GRAY_PWR  16
// GB_MODE_STEP_PULSE_PWR 17
//
// An active output MUST have a frequency assigned
// so this routine also sets a default operating frequency
// Brushed motors are set to 5000 Hz.
// Stepper motors are set to  100 Hz.
//
// return 1 on success
// return 0 on error 
int set_channel_mode(int board,int channel,int mode);

//
// Set J3 pins as endstop
//
// For stop_A, stop_B use one off:
// GB_ENDSTOP_OFF    0
// GB_ENDSTOP_LOW    1  Stop if pin low
// GB_ENDSTOP_HIGH   2  Stop if pin high 
//
// return 1 on success
// return 0 on error 
int  set_endstop (int board,int channel,int stop_A,int stop_B);


//
// Moved brush motor
// Can also be used for arbitrary power outputs
// e.g. LEDs, relay etc.
//
// For direction use one off:
// GB_MOVE_STOP   0   
// GB_MOVE_A      1
// GB_MOVE_B      2
//
// Ramp up  
// For ramp use one off:
// GB_RMAP_NONE   0  no ramp
// GB_RAMP_005    1  0.05 sec.
// GB_RAMP_012    2  0.12 sec.
// GB_RAMP_025    3  0.25 sec.
// GB_RAMP_037    4  0.37 sec.
// GB_RAMP_050    5  0.50 sec.
// GB_RAMP_075    6  0.75 sec.
// GB_RAMP_100    7  1.00 sec.
// GB_RAMP_125    8  1.25 sec.
// GB_RAMP_150    9  1.50 sec.
// GB_RAMP_175   10  1.75 sec.
// GB_RAMP_200   11  2.00 sec.
// GB_RAMP_250   12  2.50 sec.
// GB_RAMP_300   13  3.00 sec.
// GB_RAMP_400   14  4.00 sec.
// GB_RAMP_500   15  5.00 sec.
//
// return 1 on success
// return 0 on error 
int  move_brushed(int board,int channel, int direction,int ramp);

//
// Set Brushed motor PWM mode
//
// Frequencye must be in range 10-10000 Hz
// Duty cycle is from 0-100% (accuracy is 0.1 %)
//
// return 1 on success
// return 0 on error 
int  pwm_brushed (int board, int channel, int frequency, float duty_cycle);

//
// Make stepper motor take steps
//
// steps > 0 : one direction
// steps = 0 : stop
// steps < 0 : other direction
// Step range is from -8388608..8388607
//
// return 1 on success
// return 0 on error 
int  move_stepper(int board,int channel, int steps);

//
// Stop stepper motor
// 
// For mode use one off:
// GB_STOP_OFF    0  Stop with no power on anchor
// GB_STOP_ON     1  Stop with power on anchor
//
// return 1 on success
// return 0 on error 
int  stop_stepper(int board,int channel, int mode);

//
// Set stepper motor step frequency
// Frequency must be in range from 1/16..5000
// (0.0625 .. 5000)
//
// return 1 on success
// return 0 on error 
int  freq_stepper(int board, int channel, float frequency);

//
// Read error status of board
// 
// return -1 data read error 
// return  0 : No error
// return  1..N: number of error
int  read_error_status(int board);


//
// Return error status of board
// (zero means no pending errors)
// 
const char *error_string(int error_number);

//
// Set J3 pin mode
//
// Mode is on off:
// GB_PIN_SAME    0
// GB_PIN_INPUT   1
// GB_PIN_OUTPUT  2
// GB_PIN_ENDSTOP 3
// GB_PIN_ADC     4
// GB_PIN_DAC     5
// GB_PIN_I2C     6
// Modes are very depending on the pin
//
// return 1 on success
// return 0 on command agrgument error 
// return -1 on pin is function
int  set_pin_mode(int board,int pin, int mode);

// Same as above but specify all pins
// *mode is updated with error status (if there is any) 
// return 1 on all successfull
// return 0 on any command argument error 
// return -1 on any pin is function
// int  set_pins_mode(int board,int *mode);#

//
// Activate the open drain outputs
// drain0,drain1 are interpreted as C booleans:
// ==0 means not active
// !=0  means active
// Beware that activate means the pin will go LOW
//
int activate_opendrain(int board,int drain0, int drain1);

//
// Set the output pins in a high/low state
// ALL outputs are set
// The function maps output bits 0..N to pins 0..N
// bit  0 : pin 1
// bit  1 : pin 2
// ..
// bit  7 : pin 8
// bit  8 : pin 9   extra0
// bit  9 : pin 10  extra1
// bit 10 : pin 13! ADC1
// bit 11 : pin 14! ADC0
// bit 12 : pin 15! ADC3
// bit 13 : pin 16! ADC2
// bit 14 : pin 18! DAC1
// bit 15 : pin 20! DAC0
// The board will ignore pins which are NOT an output
//
int set_output_pin_state(int board, int output);


//
// Set a DAC output value
// Do to limitation of the DAC only values from 
// x to y Volts are supported
//
int  set_dac(int board,int dac,float value);

//
// Read ADC value
// 
// a return value of -1 indicates an error 
//
float read_adc(int board,int adc);


//
// Send DCC message 
// Board is usual 0-3
// channel is special here
// The LS 4 bits indicat to which channel(s) to send
//  bin hex  dec
// 0001  01   1 -> send to chan. 0
// 0010  02   2 -> send to chan. 1
// 0100  04   4 -> send to chan. 2
// 1000  08   8 -> send to chan. 3
// e.g. 
// 1010  0A  10 -> send to chan. 0 & 2
//
// lenght is the number of  bytes to send
// lenght must be between 2 and 5 (inclusive)
// The check byte is not send but is calculated by the Gertbot
// and added there.
//
int send_dcc_mess(int board,int channel,int length, unsigned char *data);
