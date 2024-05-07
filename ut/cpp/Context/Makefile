SRC_DIR		= ../../../src
GTEST_DIR	= ../.
CC			= g++
CFLAGS		+= -isystem $(GTEST_DIR)/include
CFLAGS		+= -g -Wall -Wextra -pthread 
CFLAGS		+= -fprofile-arcs -ftest-coverage
TESTS		= utContext
GTEST_HEADERS	= $(GTEST_DIR)/include/gtest/*.h	\
				  $(GTEST_DIR)/include/gtest/internal/*.h
				  
INCLUDES	:= -I. -I$(SRC_DIR)/inc -I/usr/include -I/usr/local/boost/include -I/usr/local/include

LDFLAGS = -L/usr/local/boost/lib -L/usr/lib64 -L/usr/local/lib -L/usr/local/lib/spdk -L/usr/local/lib/dpdk
LDFLAGS += -lboost_filesystem
LDFLAGS += -lrt
LDFLAGS += -lspdk -lspdk_env_dpdk -lrte_eal -lrte_mempool -lrte_ring -lrte_mbuf \
            -lrte_mempool_ring -lrte_pci -lrte_bus_pci -lrte_kvargs -lrte_vhost -lrte_net -lrte_hash \
            -lrte_telemetry -lrte_cryptodev -lpci -ludev

MODULE_DIR	:=	\
	$(SRC_DIR)/lib/context/	\
	
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
	rm -rf *.info
	rm -rf *_report
	rm -rf *.gc*

$(MODULE_DIR):
	$(MAKE) -C $@ $(GOAL)
	
%.o: %.cpp
	$(CC)	$(CFLAGS) $(INCLUDES) $(INCLUDES) -c $< -o $@
	
$(TESTS):$(MODULE_DIR) $(UT_OBJS)
	$(CC)	$(CFLAGS) $(UT_OBJS) $(MOD_OBJS) $(INCLUDES) $(LDFLAGS) ../libgtest_main.a ../libgtest.a -o $@

COV_FILES = $(patsubst %.cpp,%.gcda,$(UT_FILES)) $(patsubst %.cpp,%.gcno,$(UT_FILES))

coverage: $(TESTS)
	lcov --zerocounters --directory .
	./$(TESTS)  
	lcov --capture --directory . --output-file coverage.info  
	lcov --remove coverage.info '*/usr/*' '*/gtest/*' '*/ut/*' -o $(TESTS).info
	genhtml $(TESTS).info --output-directory $(TESTS)_report  
	
.PHONY	: all clean $(MODULE_DIR) coverage