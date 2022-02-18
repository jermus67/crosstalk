#
# Makefile
#

CIRCLEHOME = ../..

OBJS	= main.o kernel.o

LIBS	= $(CIRCLEHOME)/lib/usb/libusb.a \
	  $(CIRCLEHOME)/lib/input/libinput.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/libcircle.a \
      $(GCCHOME)/arm-none-eabi/lib/libc.a \
      $(GCCHOME)/arm-none-eabi/lib/libstdc++.a

include ../Rules.mk

-include $(DEPS)
