

#include "gb_drivers.h"

#include <stdint.h>
// typedef unsigned char uint8_t; // use this if the above include is not found 
#include "gertbot_defines.h"

//
// Get serial port code
// 
#ifdef _WIN32
#include "gertbot_winserial.h"
#else
#include "gertbot_pi_uart.h"
#endif

#include <stdio.h>

// switching checks off is dangerous but may make the code a tiny bit faster
#ifdef GB_NOCHECK
  #define GB_CHECK(x,y) 
  #define GB_CHECKN(x,y) 
#else
  #ifdef GB_CHECK_ASSERT
    #define GB_CHECK(x,y) assert(x)
    #define GB_CHECKN(x,y) assert(x)
  #else
    #include <stdio.h>
    #define GB_CHECK(x,y) if(!(x)) { fprintf(stderr,y); return 0; }
    #define GB_CHECKN(x,y) if(!(x)) { fprintf(stderr,y); return -1; }
  #endif
#endif    

static uint8_t gb_command5[] = { 0xA0,0x00,0x00,0x00,0x50};
static uint8_t gb_command6[] = { 0xA0,0x00,0x00,0x00,0x00,0x50};
static uint8_t gb_command7[] = { 0xA0,0x00,0x00,0x00,0x00,0x00,0x50};
static uint8_t gb_command10[]= { 0xA0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50};

// Keep track of the board status
static int gb_motor_mode[4][4]  = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// Keep track of pin status
// Most pins are a digital input except the ADC 
// (To keep the code readable I did NOT replace each 1 with GB_PIN_INPUT)
static int gb_pin_mode[4][20]   = {
 { 1,1,1,1,1,1,1,1, 1,1, 0,0, GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC, 0, 1, 0, 1},
 { 1,1,1,1,1,1,1,1, 1,1, 0,0, GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC, 0, 1, 0, 1},
 { 1,1,1,1,1,1,1,1, 1,1, 0,0, GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC, 0, 1, 0, 1},
 { 1,1,1,1,1,1,1,1, 1,1, 0,0, GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC, 0, 1, 0, 1}
 };

// Which end stop is high [board][channel*2+(B?1:0)]
static int end_stop_high[4][8] = { 
 { 0,0,0,0,0,0,0,0 },
 { 0,0,0,0,0,0,0,0 },
 { 0,0,0,0,0,0,0,0 },
 { 0,0,0,0,0,0,0,0 }
 };

// allow DAC calibration in future 
// Give users access to this so not 'static'
dac_cal_type dac_calibrate[4][2] = {
 { {GB_DAC_MIN,GB_DAC_MAX} , {GB_DAC_MIN,GB_DAC_MAX} },
 { {GB_DAC_MIN,GB_DAC_MAX} , {GB_DAC_MIN,GB_DAC_MAX} },
 { {GB_DAC_MIN,GB_DAC_MAX} , {GB_DAC_MIN,GB_DAC_MAX} },
 { {GB_DAC_MIN,GB_DAC_MAX} , {GB_DAC_MIN,GB_DAC_MAX} }
};

//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//
//
// Open comport 
// user_port indicates the port to use under windows
// If is is zero the program tries to find a working comport
// If there is only one it uses that
// if there are 0 or more then one it complains and exits
//
int  open_connection(int com_port)
{
#ifdef _WIN32
  int available_comports,num_coms,b;

  if (com_port==0)
  { // try to find ports which are unused 
    available_comports = available_com_ports();
    if (available_comports==0)
    { fprintf(stderr,"Could not find any available COM ports. Exiting...");
      return 0;
    }
    // check how many comports are available 
    // on the fly get a default port 
    num_coms = 0;
    for (b=0; b<32; b++)
    { if (available_comports & (1<<b))
      { num_coms++;
        com_port=b+1; // Used only if one port found!
      }
    }
    if (num_coms>1)
    { fprintf(stderr,"Need to know which COM port to use. Exiting...");
      return 0;
    }
    else
      fprintf(stderr,"Using COM%d\n",com_port);
  }

  if (open_uart(com_port))
  { fprintf(stderr,"Fatal! Could not open COM%d\n",com_port);
    return 0;
  }

  if (setup_comm_port(L"57600,n,8,1"))
  { fprintf(stderr,"Fatal! Could not configure com port");
    return 0;
  }

#else

  if (open_uart(1)!=0)
  { fprintf(stderr,"Fatal! Could not open uart: '/dev/ttyAMA0'\n");
    fprintf(stderr,"Check uart availability\n");
    fprintf(stderr,"Run 'enable_uart cf' command to check\n");
    return 0; 
  }

#endif
  return 1;

} // open_connection


