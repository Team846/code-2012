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
#warning Figure out what goes here
	uint8_t slop;
	bool top;
};

struct camera
{
	KeyTrackerData key;
	AlignmentData align;
};

#endif
