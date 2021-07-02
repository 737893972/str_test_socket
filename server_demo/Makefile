include ../Rules.make

##-----The name of the target setting
TARGET			:= ../_bin/server_demo



CFLAGS 	= 	-I . -I $(INC_PUBLIC) -O2 -Wall $(PLATFORM)

SOURCE 	= 	$(wildcard *.c)  $(wildcard *.cc)
SOURCE_PUB 	= 	 $(wildcard $(SRC_PUBLIC)/*.c)

TEMPDIR	:= _obj
TEMPDIR_PUB	:= $(SRC_PUBLIC)/_obj
OBJECTS 	= 	$(patsubst %.c,$(TEMPDIR)/%.o,$(SOURCE))
OBJECTS_PUB = 	$(patsubst $(TEMPDIR_PUB)%.c,$(TEMPDIR_PUB)/%.o,$(SOURCE_PUB))

PUBLIC_LIB := -lpthread

all: $(TARGET)


$(TEMPDIR)/%.o: %.c
	@if [ ! -d $(TEMPDIR) ]; then mkdir -p $(TEMPDIR); fi
	$(CC) $(CFLAGS) -c $< -o $@ 


$(TARGET): $(OBJECTS)
	@echo ""
	@echo "#################### Create $(TARGET)... ####################"

	$(CC) -o  $@ $(CFLAGS) $(OBJECTS) $(OBJECTS_PUB) $(PUBLIC_LIB) 

	@echo "#################### Compile OK! ####################"
	@echo ""


print:
	@echo $(PWD)

clean:
	@echo "#################### Clean Start ####################"
	find ./_obj -type f -name '*.o' -print | xargs rm -f

	rm $(TARGET) -f
	@echo "#################### Clean End ####################"
	@echo ""


