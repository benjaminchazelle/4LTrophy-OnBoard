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

	double gpslat = 48.8566140;
	double gpslng = 2.3522219;

	int pin;


	for (pin=13; pin<=16; pin++)
		set_pin_mode(tst_board,pin,GB_PIN_ADC);
	
	for (pin=1; pin<=7; pin++)
		set_pin_mode(tst_board,pin,GB_PIN_OUTPUT);
	
	while (1)
	{
			
		printf("ADC0:%5.10f\n",read_adc(tst_board,0));
		printf("ADC1:%5.10f\n",read_adc(tst_board,1));
		printf("ADC2:%5.10f\n",read_adc(tst_board,2));
		printf("ADC3:%5.10f\n",read_adc(tst_board,3));
		printf("GPSLat:%5.10f\n",gpslat);
		printf("GPSLng:%5.10f\n",gpslng);

		printf("\n");
		fflush(stdout);
		
		gpslat -= 0.0002;
		gpslng += 0.0002;
		
		sleep(1);
	}

	return 1;
}