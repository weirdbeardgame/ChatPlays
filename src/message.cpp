#include "message.h"

void Message::enque(std::string mess)
{
	std::unique_lock<std::mutex>(lock);
	messageQueue.push(mess);
	condition.notify_one(); // Post an event to all threads the action is completed
}

std::string Message::dequeue()
{
	std::unique_lock<std::mutex>(lock);
	std::string mess = messageQueue.front();
	messageQueue.pop();
	return mess;
}