#include "OBJConverter.h"

OBJConverter::OBJConverter()
{
}


OBJConverter::~OBJConverter()
{
}

unsigned int OBJConverter::Convert(char * _filename)
{
	if (Read(_filename) != 0)
		return Write(_filename);
	return 0;
}

unsigned int OBJConverter::Read(char * _filename)
{
	fopen_s(&m_pFile, _filename, "r");

	if (m_pFile != nullptr)
	{
		std::vector<DirectX::XMFLOAT4>			m_vPosition;
		std::vector<DirectX::XMFLOAT4>			m_vUV;
		std::vector<DirectX::XMFLOAT4>			m_vNormal;

		unsigned int indexCount = 0;
		while (true)
		{
			char header[128];
			int res = fscanf_s(m_pFile, "%s", header, 128);
			if (res == EOF)
				break;

			if (strcmp(header, "v") == 0)
			{
				DirectX::XMFLOAT4 pPos(0.0f, 0.0f, 0.0f, 0.0f);
				fscanf_s(m_pFile, "%f %f %f \n", &pPos.x, &pPos.y, &pPos.z);
				m_vPosition.push_back(pPos);
			}
			else if (strcmp(header, "vt") == 0)
			{
				DirectX::XMFLOAT4 pUV(0.0f, 0.0f, 0.0f, 0.0f);
				fscanf_s(m_pFile, "%f %f\n", &pUV.x, &pUV.y);
				pUV.y = 1 - pUV.y;
				m_vUV.push_back(pUV);
			}
			else if (strcmp(header, "vn") == 0)
			{
				DirectX::XMFLOAT4 pNormal(0.0f, 0.0f, 0.0f, 0.0f);
				fscanf_s(m_pFile, "%f %f %f \n", &pNormal.x, &pNormal.y, &pNormal.z);
				m_vNormal.push_back(pNormal);
			}
			else if (strcmp(header, "f") == 0)
			{
				unsigned int positionIndices[3], uvIndices[3], normalIndices[3];
				int matches = fscanf_s(m_pFile, " %d/%d/%d %d/%d/%d %d/%d/%d \n", &positionIndices[0], &uvIndices[0], &normalIndices[0],
					&positionIndices[1], &uvIndices[1], &normalIndices[1],
					&positionIndices[2], &uvIndices[2], &normalIndices[2]);
				if (matches != 9)
				{
					Clean();
					return 0;
				}

				basic_vert* pVert[3];
				{
					pVert[0] = new basic_vert;
					pVert[0]->pos.x = m_vPosition[positionIndices[0] - 1].x;
					pVert[0]->pos.y = m_vPosition[positionIndices[0] - 1].y;
					pVert[0]->pos.z = m_vPosition[positionIndices[0] - 1].z;
					pVert[0]->pos.w = m_vPosition[positionIndices[0] - 1].w;
					pVert[0]->uv.x = m_vUV[uvIndices[0] - 1].x;
					pVert[0]->uv.y = m_vUV[uvIndices[0] - 1].y;
					pVert[0]->uv.z = m_vUV[uvIndices[0] - 1].z;
					pVert[0]->uv.w = m_vUV[uvIndices[0] - 1].w;
					pVert[0]->normal.x = m_vNormal[normalIndices[0] - 1].x;
					pVert[0]->normal.y = m_vNormal[normalIndices[0] - 1].y;
					pVert[0]->normal.z = m_vNormal[normalIndices[0] - 1].z;
					pVert[0]->normal.w = m_vNormal[normalIndices[0] - 1].w;

					pVert[1] = new basic_vert;
					pVert[1]->pos.x = m_vPosition[positionIndices[1] - 1].x;
					pVert[1]->pos.y = m_vPosition[positionIndices[1] - 1].y;
					pVert[1]->pos.z = m_vPosition[positionIndices[1] - 1].z;
					pVert[1]->pos.w = m_vPosition[positionIndices[1] - 1].w;
					pVert[1]->uv.x = m_vUV[uvIndices[1] - 1].x;
					pVert[1]->uv.y = m_vUV[uvIndices[1] - 1].y;
					pVert[1]->uv.z = m_vUV[uvIndices[1] - 1].z;
					pVert[1]->uv.w = m_vUV[uvIndices[1] - 1].w;
					pVert[1]->normal.x = m_vNormal[normalIndices[1] - 1].x;
					pVert[1]->normal.y = m_vNormal[normalIndices[1] - 1].y;
					pVert[1]->normal.z = m_vNormal[normalIndices[1] - 1].z;
					pVert[1]->normal.w = m_vNormal[normalIndices[1] - 1].w;

					pVert[2] = new basic_vert;
					pVert[2]->pos.x = m_vPosition[positionIndices[2] - 1].x;
					pVert[2]->pos.y = m_vPosition[positionIndices[2] - 1].y;
					pVert[2]->pos.z = m_vPosition[positionIndices[2] - 1].z;
					pVert[2]->pos.w = m_vPosition[positionIndices[2] - 1].w;
					pVert[2]->uv.x = m_vUV[uvIndices[2] - 1].x;
					pVert[2]->uv.y = m_vUV[uvIndices[2] - 1].y;
					pVert[2]->uv.z = m_vUV[uvIndices[2] - 1].z;
					pVert[2]->uv.w = m_vUV[uvIndices[2] - 1].w;
					pVert[2]->normal.x = m_vNormal[normalIndices[2] - 1].x;
					pVert[2]->normal.y = m_vNormal[normalIndices[2] - 1].y;
					pVert[2]->normal.z = m_vNormal[normalIndices[2] - 1].z;
					pVert[2]->normal.w = m_vNormal[normalIndices[2] - 1].w;

					m_vVert.push_back(pVert[0]);
					m_vVert.push_back(pVert[1]);
					m_vVert.push_back(pVert[2]);
				}
			}
		}

		return 1;
	}
	Clean();
	return 0;
}

unsigned int OBJConverter::Write(char * _filename)
{
	char filename[128];
	strcpy_s(filename, _filename);
	int namelength = (int)strlen(filename);
	filename[namelength - 3] = 'b';
	filename[namelength - 2] = 'i';
	filename[namelength - 1] = 'n';

	m_fstream.open(filename, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);

	if (m_fstream.is_open())
	{
		uint32_t numVerts = (uint32_t)m_vVert.size();
		m_fstream.write((char*)&numVerts, sizeof(uint32_t));

		for each(basic_vert* data in m_vVert)
		{
			m_fstream.write((char*)&data, sizeof(basic_vert));
		}

		m_fstream.close();
		Clean();
		return 1;
	}

	Clean();
	return 0;
}

unsigned int OBJConverter::Clean()
{
	for each(basic_vert* data in m_vVert)
	{
		delete data;
	}
	m_vVert.clear();

	for each(uint32_t* data in m_vIndex)
	{
		delete data;
	}
	m_vIndex.clear();
	return 0;
}
