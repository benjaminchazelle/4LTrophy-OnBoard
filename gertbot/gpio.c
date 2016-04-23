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
#include <unistd.h>

int tst_board=0;

int get_return()
{
	char c, prev_c;
	c = 0;
	
	do  {
		prev_c = c;
		c = getchar();
		
	} while (c!=0x0A);
	
	return prev_c;
}

  
int main(int argc,char *argv[])
{

	if (!open_connection(0))
	{
		printf("open failed on comport\n");
		return 2;
	}

	int pin, rep, i, gpio_state, s;

	if(argc == 2) {
		
		int i, len ,r;

		r=0;

		len = strlen(argv[1]);

		for(i=0;i<len;i++) {

			r = r << 1;			
			
			if(argv[1][i] == '1') {
				r += 1;
			}
			

		}
		
		set_output_pin_state(tst_board,r);


	
	} else {
		
		for (pin=1; pin<=7; pin++)
			set_pin_mode(tst_board,pin,GB_PIN_OUTPUT);
		
		set_output_pin_state(tst_board,0);
		
	}
	
	printf("GPIO Set");
	fflush(stdout);

	return 1;
}