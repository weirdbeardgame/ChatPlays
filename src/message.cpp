#include "message.h"

Message::Message(const std::string m)
{
	// Were only going to allow this to be run once. The rest would be using the below
	if (messageQueue.empty())
	{
		std::unique_lock<std::mutex>(lock);
		messageQueue.push(m);
		condition.notify_one();
	}
	else
	{
		throw("Error Queue already allocated!");
	}
}

Message::Message(const Message& m)
	:
	messageQueue(m.messageQueue)
{
}

Message& Message::operator=(Message& m)
{
	messageQueue = m.messageQueue;
	return *this;
}

Message& Message::operator=(Message&& m) noexcept
{
	messageQueue = std::move(m.messageQueue);
	return *this;
}

Message::Message(const Message&& m) noexcept
	:
	messageQueue(std::move(m.messageQueue))
{
}

void Message::enque(std::string mess)
{
	std::unique_lock<std::mutex>(lock);
	messageQueue.push(mess);
	condition.notify_one(); // Post an event to all threads the action is completed
}

std::string Message::dequeue()
{
	std::unique_lock<std::mutex>(lock);
	if (messageQueue.empty())
	{
		return std::string();
	}
	std::string mess = messageQueue.front();
	messageQueue.pop();
	return mess;
}