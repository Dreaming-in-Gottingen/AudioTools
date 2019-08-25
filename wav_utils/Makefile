CC = g++

SOURCE_ROOT = $(shell pwd)

CFLAGS		:= -Wall -O2 -fno-buildin
CPPFLAGS 	:=
INCLUDE 	:= -I $(SOURCE_ROOT)/include
TARGET 		:= libwav_ops.so
LIBPATH 	:= ../libs

all: $(TARGET)
SRCS += wav_ops.cpp

OBJS = $(patsubst %.cpp, %.o, $(SRCS))

#OBJ_C = $(foreach file, $(OBJS), $(file))

$(OBJS): %.o:%.cpp
	$(CC) $(INCLUDE) $(CPPFLAGS) -c $(SRCS) -o $(OBJS)

$(TARGET): $(OBJS)
	$(CC) -shared -o $(TARGET) $(OBJS)
	mkdir -p $(LIBPATH)
	cp $(TARGET) $(LIBPATH)

clean:
	@rm -f *.o *.so

.PHONY: clean
