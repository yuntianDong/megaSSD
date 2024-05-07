SRC_DIR		= ../../../src
GTEST_DIR	= ../.
export CC = g++
export DFLAGS = -g -fprofile-arcs -ftest-coverage
CFLAGS		+= -isystem $(GTEST_DIR)/include
CFLAGS		+= -g -Wall -Wextra -pthread
CFLAGS		+= -fprofile-arcs -ftest-coverage
TESTS		= utINVMeDevice
GTEST_HEADERS	= $(GTEST_DIR)/include/gtest/*.h	\
				  $(GTEST_DIR)/include/gtest/internal/*.h
				  
INCLUDES	:= -I. -I$(SRC_DIR)/inc -I/usr/include -I/usr/local/include/boost/include/ -I/usr/local/include -I/usr/include/python3.6/ -I/usr/local/spdk/include

LDFLAGS = -L/usr/lib/python3.6 -L/usr/lib64 -L/usr/local/lib -L/usr/local/spdk/lib -L/usr/local/dpdk/lib -L/usr/local/include/boost/lib
LDFLAGS += -lboost_filesystem
LDFLAGS += -lboost_python36
LDFLAGS += -lpython3
LDFLAGS += -lspdk -lspdk_env_dpdk -lpci -ludev -lssl -lcrypto
LDFLAGS += -lrte_vhost -lrte_security -lrte_reorder -lrte_dmadev \
			-lrte_power -lrte_cryptodev -lrte_compressdev -lrte_timer -lrte_hash \
			-lrte_cmdline -lrte_bus_pci -lrte_pci -lrte_ethdev -lrte_meter -lrte_net -lrte_mbuf \
			-lrte_mempool -lrte_rcu -lrte_ring -lrte_eal -lrte_telemetry -lrte_kvargs 

MODULE_DIR	:=	\
	$(SRC_DIR)/lib/buffer/	\
	$(SRC_DIR)/lib/udev/	\
	$(SRC_DIR)/lib/context/	\
	$(SRC_DIR)/lib/device/nvme	\
	$(SRC_DIR)/lib/device/	\
	$(SRC_DIR)/lib/driver/stdkernel/	\
	$(SRC_DIR)/lib/driver/stdspdk/	\
	$(SRC_DIR)/lib/pattern/	\
	$(SRC_DIR)/module/xBuffer/	\
	$(SRC_DIR)/module/xPattern/	\
	$(SRC_DIR)/module/xNVMe/	\
	$(SRC_DIR)/lib/utility/	\
	
MOD_FILES	:= $(foreach d, $(MODULE_DIR), $(filter-out %/Wrapper.cpp %/Convert.cpp, $(wildcard $(d)/*.cpp)))
MOD_OBJS	:= $(patsubst %.cpp,%.o,$(MOD_FILES))
WRAP_FILES	:= $(foreach d, $(MODULE_DIR), $(wildcard $(d)/Wrapper.gc*)) $(foreach d, $(MODULE_DIR), $(wildcard $(d)/Convert.gc*))
	
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
	rm -f $(COV_FILES) $(WRAP_FILES)

$(MODULE_DIR):
	$(MAKE) -C $@ $(GOAL)
	
%.o: %.cpp
	$(CC)	$(CFLAGS) $(INCLUDES) $(INCLUDES) -c -g $< -o $@
	
$(TESTS):$(MODULE_DIR) $(UT_OBJS) $(MOD_OBJS)
	$(CC)	$(CFLAGS) $(UT_OBJS) $(MOD_OBJS) $(INCLUDES) $(LDFLAGS) ../libgtest_main.a ../libgtest.a -o $@

COV_FILES	:= $(patsubst %.cpp,%.gcda,$(MOD_FILES)) $(patsubst %.cpp,%.gcno,$(MOD_FILES))

coverage: $(TESTS)
	lcov --zerocounters --directory .
	./$(TESTS)  
	cp $(COV_FILES) .
	rm $(COV_FILES) $(WRAP_FILES)
	lcov --capture --directory . --output-file coverage.info  
	lcov --remove coverage.info '*/usr/*' '*/gtest/*' '*/ut/*' '*/xBuffer/*' '*/xPattern/*' '*/context/*' '*/buffer/*' '*/udev/*' '*/stdkernel/*' '*/stdspdk/*' '*/pattern/*' '*/utility/*'	\
	'*ClassMember.h' '*IHostBufMngr.h' '*IHostDevice.h' '*/inc/device/*' '*NVMeEngine.h' '*NVMeScan.cpp' '*DeviceHandler.cpp' '*DeviceMngr.cpp' '*IPatternMngr.h' -o $(TESTS).info
	genhtml $(TESTS).info --output-directory $(TESTS)_report  
	
.PHONY	: all clean $(MODULE_DIR) coverage