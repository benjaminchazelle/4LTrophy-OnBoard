#
#
#
gtest: test_gb_drivers.o gb_drivers.o gertbot_pi_uart.o 
	gcc -o gtest test_gb_drivers.o gb_drivers.o gertbot_pi_uart.o 

test_gb_driver.o : test_gb_drivers.c test_gb_drivers.h 
	gcc -c test_gb_drivers.o
 
gb_driver.o : gb_drivers.c gb_drivers.h gertbot_pi_uart.h
	gcc -c gb_drivers.o
 
gertbot_pi_uart.o : gertbot_pi_uart.c gertbot_pi_uart.h
	gcc -c gertbot_pi_uart.c

