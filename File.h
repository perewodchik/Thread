#pragma once
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

struct File
{
	File(fs::path _path, char* _buffer, int _size)
	{
		path = _path;
		buffer = _buffer;
		size = _size;
	}

	fs::path path;
	char* buffer;
	int size;
};

