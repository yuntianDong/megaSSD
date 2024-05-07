SRC_DIR		= ../../../src
GTEST_DIR	= ../.
CC			= g++
CPPFLAGS	+= -isystem $(GTEST_DIR)/include
CXXFLAGS	+= -g -Wall -Wextra -pthread
TESTS		= ut_test
GTEST_HEADERS	= $(GTEST_DIR)/include/gtest/*.h	\
				  $(GTEST_DIR)/include/gtest/internal/*.h
				  
LDFLAGS = -L/usr/local/boost/lib -L/usr/lib64
LDFLAGS += -lboost_thread
LDFLAGS += -lboost_system
LDFLAGS += -lboost_locale
				  
INCLUDES	:= -I. -I$(SRC_DIR)/inc -I/usr/include -I/usr/local/boost/include

MODULE_DIR	:=	\
	$(SRC_DIR)/lib/utility	\
	$(SRC_DIR)/lib/serial	\
	$(SRC_DIR)/lib/power/mcuPwrCtrlr	\
	
MOD_FILES	:= $(foreach d, $(MODULE_DIR), $(wildcard $(d)/*.cpp))
MOD_OBJS	:= $(patsubst %.cpp,%.o,$(MOD_FILES))
	
UT_DIR		:= 	.

UT_FILES	:= $(foreach d, $(UT_DIR), $(wildcard $(d)/*.cpp))
UT_OBJS		:= $(patsubst %.cpp,%.o,$(UT_FILES))

all: GOAL=all
all: $(TESTS)

clean: GOAL=clean
clean: $(MODULE_DIR)
	rm -f $(TESTS) *.o *.bin $(UT_OBJS)

$(MODULE_DIR):
	$(MAKE) -C $@ $(GOAL)
	
%.o: %.cpp
	$(CC)	$(CPPFLAGS) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	
$(TESTS):$(MODULE_DIR) $(UT_OBJS)
	$(CC)	$(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(UT_OBJS) $(MOD_OBJS) ../gtest_main.a -o $@
	
.PHONY	: all clean $(MODULE_DIR)