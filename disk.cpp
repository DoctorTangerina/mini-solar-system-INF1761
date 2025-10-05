#include "disk.h"
#include <glad/gl.h>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

Disk::Disk(int nslice)
{
	this->m_nslice = nslice;
	std::vector<float> coord;
	for (unsigned long int i = 0; i < m_nslice; i++) {
		float theta = (2.0f * M_PI * i) / m_nslice;
		float x = cosf(theta);
		float y = sinf(theta);
		float s = 0.5f + 0.5f * x;
		float t = 0.5f + 0.5f * y;
		coord.push_back(x); coord.push_back(y);
		coord.push_back(s); coord.push_back(t);
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, coord.size() * sizeof(float), coord.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

DiskPtr Disk::Make(int nslice)
{
	return DiskPtr(new Disk(nslice));
}

Disk::~Disk()
{
}

void Disk::Draw(StatePtr st)
{
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, m_nslice);
}
