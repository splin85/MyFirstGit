

ifeq ($(PRJ_DIR), )
  PRJ_DIR = $(CURDIR)/..
  export PRJ_DIR

  include $(PRJ_DIR)/make/make.topdirs
endif


subdirs += core client server

include $(PRJ_DIR)/make/make.subdirs

