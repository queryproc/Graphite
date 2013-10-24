TARGET = libmcpat.a
SHELL = /bin/bash
.PHONY: all clean
.SUFFIXES: .cc .o

ifndef NTHREADS
  NTHREADS = 4
endif

ifeq ($(TAG),dbg)
  DBG = -ggdb
  OPT = -O0 -DNTHREADS=1 -Icacti
else
  OPT = -O3 -msse2 -mfpmath=sse -DNTHREADS=$(NTHREADS) -Icacti
endif

CXXFLAGS = -Wall -Wno-unknown-pragmas $(DBG) $(OPT)
CXX = g++ -fPIC
CC  = gcc -fPIC

VPATH = cacti

SRCS  = \
  Ucache.cc \
  XML_Parse.cc \
  arbiter.cc \
  area.cc \
  array.cc \
  bank.cc \
  basic_circuit.cc \
  basic_components.cc \
  cacti_interface.cc \
  component.cc \
  core.cc \
  crossbar.cc \
  decoder.cc \
  htree2.cc \
  interconnect.cc \
  io.cc \
  iocontrollers.cc \
  logic.cc \
  mat.cc \
  memoryctrl.cc \
  noc.cc \
  nuca.cc \
  parameter.cc \
  processor.cc \
  router.cc \
  sharedcache.cc \
  subarray.cc \
  technology.cc \
  uca.cc \
  wire.cc \
  xmlParser.cc \
  powergating.cc \
  core_wrapper.cc \
  cache_wrapper.cc

OBJS = $(patsubst %.cc,obj_$(TAG)/%.o,$(SRCS))

all: obj_$(TAG)/$(TARGET)
	@echo $(OBJS)
	cp -f obj_$(TAG)/$(TARGET) $(TARGET)

obj_$(TAG)/$(TARGET): $(OBJS)
	ar rcs $@ $^

obj_$(TAG)/%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) obj_$(TAG)/$(TARGET) $(TARGET)
