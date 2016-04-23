

#include "gb_drivers.h"

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

static unsigned char gb_command4[] = { 0xA0,0x00,0x00,0x50};
static unsigned char gb_command5[] = { 0xA0,0x00,0x00,0x00,0x50};
static unsigned char gb_command6[] = { 0xA0,0x00,0x00,0x00,0x00,0x50};
static unsigned char gb_command7[] = { 0xA0,0x00,0x00,0x00,0x00,0x00,0x50};
static unsigned char gb_command8[] = { 0xA0,0x00,0x00,0x00,0x00,0x00,0x00,0x50};
static unsigned char gb_command9[] = { 0xA0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50};
static unsigned char gb_command10[]= { 0xA0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50};



gb_motor_type gb_motor[4][4] = 
{
  {  // board 0
    { // motor 0 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
    { // motor 1 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
    { // motor 2 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
    { // motor 3 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
  }, // board 0
  
  {  ////// board 1
    { // motor 0 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
    { // motor 1 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
    { // motor 2 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
    { // motor 3 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
  }, // board 1
  
  {  ////// board 2
    { // motor 0 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
    { // motor 1 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
    { // motor 2 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
    { // motor 3 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
  }, // board 2
  
  {  ////// board 3
    { // motor 0 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
    { // motor 1 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
    { // motor 2 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;  
      }    
    },
    { // motor 3 
      MOT_MODE_OFF,// byte mode;
      0,           // byte endmode;
      MOT_PWR_OFF, // byte move;
      0,           // byte ramp_up;
      0,           // byte ramp_down;
      0,           // byte ramp_halt;
      0,           // byte short_hot_stop;
      0,           // uint32_t flags;
      0,           // uint32_t frequency;
      0,           // uint32_t duty_cycle;
      0,           // int32_t  steps;
      0,           // int32_t  remain;
      0,           // int32_t  missed;
      // quad 
      { 0,         // bool on;
        0,0,       // bool max_stop,min_stop;
        0,         // bool slow_on;
        0,         // int  position;
        0,         // uint32_t duty_cycle;
        0,0,       // int  max_pos,min_pos;
        0,         // int  goto_pos;
        0          // int  slow_dist;    
      }  
    },
  }  // board 3
}; // motor[4][4]  



// Internal copy of the state of the boards
static gb_board_type gb_board[4] =
{
  {// Board 0
   0,         // present
   0,         // sync_mode
   0x0F,      // adc_enable
   0x00,      // dac_enable
   0x0000C3FF,// input_ports
   0x00,      // output_ports
   0x00,      // inputs_status
   0x00,      // outputs_status
   0x00,      // OD (open drain)
   {0.0,0.0,0.0,0.0}, // adc[4] last read value
   {0.0,0.0},  // dac[2] Last set value
   0           // attentionmode
  },
  {// Board 1
   0,         // present
   0,         // sync_mode
   0x0F,      // adc_enable
   0x00,      // dac_enable
   0x0000C3FF,// input_ports
   0x00,      // output_ports
   0x00,      // inputs_status
   0x00,      // outputs_status
   0x00,      // OD (open drain)
   {0.0,0.0,0.0,0.0}, // adc[4] last read value
   {0.0,0.0}, // dac[2] Last set value
   0          // attentionmode
  },
  {// Board 2
   0,         // present
   0,         // sync_mode
   0x0F,      // adc_enable
   0x00,      // dac_enable
   0x0000C3FF,// input_ports
   0x00,      // output_ports
   0x00,      // inputs_status
   0x00,      // outputs_status
   0x00,      // OD (open drain)
   {0.0,0.0,0.0,0.0}, // adc[4] last read value
   {0.0,0.0}, // dac[2] Last set value
   0          // attentionmode
  },
  { // Board 3
   0,         // present
   0,         // sync_mode
   0x0F,      // adc_enable
   0x00,      // dac_enable
   0x0000C3FF,// input_ports
   0x00,      // output_ports
   0x00,      // inputs_status
   0x00,      // outputs_status
   0x00,      // OD (open drain)
   {0.0,0.0,0.0,0.0}, // adc[4] last read value
   {0.0,0.0}, // dac[2] Last set value
   0          // attentionmode
  }
};  // board[4]


// Keep track of pin status
// Most pins are a digital input except the ADC
// (To keep the code readable I did NOT replace each 1 with GB_PIN_INPUT)
// At some point I should move this into the board structure 
static int gb_pin_mode[4][20]   = {
 { 1,1,1,1,1,1,1,1, 1,1, 0,0, GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC, 0, 1, 0, 1},
 { 1,1,1,1,1,1,1,1, 1,1, 0,0, GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC, 0, 1, 0, 1},
 { 1,1,1,1,1,1,1,1, 1,1, 0,0, GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC, 0, 1, 0, 1},
 { 1,1,1,1,1,1,1,1, 1,1, 0,0, GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC,GB_PIN_ADC, 0, 1, 0, 1}
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
// (most code is for it to work under windows)
// com_port indicates the port to use under windows
// If is is zero the program tries to find a working comport
// If there is only one it uses that
// if there are none or more then one it complains and exits
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

//
// Send 'stop all' command
//
unsigned char gb_stop_all_cmnd[4] = {0xA0,CMD_STOPALL,CMD_STOP2ND,0x50};
void stop_all()
{ write_uart(gb_stop_all_cmnd,4);
}

//
// send emergensy stop command
//
unsigned char gb_emerg_stop_cmnd[4] = {0xA0,CMD_PWR_OFF,CMD_STOP2ND,0x50};
void emergency_stop()
{ write_uart(gb_emerg_stop_cmnd,4);
}


//
// Get board vesion
// Board version MAJOR.MINOR is returned
// as MAJOR*100+MINOR
// return 0 on fail
// 
int get_version(int board)
{ unsigned char id,tx[8],rec[7],bytes;
  GB_CHECK(board>=0  && board<=3,   "quad_read illegal board\n");
  id = (board<<2);
  tx[0] = CMD_START_VAL;
  tx[1] = CMD_VERSION;
  tx[2] = id;
  tx[3] = CMD_STOP_VAL;
  tx[4] = CMD_STOP_VAL;
  tx[5] = CMD_STOP_VAL;
  tx[6] = CMD_STOP_VAL; 
  tx[7] = CMD_STOP_VAL; 
  write_uart(tx,8);
  bytes = read_uart(rec,4);
  if (bytes!=4 || rec[0]!=id || rec[1]!=CMD_VERSION)
    return 0;
  return rec[2]*100 + rec[3];
} // get_version

//
// Setup the mode of a channel
//
// mode is one off:
// GB_MODE_OFF    0
// GB_MODE_BRUSH  8
// GB_MODE_STEPG  1
// GB_MODE_DCC    9
//
// return 1 on success
// return 0 on error
int  set_mode(int board,int channel,int mode)
{
  GB_CHECK(board>=0   && board<=3,  "set_channel_mode illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"set_channel_mode illegal channel\n");
  GB_CHECK( mode==GB_MODE_OFF   || mode==GB_MODE_BRUSH ||
            mode==GB_MODE_DCC   ||
            mode==GB_MODE_STEPG_OFF || mode==GB_MODE_STEPP_OFF ||
            mode==GB_MODE_STEPG_PWR || mode==GB_MODE_STEPP_PWR ,
            "set_channel_mode illegal mode\n");
  GB_CHECK ( !((mode & MOT_MODE_STEP_MASK) && (channel==1 || channel==3)),
	     "set_channel_mode illegal channel/mode\n");

  gb_command5[1] = CMD_OPMODE;
  gb_command5[2] = board<<2 | channel;
  gb_command5[3] = mode;

  gb_motor[board][channel].mode = mode;
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
// Set J3 pins as endmode
//
// stop_A, stop_B is on off:
// GB_ENDSTOP_OFF    0
// GB_ENDSTOP_LOW    1  Stop if pin low
// GB_ENDSTOP_HIGH   2  Stop if pin high
//
// return 1 on success
// return 0 on error
int  set_endstop(int board,int channel,int stop_A,int stop_B)
{ int pin;
  GB_CHECK(board>=0   && board<=3,  "set_endstop illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"set_endstop illegal channel\n");
  GB_CHECK(stop_A>=GB_ENDSTOP_OFF && stop_A<=GB_ENDSTOP_HIGH,"set_endstop illegal stop A mode\n");
  GB_CHECK(stop_B>=GB_ENDSTOP_OFF && stop_B<=GB_ENDSTOP_HIGH,"set_endstop illegal stop B mode\n");
  // Set internal status
  gb_motor[board][channel].endmode = 0;
  pin = channel<<1;
  switch (stop_A)
  {
  case GB_ENDSTOP_OFF:
    gb_pin_mode[board][pin+1] = GB_PIN_INPUT;
    break;
  case GB_ENDSTOP_LOW:
    gb_pin_mode[board][pin+1] = GB_PIN_ENDSTOP;
    gb_motor[board][channel].endmode |= 0x01;
    break;
  case GB_ENDSTOP_HIGH:
    gb_pin_mode[board][pin+1] = GB_PIN_ENDSTOP;
    gb_motor[board][channel].endmode |= 0x05;
    break;
  }
  switch (stop_B)
  {
  case GB_ENDSTOP_OFF:
    gb_pin_mode[board][pin] = GB_PIN_INPUT;
    break;
  case GB_ENDSTOP_LOW:
    gb_pin_mode[board][pin] = GB_PIN_ENDSTOP;
    gb_motor[board][channel].endmode |= 0x02;
    break;
  case GB_ENDSTOP_HIGH:
    gb_pin_mode[board][pin] = GB_PIN_ENDSTOP;
    gb_motor[board][channel].endmode |= 0x0A;
    break;
  }

  gb_command5[1] = CMD_STOPSHORT;
  gb_command5[2] = board<<2 | channel;
  gb_command5[3] = gb_motor[board][channel].endmode |
                   (gb_motor[board][channel].short_hot_stop<<4);
    write_uart(gb_command5,5);
  return 1;
} // set_endstop

//
// Set short/hot mode
// Note: for SW revision 2.4 and higher replaced
// with set_shorthot.. routine
//
int set_short(int board,int channel,int shrt)
{
  GB_CHECK(board>=0   && board<=3,  "set_short illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"set_short illegal channel\n");
  GB_CHECK(shrt>=GB_SHORT_NONE && shrt<=GB_SHORT_SYST,"set_short illegal short mode\n");
  gb_motor[board][channel].short_hot_stop = shrt;
  gb_command5[1] = CMD_STOPSHORT;
  gb_command5[2] = board<<2 | channel;
  gb_command5[3] = gb_motor[board][channel].endmode |
                   (gb_motor[board][channel].short_hot_stop<<4);
  write_uart(gb_command5,5);
  return 1;
} // set_short

//
// Set short/hot mode
// set short/hot mode but does NOT touch the end-stops
// Suported only in SW release 2.4 and higher 
//
int set_shorthot(int board,int channel,int shorthot)
{ 
  GB_CHECK(board>=0   && board<=3,  "set_shorthot illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"set_shorthot illegal channel\n");
  GB_CHECK(shorthot>=GB_SHORT_NONE && shorthot<=GB_SHORT_SYST,"set_shorthot illegal short mode\n");
  gb_motor[board][channel].short_hot_stop = shorthot;
  gb_command5[1] = CMD_SHORTHOT;
  gb_command5[2] = board<<2 | channel;
  gb_command5[3] = gb_motor[board][channel].short_hot_stop<<4;
  write_uart(gb_command5,5);
  return 1;
} // set_shorthot


//
// Set Endstops & endstop glitch filter
//
int  set_endstop2(int board,int channel,int stop_A,int stop_B,
                  int filter_A,int filter_B)
{ int pin;
  GB_CHECK(board>=0   && board<=3,  "set_ramps illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"set_ramps illegal channel\n");
  GB_CHECK(stop_A>=GB_ENDSTOP_OFF && stop_A<=GB_ENDSTOP_HIGH,"set_endstop2 illegal stop A mode\n");
  GB_CHECK(stop_B>=GB_ENDSTOP_OFF && stop_B<=GB_ENDSTOP_HIGH,"set_endstop2 illegal stop B mode\n");
  GB_CHECK(filter_A>=0 && filter_A<=255,"set_endstop2 illegal filter A value\n");
  GB_CHECK(filter_B>=0 && filter_B<=255,"set_endstop2 illegal filter B value\n");
  gb_motor[board][channel].endmode = 0;
  pin = channel<<1;
  switch (stop_A)
  {
  case GB_ENDSTOP_OFF:
    gb_pin_mode[board][pin+1] = GB_PIN_INPUT;
    break;
  case GB_ENDSTOP_LOW:
    gb_pin_mode[board][pin+1] = GB_PIN_ENDSTOP;
    gb_motor[board][channel].endmode |= 0x01;
    break;
  case GB_ENDSTOP_HIGH:
    gb_pin_mode[board][pin+1] = GB_PIN_ENDSTOP;
    gb_motor[board][channel].endmode |= 0x05;
    break;
  }
  switch (stop_B)
  {
  case GB_ENDSTOP_OFF:
    gb_pin_mode[board][pin] = GB_PIN_INPUT;
    break;
  case GB_ENDSTOP_LOW:
    gb_pin_mode[board][pin] = GB_PIN_ENDSTOP;
    gb_motor[board][channel].endmode |= 0x02;
    break;
  case GB_ENDSTOP_HIGH:
    gb_pin_mode[board][pin] = GB_PIN_ENDSTOP;
    gb_motor[board][channel].endmode |= 0x0A;
    break;
  }
  
  gb_command7[1] = CMD_ENDSTOP;
  gb_command7[2] = board<<2 | channel;
  gb_command7[3] = gb_motor[board][channel].endmode;
  gb_command7[4] = filter_A;
  gb_command7[5] = filter_B;
  write_uart(gb_command7,7);
  return 1;
} // set_endstop2




//
// Set brushed ramps
//
int set_brush_ramps(int board,int channel,int ramp_up,int ramp_down,int ramp_halt)
{
  GB_CHECK(board>=0   && board<=3,  "set_ramps illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"set_ramps illegal channel\n");
  GB_CHECK(ramp_up>=GB_RAMP_NONE && ramp_up<=GB_RAMP_700,"set_ramps illegal ramp up\n");
  GB_CHECK(ramp_up>=GB_RAMP_NONE && ramp_up<=GB_RAMP_700,"set_ramps illegal ramp down\n");
  GB_CHECK(ramp_up>=GB_RAMP_NONE && ramp_up<=GB_RAMP_700,"set_ramps illegal ramp halt\n");
  // Internal structure upkeep
  gb_motor[board][channel].ramp_up = ramp_up;
  gb_motor[board][channel].ramp_down = ramp_down;
  gb_motor[board][channel].ramp_halt = ramp_halt;
  gb_command6[1] = CMD_SET_RAMP;
  gb_command6[2] = board<<2 | channel;
  gb_command6[3] = gb_motor[board][channel].ramp_up |
                   (gb_motor[board][channel].ramp_down<<4);
  gb_command6[4] = gb_motor[board][channel].ramp_halt;
  write_uart(gb_command6,6);
  return 1;
} // set_brush_ramps

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
// return 1 on success
// return 0 on error
int  move_brushed(int board,int channel, int direction)
{
  GB_CHECK(board>=0   && board<=3,  "move_brushed illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"move_brushed illegal channel\n");
  GB_CHECK(direction>=0 && direction<=2,"move_brushed illegal direction\n");
  GB_CHECK(gb_motor[board][channel].mode==GB_MODE_BRUSH,"move_brushed not in brushed mode\n");
  gb_command5[1] = CMD_LINON;
  gb_command5[2] = board<<2 | channel;
  gb_command5[3] = direction;
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
  GB_CHECK(gb_motor[board][channel].mode==GB_MODE_BRUSH,"move_brushed not in brushed mode\n");
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
  GB_CHECK(gb_motor[board][channel].mode & GB_MODE_STEP_MASK,"move_stepper not in stepper mode\n");
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
  GB_CHECK(gb_motor[board][channel].mode & GB_MODE_STEP_MASK,"stop_stepper not in stepper mode\n");
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
  GB_CHECK(gb_motor[board][channel].mode & GB_MODE_STEP_MASK,"freq_stepper not in stepper mode\n");
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
"**Obsolete error code 0x04",                     //                    0x0004
"Brush freq. error: Motor not in brushed mode",   // ERROR_LINFREQ      0x0005
"Duty cycle error: Motor not in brushed mode",    // ERROR_LINDC        0x0006
"Illegal linear frequency given",                 // ERROR_ILLPWMFREQ   0x0007
"Illegal duty cycle given",                       // ERROR_ILLPWMDC     0x0008
"Brushed motor start but not given frequency.",   // ERROR_NOLINFREQ    0x0009
"Stepper motor start but not given frequency.",   // ERROR_NOSTEPFREQ   0x000A
"Step command: Motor not in stepper mode",        // ERROR_STEP         0x000B
"Step freq. command: Motor not in stepper mode",  // ERROR_STEPFREQ     0x000C
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
"Enable 0 was negated",                           // ERROR_A_SEEN       0x001B
"Enable 1 was negated",                           // ERROR_B_SEEN       0x001C
"Enable 2 was negated",                           // ERROR_C_SEEN       0x001D
"Enable 3 was negated",                           // ERROR_D_SEEN       0x001E
"DCC illegal message (length)",                   // ERROR_DCC_MESS     0x001F
"DCC message queue overflow",                     // ERROR_DCC_OVFL     0x0020
"Illegal error status code"
};
const char *error_string(int error_number)
{
  if (error_number<0 || error_number>GB_MAX_ERROR)
    error_number = GB_MAX_ERROR;
  return   gb_error_text[error_number];
} // error_string

//
// Get motor configuration from board
//
// returns 13 bytes:
// ID, CMD, mode, endstop+short,
// freq:MS, freq:MM, freq:LS,
// duty-cycle:MS, duty-cycle:LS,
// Ramp-up+Ramp-down,  Ramp-halt,
// 0, 0
int get_motor_config(int brd,int mot,gb_motor_type *status)
{ unsigned char id,rec[16],bytes;

  id = (brd<<2) | mot;
  gb_command4[1] = CMD_MOT_CONFIG;
  gb_command4[2] = id;
  write_uart(gb_command4,4);
  bytes = read_uart(rec,13);
  if (bytes!=13 || rec[0]!=id || rec[1]!=CMD_MOT_CONFIG)
    return 0;

  status->mode            =rec[2] & 0x1F;

  status->endmode         =rec[3] & 0xF;
  status->short_hot_stop  =rec[3]>>4;

  status->frequency       =(rec[4]<<16) + (rec[5]<<8)+rec[6];
  status->duty_cycle      =(rec[7]<<8)+rec[8];
  status->ramp_up         =(rec[9] & 0x0F);
  status->ramp_down       =(rec[9] >>4);
  status->ramp_halt       =(rec[10] & 0x0F);
  // Internal structure upkeep
  gb_motor[brd][mot].mode    = status->mode;
  gb_motor[brd][mot].endmode = status->endmode;
  gb_motor[brd][mot].short_hot_stop    = status->short_hot_stop;
  gb_motor[brd][mot].frequency  = status->frequency;
  gb_motor[brd][mot].duty_cycle = status->duty_cycle;
  gb_motor[brd][mot].ramp_up   = status->ramp_up;
  gb_motor[brd][mot].ramp_down = status->ramp_down;
  gb_motor[brd][mot].ramp_halt = status->ramp_halt;
  return 1;
} // get_motor_config

//
// Get motor status from board
//
// returns 6 bytes:
// ID, CMD, move,
// step_count:MS, step_count:MM, step_count:LS
//
int get_motor_status(int brd,int mot,gb_motor_type *status)
{ unsigned char id,rec[9],bytes;

  id = (brd<<2) | mot;
  gb_command4[1] = CMD_MOT_STATUS;
  gb_command4[2] = id;
  write_uart(gb_command4,4);
  bytes = read_uart(rec,6);
  if (bytes!=6 || rec[0]!=id || rec[1]!=CMD_MOT_STATUS)
    return 0;
  status->move   =rec[2] & 0x0F;
  status->remain =(rec[3]<<16) + (rec[4]<<8)+rec[5];
  // Internal structure upkeep
  gb_motor[brd][mot].move   = status->move;
  gb_motor[brd][mot].remain = status->remain;

  return 1;
} // get_motor_status

//
// Get motor missed steps from board
//
// returns 8 bytes:
// ID, CMD,
// missed:MS, missed:MM, missed:LS
// three times 0 
//
int get_motor_missed(int brd,int mot,gb_motor_type *status)
{ unsigned char id,rec[8],bytes;
  id = (brd<<2) | mot;
  gb_command4[1] = CMD_MOT_MISSED;
  gb_command4[2] = id;
  write_uart(gb_command4,4);
  bytes = read_uart(rec,8);
  if (bytes!=8 || rec[0]!=id || rec[1]!=CMD_MOT_MISSED)
    return 0;
  status->missed =(rec[2]<<16) + (rec[3]<<8)+rec[4];
  // last 3 bytes ignored: For Future Use
  // Internal structure upkeep
  gb_motor[brd][mot].missed = status->missed;
  return 1;
} // get_motor_missed


//
// Send DCC message
// Board is usual 0-3
// channel is special here
// The LS 4 bits indicate to which channel(s) to send
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
// The DCC check byte is not send!
//
int send_dcc_mess(int board,int channel,int length, unsigned char *data)
{ int l;
  GB_CHECK(board>=0  && board<=3,   "send_dcc_mess illegal board\n");
  GB_CHECK(channel>0 && channel<=15,"send_dcc_mess illegal channel set\n");
  GB_CHECK(length>=2 && length<=5,  "send_dcc_mess illegal length\n");
  gb_command10[1] = CMD_DCC_MESS;
  gb_command10[2] = board<<2;
  gb_command10[3] = (channel<<4) | length;
  for (l=0; l<length; l++)
    gb_command10[4+l] = data[l];
  // Leave unused bytes as they where
  write_uart(gb_command10,10);
  return 1;
} // send_dcc_mess

//
// DCC configure
// This routine does NOT support the 'no idle' flag
// 
int dcc_config(int board,int channel, int repeat,int preamble,int dc)
{
  GB_CHECK(board>=0  && board<=3,   "dcc_config illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"dcc_config illegal channel\n");
  GB_CHECK(repeat>=4 && repeat<=255,"dcc_config illegal repeat\n");
  GB_CHECK(preamble>=14 && preamble<=255,"dcc_config illegal preamble\n");
  GB_CHECK(dc>=-100 && dc<=100,"dcc_config illegal dc\n");
  gb_command8[1] = CMD_DCC_CONFIG;
  gb_command8[2] = (board<<2)|channel;
  gb_command8[3] = repeat;
  gb_command8[4] = preamble;
  gb_command8[5] = dc;
  gb_command8[6] = 0; 
  // Leave unused bytes as they where
  write_uart(gb_command8,8);
  return 1;
} // dcc_config

int dcc_config_idle(int board,int channel, int repeat,int preamble,int dc,int no_idle)
{
  GB_CHECK(board>=0  && board<=3,   "dcc_config illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"dcc_config illegal channel\n");
  GB_CHECK(repeat>=4 && repeat<=255,"dcc_config illegal repeat\n");
  GB_CHECK(preamble>=14 && preamble<=255,"dcc_config illegal preamble\n");
  GB_CHECK(dc>=-100 && dc<=100,"dcc_config illegal dc\n");
  gb_command8[1] = CMD_DCC_CONFIG;
  gb_command8[2] = (board<<2)|channel;
  gb_command8[3] = repeat;
  gb_command8[4] = preamble;
  gb_command8[5] = dc;
  gb_command8[6] = no_idle ? 1 : 0; 
  // Leave unused bytes as they where
  write_uart(gb_command8,8);
  return 1;
} // dcc_config_idle

/****************************************\
 *                                      *
 * Q u a d r a t u r e   e n c o d e r  *
 *                                      *
\****************************************/

// Quadrature encoder on/off
int quad_on(int board,int channel, int on,int position,int reverse)
{ 
  GB_CHECK(board>=0  && board<=3,   "quad_on illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"quad_on illegal channel\n");
  GB_CHECK(position>=-0x07FFFFF && position<=0x07FFFFF,"quad_on illegal position\n");
  GB_CHECK(gb_motor[board][channel].mode==GB_MODE_BRUSH,"quad_on not in brushed mode\n");
  gb_command8[1] = CMD_QUAD;
  gb_command8[2] = (board<<2)|channel;
  gb_command8[3] = reverse ? 1 : 0;
  gb_command8[3] |= (on ? 0x10 : 0);
  gb_command8[4] = (position>>16) & 0xFF;
  gb_command8[5] = (position>>8) & 0xFF;
  gb_command8[6] =  position & 0xFF;
  write_uart(gb_command8,8);
  if (on)    
    gb_motor[board][channel].flags |= QUAD_ON;
  else
    gb_motor[board][channel].flags &= ~QUAD_ON;
//  gb_motor[board][channel].quad.
    
  return 1;
} // quad on

// Quadrature encoder read
int quad_read(int board,int channel, int *position,int *errors)
{ unsigned char id,rec[7],bytes;
  GB_CHECK(board>=0  && board<=3,   "quad_read illegal board\n");
  GB_CHECK(gb_motor[board][channel].flags&QUAD_ON,"quad_read not in quadrature mode\n");
  id = (board<<2) | channel;
  gb_command4[1] = CMD_QUAD_READ;
  gb_command4[2] = id;
  write_uart(gb_command4,4);
  bytes = read_uart(rec,7);
  if (bytes!=7 || rec[0]!=id || rec[1]!=CMD_QUAD_READ)
    return 0;
  *position =(rec[2]<<16) + (rec[3]<<8)+rec[4];
  // sign extend from 24 to 32 bits 
  if (*position & 0x00800000)
    *position |= 0xFF000000;
  *errors   =(rec[5]<<8)+rec[6];
  return 1;
} // quad_read

// Quadrature encoder goto
int quad_goto(int board,int channel,int position,float duty_cycle)
{ int int_dc;
int m;
    
  GB_CHECK(board>=0  && board<=3,   "quad_on illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"quad_on illegal channel\n");
  GB_CHECK(gb_motor[board][channel].flags&QUAD_ON,"quad_goto not in quadrature mode\n");
  GB_CHECK(position>=-0x07FFFFF && position<=0x07FFFFF,"quad_on illegal position\n");
  GB_CHECK(duty_cycle>=0.0 && duty_cycle<=100.0,"quad_goto illegal duty_cycle\n");
  gb_command9[1] = CMD_QUAD_GOTO;
  gb_command9[2] = (board<<2)|channel;
  gb_command9[3] = (position>>16) & 0xFF;
  gb_command9[4] = (position>>8) & 0xFF;
  gb_command9[5] =  position & 0xFF;
  int_dc = (int)(duty_cycle*10+0.5);
  gb_command9[6] = int_dc>>8;
  gb_command9[7] = int_dc & 0xFF;
  write_uart(gb_command9,9);
  return 1;
} // quad_goto

// Quadrature encoder limit & slow settings
int quad_limit(int board,int channel,
               int max_on, int min_on,int slow_on,
               int max_pos,int min_pos,
               int slow_dist,float slow_dc)
{ int dc,m;
  unsigned char flags,message[15];  
  GB_CHECK(board>=0  && board<=3,   "quad_slow illegal board\n");
  GB_CHECK(channel>=0 && channel<=3,"quad_slow illegal channel\n");
  GB_CHECK(gb_motor[board][channel].flags&QUAD_ON,"quad_slow not in quadrature mode\n");
  GB_CHECK(max_pos>=-0x07FFFFF && max_pos<=0x07FFFFF,"quad_slow illegal max. position\n");
  GB_CHECK(min_pos>=-0x07FFFFF && min_pos<=0x07FFFFF,"quad_slow illegal min. position\n");
  GB_CHECK(slow_dist>=0 && slow_dist<=0xFFFF,"quad_slow illegal slow distance\n");
  GB_CHECK(slow_dc>=0.0 && slow_dc<=100.0,"quad_slow illegal duty_cycle\n");
  flags = 0;
  if (max_on)  flags |= QUAD_TOP;
  if (min_on)  flags |= QUAD_BOT;
  if (slow_on) flags |= QUAD_GOSLOW;
  message[0] = 0xA0;
  message[1] = CMD_QUAD_LIMIT;
  message[2] = (board<<2)|channel;
  message[3] = flags;
  message[4] = (max_pos>>16) & 0xFF;
  message[5] = (max_pos>>8) & 0xFF;
  message[6] =  max_pos & 0xFF;
  message[7] = (min_pos>>16) & 0xFF;
  message[8] = (min_pos>>8) & 0xFF;
  message[9] =  min_pos & 0xFF;
  message[10] = (slow_dist>>8) & 0xFF;
  message[11] =  slow_dist & 0xFF;
  dc = (int)(slow_dc*10+0.5);
  message[12] = dc>>8;
  message[13] = dc & 0xFF;
  message[14] = 0x50;
  write_uart(message,15);
  return 1;
} // quad_limit



/********************************************\
 **                                        **
 **    BBB     OOO    AAA   RRR   DDD      **
 **    B  B   O   O  A   A  R  R  D  D     **
 **    BBBB   O   O  AAAAA  RRR   D  D     **
 **    B   B  O   O  A   A  R  R  D  D     **
 **    BBBBB   OOO   A   A  R  R  DDD      **
 **                                        **
\********************************************/


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

//
// Set the mode of a J3 pin
//
int  set_pin_mode(int board,int pin, int mode)
{ int dac_enb,adc_enb,p,mask1,mask2,channel;
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
    gb_command5[1] = CMD_STOPSHORT;
    gb_command5[2] = (board<<2) | (pin>>1);

    // Update internal motor structure
    channel = pin>>1;
    // Clear the endstop enable bits
    gb_motor[board][channel].endmode &= 0xFC;
    if (gb_pin_mode[board][p+1]==GB_PIN_ENDSTOP)
      gb_motor[board][channel].endmode |= 0x01;
    if (gb_pin_mode[board][p]  ==GB_PIN_ENDSTOP)
      gb_motor[board][channel].endmode |= 0x02;
    // From that we get the command unsigned char
    gb_command5[3] = gb_motor[board][channel].endmode |
        (gb_motor[board][channel].short_hot_stop<<4);
//    printf("ES:%02X %02X\n",gb_command5[2],gb_command5[3]);
    write_uart(gb_command5,5);
  }
  gb_pin_mode[board][pin] = mode; // set mode

  // make set-port-to-output-mode command
  // Beware:
  //    PIN[0] is connected to PA[1]
  //    PIN[1] is connected to PA[0]
  // etc.
  mask1 = 0xFF; // all inputs
  for (p=0; p<8; p++)
    if (gb_pin_mode[board][p^0x01]==GB_PIN_OUTPUT)
      mask1 &= ~(1<<p);

  // etc.
  mask2=0xFF; // all inputs
  if (gb_pin_mode[board][8]==GB_PIN_OUTPUT)
    mask2 &= ~0x02;
  if (gb_pin_mode[board][9]==GB_PIN_OUTPUT)
    mask2 &= ~0x01;
  if (gb_pin_mode[board][12]==GB_PIN_OUTPUT)
    mask2 &= ~0x08;
  if (gb_pin_mode[board][13]==GB_PIN_OUTPUT)
    mask2 &= ~0x04;
  if (gb_pin_mode[board][14]==GB_PIN_OUTPUT)
    mask2 &= ~0x20;
  if (gb_pin_mode[board][15]==GB_PIN_OUTPUT)
    mask2 &= ~0x10;
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
// Set all J3 pin modes in one go
// (mode shoud be an int array holding the modes)
// Arrays counts from 0 for pins counting from 1!
// Current board version requires 20 entries
// entries 10, 11, 16, 18 are ignored
//
int  set_allpins_mode(int board,int *mode)
{ int dac_enb,adc_enb,p,mask1,mask2;
  int pin,adc_dac_change,endstop_change[4];
  char warning[40];
  GB_CHECK(board>=0 && board<=3,  "set_allpins_mode illegal board\n");

  /*
    Override function code:
    */
  adc_dac_change = 0;
  endstop_change[0] = 0;
  endstop_change[1] = 0;
  endstop_change[2] = 0;
  endstop_change[3] = 0;
  for (pin=0; pin<20; pin++)
  { if (mode[pin]==GB_PIN_SAME)
      continue;
     if (pin==10 || pin==11 || pin==16 || pin==18 )
        continue;
    sprintf(warning,"set_allpins_mode pin %d illegal mode\n",pin);
    GB_CHECK(mode[pin]>GB_PIN_SAME && mode[pin]<=GB_PIN_I2C,warning);
    GB_CHECK((gb_pin_modes[pin] & (1<<mode[pin])),warning);
    if ( ((gb_pin_mode[board][pin]==GB_PIN_ADC) && (mode[pin]!=GB_PIN_ADC)) ||
         ((gb_pin_mode[board][pin]!=GB_PIN_ADC) && (mode[pin]==GB_PIN_ADC)) ||
         ((gb_pin_mode[board][pin]==GB_PIN_DAC) && (mode[pin]!=GB_PIN_DAC)) ||
         ((gb_pin_mode[board][pin]!=GB_PIN_DAC) && (mode[pin]==GB_PIN_DAC))
       )
     adc_dac_change = 1;

    if ((gb_pin_mode[board][pin]==GB_PIN_ENDSTOP) && (mode[pin]!=GB_PIN_ENDSTOP) ||
        (gb_pin_mode[board][pin]!=GB_PIN_ENDSTOP) && (mode[pin]==GB_PIN_ENDSTOP)
       )
      endstop_change[pin>>1] = 1;
    // After checking fill in new mode value
    gb_pin_mode[board][pin] = mode[pin];
  }

  //
  // Next code is almost indentical to "set_pin_mode"
  //
  if (adc_dac_change)
  { // switch ADC/DAC on/off
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
    write_uart(gb_command6,6);
  }

  for (p=0; p<4; p++)
  {
    if (endstop_change[p])
    { // switch endmode on/off
      // Clear the endstop enable bits
      gb_motor[board][p].endmode &= 0xFC;
      if (gb_pin_mode[board][p*2+1]==GB_PIN_ENDSTOP)
        gb_motor[board][p].endmode |= 0x01;
      if (gb_pin_mode[board][p*2]  ==GB_PIN_ENDSTOP)
        gb_motor[board][p].endmode |= 0x02;
      // From that we get the command unsigned char
      gb_command5[1] = CMD_STOPSHORT;
      gb_command5[2] = (board<<2) | p;
      gb_command5[3] = gb_motor[board][p].endmode |
          (gb_motor[board][p].short_hot_stop<<4);
  //    printf("ES:%02X %02X\n",gb_command5[2],gb_command5[3]);
      write_uart(gb_command5,5);
    }
  }

  // make set-port-to-output-mode command
  mask1 = 0xFF; // all inputs
  for (p=0; p<8; p++)
    if (gb_pin_mode[board][p^0x01]==GB_PIN_OUTPUT)
      mask1 &= ~(1<<p);

  mask2=0xFF; // all inputs
  if (gb_pin_mode[board][8]==GB_PIN_OUTPUT)
    mask2 &= ~0x02;
  if (gb_pin_mode[board][9]==GB_PIN_OUTPUT)
    mask2 &= ~0x01;
  if (gb_pin_mode[board][12]==GB_PIN_OUTPUT)
    mask2 &= ~0x08;
  if (gb_pin_mode[board][13]==GB_PIN_OUTPUT)
    mask2 &= ~0x04;
  if (gb_pin_mode[board][14]==GB_PIN_OUTPUT)
    mask2 &= ~0x20;
  if (gb_pin_mode[board][15]==GB_PIN_OUTPUT)
    mask2 &= ~0x10;
  if (gb_pin_mode[board][17]==GB_PIN_OUTPUT)
    mask2 &= ~0x80;
  if (gb_pin_mode[board][19]==GB_PIN_OUTPUT)
    mask2 &= ~0x40;

  gb_command7[1] = CMD_SETIO;
  gb_command7[2] = board<<2;
  gb_command7[3] = 0;
  gb_command7[4] = mask2;
  gb_command7[5] = mask1;

  gb_command7[1] = CMD_SETIO;
  gb_command7[2] = board<<2;
  gb_command7[3] = 0;
  gb_command7[4] = mask2;
  gb_command7[5] = mask1;
  write_uart(gb_command7,7);

  return 1;
} // set_allpins_mode


//
// Read board I/O status
//
// Return 0: read failed
// return 1: read succesfull
//
// Note: the internal driver status is also updated
//
int  get_io_setup(int brd,int *status) // int status[20]
{ int i,id,bytes;
  unsigned char out_data[19],in_data[16];
  GB_CHECK(brd>=0 && brd<=3,  "get_io_status illegal board\n");
  id = brd<<2;

  // Read input/output status
  // return is:
  //  - ID, CMD            (2 bytes)  0, 1
  //  - adc_enable     3-0 (1 byte)   2
  //  - dac_enable     1-0 (1 byte)   3
  //  - inputs pins   23-0 (3 bytes)  4, 5, 6
  //  - output pins   23-0 (3 bytes)  7, 8, 9
  //  - output status 23-0 (3 bytes) 10,11,12
  //  - open drain status (1 unsigned char)   13
  //  - unused (2 bytes)             14,15
  // Total 16 bytes
  out_data[0] = 0xA0;
  out_data[1] = CMD_IO_STATUS;
  out_data[2] = id;
  for (i=3; i<19; i++)
    out_data[i] = CMD_STOP_VAL;
  write_uart(out_data,19);
  bytes = read_uart(in_data,16);
  if (bytes!=16 || in_data[0]!=id || in_data[1]!=CMD_IO_STATUS)
    return 0;

  // EXT
  for (i=0; i<8; i++)
    if (in_data[6] & (1<<i))
      gb_pin_mode[brd][i^1] = GB_PIN_INPUT;
    else
      if (in_data[9] & (1<<i))
        gb_pin_mode[brd][i^1] = GB_PIN_OUTPUT;
      else
        gb_pin_mode[brd][i^1] = GB_PIN_ENDSTOP;

  // Spares
  for (i=0; i<2; i++)
    if (in_data[5] & (1<<i))
      gb_pin_mode[brd][i^1+8] = GB_PIN_INPUT;
    else
      gb_pin_mode[brd][i^1+8] = GB_PIN_OUTPUT;

  // ADCs
  for (i=0; i<4; i++)
    if (in_data[2] & (1<<i))
      gb_pin_mode[brd][i^1+12] = GB_PIN_ADC;
    else
      if (in_data[5] & (0x04<<i))
        gb_pin_mode[brd][i^1+12] = GB_PIN_INPUT;
      else
        gb_pin_mode[brd][i^1+12] = GB_PIN_OUTPUT;

  // DACs
  if (in_data[3] & (0x02)) // DAC1
    gb_pin_mode[brd][17] = GB_PIN_DAC;
  else
    if (in_data[5] & 0x40)
      gb_pin_mode[brd][17] = GB_PIN_INPUT;
    else
      gb_pin_mode[brd][17] = GB_PIN_OUTPUT;

  if (in_data[3] & (0x01)) // DAC0
    gb_pin_mode[brd][19] = GB_PIN_DAC;
  else
    if (in_data[5] & 0x80)
      gb_pin_mode[brd][19] = GB_PIN_INPUT;
    else
      gb_pin_mode[brd][19] = GB_PIN_OUTPUT;

  for (i=0; i<20; i++)
      status[i] = gb_pin_mode[brd][i];
  return 1;

} // get_io_status


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
// bit 10 : pin 13  ADC1
// bit 11 : pin 14  ADC0
// bit 12 : pin 15  ADC3
// bit 13 : pin 16  ADC2
// bit 14 : pin 18  DAC0
// bit 15 : pin 20  DAC1
// The board will ignore pins which are NOT an output
//
int set_output_pin_state(int board, int output)
{ int out_swap;
  GB_CHECK(board>=0 && board<=3,  "set_output_pin_state illegal board\n");
  // Pins 18/20 (bits 14 & 15) have to be swapped
  // (Or should I do this in MC??)
  out_swap  = output & 0x3FFF;
  out_swap |= output & 0x4000 ? 0x8000 : 0;
  out_swap |= output & 0x8000 ? 0x4000 : 0;
  gb_command7[1] = CMD_WRITEIO;
  gb_command7[2] = board<<2;
  gb_command7[3] = 0x00; // for future use
  gb_command7[4] = (out_swap >> 8) & 0xFF;
  gb_command7[5] = out_swap & 0xFF;
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
// Read inputs
//
// return -1 (0xFFFFFF) on error
// return input values otherwise
// Return value is in range 0x000..0x0FFF
//
int read_inputs(int board)
{
  unsigned char id,reply[5];
  unsigned int i_value,bytes;
  GB_CHECKN(board>=0 && board<=3, "read_inputs illegal board\n");

  id = (board << 2);
  gb_command6[1] = CMD_READIO;
  gb_command6[2] = id;
  gb_command6[3] = CMD_STOP_VAL;
  gb_command6[4] = CMD_STOP_VAL;
  gb_command6[5] = CMD_STOP_VAL;
  write_uart(gb_command6,6);
  bytes = read_uart(reply,5);
  if (bytes!=5 || reply[0]!=id || reply[1]!=CMD_READIO)
    return -1;
  i_value  = reply[4];
  i_value |= reply[3]<<8;
  i_value |= reply[2]<<16;
  return i_value;
} // read_inputs

int set_baudrate(int b)
{ unsigned char mess[5];
   GB_CHECKN(b>=0 && b<=4, "set_baudrate illegal value\n");
 
  gb_command6[1]=CMD_SETBAUD;
  gb_command6[2]=0x18;
  gb_command6[3]=0x81;
  gb_command6[4]=b;
  write_uart(mess,6);
  // Now swicth local buadrate
#ifdef _WIN32
  switch (b)
  {
  case 0 : setup_comm_port(L"19200,n,8,1"); break;
  case 1 : setup_comm_port(L"38400,n,8,1"); break;
  case 2 : setup_comm_port(L"57600,n,8,1"); break;
  case 3 : setup_comm_port(L"115200,n,8,1"); break;
  case 4 : setup_comm_port(L"230400,n,8,1"); break;
  }
  return 0;
#else
  set_baud_rate(0,b);
  return 1;
#endif  
} // set_baudrate


// show mess   
//for (m=0; m<8; m++)
//  printf("0x%02x ",gb_command8[m]);
//printf("\n");  


