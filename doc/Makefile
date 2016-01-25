O ?= doxy-mamma

all: doxygen

clean:
	$(RM) -rf $(O)
doxygen:
        GIT_VERSION=$(GIT_VERSION) OUTPUT=$(O) doxygen ./doxygen-mamma-config

.PHONY: all clean doxygen
