	 
CFLAGS := -g -I /usr/include/opencv-2.3.1
LDFLAGS := -L/usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann  -rdynamic
EXEFILES = 

all: $(EXEFILES) KeyTracker

KeyTracker: ../headers/messenger.h ../source/messenger.cpp ../global.h ../sat_main.cpp
	g++ $(CFLAGS) -o $@ ../source/messenger.cpp ../sat_main.cpp  $(LDFLAGS) 

$(EXEFILES): %:%.cpp
	g++ $(CFLAGS) $(LDFLAGS) -o $@ $<  

