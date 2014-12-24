
CC        = $(CROSS_COMPILE)g++
STRIP     = $(CROSS_COMPILE)strip

TARGET    = jpyrate

HTTP_SRCS = http/httpclient.cpp http/httpcurl.cpp
SRCS	  = main.cpp
OBJS	  = $(HTTP_SRCS:%.cpp=%.o) $(SRCS:%.cpp=%.o)
DEPS	  = $(HTTP_SRCS:%.cpp=%.d) $(SRCS:%.cpp=%.d)
INCLUDE_DIRS = ./http ./include .

CFLAGS    = -Wall -O2 $(addprefix -I,$(INCLUDE_DIRS))
LDFLAGS   = -lcurl


all: $(TARGET)

-include $(DEPS)

clean: 
	rm -f $(OBJS) $(DEPS) $(TARGET) *~ *.scc *.vcproj *.vspscc


$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(DEBUGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@


.cpp.o:
	$(CC) -c -MMD -MP -o $@ $(CFLAGS) $(DEBUGS) $<

