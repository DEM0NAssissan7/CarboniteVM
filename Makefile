all: cvm


WARNINGS = -Wall
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2


cvm: Makefile src/*
	$(CC) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) src/main.c -Wall

clean:
	rm -f cvm

# Builder will call this to install the application before running.
install:
	echo "Installing is not supported"

# Builder uses this target to run your application.
run:
	./cvm

quick:
	make --quiet
	make run --quiet

test:
	make
	./cvm -c example/example.config example/example.cvm
