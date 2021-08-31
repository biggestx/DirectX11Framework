#include "Buffer.h"

USING_BIGGEST

CBuffer::CBuffer() :
m_pBuffer(NULL)
{
}


CBuffer::~CBuffer()
{
	SAFE_RELEASE(m_pBuffer);
}
