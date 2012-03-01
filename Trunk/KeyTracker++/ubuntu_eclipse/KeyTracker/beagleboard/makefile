CFLAGS := `pkg-config --cflags opencv`
LDFLAGS := `pkg-config --libs opencv`
EXEFILES = 

all: $(EXEFILES) KeyTracker

KeyTracker: ../headers/messenger.h ../source/messenger.cpp ../global.h ../cvFunction.h ../robot_main.cpp
	g++ $(CFLAGS) -o $@ ../source/messenger.cpp ../robot_main.cpp $(LDFLAGS) -D BEAGLEBOARD=0

Test: ../headers/messenger.h ../source/messenger.cpp ../global.h ../cvFunction.h ../test_main.cpp
	g++ $(CFLAGS) -o $@ ../source/messenger.cpp ../test_main.cpp $(LDFLAGS)

$(EXEFILES): %:%.cpp
	g++ $(CFLAGS) $(LDFLAGS) -o $@ $<  

