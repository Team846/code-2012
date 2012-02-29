#ifndef CAMERA_DATA_H_
#define CAMERA_DATA_H_

struct KeyTrackerData
{
	uint8_t red;
	uint8_t blue;
	uint8_t higher;
};

struct AlignmentData
{
	uint8_t arbitraryOffsetFromUDP;
	bool isUpperTarget;
};

struct camera
{
	KeyTrackerData key;
	AlignmentData align;
};

#endif
