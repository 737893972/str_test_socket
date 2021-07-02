DEST_PATH		:=

##-----1.Choose the paltform
RUN_PLATFORM 	:= 	LINUX

############################################################################

##-----Cross compile tools selection(It depends on the paltform) 
ifeq ($(RUN_PLATFORM),LINUX)
	CROSS_COMPILE := 
	PLATFORM := -DUBUNTU
else
	CROSS_COMPILE := 
	PLATFORM := -DUBUNTU
endif



################################################################################

CC		:= 	gcc
AR		:=	ar
STRIP	:= 	strip



###############自定义部#################
SRC_PUBLIC := ../apptool
INC_PUBLIC := ../apptool

PUBLIC_LIB := -lpthread