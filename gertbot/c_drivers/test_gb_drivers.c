//
// Test GB driver code
//
//
//

#include "gb_drivers.h"
#include "gertbot_defines.h"

#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS(x) Sleep(x)
#else
#define SLEEP_MS(x) usleep((x)*1000)
#endif

/*
#ifdef _WIN32
  #include "gertbot_winserial.h"
#else
  #include "gertbot_pi_uart.h"
#endif
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int errors = 0;
int board=0;
char *this_program;

void test_mode();
void test_endstop();
void test_move_brushed();
void test_pwm_brushed();
void test_move_stepper();
void test_freq_stepper();
void test_stop_stepper();
void test_read_error_status();
void test_set_pin_mode();
void test_adc_dac();
void test_outputs();

/*
int write_uart(const unsigned char *data,int length)
{ while (length--)
    printf("0x%02X ",*data++);
  printf("\n");
  return 0; // not yet used
}
*/

void usage()
{
  fprintf(stderr,"Usage: %s [-pPORT] [-tTEST] [-bBOARD]\n",this_program);
  fprintf(stderr,"PORT is the comport number to use (1-32)\n");
  fprintf(stderr,"BOARD is the board to use (0-3)\n");
  fprintf(stderr,"TEST is the test to run:\n");
  fprintf(stderr,"  0 : run all\n");
  fprintf(stderr,"  1 : test_mode\n");
  fprintf(stderr,"  2 : test_endstop\n");
  fprintf(stderr,"  3 : test_move_brushed\n");
  fprintf(stderr,"  4 : test_pwm_brushed\n");
  fprintf(stderr,"  5 : test_move_stepper\n");
  fprintf(stderr,"  6 : test_freq_stepper\n");
  fprintf(stderr,"  7 : test_stop_stepper\n");
  fprintf(stderr,"  8 : test_read_error_status\n");
  fprintf(stderr,"  9 : test_set_pin_mode\n");
  fprintf(stderr," 10 : test_adc_dac\n");
  fprintf(stderr," 11 : test_outputs\n");
  
  fprintf(stderr,"\n");
  fprintf(stderr,"\n");
  fprintf(stderr,"\n");
} // usage



int main(int argc,char *argv[])
{ int com_port,a,run_test;
  com_port = 0;
  run_test = 0;
  this_program = argv[0];

  for (a=1; a<argc; a++)
  {
    if (!strncmp(argv[a],"-p",2))
    { // set port
      if (sscanf(argv[a]+2,"%d",&com_port)!=1)
        usage();
    }
    if (!strncmp(argv[a],"-t",2))
    { // set test
      if (sscanf(argv[a]+2,"%d",&run_test)!=1)
        usage();
    }
    if (!strncmp(argv[a],"-b",2))
    { // set board
      if (sscanf(argv[a]+2,"%d",&board)!=1)
        usage();
    }
    
  }

  printf("This test should be run on Raspberry-Pi\n");
  printf("or with a scope attached\n");
  printf("Currently testing board %d\n",board);
  printf("Press RETURN after each test\n");
  if (!open_connection(com_port))
  { printf("open failed on comport\n");
    return 2;
  }

  if (run_test==1) test_mode();
  if (run_test==2) test_endstop();
  if (run_test==3) test_move_brushed();
  if (run_test==4) test_pwm_brushed();
  if (run_test==5) test_move_stepper();
  if (run_test==6) test_freq_stepper();
  if (run_test==7) test_stop_stepper();
  if (run_test==8) test_read_error_status();
  if (run_test==9) test_set_pin_mode();
  if (run_test==10) test_adc_dac();
  if (run_test==11) test_outputs();



  if (errors==0)
    printf("No errors detected\n");
  else
    printf("%d errors detected\n",errors);

  return errors ? 1 : 0;
} // main

