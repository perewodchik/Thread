#pragma once
#include "File.h"
#include "SafeQueue.h"
#include <thread>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <condition_variable>

namespace fs = std::filesystem;
using namespace std::chrono_literals;

class FileManager
{
public:
	FileManager();
	void run();
	void readFiles();
	void copyFiles();
private:
	std::thread m_readThread;
	std::vector<std::thread> m_writeThreads;
	std::mutex m_writeMutex;
	std::condition_variable m_conditionVar;
	SafeQueue<File> m_filesQ;
	int m_availableThreads;
	fs::path m_currentDirectory;
};

