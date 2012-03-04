#ifndef CAMERA_DATA_H_
#define CAMERA_DATA_H_

namespace ACTION
{
namespace CAMERA
{
enum targetStatus
{
	TOP, BOTTOM, NO_TARGET
};

const int8_t INVALID_DATA = 0x80; //-128
}
}

struct KeyTrackerData
{
	uint8_t red;
	uint8_t blue;
	uint8_t higher;
};

struct AlignmentData
{
	int8_t arbitraryOffsetFromUDP;
	ACTION::CAMERA::targetStatus status;
};

struct camera
{
	KeyTrackerData key;
	AlignmentData align;
};

#endif
