#pragma once
#include <string>
#include <fstream>
#include <vector>

namespace Core::Emulator
{
    class Reader
    {
    public:
        Reader() {}
        Reader(const std::string& StreamPath)
        {
            this->SetStream(StreamPath);
        }

        bool SetStream(const std::string& StreamPath)
        {
            if(m_CurrentStream.is_open())
            {
                m_CurrentStream.close();
            }
            this->m_CurrentStream = std::ifstream(StreamPath, std::ios::binary);
            return this->m_CurrentStream.good();
        }

        std::streamoff Seek(const std::streamoff& Offset, const std::ios::seekdir& Method = std::ios::cur)
        {
            this->m_CurrentStream.seekg(Offset, Method);
            return this->m_CurrentStream.tellg();
        }

        std::vector<std::uint8_t> ReadBytes(std::int64_t Count)
        {
            std::vector<std::uint8_t> Bytes(Count);
            this->m_CurrentStream.read(reinterpret_cast<char*>(Bytes.data()), Count);
            return Bytes;
        }

    private:
        std::ifstream m_CurrentStream;
    };
}