# Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc
#
# @author zhang jian ming
# @date 03/01/18


LIB = librpcserver.a

INC_DIRS = ../include                   \
           $(RPC_DIR)/common/include    \
           $(RPC_DIR)/core/include      \
           ${SAI_DIR}/include           \
           $(BUILD_DIR)/include         \
           $(BUILD_DIR)/include/frr     \
           $(L2_DIR)/l2d/vlan/include   \
           $(L2_DIR)/l2d/include        \
           $(L2_DIR)/l2d/sys/ip/include \
           $(L2_DIR)/l2d/sys/id/include \
           $(L2_DIR)/l2d/fdb/include    \
           $(L2_DIR)/l2d/trunk/include  \
           $(L2_DIR)/l2d/sys/ntp/include\
           $(L2_DIR)

EXTRA_LIBS   =
EXTRA_CFLAGS = $(FRR_CFLAGS)


include $(PRJ_DIR)/make/make.rule

#
# The below: LIB,EXE and clean target can be as POST process,
# you can add commands to do anything you want and do NOT
# modify the $(PRJ_DIR)/make/make.rule.
#
# @Attetion
#
# There are TWO colons after target!
#
#$(LIB)::
#$(EXE)::
#clean::
#
# eg. copy conf dir to BLD_DIR
#
#$(EXE)::
#   @cp -a conf/ $(BLD_DIR)
#
