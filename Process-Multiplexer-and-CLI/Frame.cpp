// ----- << External Library >> ----- //
#include "Frame.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

// ----- << Implementation >> ----- //
Frame::Frame(unsigned long long ID, size_t size)
	: frameID(ID), frameSize(size)
{
	// Eeach address location consumes 1 byte
	for (unsigned long long i = 0; i < size; i++)
	{
		std::ostringstream stream;
		stream	<< std::hex << std::uppercase
				<< std::setw(5) << std::setfill('0') << i;
		std::string key = stream.str();

		frameData[key] = "00";
	}
}

Frame::~Frame() = default;

unsigned long long Frame::getID() const
{
	return frameID;
}

std::string Frame::read(std::string frameAddress)
{
	return frameData.at(frameAddress);
}

void Frame::write(std::string frameAddress, std::string data)
{
	//std::unique_lock<std::mutex> writeLock(frameWriteMutex);
	auto frame = frameData.find(frameAddress);
	
	if (frame != frameData.end())
	{
		frame->second = data;
	}
	else
	{
		// Do something here...
	}
}

bool Frame::operator==(const Frame& frame) const
{
	return frameID == frame.getID();
}

void Frame::print() const
{
	std::cout << "\n\nFrame ID: " << frameID << "\n\n";
	for (auto pair = frameData.begin(); pair != frameData.end(); pair++)
	{
		std::cout << pair->first << " : " << pair->second << "\n";
	}
}

std::size_t FrameHash::operator()(const Frame& frame) const
{
	return std::hash<unsigned long long>()(frame.getID());
}