#pragma once
#include <thread>
#include <vector>
#include "SafeQueue.h"
#include "File.h"
#include <fstream>
#include <sstream>


class FileManager
{
public:
	FileManager() = default;
	void readFiles()
	{
		auto currentDirectory = fs::current_path();
		auto filesDirectory = currentDirectory / "files";
		for (auto& file : fs::directory_iterator(filesDirectory))
		{
			std::cout << file.path().filename().string() << "\n";
			if (!(file.path().extension() == ".jpg" |
			file.path().extension() == ".mp3"))
				continue;

			/*Открываем файл для чтения*/
			std::ifstream fileStream;
			fileStream.open(file.path(), std::ios::binary | std::ios::in);
			if (!fileStream.is_open())
				std::cout << "Could not open file\n";

			/*Собираем информацию: буффер и размер*/
			auto pbuf = fileStream.rdbuf();
			int size = pbuf->pubseekoff(0, std::ios::end, std::ios::in);
			pbuf->pubseekpos(0, std::ios::in);
			char* buffer = new char[size];
			pbuf->sgetn(buffer, size);

			/*Открываем файл для записи*/
			std::ofstream outStream(file.path().parent_path().parent_path() / file.path().filename(), 
				std::ios::binary | std::ios::out);
			if (!outStream.is_open())
				std::cout << "Could not open file\n";

			/*Записываем данные в новый файл*/
			outStream.write(buffer, size);

			/*Высвобождаем буффер*/
			delete[] buffer;
		}
	}
private:
	std::thread m_readThread;
	std::vector<std::thread> m_threads;
	SafeQueue<File> m_files;
};

