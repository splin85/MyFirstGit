

LIB = librpcclient.a

INC_DIRS = ../include                \
           $(RPC_DIR)/common/include \
           $(RPC_DIR)/core/include   \
           ${SAI_DIR}/include        \
           $(BUILD_DIR)/include      \
           $(BUILD_DIR)/include/frr  \
           $(IGMPD_DIR)/include
EXTRA_LIBS =

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
#	@cp -a conf/ $(BLD_DIR)
#
