BASEDIR	:= $(dir $(firstword $(MAKEFILE_LIST)))
VPATH	:= $(BASEDIR)

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing header files
#---------------------------------------------------------------------------------
TARGET		:=	ftpiiu
SOURCES		:=	src
INCLUDES	:=	include

#---------------------------------------------------------------------------------
# version
#---------------------------------------------------------------------------------
VERSION_MAJOR	:=	0
VERSION_MINOR	:=	5
GITREV			:=	$(shell git rev-parse HEAD 2>/dev/null | cut -c1-8)
VERSION			:=	$(VERSION_MAJOR).$(VERSION_MINOR)-$(GITREV)

#---------------------------------------------------------------------------------
# build flags
#---------------------------------------------------------------------------------
CFLAGS		+=	-O2 -D_GNU_SOURCE -DVERSION_STRING="\"FTPiiU v$(VERSION)\""

#---------------------------------------------------------------------------------
# wut libraries
#---------------------------------------------------------------------------------
LDFLAGS		+=	$(WUT_NEWLIB_LDFLAGS) $(WUT_DEVOPTAB_LDFLAGS) \
				-lcoreinit -lsysapp -lnn_ac -lnsysnet -lproc_ui -lwhb

#---------------------------------------------------------------------------------
# includes
#---------------------------------------------------------------------------------
CFLAGS		+=	$(foreach dir,$(INCLUDES),-I$(dir))

#---------------------------------------------------------------------------------
# generate a list of objects
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
OBJECTS		+=	$(CFILES:.c=.o)

#---------------------------------------------------------------------------------
# targets
#--------------------------------------------------------------------------------
$(TARGET).rpx: $(OBJECTS)

clean:
	$(info clean ...)
	@rm -rf $(TARGET).rpx $(OBJECTS) $(OBJECTS:.o=.d)

.PHONY: clean

#---------------------------------------------------------------------------------
# wut
#---------------------------------------------------------------------------------
include $(WUT_ROOT)/share/wut.mk
