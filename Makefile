# Makefile for jpyrate
# written by chromabox

## initialize srcs
S_SRCS := 
C_SRCS := 
OBJS := 
SYSDEFS := 
NO_ALIGNED := 
USER_DEFINES :=

## cross tool define
CROSS_COMPILE = 

CC        = $(CROSS_COMPILE)gcc
CPP       = $(CROSS_COMPILE)g++
OBJCOPY   = $(CROSS_COMPILE)objcopy
OBJDUMP   = $(CROSS_COMPILE)objdump
SIZE      = $(CROSS_COMPILE)size
AR        = $(CROSS_COMPILE)ar
LD        = $(CROSS_COMPILE)ld
NM        = $(CROSS_COMPILE)nm
STRIP     = $(CROSS_COMPILE)strip
READELF     = $(CROSS_COMPILE)readelf

## other tool and commands

ifeq ($(OS),Windows_NT)
MAKEDIR   = busybox mkdir -p
else
MAKEDIR   = mkdir -p
endif

## curlcpp Library  ###############################################
LIB_CURLCPPNX_DIR   = ./lib/curlcppnx
LIB_INCPATH  += -I$(LIB_CURLCPPNX_DIR)/include

CURLCPPNX_LIBDIR = $(LIB_CURLCPPNX_DIR)/build
CURLCPPNX_LIB = $(CURLCPPNX_LIBDIR)/libcurlcppnx.a
CURLCPPNX_LDPATH = -L$(CURLCPPNX_LIBDIR)
CURLCPPNX_LDLINK = -l:libcurlcppnx.a -lcurl

## picojson Library  ###############################################
PICOJSON_DIR	= ./lib/picojson
LIB_INCPATH  += -I$(PICOJSON_DIR)

## source and defines ################################################
#

TARGET				= jpyrate
USER_DEFINES 		+= 

USER_AFLAGS			+= 
USER_CFLAGS			+= 
USER_LDFLAGS		+=

USER_INCPATH		= -I./include

USER_CSRCS			= src/main.cpp

TARGET_DIR	  = ./build

# g++ cpp version(C++20)
CPP_VERFLAG	  = -std=gnu++20

## debug and optimize ################################
ifeq ($(DEBUG_BUILD),0)
CPP_DDEBUG = 
CPP_OPTIMIZE = 2
MAKE_FLAGS = "CURLCPPNX_DEBUG_BUILD=0"
else
CPP_DDEBUG = -DDEBUG
CPP_OPTIMIZE = g
MAKE_FLAGS = "CURLCPPNX_DEBUG_BUILD=1"
endif

ifeq ($(DEBUG_BUILD),2)
CPP_DDEBUG = -DDEBUG
CPP_OPTIMIZE = 0
MAKE_FLAGS = "CURLCPPNX_DEBUG_BUILD=2"
endif

#######################################################
## Last Files define
#######################################################

## build target definition #####################################
OBJDIR = $(TARGET_DIR)

INCLUDES = $(LIB_INCPATH) $(USER_INCPATH)
C_SRCS = $(USER_CSRCS)

## build objects definition ####################################

OBJS = $(addprefix $(OBJDIR)/, $(C_SRCS:%.cpp=%.o))
DEPS = $(addprefix $(OBJDIR)/, $(C_SRCS:%.cpp=%.d)) 

TARGET_EXE = $(TARGET_DIR)/$(TARGET)

#######################################################
## Last tool options
#######################################################

#NO_ALIGNED = -mno-unaligned-access
NO_ALIGNED = 

## Compiler options #####################################
CPPFLAGS   =  -Wall $(CPP_VERFLAG) $(NO_ALIGNED)
CPPFLAGS  += -g -O$(CPP_OPTIMIZE) $(CPP_DDEBUG)
CPPFLAGS  += $(INCLUDES)
CPPFLAGS  += $(SYSDEFS)

## Linker options #######################################
LDFLAGS  = $(CURLCPPNX_LDLINK)
LDFLAGS  += -Wl,--build-id
LDFLAGS  += $(USER_LDFLAGS)
LDFLAGS  += $(CURLCPPNX_LDPATH)

#######################################################
## make rules
#######################################################

# disable gnumake auto remove object(.o)
.SECONDARY: $(OBJS)

all: viewenv $(TARGET_EXE) endmess


clean: 
	@echo clean objects...
	@rm -rf $(TARGET_EXE)
	@rm -rf $(TARGET_DIR)
	- cd $(LIB_CURLCPPNX_DIR) ; $(MAKE) $(MAKE_FLAGS) clean
	@echo done.

# show enviromnt and compile options
viewenv:
	@echo
	@echo --------------------
	@echo build information:
	@echo
	@echo
ifeq ($(DEBUG_BUILD),0)
	@echo Debug : No
else
	@echo Debug : Yes
endif
	@echo
	@echo CPPFLAGS $(CPPFLAGS)
	@echo
	@echo LDFLAGS $(LDFLAGS)
	@echo --------------------
	@echo

endmess:
	@echo 
	@echo done.

$(CURLCPPNX_LIB):
	- cd $(LIB_CURLCPPNX_DIR) ; $(MAKE) $(MAKE_FLAGS) 

$(TARGET_EXE): $(CURLCPPNX_LIB) $(OBJS) 
	@echo link: $@
	@$(CPP) -o $@ $(CURLCPPNX_LIB) $(OBJS) $(LDFLAGS) 
	@echo

$(addprefix $(OBJDIR)/, %.o): %.cpp
	@echo compile: $<
	-@$(MAKEDIR) $(dir $@) 
	@$(CPP) -c -MMD -MP -o $@ $(CPPFLAGS) $<
	@echo 

-include $(DEPS)
