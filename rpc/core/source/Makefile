# Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc
#
# @author zhang jian ming
# @date 03/01/18


LIB = librpccore.a

INC_DIRS = ../include           \
           $(BUILD_DIR)/include \
           $(BUILD_DIR)/include/frr
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
