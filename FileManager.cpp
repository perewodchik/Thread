#include "FileManager.h"


FileManager::FileManager() {
	m_availableThreads = std::thread::hardware_concurrency();
	if (m_availableThreads == 0)
		m_availableThreads = 2;
	std::cout << m_availableThreads << std::endl;
	m_currentDirectory = fs::current_path();
}

void FileManager::run() {
	m_readThread = std::move(std::thread(&FileManager::readFiles,this));
	m_availableThreads--;
	while (m_availableThreads--)
	{
		
		m_writeThreads.push_back(std::thread(&FileManager::copyFiles, this));
	}
	m_readThread.join();
}
void FileManager::readFiles()
{
	auto filesDirectory = m_currentDirectory / "files";
	while (true)
	{
		for (auto& file : fs::directory_iterator(filesDirectory))
		{
			auto fileName = file.path().filename().string();
			std::cout << fileName  << "\n";
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

			/*Записываем собранный файл в очередь*/
			m_filesQ.push(std::make_shared<File>
				(file.path().filename().string(),
					buffer, size));

			fileStream.close();
			fs::remove(file.path());
			
			m_conditionVar.notify_one();
		}

		std::this_thread::sleep_for(1s);
	}
	
}
void FileManager::copyFiles() {
	while (true)
	{
		/*Ожидаем файл*/
		std::unique_lock<std::mutex> uq(m_writeMutex);
		std::cout << "Waiting jopa\n";
		m_conditionVar.wait(uq, [this] { return !m_filesQ.empty(); });

		std::cout << "Writing jopa\n";

		/*Копируем файл в папку*/
		auto cFile = m_filesQ.pop();
		auto filesDirectory = m_currentDirectory / "jpg";
		std::cout << filesDirectory / cFile->fileName << std::endl;
		std::ofstream outStream(filesDirectory / cFile->fileName, std::ios::binary | std::ios::out);
		if (!outStream.is_open())
			std::cout << "Try agane\n";

		outStream.write(cFile->buffer, cFile->size);
		delete[] cFile->buffer;
	}
}