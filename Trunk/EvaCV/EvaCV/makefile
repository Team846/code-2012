	 
CFLAGS := -g -I /usr/include/
LDFLAGS := -L/usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann  -rdynamic
EXEFILES = 

all: $(EXEFILES) reportTarget

reportTarget: reportTarget.h reportTarget.cpp reportTargetMain.cpp
	g++ $(CFLAGS) -o $@ reportTarget.cpp reportTargetMain.cpp  $(LDFLAGS) 

$(EXEFILES): %:%.cpp
	g++ $(CFLAGS) $(LDFLAGS) -o $@ $<  

