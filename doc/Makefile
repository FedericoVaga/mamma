GIT_VERSION = $(shell cd $(src); git describe --dirty --long --tags)

all: doxygen

clean:
	$(RM) -rf doxy-mamma
doxygen:
	GIT_VERSION=$(GIT_VERSION) doxygen ./doxygen-mamma-config

.PHONY: all clean doxygen
