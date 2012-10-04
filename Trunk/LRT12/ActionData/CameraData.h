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
	int32_t arbitraryOffsetFromUDP;
	bool hasNewData;
	uint32_t distance;
	ACTION::CAMERA::targetStatus status;
};

typedef struct 
{
	uint32_t x, y, radius;
} BallInfo;
#define MAX_NUM_BALLS 32

struct CameraData
{
	KeyTrackerData key;
	AlignmentData align;
	
	SEM_ID ballSem;
	BallInfo balls[MAX_NUM_BALLS];
	int numDetectedBalls;
	bool hasBeenUpdated;
	CameraData()
	{
		key.red = key.blue = key.higher = 0;
		align.status = ACTION::CAMERA::NO_TARGET;
		align.arbitraryOffsetFromUDP = 0;
		ballSem = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE);
		numDetectedBalls = 0;
		hasBeenUpdated = false;
	}
};

#endif
