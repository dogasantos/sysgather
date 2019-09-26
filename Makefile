CC=gcc
CFLAGS=-Wall -ggdb -fPIE -pie
PREFIX=/opt/cpgather
COMPILE = $(CC) $(CFLAGS)
DOC=doc
SRC=src
BINS=cpgather

all:cpgather

cpgather: core-main.c core-environ.c core-misc.c core-network.c core-security.c core-services.c core-user.c core-output.c core-plugins.c core-list.c
	$(COMPILE) core-main.c core-environ.c core-misc.c core-network.c core-security.c core-services.c core-user.c core-output.c core-plugins.c core-list.c -o ${BINS}

clean:
	rm -rf ${BINS}

uninstall: 
	rm -rf ${PREFIX}

install:
	mkdir -p ${PREFIX}/bin
	mkdir -p ${PREFIX}/src
	install -m 555 ${BINS} ${PREFIX}/bin/
	mv ${DOC} ${PREFIX}/doc
	cp *.c ${PREFIX}/src
	cp -rf lib ${PREFIX}/src/