//
// Test operating modes 
//
#define TST_NUM_MODES 7
const char *mode_string[]= {
  "Off",       // 0x00
  "Brushed",   // 0x01
  "DCC",       // 0x02
  "Illegal",   // 0x03
  "Illegal",   // 0x04
  "Illegal",   // 0x05
  "Illegal",   // 0x06
  "Illegal",   // 0x07
  "Step Gray  Off", // 0x08
  "Step Pulse Off",// 0x09
  "Illegal",   // 0x0A
  "Illegal",   // 0x0B
  "Illegal",   // 0x0C
  "Illegal",   // 0x0D
  "Illegal",   // 0x0E
  "Illegal",   // 0x0F
  "Illegal",   // 0x10
  "Illegal",   // 0x11
  "Illegal",   // 0x12
  "Illegal",   // 0x13
  "Illegal",   // 0x14
  "Illegal",   // 0x15
  "Illegal",   // 0x16
  "Illegal",   // 0x17
  "Step Gray  Pwr", // 0x18
  "Step Pulse Pwe", // 0x19
  "Illegal",   // 0x16
  "Illegal"    // 0x17
  // In fact there are 256 codes but forget about that
};
void test_mode()
{ int channel,mode,status,mode_code;
  static int mode_table[TST_NUM_MODES] = {
    GB_MODE_OFF,
    GB_MODE_BRUSH,
    GB_MODE_DCC,
    GB_MODE_STEPG_OFF,
    GB_MODE_STEPP_OFF,
    GB_MODE_STEPG_PWR,
    GB_MODE_STEPP_PWR
  };

  printf("Test operating mode on board %d\n",board);
  for (channel=0; channel<4; channel++)
  {
    for (mode=0; mode<TST_NUM_MODES; mode++)
    { mode_code = mode_table[mode];
      if ((mode_code & MOT_MODE_STEP_MASK) && (channel & 1))
        continue; // no stepper mode on odd channels
      status = set_channel_mode(board,channel,mode_code);
      printf("channel %d, mode %d (%s)\n",channel,
                  mode_code,mode_string[mode_code]);
      getchar();
      if (status==0)
      { errors++;
          fprintf(stderr,"test_mode failed\n");
      }
    } // mode
  } // channel
} // 

const char *endstop_string[]= {
  "Off ",
  "Low ",
  "High"
};

void test_endstop()
{ int channel,mode_A,mode_B,status;

  printf("Test end-stop mode on board %d\n",board);
  for (channel=0; channel<4; channel++)
  {
    for (mode_A=0; mode_A<3; mode_A++)
    {
      for (mode_B=0; mode_B<3; mode_B++)
      {
        status = set_endstop(board,channel,mode_A,mode_B);
        printf("channel %d, mode A %d (%s) mode B %d (%s)\n",channel,
           mode_A,endstop_string[mode_A],
           mode_B,endstop_string[mode_B]);
        if (status==0)
        { errors++;
          fprintf(stderr,"test_endstop failed\n");
        }
      } // mode B
    } // mode A
  } // channel
} // test_endstop

const char *movebrush_string[]= {
  "Stop  ",
  "Move A",
  "Move B"
};
const char *ramp_string[]= {
 "no ramp  ",
 "0.05 sec.",
 "0.12 sec.",
 "0.25 sec.",
 "0.37 sec.",
 "0.50 sec.",
 "0.75 sec.",
 "1.00 sec.",
 "1.25 sec.",
 "1.50 sec.",
 "1.75 sec.",
 "2.00 sec.",
 "2.50 sec.",
 "3.00 sec.",
 "4.00 sec.",
 "5.00 sec."
};
void test_move_brushed()
{ int channel,mode,ramp,status;

  printf("Test move brush mode on board %d\n",board);
  for (channel=0; channel<4; channel++)
  { 
    set_channel_mode(board,channel,GB_MODE_BRUSH);
    for (mode=0; mode<3; mode++)
    {
      for (ramp=0; ramp<16; ramp+=3) // Not testing each ramp!
      {
        status = move_brushed(board,channel,mode,ramp);
        printf("channel %d, mode %d (%s) ramp %d (%s)\n",channel,
           mode,movebrush_string[mode],ramp,ramp_string[ramp]);
        getchar();
        if (status==0)
        { errors++;
          fprintf(stderr,"test_move_brushed failed\n");
        }
      } // ramp
    } // mode
  } // channel
} // test_move_brushed

