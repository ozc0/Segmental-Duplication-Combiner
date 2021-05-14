COMBINER_VERSION := "0.1"
COMBINER_UPDATE := "May 12, 2021"
COMBINER_DEBUG := 0
BUILD_DATE := "$(shell date)"
CC=g++
CFLAGS =  -O3 -funroll-loops -g -I htslib -DRANDOM_VERSION=\"$(RANDOM_VERSION)\" -DBUILD_DATE=\"$(BUILD_DATE)\" -DRANDOM_UPDATE=\"$(RANDOM_UPDATE)\" -DRANDOM_DEBUG=$(RANDOM_DEBUG)
LDFLAGS = -lz -lm -lpthread -llzma -lbz2 -lcurl
NOCRAMFLAGS = -lz -lm -lpthread
SOURCES = seg_dup_combiner.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = combiner
INSTALLPATH = /usr/local/bin/

.PHONY: 

all: $(SOURCES) $(EXECUTABLE)
	rm -rf *.o

combiner: 

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) *.o *~

nocram: $(OBJECTS)
	cd autoheader && autoconf && ./configure --disable-lzma --disable-bz2 --disable-libcurl && make && cd ..
	$(CC) $(OBJECTS) -o $(EXECUTABLE)-nocram $(NOCRAMFLAGS)

install:
	cp $(EXECUTABLE) $(INSTALLPATH)
