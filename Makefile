DIRS := lib examples

all clean:
	$(MAKE) TARGET=$@ $(DIRS)

$(DIRS):
	$(MAKE) -C $@ $(TARGET) CUT=`pwd`

.PHONY: clean all examples lib
