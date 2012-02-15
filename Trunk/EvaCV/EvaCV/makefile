	 
CFLAGS := -g -I /usr/include/opencv 
LDFLAGS := -L /usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann 
EXEFILES = 

all: $(EXEFILES) reportTarget

reportTarget: reportTarget.h reportTarget.cpp reportTargetMain.cpp
	g++ $(CFLAGS) $(LDFLAGS) -o $@ reportTarget.cpp reportTargetMain.cpp   

$(EXEFILES): %:%.cpp
	g++ $(CFLAGS) $(LDFLAGS) -o $@ $<  

