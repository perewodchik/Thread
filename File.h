#pragma once
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

class File
{
public:
	enum class FILETYPE { jpg, mp3 };
	File(fs::path path, FILETYPE type)
	{
		m_path = path;
		m_fileType = type;
	}
private:
	FILETYPE m_fileType;
	fs::path m_path;
};

