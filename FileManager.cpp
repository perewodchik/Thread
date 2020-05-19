#include "FileManager.h"
#include <iomanip>

FileManager::FileManager() {
	/*������ ���������� ������� */
	m_availableThreads = std::thread::hardware_concurrency();
	if (m_availableThreads == 0)
		m_availableThreads = 2;
	std::cout << "AVAILABLE THREADS: " << m_availableThreads << std::endl;

	m_currentDirectory = fs::current_path();
}

void FileManager::run() {
	/*��������� ������  ������ � ������*/
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
			/*���������� �������� �����*/
			if (!(file.path().extension() == ".jpg" |
				file.path().extension() == ".mp3"))
				continue;

			/*��������� ���� ��� ������*/
			std::ifstream fileStream;
			fileStream.open(file.path(), std::ios::binary | std::ios::in);
			if (!fileStream.is_open()) {
				std::cout << "Could not open file\n";
				continue;
			}
			std::cout << "READ THREAD" << std::this_thread::get_id()
				<< ": reading " << file.path().filename().string() << "\n";

			/*�������� ����������: ������ � ������*/
			auto pbuf = fileStream.rdbuf();
			int size = pbuf->pubseekoff(0, std::ios::end, std::ios::in);
			pbuf->pubseekpos(0, std::ios::in);
			char* buffer = new char[size];
			pbuf->sgetn(buffer, size);
			/*��������� 1: ���������� ������ � ������� 
			����� ��������� ����� ��� �����������*/

			/*���������� ��������� ���� � �������*/
			m_filesQ.push(std::make_shared<File>
				(file.path(), buffer, size));

			/*������� ���� �� �����*/
			fileStream.close();
			fs::remove(file.path());
			
			/*�������� ����������� ��� �����������*/
			m_conditionVar.notify_one();
		}

		std::this_thread::sleep_for(1s);
	}
}

void FileManager::copyFiles() {
	while (true)
	{
		/*������� ����*/
		std::unique_lock<std::mutex> uq(m_writeMutex);
		m_conditionVar.wait(uq, [this] { return !m_filesQ.empty(); });


		/*������� ���� �� �������*/
		auto cFile = m_filesQ.pop();
		auto filesDirectory = m_currentDirectory / cFile->path.filename().extension().string().substr(1);

		/*�������� ���� � �����*/
		std::ofstream outStream(filesDirectory / cFile->path.filename().string(), std::ios::binary | std::ios::out);
		if (!outStream.is_open()) {
			std::cout << "Could not open file for writing\n";
			continue;
		}
		outStream.write(cFile->buffer, cFile->size);
		std::cout << "WRITE THREAD" << std::this_thread::get_id() << ": copied " << cFile->path.filename().string() << "\n";

		/*������������ ������*/
		delete[] cFile->buffer;
		
		
	}
}