#include "Windows/control.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <mutex>

Emit::Emit()
{
	control = json
	{
		{"commands", commands},
		//{"controller", controller},
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
		j += e.control;
	}
	else
	{
		j += control;
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

void Emit::poll()
{
	// Recieve commands from chat and press into emit
	while (isActive)
	{
		std::string keyCode;
		std::cout << "Enter a keycode: ";
		std::cin >> keyCode;

		if (keyCode == "Exit")
		{
			isActive = false;
		}
		cmd = GetCommands(keyCode);
		emit(cmd, false);
	}
}

int Emit::CreateController()
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
		emit(Buttons::CLEAR, true);
	}
}
// Reset will kill input! Input is never sent before reset is called
// This needs to be a loop
void Emit::emit(Buttons cmd, bool manualControl)
{
	std::string keyCode;
	while (isActive)
	{
		report = new XUSB_REPORT();
		Sleep(500);
		resetABS();
		releaseBtn(cmd);
		vigem_target_x360_update(driver, xbox, *report);
		if (manualControl)
		{
			// I need a better way to poll this.
			std::cout << "Enter a keycode: ";
			std::cin >> keyCode;

			if (keyCode == "Exit")
			{
				isActive = false;
			}
			cmd = GetCommands(keyCode);
		}
		axisData axis;
		switch (cmd)
		{
		case Buttons::UP:
			// Set to max values of Xinput
			axis.set(0, 32767, 0, 0);
			break;
		case Buttons::DOWN:
			axis.set(0, -32768, 0, 0);
			break;
		case Buttons::RIGHT:
			axis.set(32767, 0, 0, 0);
			break;
		case Buttons::LEFT:
			axis.set(-32768, 0, 0, 0);
			break;
		}

		if (cmd > Buttons::RIGHT)
		{
			pressBtn(cmd);
		}
		else
		{
			moveABS(axis);
		}
		vigem_target_x360_update(driver, xbox, *report);
	}
}

void Emit::pressBtn(Buttons& btn)
{
	if (report == nullptr)
	{
		report = new XUSB_REPORT();
	}
	report->wButtons |= buttonPos[btn];
}

void Emit::releaseBtn(Buttons& btn)
{
	report->wButtons = 0; //&= ~(1 << buttonPos[btn]);
}

void Emit::moveABS(axisData& axis)
{
	report->sThumbLX = axis.get(0);
	report->sThumbLY = axis.get(1);
	report->sThumbRX = axis.get(2);
	report->sThumbRY = axis.get(3);
}

void Emit::resetABS()
{
	report->sThumbLX = 0;
	report->sThumbLY = 0;
	report->sThumbRX = 0;
	report->sThumbRY = 0;
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