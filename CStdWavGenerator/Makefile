CC = gcc

SOURCE_ROOT = $(shell pwd)

CFLAGS		:= -Wall -O2 -fno-buildin
CPPFLAGS	:=
TARGET 		:= StandardWaveGenerator

all: $(TARGET)
SRCS += StandardWaveGenerator.c

OBJS = $(patsubst %.c, %.o, $(SRCS))

#OBJ_C = $(foreach file, $(OBJS), $(file))

$(OBJS): %.o:%.c
	$(CC) $(CPPFLAGS) -c $(SRCS) -o $(OBJS)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

clean:
	@rm -f *.o $(TARGET)

.PHONY: clean