void test_pwm_brushed ()
{ int channel,freq,status; 
  float dc;

  printf("Test PWM brushed on board %d\n",board);
  printf("Use scope!\n",board);
  for (channel=0; channel<4; channel++)
  { // Set channel for brushed and start motor 
    set_channel_mode(board,channel,GB_MODE_BRUSH);
    move_brushed(board,channel,1,0);
    printf("Start test channel %d ",channel); getchar();
    for (freq=10; freq<10000; freq=freq*2+3)
    {
      printf("channel %d, freq %d DC 0-100\n",channel,freq);
      for (dc=0.0; dc<=100.0; dc=dc+2)
      {
        status = pwm_brushed(board,channel,freq,dc);
        if (status==0)
        { errors++;
          fprintf(stderr,"test_pwm_brushed failed\n");
        }
      } // duty cycle
    } // frequency
  } // channel
} // test_pwm_brushed

const int step_table[] = { 100000,50000,25000,5000,500,4,4};
void test_move_stepper()
{ int channel,s,steps,status,secs;

  printf("Test move stepper on board %d\n",board);
  for (channel=0; channel<4; channel+=2)
  { 
    set_channel_mode(board,channel,GB_MODE_STEPG_OFF);
    freq_stepper(board,channel,5000);
    // Now at 5000 Hz 
    for (s=0; s<7; s++)
    { steps = step_table[s];
      secs = steps/5000;
      printf("channel %d, steps %d (%d sec.)\n",channel,steps,secs);
      fflush(stdout); getchar();
      status = move_stepper(board,channel,steps);
      if (status==0)
      { errors++;
        fprintf(stderr,"test_move_stepper failed\n");
      }
      SLEEP_MS(secs*1000+500);
      printf("done\n");
    } // steps

    for (s=0; s<7; s++)
    { steps = -step_table[s];
      secs = -steps/5000;
      printf("channel %d, steps %d (%d sec.)\n",channel,steps,secs);
      fflush(stdout); getchar();
      status = move_stepper(board,channel,steps);
      if (status==0)
      { errors++;
        fprintf(stderr,"test_move_stepper failed\n");
      }
      SLEEP_MS(secs*1000+500);
      printf("done\n");
    } // steps
  } // channel
} // test_move_stepper

void test_freq_stepper()
{ int channel,status; 
  float freq;

  printf("Test stepper frequency on board %d\n",board);
  printf("Use scope!\n",board);
  for (channel=0; channel<4; channel+=2)
  { // Set channel for stepper  
    set_channel_mode(board,channel,GB_MODE_STEPG_OFF);
    for (freq=(float)0.5; freq<=(float)5000; freq=freq*(float)2+(float)0.5)
    {
      printf("channel %d, freq %4.2f\n",channel,freq);
      status = freq_stepper(board,channel,freq);
      if (status==0)
      { errors++;
        fprintf(stderr,"test_freq_stepper failed\n");
      }
      getchar();
    } // frequency
  } // channel
} // test_freq_stepper

const char *stop_string[] = {"Passive","Active"};
void test_stop_stepper()
{ int channel,status,mode,repeat; 

  printf("Test stop stepper on board %d\n",board);
  printf("Use scope!\n",board);
  for (channel=0; channel<4; channel+=2)
  { // Set channel for stepper and start motor 
    set_channel_mode(board,channel,GB_MODE_STEPG_PWR);
    freq_stepper(board,channel,100);
    for (mode=0; mode<2; mode++)
    { // On active stop might need to stop several times
      // to see end-state signals change
      // (only needed hen using scope)
      repeat = mode * 5+1;
      while (repeat--)
      {
        move_stepper(board,channel,5000000);
        printf("channel %d, stop %d (%s)\n",channel,mode,stop_string[mode]);
        getchar();
        status = stop_stepper(board,channel,mode);
        if (status==0)
        { errors++;
          fprintf(stderr,"test_stop_stepper failed\n");
        }
        getchar();
      } // repeat 
    } // mode
    printf("channel %d, End stop %d (%s)\n",channel,GB_STOP_OFF,stop_string[GB_STOP_OFF]);
    stop_stepper(board,channel,GB_STOP_OFF); // always end with stop passive
    getchar();
  } // channel
} // test_stop_stepper


