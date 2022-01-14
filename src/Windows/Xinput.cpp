#include "Windows/Xinput.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <mutex>


VOID CALLBACK notification(
	PVIGEM_CLIENT Client,
	PVIGEM_TARGET Target,
	UCHAR LargeMotor,
	UCHAR SmallMotor,
	UCHAR LedNumber,
	LPVOID UserData
)
{
	static int count = 1;

	// Send Data to Twitch Chat. Rumble Goes BRRRRRRR
	/*std::cout.width(3);
	std::cout << count++ << " ";
	std::cout.width(3);
	std::cout << (int)LargeMotor << " ";
	std::cout.width(3);
	std::cout << (int)SmallMotor << std::endl;*/
}

bool Emit::AttachController()
{
	driver = vigem_alloc();
	if (driver == nullptr)
	{
		std::cerr << "Oops! Driver no allocate! Unga Bunga. Me confused!" << std::endl;
		isActive = false;
		return -1;
	}
	else
	{
		// We have driver. Now establish bus connection.
		const auto bus = vigem_connect(driver);
		if (!VIGEM_SUCCESS(bus))
		{
			std::cerr << "ViGEm Bus connection failed with error code: 0x" << std::hex << bus << std::endl;
			isActive = false;
			return -1;
		}

		xbox = vigem_target_x360_alloc();
		const auto plugEv = vigem_target_add(driver, xbox);
		if (!VIGEM_SUCCESS(plugEv))
		{
			std::cerr << "Target plugin failed with error code: 0x" << std::hex << plugEv << std::endl;
			isActive = false;
			return -1;
		}
		vigem_target_x360_register_notification(driver, xbox, notification, nullptr);
		isActive = true;
	}
}

bool Emit::SendInput(Buttons cmd, ABS a, float axis, int heldFor)
{
	report = new XUSB_REPORT();
	bool err = false;

	err = PressButton(cmd);
	Sleep(heldFor);
	err = ReleaseButton(cmd);
	if (a != ABS::CLEAR)
	{
		err = MoveABS(a, axis);
		Sleep(heldFor);
		err = ResetABS(a);
	}
	return err;
}

int Emit::PressButton(Buttons btn)
{
	if (report == nullptr)
	{
		report = new XUSB_REPORT();
	}
	report->wButtons |= buttonPos[btn];

	return vigem_target_x360_update(driver, xbox, *report);
}

int Emit::ReleaseButton(Buttons btn)
{
	report->wButtons = 0; //&= ~(1 << buttonPos[btn]);
	return vigem_target_x360_update(driver, xbox, *report);
}

int Emit::MoveABS(ABS abs, float axis)
{
	switch (abs)
	{
	case ABS::L_X_AXIS:
		report->sThumbLX += axis;
		break;

	case ABS::L_Y_AXIS:
		report->sThumbLY += axis;
		break;

	case ABS::R_X_AXIS:
		report->sThumbRX += axis;
		break;

	case ABS::R_Y_AXIS:
		report->sThumbRY += axis;
		break;

	case ABS::L2:
		report->bLeftTrigger += axis;
		break;

	case ABS::R2:
		report->bRightTrigger += axis;
		break;
	}
	return vigem_target_x360_update(driver, xbox, *report);
}

int Emit::ResetABS(ABS abs)
{
	report->sThumbLX = 0;
	report->sThumbLY = 0;
	report->sThumbRX = 0;
	report->sThumbRY = 0;
	report->bRightTrigger = 0;
	report->bLeftTrigger = 0;
	return vigem_target_x360_update(driver, xbox, *report);
}

void Emit::DisconnectController()
{
	vigem_target_x360_unregister_notification(xbox);
	vigem_disconnect(driver);
}