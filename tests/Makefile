CC = g++

SOURCE_ROOT = $(shell pwd)

CFLAGS		:= -Wall -O2 -fno-buildin
CPPFLAGS	:=
INCLUDE 	:= -I $(SOURCE_ROOT)/../wav_utils/include
LIBVAR		:= -lwav_ops
LIBPATH		:= -L ../libs
TARGET 		:= WavOpsTest

all: $(TARGET)
SRCS += WavFileOpsTest.cpp

OBJS = $(patsubst %.cpp, %.o, $(SRCS))

#OBJ_C = $(foreach file, $(OBJS), $(file))

$(OBJS): %.o:%.cpp
	$(CC) $(INCLUDE) $(CPPFLAGS) -c $(SRCS) -o $(OBJS)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(LIBPATH) $(LIBVAR) $(OBJS)

clean:
	@rm -f *.o $(TARGET)

.PHONY: clean
