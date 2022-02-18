#
# Makefile
#

CIRCLEHOME = ../../..

OBJS	= main.o kernel.o

LIBS	= ../libsdcard.a \
	  $(CIRCLEHOME)/lib/fs/fat/libfatfs.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/libcircle.a \
      $(GCCHOME)/arm-none-eabi/lib/libc.a \
      $(GCCHOME)/arm-none-eabi/lib/libstdc++.a

include $(CIRCLEHOME)/sample/Rules.mk

-include $(DEPS)
