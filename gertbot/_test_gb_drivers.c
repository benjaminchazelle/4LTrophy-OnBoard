//
// Test GB driver code
//
//
//

#include "gb_drivers.h"
#include "gertbot_defines.h"

#ifdef _WIN32
#include <windows.h>

#else

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
int tst_board=0;
int tst_chan = 0;
char *this_program;

void test_mode();
void test_endstop_short();
void test_move_brushed();
void test_pwm_brushed();
void test_move_stepper();
void test_freq_stepper();
void test_stop_stepper();
void test_read_error_status();
void test_set_pin_mode();
void test_adc_dac();
void test_outputs();
void test_inputs();
void test_get_io_setup();
void test_get_motor_config();
void test_brush_ramps(int);
void test_get_motor_status_missed();
void test_dcc();
void test_quad();
void test_endstop2();
void test_shorthot();


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
  fprintf(stderr,"Usage: %s [-pPORT] [-tTEST] [-bBOARD] [-cCHANNEL]\n",this_program);
  fprintf(stderr,"PORT is the comport number to use (1-32)\n");
  fprintf(stderr,"BOARD is the tst_board to use (0-3)\n");
  fprintf(stderr,"TEST is the test to run:\n");
  fprintf(stderr,"  1 : test_mode\n");
  fprintf(stderr,"  2 : test_endstop_short\n");
  fprintf(stderr,"  3 : test_move_brushed\n");
  fprintf(stderr,"  4 : test_pwm_brushed\n");
  fprintf(stderr,"  5 : test_move_stepper\n");
  fprintf(stderr,"  6 : test_freq_stepper\n");
  fprintf(stderr,"  7 : test_stop_stepper\n");
  fprintf(stderr,"  8 : test_read_error_status\n");
  fprintf(stderr,"  9 : test_set_pin_mode\n");
  fprintf(stderr," 10 : test_adc_dac\n");
  fprintf(stderr," 11 : test_outputs\n");
  fprintf(stderr," 12 : test_inputs\n");
  fprintf(stderr," 13 : test_get_io_setup\n");
  fprintf(stderr," 14 : test_brush_ramps 0\n");
  fprintf(stderr," 15 : test_get_motor_config\n");
  fprintf(stderr," 16 : test_get_motor_status_missed\n");
  fprintf(stderr," 17 : test_dcc\n");
  fprintf(stderr," 18 : test_quad\n");
  fprintf(stderr," 19 : test_endstop2\n");
  fprintf(stderr," 20 : test_shorthot\n");
  
  fprintf(stderr,"Press RETURN during tests\n");

  fprintf(stderr,"\n");
} // usage

int get_return()
{ char c,prev_c;
  c = 0;
  do  {
      prev_c = c;
      c = getchar();
   } while (c!=0x0A);
   return prev_c;
} // get_return;



