#include "Windows/control.h"
#include <windows.h>
#include <stdio.h>

Emit::Emit()
{
	control = json
	{
		"Emit" ,
		{
			{"commands", commands},
			//{"controller", controller},
		}
	};

}

Emit::Emit(json j)
{
	//from_json(j, *this);
}

void Emit::save(json& j, bool isDefault)
{
	if (isDefault)
	{
		Emit e = Emit();
		j.push_back(e.control);
	}
	else
	{
		j.push_back(control);
	}
}

void Emit::initalConfig()
{

}

int Emit::CreateController()
{

	dStat = 1;

	// Get the driver attributes (Vendor ID, Product ID, Version Number)
	if (!vJoyEnabled()) 
	{
		printf("Function vJoyEnabled Failed - make sure that vJoy is installed and enabled\n");
		int dummy = getchar();
		dStat = -2;
		return false;
	}
	else 
	{
		// Why did they use a wide const char here?
		wprintf(L"Vendor: %s\nProduct :%s\nVersion Number:%s\n", static_cast<TCHAR*> (GetvJoyManufacturerString()), static_cast<TCHAR*>(GetvJoyProductString()), static_cast<TCHAR*>(GetvJoySerialNumberString()));
	};

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

	// The device is claimed!
	controller.bDevice = (BYTE)dStat;
}

bool Emit::emit(Buttons cmd)
{
	axisData axis;
	switch (cmd)
	{
	case Buttons::UP:
		// Set to max values of Xinput
		axis.set(0, 32767, 0, 0); 
		break;
	case Buttons::DOWN:
		axis.set(0, -32767, 0, 0);
		break;
	case Buttons::RIGHT:
		axis.set(32767, 0, 0, 0);
		break;
	case Buttons::LEFT:
		axis.set(-32767, 0, 0, 0);
		break;
	}

	if (cmd > Buttons::RIGHT)
	{
		return pressBtn(cmd);
	}
}

int Emit::pressBtn(Buttons btn)
{
	JOYSTICK_POSITION pos;
	pos.lButtons |= 1 << buttonPos[btn];
	UpdateVJD(dStat, &pos);
	Sleep(1);
	return releaseBtn(btn);
}

int Emit::releaseBtn(Buttons btn)
{
	JOYSTICK_POSITION pos;
	pos.lButtons &= ~(1 << buttonPos[btn]);
	return UpdateVJD(dStat, &pos);
}

int Emit::moveABS(axisData axis)
{
	JOYSTICK_POSITION pos;
	pos.wAxisX = axis.get(0);
	pos.wAxisY = axis.get(1);
	pos.wAxisVX = axis.get(2);
	pos.wAxisVY = axis.get(3);
	return UpdateVJD(dStat, &pos);
}

int Emit::resetABS()
{
	ResetAll();
	return 0;
}

Buttons Emit::GetCommands(std::string key)
{
	if (commands.find(key) != commands.end())
	{
		std::cout << "Command found" << std::endl;
		return commands[key];
	}
	else
	{
		return Buttons(-1);
	}
}