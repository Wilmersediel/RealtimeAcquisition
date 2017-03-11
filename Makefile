.PHONY: all clean

all: AcquisitionEngine.c
	gcc -Wall -o plant -I./include/ ./dUQx.c ./include/RS-232/rs232.c \
	AcquisitionEngine.c control.c -lrt -lm 

example: timer_example.c
	gcc -g -Wall -o timer_test timer_example.c -lrt

clean:
	rm plant