uint8_t gb_stop_all_cmnd[4] = {0xA0,CMD_STOPALL,CMD_STOP2ND,0x50};
void stop_all()
{ write_uart(gb_stop_all_cmnd,4);
}

uint8_t gb_emerg_stop_cmnd[4] = {0xA0,CMD_PWR_OFF,CMD_STOP2ND,0x50};
void emergency_stop()
{ write_uart(gb_emerg_stop_cmnd,4);
}

//
// Setup an output on a board
//
// mode is on off:
// GB_MODE_OFF    0
// GB_MODE_BRUSH  8
// GB_MODE_STEPG  1
// GB_MODE_DCC    9
//
// return 1 on success
// return 0 on error 
int  set_channel_mode(int board,int channel,int mode)
{
  GB_CHECK(board>=0   && board<=3,  "set_channel_mode illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"set_channel_mode illegal channel\n");
  GB_CHECK( mode==GB_MODE_OFF   || mode==GB_MODE_BRUSH ||
            mode==GB_MODE_DCC   ||
            mode==GB_MODE_STEPG_OFF || mode==GB_MODE_STEPP_OFF ||
            mode==GB_MODE_STEPG_PWR || mode==GB_MODE_STEPG_PWR ,
            "set_channel_mode illegal mode\n");
  GB_CHECK ( !(mode!=MOT_MODE_STEP_MASK) || (channel!=1 && channel!=3),
	     "set_channel_mode illegal channel/mode\n");

  gb_command5[1] = CMD_OPMODE;
  gb_command5[2] = board<<2 | channel;
  gb_command5[3] = mode;
      
  gb_motor_mode[board][channel] = mode;
  write_uart(gb_command5,5);
  // Some operating modes need frequency
  // Send that out too
  if (mode==GB_MODE_BRUSH)
  { // set 5KHz
    gb_command6[1] = CMD_LINFREQ;
    gb_command6[2] = board<<2 | channel;
    gb_command6[3] = 5000>>8;
    gb_command6[4] = 5000 & 0xFF;
    write_uart(gb_command6,6);
  }
  if (mode&MOT_MODE_STEP_MASK)
  { // set stepper to 100Hz (*256)
    gb_command7[1] = CMD_STEPFREQ;
    gb_command7[2] = board<<2 | channel;
    gb_command7[3] = (25600>>16) & 0xFF;
    gb_command7[4] = (25600>>8) & 0xFF;
    gb_command7[5] =  25600 & 0xFF;
    write_uart(gb_command7,7);  
  }
  return 1;
} // set_channel_mode


//
// Set J3 pins as endstop
//
// stop_A, stop_B is on off:
// GB_ENDSTOP_OFF    0
// GB_ENDSTOP_LOW    1  Stop if pin low
// GB_ENDSTOP_HIGH   2  Stop if pin high 
//
// return 1 on success
// return 0 on error 
int  set_endstop (int board,int channel,int stop_A,int stop_B)
{ int pin;
  GB_CHECK(board>=0   && board<=3,  "set_endstop illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"set_endstop illegal channel\n");
  GB_CHECK(stop_A>=GB_ENDSTOP_OFF && stop_A<=GB_ENDSTOP_HIGH,"set_endstop illegal stop A mode\n"); 
  GB_CHECK(stop_B>=GB_ENDSTOP_OFF && stop_B<=GB_ENDSTOP_HIGH,"set_endstop illegal stop B mode\n"); 
  gb_command5[1] = CMD_ENDSTOP;
  gb_command5[2] = board<<2 | channel;
  gb_command5[3] = 0;
  if (stop_A)  gb_command5[3] |= 0x01;
  if (stop_B)  gb_command5[3] |= 0x02;
  if (stop_A==GB_ENDSTOP_HIGH)  gb_command5[3] |= 0x04;
  if (stop_B==GB_ENDSTOP_HIGH)  gb_command5[3] |= 0x08;
  write_uart(gb_command5,5);
  // Update the I/O table
  pin = channel*2;
  if (stop_A)
    gb_pin_mode[board][pin] = GB_PIN_ENDSTOP;
  else
    gb_pin_mode[board][pin] = GB_PIN_INPUT;
  pin++;
  if (stop_B)
    gb_pin_mode[board][pin] = GB_PIN_ENDSTOP;
  else
    gb_pin_mode[board][pin] = GB_PIN_INPUT;
  // Update the endstop table
  end_stop_high[board][channel*2]   = stop_A==GB_ENDSTOP_HIGH ? 1 : 0;
  end_stop_high[board][channel*2+1] = stop_B==GB_ENDSTOP_HIGH ? 1 : 0;
  return 1;
} // set_endstop


//
// Moved brush motor
// Can also be used for arbitrary power outputs
// e.g. LEDs, relay etc.
//
// Direction is one off:
// GB_MOVE_STOP   0   
// GB_MOVE_A      1
// GB_MOVE_B      2
//
// Ramp up  
// is on off:
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
int  move_brushed(int board,int channel, int direction,int ramp)
{
  GB_CHECK(board>=0   && board<=3,  "move_brushed illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"move_brushed illegal channel\n");
  GB_CHECK(direction>=0 && direction<=2,"move_brushed illegal direction\n");
  GB_CHECK(ramp>=0 && ramp<=15,"move_brushed illegal ramp rate\n");
  GB_CHECK(gb_motor_mode[board][channel]==GB_MODE_BRUSH,"move_brushed not in brushed mode\n");
  gb_command5[1] = CMD_LINON;
  gb_command5[2] = board<<2 | channel;
  gb_command5[3] = ramp<<4 | direction;
  write_uart(gb_command5,5);
  return 1;
} // move_brushed

//
// Set Brushed motor PWM mode
//
// Frequency must be in range 10-10000 Hz
// Duty cycle is from 0.0-100.0% (accuracy is 0.1 %)
//
// return 1 on success
// return 0 on error 
int  pwm_brushed (int board, int channel, int frequency, float duty_cycle)
{ int dc;
  GB_CHECK(board>=0   && board<=3,  "move_brushed illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"move_brushed illegal channel\n");
  GB_CHECK(frequency>=10 && frequency<=10000,"move_brushed illegal frequency\n");
  GB_CHECK(duty_cycle>=0.0 && duty_cycle<=100.0,"move_brushed illegal duty_cycle\n");
  GB_CHECK(gb_motor_mode[board][channel]==GB_MODE_BRUSH,"move_brushed not in brushed mode\n");
  gb_command6[1] = CMD_LINFREQ;
  gb_command6[2] = board<<2 | channel;
  gb_command6[3] = frequency>>8;
  gb_command6[4] = frequency & 0xFF;
  write_uart(gb_command6,6);
  dc = (int)(duty_cycle*10+0.5);
  gb_command6[1] = CMD_LINDC;
  gb_command6[3] = dc>>8;
  gb_command6[4] = dc & 0xFF;
  write_uart(gb_command6,6);
  return 1;
} // pwm_brushed

//
// Make stepper motor take steps
//
// steps > 0 : one direction
// steps = 0 : stop
// steps < 0 : other direction
// Range is from -8388608..8388607
// return 1 on success
// return 0 on error 
int  move_stepper(int board,int channel, int steps)
{
  GB_CHECK(board>=0   && board<=3,  "move_stepper illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"move_stepper illegal channel\n");
  GB_CHECK(steps>=-8388608 && steps<=8388607,"move_stepper illegal steps\n");
  GB_CHECK(gb_motor_mode[board][channel] & GB_MODE_STEP_MASK,"move_stepper not in stepper mode\n");
  gb_command7[1] = CMD_STEP;
  gb_command7[2] = board<<2 | channel;
  gb_command7[3] =  steps>>16;
  gb_command7[4] = (steps>>8) & 0xFF;
  gb_command7[5] =  steps & 0xFF;
  write_uart(gb_command7,7);  
  return 1;
} // move_stepper

//
// Stop stepper motor
// 
// Mode is one off:
// GB_STOP_OFF    0  Stop with no power on anchor
// GB_STOP_ON     1  Stop with power on anchor
//
// return 1 on success
// return 0 on error 
int  stop_stepper(int board,int channel, int mode)
{
  GB_CHECK(board>=0   && board<=3,  "stop_stepper illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"stop_stepper illegal channel\n");
  GB_CHECK(mode==0 || mode==1,      "stop_stepper illegal mode\n");
  GB_CHECK(gb_motor_mode[board][channel] & GB_MODE_STEP_MASK,"stop_stepper not in stepper mode\n");
  if (mode==0)
  { 
    gb_command5[1] = CMD_LINON;
    gb_command5[2] = board<<2 | channel;
    gb_command5[3] = 0;
    write_uart(gb_command5,5);
  }
  else
  {
    gb_command7[1] = CMD_STEP;
    gb_command7[2] = board<<2 | channel;
    gb_command7[3] = 0;
    gb_command7[4] = 0;
    gb_command7[5] = 0;
    write_uart(gb_command7,7);  
  }
  return 1;
} // stop_stepper

//
// Set stepper motor step frequency
//
// return 1 on success
// return 0 on error 
int  freq_stepper(int board, int channel, float frequency)
{ int f;
  GB_CHECK(board>=0   && board<=3,  "freq_stepper illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"freq_stepper illegal channel\n");
  GB_CHECK(frequency>=0.0625 && frequency<=5000.0,"freq_stepper illegal steps\n");
  GB_CHECK(gb_motor_mode[board][channel] & GB_MODE_STEP_MASK,"freq_stepper not in stepper mode\n");
  gb_command7[1] = CMD_STEPFREQ;
  gb_command7[2] = board<<2 | channel;
  f = (int)(frequency*256.0 + 0.5);
  gb_command7[3] =  f>>16;
  gb_command7[4] = (f>>8) & 0xFF;
  gb_command7[5] =  f & 0xFF;
  write_uart(gb_command7,7);  
  return 1;
} // freq_stepper

//
// Return error status of board
// (zero means no pending errors)
int  read_error_status(int board)
{ unsigned char reply[4];
  int bytes,error;
  gb_command6[1] = CMD_GET_ERROR;
  gb_command6[2] = board<<2;
  gb_command6[3] = CMD_STOP_VAL;
  gb_command6[4] = CMD_STOP_VAL;
  write_uart(gb_command6,6); 
  bytes = read_uart(reply,4);
  if (bytes!=4 || reply[0]!=board<<2 || reply[1]!=CMD_GET_ERROR)
    return -1;
  error = (reply[2]<<8) | reply[3];
  return(error);                       
} // error_status


//
// Return string indication error
//

#define GB_MAX_ERROR 0x0021
static const char *gb_error_text[] = {
"No error"                                      , // 0x0000                        
"Command input buffer overflow",                  // ERROR_INP_OVERFLOW 0x0001 
"Program internal error",                         // ERROR_FATALSYSTEM  0x0002 
"Mode command value error",                       // ERROR_MODEERR      0x0003 
"End stop parameter wrong",                       // ERROR_ENDSTOP      0x0004 
"Lin. freq error: Motor was not in linear mode",  // ERROR_LINFREQ      0x0005 
"Duty cycle error: Motor was not in linear mode", // ERROR_LINDC        0x0006 
"Illegal linear freq given",                      // ERROR_ILLPWMFREQ   0x0007 
"Illegal duty cycle given",                       // ERROR_ILLPWMDC     0x0008 
"Brushed motor start but not given freq.",        // ERROR_NOLINFREQ    0x0009 
"Stepper motor start but not given freq.",        // ERROR_NOSTEPFREQ   0x000A 
"Step command: Motor was not in step mode",       // ERROR_STEP         0x000B 
"Step freq. command: Motor was not in step mode", // ERROR_STEPFREQ     0x000C 
"**Obsolete error code 0x0D",                     //                    0x000D
"**Obsolete error code 0x0E",                     //                    0x000E
"**Obsolete error code 0x0F",                     //                    0x000F
"Start command given with halt active",           // ERROR_HALTACTIVE   0x0010 
"Timer queue pool empty",                         // ERROR_TQ_EMPTY     0x0011 
"Serial output queue overflow",                   // ERROR_SEROUTOVFL   0x0012 
"Illegal stepper frequency",                      // ERROR_ILLSTEPFREQ  0x0013 
"Write to DAC which is disabled",                 // ERROR_DACDIS       0x0014 
"Read from ADC which is disabled",                // ERROR_ADCDIS       0x0015 
"Enable ADC illegal mask bits",                   // ERROR_ADCMASK      0x0016 
"Enable DAC illegal mask bits",                   // ERROR_DACMASK      0x0017 
"Linear on/off direction but not a lin. motor",   // ERROR_LINON        0x0018 
"Illegal command",                                // ERROR_ILLCOMMAND   0x0019 
"HALT was active",                                // ERROR_HALTSEEN     0x001A 
"Enable A was active",                            // ERROR_A_SEEN       0x001B 
"Enable B was active",                            // ERROR_B_SEEN       0x001C 
"Enable c was active",                            // ERROR_C_SEEN       0x001D 
"Enable D was active",                            // ERROR_D_SEEN       0x001E 
"DCC message queue overflow",                     // ERROR_DCC_OVFL     0x001F 
"DCC illegal message (length)",                   // ERROR_DCC_MESS     0x0020 
"Illegal error status code"                                                         
};
const char *error_string(int error_number)
{
  if (error_number<0 || error_number>GB_MAX_ERROR)
    error_number = GB_MAX_ERROR;
  return   gb_error_text[error_number];
} // error_string

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
// return 0 on error 

// Legal pin modes table
#define GB_PINIO  (1<<GB_PIN_INPUT) | (1<<GB_PIN_OUTPUT)
#define GB_PINNONE 0
static const int gb_pin_modes[20] = {
 GB_PINIO   | (1 << GB_PIN_ENDSTOP)  , //  1
 GB_PINIO   | (1 << GB_PIN_ENDSTOP)  , //  2
 GB_PINIO   | (1 << GB_PIN_ENDSTOP)  , //  3
 GB_PINIO   | (1 << GB_PIN_ENDSTOP)  , //  4
 GB_PINIO   | (1 << GB_PIN_ENDSTOP)  , //  5
 GB_PINIO   | (1 << GB_PIN_ENDSTOP)  , //  6
 GB_PINIO   | (1 << GB_PIN_ENDSTOP)  , //  7
 GB_PINIO   | (1 << GB_PIN_ENDSTOP)  , //  8
 GB_PINIO                            , //  9
 GB_PINIO                            , // 10
 GB_PINNONE                          , // 11
 GB_PINNONE                          , // 12
 GB_PINIO   | (1 << GB_PIN_ADC)      , // 13
 GB_PINIO   | (1 << GB_PIN_ADC)      , // 14
 GB_PINIO   | (1 << GB_PIN_ADC)      , // 15
 GB_PINIO   | (1 << GB_PIN_ADC)      , // 16
 GB_PINNONE                          , // 17
 GB_PINIO   | (1 << GB_PIN_DAC)      , // 18
 GB_PINNONE                          , // 19
 GB_PINIO   | (1 << GB_PIN_DAC)        // 20
};                                              


int  set_pin_mode(int board,int pin, int mode)  
{ int dac_enb,adc_enb,p,mask1,mask2;                                             
  if (mode==GB_PIN_SAME)
    return 1;
  GB_CHECK(board>=0   && board<=3,  "set_pin_mode illegal board\n");
  GB_CHECK(pin>=1     && pin<=20,   "set_pin_mode illegal pin\n");
  GB_CHECK(mode>GB_PIN_SAME && mode<=GB_PIN_I2C,"set_pin_mode illegal mode\n");
  pin--; // range 0-19 
  GB_CHECK((gb_pin_modes[pin] & (1<<mode)),"set_pin_mode illegal mode\n");
  
/*
  Override function code:
  */

  if ( ((gb_pin_mode[board][pin]==GB_PIN_ADC) && (mode!=GB_PIN_ADC)) ||
       ((gb_pin_mode[board][pin]!=GB_PIN_ADC) && (mode==GB_PIN_ADC)) ||
       ((gb_pin_mode[board][pin]==GB_PIN_DAC) && (mode!=GB_PIN_DAC)) ||
       ((gb_pin_mode[board][pin]!=GB_PIN_DAC) && (mode==GB_PIN_DAC)) 
     )
  { // switch ADC/DAC on/off
    gb_pin_mode[board][pin] = mode; // override old mode 
    dac_enb  = gb_pin_mode[board][19]==GB_PIN_DAC ? 0x01 : 0x00; // DAC0
    dac_enb |= gb_pin_mode[board][17]==GB_PIN_DAC ? 0x02 : 0x00; // DAC1
    adc_enb  = gb_pin_mode[board][13]==GB_PIN_ADC ? 0x01 : 0x00; // ADC0
    adc_enb |= gb_pin_mode[board][12]==GB_PIN_ADC ? 0x02 : 0x00; // ADC1
    adc_enb |= gb_pin_mode[board][15]==GB_PIN_ADC ? 0x04 : 0x00; // ADC2
    adc_enb |= gb_pin_mode[board][14]==GB_PIN_ADC ? 0x08 : 0x00; // ADC3
      
    gb_command6[1] = CMD_SETADCDAC;
    gb_command6[2] = board<<2;
    gb_command6[3] = adc_enb;
    gb_command6[4] = dac_enb;
//    printf("AD:%02X %02X\n",gb_command6[3],gb_command6[4]);
    write_uart(gb_command6,6); 
  }

  if ((gb_pin_mode[board][pin]==GB_PIN_ENDSTOP) && (mode!=GB_PIN_ENDSTOP) ||
      (gb_pin_mode[board][pin]!=GB_PIN_ENDSTOP) && (mode==GB_PIN_ENDSTOP) 
     ) 
  { // switch endmode on/off
    gb_pin_mode[board][pin] = mode; // override old mode 
    p = pin & 0x0E;
    gb_command5[1] = CMD_ENDSTOP;
    gb_command5[2] = (board<<2) | (pin>>1);
    gb_command5[3] = 0;
    if (gb_pin_mode[board][p+1] ==GB_PIN_ENDSTOP)  gb_command5[3] |= 0x02;
    if (gb_pin_mode[board][p]   ==GB_PIN_ENDSTOP)  gb_command5[3] |= 0x01;
    if (end_stop_high[board][p+1]) gb_command5[3] |= 0x08;
    if (end_stop_high[board][p])   gb_command5[3] |= 0x04;
//    printf("ES:%02X %02X\n",gb_command5[2],gb_command5[3]);
    write_uart(gb_command5,5);
  }
  gb_pin_mode[board][pin] = mode; // set mode 

  // make set-port-to-output-mode command 
  mask1 = 0xFF; // all inputs 
  for (p=0; p<8; p++)
    if (gb_pin_mode[board][p]==GB_PIN_OUTPUT)
      mask1 &= ~(1<<p);

  // ** Something weird here. 
  // But if so I have a bug in the GUI AND in the MC code
  // which cancel each other out! 
  // Have not found such a bug.
  // Why do I have to twiddle bits 12..19???
  mask2=0xFF; // all inputs 
  if (gb_pin_mode[board][8]==GB_PIN_OUTPUT)
    mask2 &= ~0x01;
  if (gb_pin_mode[board][9]==GB_PIN_OUTPUT)
    mask2 &= ~0x02;
  if (gb_pin_mode[board][12]==GB_PIN_OUTPUT)
    mask2 &= ~0x08; // **Does NOT make sense!
  if (gb_pin_mode[board][13]==GB_PIN_OUTPUT)
    mask2 &= ~0x04; // **Does NOT make sense!
  if (gb_pin_mode[board][14]==GB_PIN_OUTPUT)
    mask2 &= ~0x20; // **Does NOT make sense!
  if (gb_pin_mode[board][15]==GB_PIN_OUTPUT)
    mask2 &= ~0x10; // **Does NOT make sense!
  if (gb_pin_mode[board][17]==GB_PIN_OUTPUT)
    mask2 &= ~0x80;
  if (gb_pin_mode[board][19]==GB_PIN_OUTPUT)
    mask2 &= ~0x40;
    
  gb_command7[1] = CMD_SETIO;
  gb_command7[2] = board<<2;
  gb_command7[3] = 0;
  gb_command7[4] = mask2;
  gb_command7[5] = mask1;
//  printf("Out:%02X %02X %02X\n",gb_command7[3],gb_command7[4],gb_command7[5]);
  write_uart(gb_command7,7); 

  return 1;
} // set_pin_mode                                             


//
// Set the output pins in a high/low state
// ALL outputs are set
// The function maps output bits 0..N to pins 0..N
// bit  0 : pin 1
// bit  1 : pin 2
// ..
// bit  6 : pin 7
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
int set_output_pin_state(int board, int output)
{ int out_swap;
  GB_CHECK(board>=0 && board<=3,  "set_output_pin_state illegal board\n");
  // Pins 18/20 (bits 14 & 15) have to be swapped
  // (Or should I do this in MC??)
//  out_swap = output & 0x3FFF;
//  out_swap |= output & 0x4000 ? 0x8000 : 0;
//  out_swap |= output & 0x8000 ? 0x4000 : 0;
  gb_command7[1] = CMD_WRITEIO;
  gb_command7[2] = board<<2;
  gb_command7[3] = 0x00; // for future use
  gb_command7[4] = (output >> 8) & 0xFF;
  gb_command7[5] = output & 0xFF;
  write_uart(gb_command7,7); 
  return 1;
} // set_output_pin_state

   
//
// Activate the open drain outputs
// drain0,drain1 are interpreted as C booleans:
// ==0 means not active
// !=0  means active
// Beware that activate means the pin will go LOW
//
int activate_opendrain(int board,int drain0, int drain1)
{
  GB_CHECK(board>=0 && board<=3,  "activate_opendrain illegal board\n");
  gb_command5[1] = CMD_OD;
  gb_command5[2] = board<<2;
  gb_command5[3] = drain0 ? 0x01 : 0x00;
  if (drain1)
     gb_command5[3] |= 0x02;
  write_uart(gb_command5,5); 
  return 1;
} // activate_opendrain

                                        
//                                              
// Set a DAC output value                       
// This routine sends a 16-bit value
// which allows future 16-bit DAC's
//
//                                              
int set_dac(int board,int dac,float value)   
{ int ivalue;
  GB_CHECK(board>=0 && board<=3, "set_dac illegal board\n");
  GB_CHECK(dac>=0 && dac<=1, "set_dac illegal DAC\n");
  GB_CHECK(value>=dac_calibrate[board][dac].min && value<=dac_calibrate[board][dac].max, "set_dac illegal value\n");
  // Map value onto 12 bit range
  value -= dac_calibrate[board][dac].min;
  ivalue = (int)(value * 4095.0/(dac_calibrate[board][dac].max-dac_calibrate[board][dac].min)+0.5);
  gb_command6[1] = CMD_DAC;
  gb_command6[2] = board<<2 | dac;
  gb_command6[3] = ivalue>>8;
  gb_command6[4] = ivalue & 0xFF;
  write_uart(gb_command6,6);
  return 1;                               
} // set_dac                                            
                                                
                                                
//                                              
// Read ADC value                               
//                                              
// a return value of -1 indicates an error      
//                                              
float read_adc(int board,int adc)              
{ unsigned char reply[4];
  int ivalue,bytes,id;
  GB_CHECKN(board>=0 && board<=3, "read_adc illegal board\n");
  GB_CHECKN(adc>=0 && adc<=3, "read_adc illegal ADC\n");
  id = (board<<2) | adc;
  gb_command7[1] = CMD_GET_ADC;
  gb_command7[2] = id;
  gb_command7[3] = CMD_STOP_VAL;
  gb_command7[4] = CMD_STOP_VAL;
  gb_command7[5] = CMD_STOP_VAL;
  write_uart(gb_command7,7); 
  bytes = read_uart(reply,4);
  if (bytes!=4 || reply[0]!=id || reply[1]!=CMD_GET_ADC)
    return -1;
  ivalue = (reply[2]<<8) | reply[3];
  return(ivalue*(float)(3.3/4095.0));                      
} // read_adc                                             
                                                





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
// The check byte is not send!
//
int send_dcc_mess(int board,int channel,int length, unsigned char *data)
{ int l;
  GB_CHECK(board>=0  && board<=3,   "send_dcc_cmnd illegal board\n");
  GB_CHECK(channel>0 && channel<=15,"send_dcc_cmnd illegal channel set\n");
  GB_CHECK(length>=2 && length<=5,  "send_dcc_cmnd illegal length\n");
  gb_command10[1] = CMD_DCC_MESS;
  gb_command10[2] = board<<2;
  gb_command10[3] = channel<<4 | length;
  for (l=0; l<length; l++)
    gb_command10[4+l] = data[l];
  // Leave unused bytes as they where
  write_uart(gb_command10,10);  
  return 1;
} // send_dcc_mess


