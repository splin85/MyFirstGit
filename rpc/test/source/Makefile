

ifeq ($(PRJ_DIR), )
  PRJ_DIR = $(CURDIR)/../../..
  include $(PRJ_DIR)/make/make.topdirs
endif

.PHONY: clean all

ifneq ($(CROSS_COMPILE), )
  SYSROOT = --sysroot=/opt/fsl-qoriq/2.0/sysroots/cortexa7hf-vfp-neon-fsl-linux-gnueabi
  CC = $(CROSS_COMPILE)gcc
  AR = $(CROSS_COMPILE)ar
  LD = $(CROSS_COMPILE)ld
  STRIP = $(CROSS_COMPILE)strip
  NM = $(CROSS_COMPILE)nm
  OBJCOPY = $(CROSS_COMPILE)objcopy
  FHOS_CFLAGS = -march=armv7-a -mfloat-abi=hard -mfpu=neon -mtune=cortex-a7 $(SYSROOT)
else
  OBJCOPY = objcopy
endif



all: client server

INC_FLAGS = -I ${PRJ_DIR}/common/include \
            -I $(RPC_DIR)/common/include \
            -I $(RPC_DIR)/core/include   \
            -I $(BUILD_DIR)/include \
            -I $(SAI_DIR)/include


CFLAGS = $(INC_FLAGS) $(FHOS_CFLAGS) -pthread
C_CFLAGS = $(CFLAGS) -I $(RPC_DIR)/client/include
S_CFLAGS = $(CFLAGS) -I $(RPC_DIR)/server/include

LDFLAGS = -L $(BUILD_DIR)/lib -ljson-c -lcares -lfrr


client:
	$(CC) $(C_CFLAGS) -o client_test client_test.c $(LDFLAGS) -lrpcclient -lrpccore

server:
	$(CC) $(S_CFLAGS) -o server_test server_test.c $(LDFLAGS) -lrpcserver -lrpccore


clean:
	@rm -f client server
