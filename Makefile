include Rules.make

all clean cleanall:
	if [ -d ./apptool ]; then make $@ -C apptool; fi
	if [ -d ./client_demo ]; then make $@ -C client_demo; fi
	if [ -d ./server_demo ]; then make $@ -C server_demo; fi
	
install:
#	if [ -d $(TARGET_ROOT_DIR)/release ]; then make $@ -C release; fi
	
	