#include "Windows/control.h"

Emit::Emit()
{

}

Emit::Emit(json j)
{

}


bool Emit::CreateController()
{
	VjdStat driverCheck = GetVJDStatus(dStat);
	switch (driverCheck)
	{
	case VJD_STAT_OWN:
		printf("vJoy Device %d is already owned by this feeder\n", dStat);
		break;
	case VJD_STAT_FREE:
		printf("vJoy Device %d is free\n", dStat);
		break;
	case VJD_STAT_BUSY:
		printf("vJoy Device %d is already owned by another feeder\nCannot continue\n", dStat);
		return -3;
	case VJD_STAT_MISS:
		printf("vJoy Device %d is not installed or disabled\nCannot continue\n", dStat);
		return -4;
	default:
		printf("vJoy Device %d general error\nCannot continue\n", dStat);
		return -1;
	};

	if (driverCheck == VJD_STAT_OWN || driverCheck == VJD_STAT_FREE)
	{
		// The driver is claimable!
		int err = AcquireVJD(dStat);
		if (err < 0)
		{
			printf("Error claiming device! \n");
		}
	}

	// The device driver is claimed. Let's make a device and do somthin with it!

}