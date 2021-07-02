include ../Rules.make

TARGET			:=

CFLAGS 		= 	-I .  -O2 -Wall $(PLATFORM)

SOURCE 		= 	$(wildcard *.c)  $(wildcard *.cc)

TEMPDIR		:= ./_obj
OBJECTS 	= 	$(patsubst %.c,$(TEMPDIR)/%.o,$(SOURCE))


all:$(OBJECTS)

$(TEMPDIR)/%.o: %.c
	@if [ ! -d $(TEMPDIR) ]; then mkdir -p $(TEMPDIR); fi
	$(CC) $(CFLAGS) -c $< -o $@ 
	
	
$(TARGET): $(OBJECTS)
	@echo "#################### Compile OK! ####################"
	@echo ""

print:
	@echo $(PWD)

clean:
	@echo "#################### Clean Start$(PWD) ####################"
	find ./_obj -type f -name '*.o' -print | xargs rm -f
	@echo "#################### Clean End ####################"
	@echo ""


