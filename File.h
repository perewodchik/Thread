#pragma once
#include <iostream>


struct File
{
	File(std::string _fileName, char* _buffer, int _size)
	{
		fileName = _fileName;
		buffer = _buffer;
		size = _size;
	}

	std::string fileName;
	char* buffer;
	int size;
};