void test_read_error_status()
{ int error;
  printf("Test read error status on board %d\n",board);
  // generate some errors first
  set_channel_mode(board,0,GB_MODE_STEPG_OFF);
  move_brushed(board,0,GB_MOVE_A,0);
  pwm_brushed(board,0,100,100.0);

  do {
    error = read_error_status(board);
    if (error==-1)
      printf("Read error status failed\n");
    else
      printf("%d = %s\n",error,error_string(error));
  } while (error);

  set_channel_mode(board,2,GB_MODE_BRUSH);
  pwm_brushed(board,2,5,100.0);
  move_stepper(board,2,10);  

  do {
    error = read_error_status(board);
    if (error==-1)
      printf("Read error status failed\n");
    else
      printf("%d = %s\n",error,error_string(error));
  } while (error);
  error = -1;
  printf("%d = %s\n",error,error_string(error));
  error = 10000;
  printf("%d = %s\n",error,error_string(error));
  
} // test_read_error_status


void test_set_pin_mode()
{ int pin;
  printf("Test set pin mode on board %d\n",board);
  for (pin=1; pin<=8; pin++)
  { set_pin_mode(board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    getchar();
  }
  for (pin=1; pin<=8; pin++)
  { set_pin_mode(board,pin,GB_PIN_OUTPUT);
    printf("pin %d output\n",pin);
    getchar();
  }
  for (pin=1; pin<=8; pin++)
  { set_pin_mode(board,pin,GB_PIN_ENDSTOP);
    printf("pin %d endstop\n",pin);
    getchar();
  }
  for (pin=1; pin<=8; pin++)
  { set_pin_mode(board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    getchar();
  }
  for (pin=9; pin<=10; pin++)
  { set_pin_mode(board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    getchar();
    set_pin_mode(board,pin,GB_PIN_OUTPUT);
    printf("pin %d output\n",pin);
    getchar();
    set_pin_mode(board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    getchar();
  }
  for (pin=13; pin<=16; pin++)
  { set_pin_mode(board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    getchar();
    set_pin_mode(board,pin,GB_PIN_OUTPUT);
    printf("pin %d output\n",pin);
    getchar();
    set_pin_mode(board,pin,GB_PIN_ADC);
    printf("pin %d ADC\n",pin);
    getchar();
    set_pin_mode(board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    getchar();
  }
  for (pin=18; pin<=20; pin+=2)
  { set_pin_mode(board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    getchar();
    set_pin_mode(board,pin,GB_PIN_OUTPUT);
    printf("pin %d output\n",pin);
    getchar();
    set_pin_mode(board,pin,GB_PIN_DAC);
    printf("pin %d DAC\n",pin);
    getchar();
    set_pin_mode(board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    getchar();
  }
  
  
  
}// test_set_pin_mode

//
// Test ADC and DAC 
// Assumes 'set_pin_mode' is working 
//
void test_adc_dac()
{ int pin,d,rep;
  float dac,dac_min,dac_max,dac_step;
  printf("Test ADC & DAC on board %d\n",board);
  for (pin=13; pin<=16; pin++)
    set_pin_mode(board,pin,GB_PIN_ADC);
  for (rep=0; rep<4; rep++)
  { for (d=0; d<4; d++)
      printf("Read ADC %d = %5.3f\n",d,read_adc(board,d));
    getchar();
  }
  set_pin_mode(board,18,GB_PIN_DAC);
  set_pin_mode(board,20,GB_PIN_DAC);
  // assume calibrate table has default values
  // so not searching for highest minimum and lowest maximum
  dac_min = dac_calibrate[board][0].min;
  dac_max = dac_calibrate[board][0].max;
  dac_step = (dac_max-dac_min)/4;
  for (dac=dac_min; dac<=dac_max; dac+= dac_step)
  { for (d=0; d<2; d++)
    { printf("Write DAC %d = ~%5.3f\n",d,dac);
      set_dac(board,d,dac);
    }
    getchar();
  }
} // test_adc_dac

static int pin_set[16] = { 1,2,3,4,5,6,7,8,9,10,13,14,15,16,18,20};
void test_outputs()
{ int i,pin,d0,d1;
  printf("Test outputs board %d\n",board);
  
  for (i=0; i<4; i++)
  { d0 = i & 1;
    d1 = (i & 2)>>1;
    printf("Open drain %d %d\n",d0,d1);
    activate_opendrain(board,d0,d1);
    getchar();
  }
  for (pin=0; pin<16; pin++)
    set_pin_mode(board,pin_set[pin],GB_PIN_OUTPUT);
  
  for (pin=0; pin<16; pin++)
  { printf("pin %d high\n",pin_set[pin]);
    set_output_pin_state(board,1<<pin);
    getchar();
  }
  
} // test_outputs