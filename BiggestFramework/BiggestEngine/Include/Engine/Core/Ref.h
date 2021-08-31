#pragma once

#include "../BiggestEngine.h"

BIGGEST_BEGIN

class BIGGEST_DLL CRef
{
protected:
	string		m_strTag;
	string		m_strTypeName;
	int			m_iRefCount;
	bool		m_bEnable;
	bool		m_bAlive;

public:
	string GetTag();
	string GetTypeName();
	bool GetEnable();
	bool GetAlive();

public:
	template <typename T>
	void SetTypeName()
	{
		char	strName[256] = {};
		strcpy_s(strName, typeid(T).name());

		int	iLength = strlen(strName);

		for (int i = iLength - 1; i >= 0; --i)
		{
			if (strName[i] == ':')
			{
				memset(strName, 0, sizeof(char) * (i + 1));

				for (int j = i + 1; j < iLength; ++j)
				{
					strName[j - (i + 1)] = strName[j];
				}

				break;
			}
		}

		m_strTypeName = strName;
	}

public:
	void SetTag(const string& strTag);
	void AddRef();
	void Release();
	void Remove();
	void Death();
	void Enable(bool bEnable = true);

public:
	CRef();
	virtual ~CRef();
};

BIGGEST_END
