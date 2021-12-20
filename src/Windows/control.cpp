#include "Windows/control.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <mutex>

Emit::Emit(json j, Message* q)
{
	queue = q;
	from_json(j, *this);
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
		control = *this;
		j += control;
	}
}

Emit* Emit::InitalConfig()
{
	std::string toEnter;
	std::map<std::string, Buttons> tempBuffer;
	
	std::cout << "Please Enter commands for chat: " << std::endl;
	std::cout << "Default will return default settings" << std::endl;

	if (commands.size() > 0)
	{
		std::cout << "Quit will return to previous settings" << std::endl;

		tempBuffer = std::move(commands);

		if (toEnter == "Quit")
		{
			std::cout << "Quit Typed" << std::endl;
			commands = std::move(tempBuffer);
			return this;
		}
	}
	for (int i = 0; i < commandEnumList.size(); i++)
	{
		std::cout << commandEnumList[i] << ": ";
		std::cin >> toEnter;

		if (toEnter == "Default")
		{
			std::cout << "Setting Commands to default" << std::endl;
			commands = std::move(defaultCommands);
			return this;
		}
		commands.emplace(toEnter, (Buttons)i);
		toEnter.clear();
	}

	// Why is this returning null. What's not being initalized in the background correctly?
	return this;
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

// Thread needs to switch to this function. Whoops!

void Emit::ControllerThread(Message* q, Emit settings, bool manualControl)
{
	std::string keyCode;
	if (!isActive)
	{
		CreateController(q, settings);
	}
	// Recieve commands from chat and press into emit
	while (isActive)
	{
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
			emit(q, cmd);
			report = new XUSB_REPORT();
			Sleep(500);
			resetABS();
			releaseBtn(cmd);
			vigem_target_x360_update(driver, xbox, *report);

		}
		else if (manualControl == false)
		{
			std::string keyCode = queue->dequeue();
			// It never seems to get past this.
			if (keyCode != std::string())
			{
				cmd = GetCommands(keyCode);
				if (cmd != Buttons::CLEAR)
				{
					emit(q, cmd);
					report = new XUSB_REPORT();
					Sleep(500);
					resetABS();
					releaseBtn(cmd);
					vigem_target_x360_update(driver, xbox, *report);
				}
			}
		}
	}
}

int Emit::CreateController(Message* q, Emit settings)
{
	driver = vigem_alloc();
	queue = q;
	commands = std::move(settings.commands);
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
	return 0;
}
// Reset will kill input! Input is never sent before reset is called
// This needs to be a loop
void Emit::emit(Message* q, Buttons cmd)
{
	report = new XUSB_REPORT();
	axisData axis;
	switch (cmd)
	{
	case Buttons::UP:
		// Set to max values of Xinput
		axis.set(0, 32767, 0, 0, 0, 0);
		break;
	case Buttons::DOWN:
		axis.set(0, -32768, 0, 0, 0, 0);
		break;
	case Buttons::RIGHT:
		axis.set(32767, 0, 0, 0, 0, 0);
		break;
	case Buttons::LEFT:
		axis.set(-32768, 0, 0, 0, 0, 0);
		break;
	case Buttons::R2:
		axis.set(0, 0, 0, 0, 30, 0);
		break;

	case Buttons::L2:
		axis.set(0, 0, 0, 0, 0, 30);
		break;
	}

	if (cmd > Buttons::R2)
	{
		pressBtn(cmd);
	}
	else
	{
		moveABS(axis);
	}
	vigem_target_x360_update(driver, xbox, *report);
	cmd = Buttons::CLEAR;
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
	report->bRightTrigger = axis.get(4);
	report->bLeftTrigger = axis.get(5);
}

void Emit::resetABS()
{
	report->sThumbLX = 0;
	report->sThumbLY = 0;
	report->sThumbRX = 0;
	report->sThumbRY = 0;
	report->bRightTrigger = 0;
	report->bLeftTrigger = 0;
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

void from_json(const nlohmann::json& j, Emit& p)
{
	j[1]["commands"].get_to(p.commands);
}

void to_json(nlohmann::json& j, const Emit& p)
{
	j["commands"] = p.commands;
}