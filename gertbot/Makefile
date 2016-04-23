#
#
#
ADC: adc.o gb_drivers.o gertbot_pi_uart.o 
	gcc -o ADC adc.o gb_drivers.o gertbot_pi_uart.o 
	
GPIO: gpio.o gb_drivers.o gertbot_pi_uart.o 
	gcc -o GPIO gpio.o gb_drivers.o gertbot_pi_uart.o 

adc_driver.o : adc.c adc.h 
	gcc -c adc.c
gpio_driver.o : gpio.c gpio.h 
	gcc -c gpio.c
 
gb_drivers.o : gb_drivers.c gb_drivers.h gertbot_pi_uart.h gertbot_defines.h
	gcc -c gb_drivers.c
 
gertbot_pi_uart.o : gertbot_pi_uart.c gertbot_pi_uart.h
	gcc -c gertbot_pi_uart.c

