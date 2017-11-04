SRC_DIR = ./src
INCLUDE_FLAGS = -I./include

SRCS = \
$(SRC_DIR)/all_system.cpp \
$(SRC_DIR)/cpu.cpp \
$(SRC_DIR)/cpu_sw_bus.cpp \
$(SRC_DIR)/cpu_sw_main.cpp \
$(SRC_DIR)/din_dout.cpp \
$(SRC_DIR)/main.cpp \
$(SRC_DIR)/master_driver.cpp \
$(SRC_DIR)/slave_driver.cpp

####################################

SYSTEMC_HOME =/home/igor/Downloads/systemc-2.3.1a

TARGET_ARCH = linux64

ARCH_SUFFIX = -$(TARGET_ARCH)

## How to instruct the dynamic linker to locate the SystemC library
LDFLAG_RPATH = -Wl,-rpath=

# default compiler flags
FLAGS_COMMON = -g -Wall
FLAGS_STRICT = -pedantic -Wno-long-long
FLAGS_WERROR = -Werror

# combined flags
SYSTEMC_CXXFLAGS = $(FLAGS_COMMON) $(FLAGS_STRICT) $(FLAGS_WERROR)

# Explicit location of the SystemC headers
SYSTEMC_INC_DIR = $(SYSTEMC_HOME)/include

# Explicit location if the SystenC library
SYSTEMC_LIB_DIR = $(SYSTEMC_HOME)/lib$(ARCH_SUFFIX)

#########################

SYSTEMC_LDFLAGS  = -L $(SYSTEMC_LIB_DIR) \
                    $(LDFLAG_RPATH)$(SYSTEMC_LIB_DIR)
SYSTEMC_LIBS     = -lsystemc -lm

INCDIR   += -I$(SYSTEMC_INC_DIR)
INCDIR   += $(INCLUDE_FLAGS)

CXXFLAGS  += $(CFLAGS) $(SYSTEMC_CXXFLAGS) $(INCDIR) $(SYSTEMC_DEFINES)
LDFLAGS   += $(CFLAGS) $(SYSTEMC_CXXFLAGS) $(LIBDIR) $(SYSTEMC_LDFLAGS)
LIBS      += $(SYSTEMC_LIBS) $(EXTRA_LIBS)

CXX=g++
CC=$(CXX)

#########################

sc_executable: $(SRCS) $(SYSTEMC_LIB_DIR)/libsystemc.a
	$(CC) $(CXXFLAGS) $(LDFLAGS) -o $@ $(SRCS) $(LIBS) 2>&1

clean:
	rm -f sc_executable *.out *.vcd

all: sc_executable

run: sc_executable
	./sc_executable && nohup gtkwave waveforms.vcd &