int main(int argc,char *argv[])
{ int com_port,a,run_test;
  int major,minor;
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
    { // set tst_board
      if (sscanf(argv[a]+2,"%d",&tst_board)!=1)
        usage();
    }
     if (!strncmp(argv[a],"-c",2))
    { // set channel
      if (sscanf(argv[a]+2,"%d",&tst_chan)!=1)
        usage();
    }
   
  }

  printf("This test should be run on Raspberry-Pi\n");
  printf("or with a scope attached\n");
  printf("Currently testing board %d\n",tst_board);
  if (!open_connection(com_port))
  { printf("open failed on comport\n");
    return 2;
  }
  
  a=get_version(tst_board);
  if (a==0)
    fprintf(stderr,"\n***\n***  Warning: get_version returned ZERO value\n***\n");
  else
  { major = a/100;
    minor = a%100;
    printf("\nBoard version is %d.%02d\n\n",major,minor);
  }

  if (run_test==0)
    usage();

  if (run_test==1) test_mode();
  if (run_test==2) test_endstop_short();
  if (run_test==3) test_move_brushed();
  if (run_test==4) test_pwm_brushed();
  if (run_test==5) test_move_stepper();
  if (run_test==6) test_freq_stepper();
  if (run_test==7) test_stop_stepper();
  if (run_test==8) test_read_error_status();
  if (run_test==9) test_set_pin_mode();
  if (run_test==10) test_adc_dac();
  if (run_test==11) test_outputs();
  if (run_test==12) test_inputs();
  if (run_test==13) test_get_io_setup();
  if (run_test==14) test_brush_ramps(0);
  if (run_test==15) test_get_motor_config();
  if (run_test==16) test_get_motor_status_missed();
  if (run_test==17) test_dcc();
  if (run_test==18) test_quad();
  if (run_test==19) test_endstop2();
  if (run_test==20) test_shorthot();

  if (errors==0)
    printf("No routine call errors detected\n");
  else
    printf("%d routine call errors detected\n",errors);

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
  "Step Pulse Pwr", // 0x19
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

  printf("Test operating mode on board %d\n",tst_board);
  for (channel=0; channel<4; channel++)
  {
    for (mode=0; mode<TST_NUM_MODES; mode++)
    { mode_code = mode_table[mode];
      if ((mode_code & MOT_MODE_STEP_MASK) && (channel & 1))
        continue; // no stepper mode on odd channels
      status = set_mode(tst_board,channel,mode_code);
      printf("channel %d, mode %d (%s)\n",channel,
                  mode_code,mode_string[mode_code]);
      get_return();
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

const char *short_string[]= {
"nothing     ",
"channel     ",
"channel pair",
"tst_board       ",
"system      "
};

//
// Testing comprises of calling the routine
// Then using the GUI to read back
// Which means the GUI had better be OK as well
//
void test_endstop_short()
{ int channel,mode_A,mode_B,status,skip,shrt;
  skip = 0;
  printf("Test end-stop mode on board %d (s=skip)\n",tst_board);
  for (channel=0; channel<4 && !skip; channel++)
  {
    for (mode_A=0; mode_A<3; mode_A++)
    {
      for (mode_B=0; mode_B<3; mode_B++)
      {
        status = set_endstop(tst_board,channel,mode_A,mode_B);
        printf("channel %d, mode A %d (%s) mode B %d (%s)\n",channel,
           mode_A,endstop_string[mode_A],
           mode_B,endstop_string[mode_B]);
        if (status==0)
        { errors++;
          fprintf(stderr,"test_endstop failed\n");
        }
        skip=get_return()=='s';
      } // mode B
    } // mode A
  } // channel


  printf("Test short/hot mode on board %d\n",tst_board);
  for (channel=0; channel<4 && !skip; channel++)
  {
    for (shrt=GB_SHORT_NONE; shrt<=GB_SHORT_SYST; shrt++)
    {
      printf("channel %d, short/hot mode %d (Stop %s)\n",
          channel,shrt,short_string[shrt]);
      status = set_short(tst_board,channel,shrt);
      if (status==0)
      { errors++;
        fprintf(stderr,"test_endstop failed\n");
      }
      get_return();
    }
  }
} // test_endstop_short

const char *movebrush_string[]= {
  "Stop  ",
  "Move A",
  "Move B"
};
const char *ramp_string[]= {
 "no ramp  ",
 "0.10 sec.",
 "0.25 sec.",
 "0.50 sec.",
 "0.75 sec.",
 "1.00 sec.",
 "1.25 sec.",
 "1.50 sec.",
 "1.75 sec.",
 "2.00 sec.",
 "2.25 sec.",
 "2.50 sec.",
 "3.00 sec.",
 "4.00 sec.",
 "5.00 sec.",
 "7.00 sec."
};

void test_move_brushed()
{ int channel,mode,status;

  printf("Test move brush mode on board %d\n",tst_board);
  // At this moment the short/hort control part of the driver
  // has not yet been writen, let alone tested
  /// User has to use GUI to set that
  printf("Need short/hot disabled!!\n(Press <return> when that is done) ");
  get_return();
  for (channel=0; channel<4; channel++)
  { 
    set_mode(tst_board,channel,GB_MODE_BRUSH);
    for (mode=0; mode<3; mode++)
    {
        if (mode==0)
           move_brushed(tst_board,channel,GB_MOVE_A); // start before stopping 
        else
           stop_all(); // stop before testing start 
        SLEEP_MS(1000);
        status = move_brushed(tst_board,channel,mode);
        printf("channel %d, mode %d (%s)\n",channel,
           mode,movebrush_string[mode]);
        get_return();
        if (status==0)
        { errors++;
          fprintf(stderr,"test_move_brushed failed\n");
        }
    } // mode
    
    // Switch off when done testing 
    set_mode(tst_board,channel,GB_MODE_OFF);
  } // channel
  
} // test_move_brushed

//
// Testing RAMPS is difficult
// There is a lot to test
// e.g. testing direction change while in the middle of ramping up/down/reverse
// For now testing that the data gets send
//
static int ramp_ms[] = {
     0,  //  0
   100,  //  1
   250,  //  2
   500,  //  3
   750,  //  4
  1000,  //  5
  1250,  //  6
  1500,  //  7
  1750,  //  8
  2000,  //  9
  2250,  // 10
  2500,  // 11
  3000,  // 12
  4000,  // 13
  5000,  // 14
  7000   // 15
};

void test_brush_ramps(int channel)
{ int move,ramp,skip;

  printf("Test move ramping using tst_board %d channel %d\n",tst_board,channel);
  printf("Assumes short/hot disabled has been tested\n");
  set_mode(tst_board,channel,GB_MODE_BRUSH);
  set_short(tst_board,channel,GB_SHORT_NONE);

  // Test starting

  printf("Starting (ramp-up) press 's' to skip\n");
  skip=0;
  for (ramp=0; ramp<16  && !skip; ramp+=4)
  {
    stop_all(); // stop before testing start 
    SLEEP_MS(500);    
    for (move=GB_MOVE_A; move<=GB_MOVE_B && !skip; move++)
    {

      printf("Ramp-up %s, speed %s\n",movebrush_string[move],ramp_string[ramp]);
      set_brush_ramps(tst_board,channel,ramp,ramp,ramp);
      printf("+++"); get_return();
      move_brushed(tst_board,channel,move);
      SLEEP_MS(ramp_ms[ramp]+2000);

      printf("Ramp-down, speed %s\n",ramp_string[ramp]);
      printf("+++"); get_return();
      move_brushed(tst_board,channel,GB_MOVE_STOP);
      SLEEP_MS(ramp_ms[ramp]+2000);

      printf("Stop speed %s\n",ramp_string[ramp]);
      set_brush_ramps(tst_board,channel,0,0,ramp);
      SLEEP_MS(GB_RAMP_050);
      move_brushed(tst_board,channel,move);
      printf("s++"); skip=get_return()=='s';
      stop_all(); // stop before testing start 
      SLEEP_MS(ramp_ms[ramp]);
    } // move
  } // ramp

  printf("More tests to write......\n");
  
  // Switch off when done testing 
  set_mode(tst_board,channel,GB_MODE_OFF);
} // test_move_brushed


void test_pwm_brushed ()
{ int channel,frequency,status; 
  float dc;

  printf("Test PWM brushed on board %d\n",tst_board);
  printf("Use scope!\n",tst_board);
  for (channel=0; channel<4; channel++)
  { // Set channel for brushed and start motor 
    set_mode(tst_board,channel,GB_MODE_BRUSH);
    move_brushed(tst_board,channel,1);
    printf("Start test channel %d ",channel); get_return();
    for (frequency=10; frequency<10000; frequency=frequency*2+3)
    {
      printf("channel %d, frequency %d DC 0-100\n",channel,frequency);
      for (dc=0.0; dc<=100.0; dc=dc+2)
      {
        status = pwm_brushed(tst_board,channel,frequency,dc);
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

  printf("Test move stepper on board %d\n",tst_board);
  for (channel=0; channel<4; channel+=2)
  { 
    set_mode(tst_board,channel,GB_MODE_STEPG_OFF);
    freq_stepper(tst_board,channel,5000);
    // Now at 5000 Hz 
    for (s=0; s<7; s++)
    { steps = step_table[s];
      secs = steps/5000;
      printf("channel %d, steps %d (%d sec.)\n",channel,steps,secs);
      fflush(stdout); get_return();
      status = move_stepper(tst_board,channel,steps);
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
      fflush(stdout); get_return();
      status = move_stepper(tst_board,channel,steps);
      if (status==0)
      { errors++;
        fprintf(stderr,"test_move_stepper failed\n");
      }
      SLEEP_MS(secs*1000+500);
      printf("done\n");
    } // steps
  } // channel
} // test_move_stepper

//
// Test frequency stepper motor
//
void test_freq_stepper()
{ int channel,status; 
  float frequency;

  printf("Test stepper frequency on board %d\n",tst_board);
  printf("Use scope!\n",tst_board);
  for (channel=0; channel<4; channel+=2)
  { // Set channel for stepper  
    set_mode(tst_board,channel,GB_MODE_STEPG_OFF);
    for (frequency=(float)0.5; frequency<=(float)5000; frequency=frequency*(float)2+(float)0.5)
    {
      printf("channel %d, frequency %4.2f\n",channel,frequency);
      status = freq_stepper(tst_board,channel,frequency);
      if (status==0)
      { errors++;
        fprintf(stderr,"test_freq_stepper failed\n");
      }
      status = move_stepper(tst_board,channel,100);
      get_return();
    } // frequency
  } // channel
} // test_freq_stepper

//
// test stop stepper motor 
//
const char *stop_string[] = {"Passive","Active"};
void test_stop_stepper()
{ int channel,status,mode,repeat; 

  printf("Test stop stepper on board %d\n",tst_board);
  printf("Use scope!\n",tst_board);
  for (channel=0; channel<4; channel+=2)
  { // Set channel for stepper and start motor 
    set_mode(tst_board,channel,GB_MODE_STEPG_PWR);
    freq_stepper(tst_board,channel,100);
    for (mode=0; mode<2; mode++)
    { // On active stop might need to stop several times
      // to see end-state signals change
      // (only needed hen using scope)
      repeat = mode * 5+1;
      while (repeat--)
      {
        move_stepper(tst_board,channel,5000000);
        printf("channel %d, stop %d (%s)\n",channel,mode,stop_string[mode]);
        get_return();
        status = stop_stepper(tst_board,channel,mode);
        if (status==0)
        { errors++;
          fprintf(stderr,"test_stop_stepper failed\n");
        }
        get_return();
      } // repeat 
    } // mode
    printf("channel %d, End stop %d (%s)\n",channel,GB_STOP_OFF,stop_string[GB_STOP_OFF]);
    stop_stepper(tst_board,channel,GB_STOP_OFF); // always end with stop passive
    get_return();
  } // channel
} // test_stop_stepper

//
// test read error status
//
void test_read_error_status()
{ int error,max;
  printf("Test read error status on board %d\n",tst_board);
  // generate some errors first
  set_mode(tst_board,0,GB_MODE_STEPG_OFF);
  move_brushed(tst_board,0,GB_MOVE_A);
  pwm_brushed(tst_board,0,100,100.0);

  max = 16;
  do {
    get_return();
    error = read_error_status(tst_board);
    if (error==-1)
      printf("Read error status failed\n");
    else
      printf("Got %d = %s\n",error,error_string(error));
  } while (error && --max);

  set_mode(tst_board,2,GB_MODE_BRUSH);
  pwm_brushed(tst_board,2,5,100.0);
  move_stepper(tst_board,2,10);  

  max = 16;
  do {
    error = read_error_status(tst_board);
    if (error==-1)
      printf("Read error status failed\n");
    else
      printf("Got %d = %s\n",error,error_string(error));
  } while (error && --max);
  printf("Test error_string with illegal error values\n");
  error = -1;
  printf("%d = %s\n",error,error_string(error));
  error = 10000;
  printf("%d = %s\n",error,error_string(error));
  
} // test_read_error_status


void test_set_pin_mode()
{ int pin;
  int modes[20];
  printf("Test set pin mode on board %d\n",tst_board);
  for (pin=1; pin<=8; pin++)
  { set_pin_mode(tst_board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    get_return();
  }
  for (pin=1; pin<=8; pin++)
  { set_pin_mode(tst_board,pin,GB_PIN_OUTPUT);
    printf("pin %d output\n",pin);
    get_return();
  }
  for (pin=1; pin<=8; pin++)
  { set_pin_mode(tst_board,pin,GB_PIN_ENDSTOP);
    printf("pin %d endmode\n",pin);
    get_return();
  }
  for (pin=1; pin<=8; pin++)
  { set_pin_mode(tst_board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    get_return();
  }
  for (pin=9; pin<=10; pin++)
  { set_pin_mode(tst_board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    get_return();
    set_pin_mode(tst_board,pin,GB_PIN_OUTPUT);
    printf("pin %d output\n",pin);
    get_return();
    set_pin_mode(tst_board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    get_return();
  }
  for (pin=13; pin<=16; pin++)
  { set_pin_mode(tst_board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    get_return();
    set_pin_mode(tst_board,pin,GB_PIN_OUTPUT);
    printf("pin %d output\n",pin);
    get_return();
    set_pin_mode(tst_board,pin,GB_PIN_ADC);
    printf("pin %d ADC\n",pin);
    get_return();
    set_pin_mode(tst_board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    get_return();
  }
  for (pin=18; pin<=20; pin+=2)
  { set_pin_mode(tst_board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    get_return();
    set_pin_mode(tst_board,pin,GB_PIN_OUTPUT);
    printf("pin %d output\n",pin);
    get_return();
    set_pin_mode(tst_board,pin,GB_PIN_DAC);
    printf("pin %d DAC\n",pin);
    get_return();
    set_pin_mode(tst_board,pin,GB_PIN_INPUT);
    printf("pin %d input\n",pin);
    get_return();
  }
  
  printf("Test set all pins mode on board %d\n",tst_board);
  modes[0]  = GB_PIN_ENDSTOP;
  modes[1]  = GB_PIN_OUTPUT;
  modes[2]  = GB_PIN_INPUT;
  modes[3]  = GB_PIN_ENDSTOP;
  modes[4]  = GB_PIN_OUTPUT; 
  modes[5]  = GB_PIN_INPUT;  
  modes[6]  = GB_PIN_ENDSTOP;
  modes[7]  = GB_PIN_OUTPUT; 
  modes[8]  = GB_PIN_INPUT;  
  modes[9]  = GB_PIN_OUTPUT;
  modes[10] = GB_PIN_INPUT; // impossible but should be ignored
  modes[11] = GB_PIN_INPUT; // impossible but should be ignored
  modes[12] = GB_PIN_INPUT;
  modes[13] = GB_PIN_INPUT;
  modes[14] = GB_PIN_ADC;
  modes[15] = GB_PIN_ADC;
  modes[16] = GB_PIN_INPUT; // impossible but should be ignored
  modes[17] = GB_PIN_INPUT; 
  modes[18] = GB_PIN_INPUT; // impossible but should be ignored
  modes[19] = GB_PIN_DAC;
  printf("E O I E O I E O I O - - I I A A - I - D\n");
  set_allpins_mode(tst_board,modes);
  get_return();

  modes[0]  = GB_PIN_OUTPUT;
  modes[1]  = GB_PIN_INPUT;
  modes[2]  = GB_PIN_ENDSTOP;
  modes[3]  = GB_PIN_OUTPUT; 
  modes[4]  = GB_PIN_INPUT;  
  modes[5]  = GB_PIN_ENDSTOP;
  modes[6]  = GB_PIN_OUTPUT; 
  modes[7]  = GB_PIN_INPUT;  
  modes[8]  = GB_PIN_OUTPUT;
  modes[9]  = GB_PIN_INPUT;
  modes[10] = GB_PIN_INPUT; // impossible but should be ignored
  modes[11] = GB_PIN_INPUT; // impossible but should be ignored
  modes[12] = GB_PIN_ADC;
  modes[13] = GB_PIN_ADC;
  modes[14] = GB_PIN_OUTPUT;
  modes[15] = GB_PIN_INPUT;
  modes[16] = GB_PIN_INPUT; // impossible but should be ignored
  modes[17] = GB_PIN_DAC; 
  modes[18] = GB_PIN_INPUT; // impossible but should be ignored
  modes[19] = GB_PIN_OUTPUT;
  printf("O I E O I E O I O I - - A A O I - D - O\n");
  set_allpins_mode(tst_board,modes);
  get_return();
  
  modes[0]  = GB_PIN_INPUT;
  modes[1]  = GB_PIN_ENDSTOP;
  modes[2]  = GB_PIN_OUTPUT;
  modes[3]  = GB_PIN_INPUT;
  modes[4]  = GB_PIN_ENDSTOP;
  modes[5]  = GB_PIN_OUTPUT;
  modes[6]  = GB_PIN_INPUT;
  modes[7]  = GB_PIN_ENDSTOP;
  modes[8]  = GB_PIN_OUTPUT;
  modes[9]  = GB_PIN_OUTPUT;
  modes[10] = GB_PIN_INPUT; // impossible but should be ignored
  modes[11] = GB_PIN_INPUT; // impossible but should be ignored
  modes[12] = GB_PIN_OUTPUT;
  modes[13] = GB_PIN_ADC;
  modes[14] = GB_PIN_ADC;
  modes[15] = GB_PIN_INPUT;
  modes[16] = GB_PIN_INPUT; // impossible but should be ignored
  modes[17] = GB_PIN_DAC; 
  modes[18] = GB_PIN_INPUT; // impossible but should be ignored
  modes[19] = GB_PIN_DAC;
  printf("I E O I E O I E O O - - O A A I - D - D\n");
  set_allpins_mode(tst_board,modes);
  get_return();
  
}// test_set_pin_mode

//
// Test ADC and DAC 
// Assumes 'set_pin_mode' is working 
//
void test_adc_dac()
{ int pin,d,rep;
  float dac,dac_min,dac_max,dac_step;
  printf("Test ADC & DAC on board %d\n",tst_board);
  for (pin=13; pin<=16; pin++)
    set_pin_mode(tst_board,pin,GB_PIN_ADC);
  for (rep=0; rep<4; rep++)
  { for (d=0; d<4; d++)
      printf("Read ADC %d = %5.3f\n",d,read_adc(tst_board,d));
    get_return();
  }
  set_pin_mode(tst_board,18,GB_PIN_DAC);
  set_pin_mode(tst_board,20,GB_PIN_DAC);
  // assume calibrate table has default values
  // so not searching for highest minimum and lowest maximum
  dac_min = dac_calibrate[tst_board][0].min;
  dac_max = dac_calibrate[tst_board][0].max;
  dac_step = (dac_max-dac_min)/4;
  for (dac=dac_min; dac<=dac_max; dac+= dac_step)
  { for (d=0; d<2; d++)
    { printf("Write DAC %d = ~%5.3f\n",d,dac);
      set_dac(tst_board,d,dac);
    }
    get_return();
  }
} // test_adc_dac

static int pin_set[16] = { 1,2,3,4,5,6,7,8,9,10,13,14,15,16,18,20};
void test_outputs()
{ int i,pin,d0,d1;
  printf("Test outputs tst_board %d\n",tst_board);
  
  for (i=0; i<4; i++)
  { d0 = i & 1;
    d1 = (i & 2)>>1;
    printf("Open drain %d %d\n",d0,d1);
    activate_opendrain(tst_board,d0,d1);
    get_return();
  }
  for (pin=0; pin<16; pin++)
    set_pin_mode(tst_board,pin_set[pin],GB_PIN_OUTPUT);
  
  for (pin=0; pin<16; pin++)
  { printf("pin %d high\n",pin_set[pin]);
    set_output_pin_state(tst_board,1<<pin);
    get_return();
  }
  
} // test_outputs

void test_inputs()
{ int i,status,pin,mode[20];
  printf("Test inputs tst_board %d\n",tst_board);
  printf("Performing ten reads\n");
  for (pin=0; pin<20; pin++)
    mode[pin]=GB_PIN_INPUT;
  set_allpins_mode(tst_board,mode);
  for (i=0; i<10; i++)
  { printf("Press return to read\n");
    get_return();
    status = read_inputs(tst_board);
    if (status==-1)
      printf("read inputs failed\n");
    else
      printf("try %d: 0x%06X\n",i+1,status);
  }

} // test_inputs

static char *io_string[] = { 
 " ---   ",
 "input  ",
 "output ",
 "endmode",
 "adc    ",
 "dac    ",
 "i2c    "
};
void test_get_io_setup()
{ int r,ok,i,s,status[20];
  printf("Get I/O status tst_board %d\n",tst_board);
  printf("Performing four reads\n");
  for (r=0; r<4; r++)
  {
   get_return();
   ok = get_io_setup(tst_board,status);
   if (!ok)
   {
     printf("get_io_setup() call failed\n");
     errors++;
   }
   else
   {
     for (i=0; i<20; i++)
     { s = status[i];
       if (s<0 || s>6)
         printf("get_io_setup illegal resturn value: %d\n",s);
       else
         printf("Pin %d=%d:%s\n",i+1,s,io_string[s]);
     }
   }   
  }
} // test_get_io_setup

const static char *move_string[] = {
" Stop ", "  A   ", "  B   ", " Pwr  " };


void test_get_motor_config()
{ int r,m,status;
  gb_motor_type mot_data;
  printf("Get motor config tst_board %d\n",tst_board);
  printf("Performing ten reads\n");
  for (r=0; r<10; r++)
  { printf("Give  motor number 0-3 ");
    m = getchar()-'0';
    get_return();
    status = get_motor_config(tst_board,m,&mot_data);
    printf("Board %d, Motor %d\n",tst_board,m);
    printf("   Mode:%d (%s)\n",mot_data.mode,mode_string[mot_data.mode]);
    if (mot_data.mode==0)
      continue;
    if (mot_data.mode==GB_MODE_BRUSH ||
        mot_data.mode & GB_MODE_STEP_MASK)
    {
      printf("   Endstop:");
      if (mot_data.endmode & 0x01)
      { // A is on 
        if (mot_data.endmode & 0x04)
          printf("A-high ");
        else
          printf("A-low  ");
      }
      else
        printf("A-off  ");
      if (mot_data.endmode & 0x02)
      { // B is on 
        if (mot_data.endmode & 0x08)
          printf("B-high\n");
        else
          printf("B-low\n");
      }
      else
        printf("B-off\n");
      printf("Short/hot: %s\n",short_string[mot_data.short_hot_stop]);
      if (mot_data.mode==GB_MODE_BRUSH)
      {
         printf("Ramp up speed   : %s\n",ramp_string[mot_data.ramp_up]);
         printf("Ramp down speed : %s\n",ramp_string[mot_data.ramp_down]);
         printf("Ramp halt speed : %s\n",ramp_string[mot_data.ramp_halt]);
         printf("   Frequency : %d\n",mot_data.frequency);
        printf("   Duty Cycle: %2.1f\n",mot_data.duty_cycle/10.0);
      } // brushed mode 
      else
      { // stepper mode
         printf("   Freq: %6.2f\n",mot_data.frequency/256.0);
      }
    } // motor mode 
  }
} // test_get_motor_config 
  

void test_get_motor_status_missed()
{ int r,m,status;
  gb_motor_type mot_data;
  printf("Get motor status tst_board %d\n",tst_board);
  printf("Performing ten reads\n");
  for (r=0; r<10; r++)
  { printf("Give  motor number 0-3 ");
    m = getchar()-'0';
    get_return();
    status = get_motor_status(tst_board,m,&mot_data);
    if (status==0)
    { printf("get_motor_status failed\n");
      errors++;
      continue;
    }
    status = get_motor_missed(tst_board,m,&mot_data);
    if (status==0)
    { printf("get_motor_missed failed\n");
      errors++;
      continue;
    }
    printf("Board %d, Motor %d\n",tst_board,m);
    printf("   Move:%d (%s)\n",mot_data.move,move_string[mot_data.move]);
    if (mot_data.mode & GB_MODE_STEP_MASK)
    { printf("   Steps remaining: %d\n",mot_data.remain);
      printf("   Steps missed:    %d\n",mot_data.missed);
    }
  }
} // test_get_motor_status

void test_dcc()
{ int p,r,dc,m,status,channel;
  unsigned char message[6];
  int time,error;
  
  channel = 0;
  
  
  
  printf("Test DCC on board %d\n",tst_board);
  printf("This test WILL need a scope!\n");
  
  // Disabled end-stops and short stop
  message[0]=CMD_START_VAL;
  message[1]=CMD_STOPSHORT;
  message[2]=(tst_board<<2) | channel;
  message[3]=0; // No endstops, no short
  message[4]=CMD_STOP_VAL;
  write_uart(message,5); 

  set_mode(tst_board,channel,GB_MODE_DCC);
 
  /*
  for (p=16; p<255; p+= 32)
  {
    dcc_config(tst_board,channel,4,p,0);
    printf("Preamble %d\n",p);
    get_return();
  }

  // Can't test the complete 255 repeat as my scope memory is not deep enough
  // Assume that if 4,5,6... works, 255 wil work too.
  message[0]=0x81;
  message[1]=0x00;
  message[2]=0x81;
  for (r=4; r<8; r++)
  { 
    do {
      printf("Repeat=%d\n",r);
      dcc_config(tst_board,channel,r,16,0);
      send_dcc_mess(tst_board,0x0F,3,message);
      printf("n=next, <return> is re-run\n",r);
      p = get_return();
    } while (p!='n');
  }

  // System back to default 
  dcc_config(tst_board,channel,4,16,0);
  // test message flood gives error
  // Can't really test for absence of illegal bit patterns

  // make sure no errors pending
  do {
    error = read_error_status(tst_board);
  } while (error);
  */

  printf("test system flooding\n");
  // bits: one = 116usec,  zero=200usec,
  // Message is 16x preamble, 0 10000001 0 00000000 0 10000001 0 10000001 
  // 22 ones, 30 zeros = 8.5 msec
  // 
  // Idle packet 16x preamble 0 11111111 0 00000000 0 11111111
  // 32 ones, 11 zeros = 5.9 msec
  // total is 14.4 ms send five times = 72 msec 

  // Overflow queue: 
  for (m=0; m<100; m++)
  { send_dcc_mess(tst_board,0x0F,3,message);
    // SLEEP_MS(time);
  }

  status=0;
  do {
    error = read_error_status(tst_board);
    if (error && error!=ERROR_DCC_OVFL)
    { printf("unexpected error: %s\n",error_string(error));
      errors++;
    }
    else
      status++;

  } while (error);
  if (status==0)
  { printf("Missing DCC overflow error\n");
    errors++;
  }
  else
    printf("Reported %d DCC overflow errors\n",status);
  printf("Press return to continue ");
  get_return();


  // try again with pacing
  for (m=0; m<100; m++)
  { send_dcc_mess(tst_board,0x0F,3,message);
    SLEEP_MS(68);
  }
  error = read_error_status(tst_board);
  if (error)
  { printf("unexpected error: %s\n",error_string(error));
    errors++;
  }
  else
    printf("No errors detected\n");
  printf("Press return to continue ");
  get_return();

/*
  skipping DCC testing as I can't do more then look at the scope
  and check against existing HW. 
  for (r=-100; r<100; r+= 20)
  { time = 4*(4*8+16)*34; // repeat * (4*8 bits + preamble) * 34 milisec.
    dcc_config(tst_board,channel,4,16,dc);
    printf("DC %d (100 packets)\n",dc);
    for (m=0; m<100; m++)
    { send_dcc_mess(tst_board,0x0F,3,message);
      SLEEP_MS(time);
    }
    get_return();
  }
*/
  // DCC message 
 
} // test_dcc

//
//
//

int start_set[3] = { 0, 8300000, -8300000};

void test_quad()
{ int p,l,d,pos,err,start,t;
  int errors;
  float dc;
  
  printf("Test QUAD modes\n");
  errors = 0;
  set_mode(tst_board,tst_chan,GB_MODE_BRUSH);
  for (p=-8000000; p<8000000; p+=1000000)
  { quad_on(tst_board,tst_chan,1,p,0);
    quad_read(tst_board,tst_chan,&pos,&err);
    if (pos!=p)
    { printf("Quad position error set=%d, read pos=%d\n",p,pos);
      errors++;
    }
    if (err!=0)
    { printf("Quad read error set:%d\n",err);      
      errors++;
    }
  }
  if (errors)
    return;
  printf("Quad set/read test passed\n");
/*
  for (t=0; t<3; t++)
  { start = start_set[t];
    switch (t)
    {
    case 0 : dc =  50.0; break;
    case 1 : dc =  75.0; break;
    case 2 : dc = 100.0; break;
    }
      
    quad_on(tst_board,tst_chan,1,start,1);
    printf("Quad At %d\n",start);
    for (p=-5000; p<5000; p+=1000)
    { printf("Goto %d 50%% (press return) ",p+start); 
      quad_goto(tst_board,tst_chan,p+start,50.0);
      get_return();
    }
  }
*/
  quad_on(tst_board,tst_chan,1,0,1);
  printf("Quad At 0\n");
  printf("Limits: all on, 1000..-1000, 30% @200\n");
  
int quad_limit(int board,int channel,
               int max_on, int min_on,int slow_on,
               int max_pos,int min_pos,
               int slow_dist,float slow_dc);  
  
  quad_limit(tst_board,tst_chan,1,1,1,1000,-1000,200,30.0);
  printf("goto 500 @75 (press return when done)\n");
  quad_goto(tst_board,tst_chan,500,75.0);
  get_return();
  printf("goto 2000 @100%% (press return when done)\n");
  quad_goto(tst_board,tst_chan,2000,100.0);
  get_return();
  printf("goto 500 @75%% (press return when done)\n");
  quad_goto(tst_board,tst_chan,500,75.0);
  get_return();
  printf("goto 0 @100%% (press return when done)\n");
  quad_goto(tst_board,tst_chan,0,100.0);
  get_return();
  printf("goto -2000 @60%%(press return when done)\n");
  quad_goto(tst_board,tst_chan,-2000,60.0);
  get_return();
  

} // test_quad


//
// Testing comprises of calling the routine
// Then using the GUI to read back
// Which means the GUI had better be OK as well
//
void test_endstop2()
{ int channel,mode_A,mode_B,status,skip;
  int filt_A,filt_B;
  skip = 0;
  printf("Test end-stop & filter mode on board %d (s=skip)\n",tst_board);
  for (channel=0; channel<4 && !skip; channel++)
  {
    for (mode_A=0; mode_A<3; mode_A++)
    {
      for (mode_B=0; mode_B<3; mode_B++)
      {
        filt_A = 85*mode_A;
        filt_B = 75*mode_B+75;
        status = set_endstop2(tst_board,channel,mode_A,mode_B,filt_A,filt_B);
        printf("channel %d, mode A %d (%s) mode B %d (%s)\n",channel,
           mode_A,endstop_string[mode_A],
           mode_B,endstop_string[mode_B]);
        if (status==0)
        { errors++;
          fprintf(stderr,"test_endstop failed\n");
        }
        skip=get_return()=='s';
      } // mode B
    } // mode A
  } // channel
} // test_endstop2


void test_shorthot()
{ int channel,mode_A,mode_B,status,skip,shrt;
  printf("Test short/hot mode on board %d\n",tst_board);
  skip=0;
  for (channel=0; channel<4 && !skip; channel++)
  {
    for (shrt=GB_SHORT_NONE; shrt<=GB_SHORT_SYST; shrt++)
    {
      printf("channel %d, short/hot mode %d (Stop %s)\n",
          channel,shrt,short_string[shrt]);
      status = set_shorthot(tst_board,channel,shrt);
      get_return();
    }
  }
} // test_endstop_short
