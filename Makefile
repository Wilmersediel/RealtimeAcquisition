.PHONY: all clean

OBJS = SaveData.o control.o

all: AcquisitionEngine.c ${OBJS}
	gcc -Wall -o plant -I./include/ ./dUQx.c ./include/RS-232/rs232.c \
	AcquisitionEngine.c ${OBJS} -lrt -lm 

SaveData.o: SaveData.c
	gcc -Wall -c -I./include/ -o SaveData.o SaveData.c

control.o: control.c
	gcc -Wall -c -I./include/ -o control.o control.c


clean:
	rm plant ${OBJS}



