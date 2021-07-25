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

int Emit::CreateController()
{

	driver = vigem_alloc();
	if (driver == nullptr)
	{
		std::cerr << "Oops! Driver no allocate! Unga Bunga. Me confused!" << std::endl;
		return -1;
	}
	else
	{
		// We have driver. Now establish bus connection.
		const auto bus = vigem_connect(driver);
		if (!VIGEM_SUCCESS(bus))
		{
			std::cerr << "ViGEm Bus connection failed with error code: 0x" << std::hex << bus << std::endl;
			return -1;
		}

		xbox = vigem_target_x360_alloc();
		const auto plugEv = vigem_target_add(driver, xbox);
		if (!VIGEM_SUCCESS(plugEv))
		{
			std::cerr << "Target plugin failed with error code: 0x" << std::hex << plugEv << std::endl;
			return -1;
		}
		vigem_target_x360_register_notification(driver, xbox, notification, nullptr);
	}
}

bool Emit::emit(Buttons& cmd)
{
	axisData axis;
	switch (cmd)
	{
	case Buttons::UP:
		// Set to max values of Xinput
		axis.set(0, 65535, 0, 0); 
		break;
	case Buttons::DOWN:
		axis.set(0, -65535, 0, 0);
		break;
	case Buttons::RIGHT:
		axis.set(65535, 0, 0, 0);
		break;
	case Buttons::LEFT:
		axis.set(-65535, 0, 0, 0);
		break;
	}

	if (cmd > Buttons::RIGHT)
	{
		return pressBtn(cmd);
	}
	else
	{
		moveABS(axis);
		Sleep(1);
		resetABS();
	}
}

int Emit::pressBtn(Buttons& btn)
{
	report = new XUSB_REPORT();
	report->wButtons |= 1 << buttonPos[btn];
	return vigem_target_x360_update(driver, xbox, *report);
}

int Emit::releaseBtn(Buttons& btn)
{
	report = new XUSB_REPORT();
	report->wButtons &= ~(1 << buttonPos[btn]);
	return vigem_target_x360_update(driver, xbox, *report);
}

int Emit::moveABS(axisData axis)
{
	report = new XUSB_REPORT();
	report->sThumbLX = axis.get(0);
	report->sThumbLY = axis.get(1);
	report->sThumbRX = axis.get(2);
	report->sThumbRY = axis.get(3);
	return vigem_target_x360_update(driver, xbox, *report);
}

int Emit::resetABS()
{
	report = new XUSB_REPORT();
	report->sThumbLX = 32767;
	report->sThumbLY = 32767;
	report->sThumbRX = 32767;
	report->sThumbRY = 32767;
	return vigem_target_x360_update(driver, xbox, *report);
}

Buttons& Emit::GetCommands(std::string key)
{
	Buttons c = CLEAR;
	if (commands.find(key) != commands.end())
	{
		std::cout << "Command found" << std::endl;
		return commands[key];
	}
	else
	{
		return c;
	}
}