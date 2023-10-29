CC=gcc

ifeq ($(INPROC_LIB_NAME),)
INPROC_LIB_NAME := libinproc_inject.so
endif

ifeq ($(INPROC_EXECUTABLE_NAME),)
INPROC_EXECUTABLE_NAME := inproc
endif

ifeq ($(INPROC_OUTPUT_DIR),)
INPROC_OUTPUT_DIR := build
endif

all: pre app lib
pre:
	mkdir -p $(INPROC_OUTPUT_DIR)
app: pre
	$(CC) -o $(INPROC_OUTPUT_DIR)/$(INPROC_EXECUTABLE_NAME) src/main.c
lib: pre
	$(CC) -shared -fPIC -fvisibility=hidden -lssl -o $(INPROC_OUTPUT_DIR)/$(INPROC_LIB_NAME) src/lib.c 
test:
	$(CC) -O3 -lssl3 -Wl,-z,relro,-z,now -fno-plt -o $(INPROC_OUTPUT_DIR)/test test/test.c
