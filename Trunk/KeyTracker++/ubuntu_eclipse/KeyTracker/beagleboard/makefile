CFLAGS := `pkg-config --cflags opencv`
LDFLAGS := `pkg-config --libs opencv`
CC = g++

XCFLAGS := `$(CROSS_COMPILE)pkg-config --cflags opencv`
XLDFLAGS := `$(CROSS_COMPILE)pkg-config --libs opencv`
XCC = $(CROSS_COMPILE)g++

TRANSFER = scp -C
DESTINATION = ubuntu@omap.local:/home/ubuntu/KeyTracker/beagleboard

all: x86 test bb

x86: ../headers/messenger.h ../source/messenger.cpp ../global.h ../cvFunction.h ../robot_main.cpp
	$(CC) $(CFLAGS) -o KeyTracker_x86 ../source/messenger.cpp ../robot_main.cpp $(LDFLAGS) -D BEAGLEBOARD

test: ../headers/messenger.h ../source/messenger.cpp ../global.h ../cvFunction.h ../test_main.cpp
	$(CC) $(CFLAGS) -o KeyTracker_gui ../source/messenger.cpp ../test_main.cpp $(LDFLAGS)

bb: ../headers/messenger.h ../source/messenger.cpp ../global.h ../cvFunction.h ../robot_main.cpp
	$(XCC) $(XCFLAGS) -o KeyTracker ../source/messenger.cpp ../robot_main.cpp $(XLDFLAGS) -D BEAGLEBOARD

install: bb
	ssh ubuntu@omap.local -C "rm -f /home/ubuntu/KeyTracker/beagleboard/KeyTracker"
	$(TRANSFER) ./KeyTracker $(DESTINATION)

clean:
	rm -rf KeyTracker*

$(EXEFILES): %:%.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<  

