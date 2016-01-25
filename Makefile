DIRS := lib examples doc

all clean:
	$(MAKE) TARGET=$@ $(DIRS)

$(DIRS):
	$(MAKE) -C $@ $(TARGET) MAMMA=`pwd`

.PHONY: clean all examples lib doc
