#pragma once
#include "Vertex.h"
#include <fstream>
#include <vector>

class OBJConverter
{
public:
	OBJConverter();
	~OBJConverter();

	unsigned int Convert(char* _filename);

private:
	FILE*								m_pFile = nullptr;
	std::fstream							m_fstream;

	std::vector<uint32_t*>					m_vIndex;
	std::vector<basic_vert*>				m_vVert;

	unsigned int Read(char* _filename);
	unsigned int Write(char* _filename);
	unsigned int Clean();
};

