# Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc
#
# @author zhang jian ming
# @date 03/01/18


ifeq ($(PRJ_DIR), )
  PRJ_DIR = $(CURDIR)/..
  export PRJ_DIR

  include $(PRJ_DIR)/make/make.topdirs
endif


subdirs += core client server

include $(PRJ_DIR)/make/make.subdirs

