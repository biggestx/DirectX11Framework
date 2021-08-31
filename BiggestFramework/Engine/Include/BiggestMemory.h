#pragma once


template <typename T>
void Safe_Delete_VecList(T& p)
{
	T::iterator	iter = p.begin();

	while (iter != p.end())
	{
		SAFE_DELETE((*iter));
		++iter;
	}

	p.clear();
}

template <typename T>
void Safe_Release_VecList(T& p)
{
	T::iterator	iter = p.begin();

	while (iter != p.end())
	{
		(*iter)->Release();
		++iter;
	}

	p.clear();
}

template <typename T>
void Safe_Delete_Map(T& p)
{
	T::iterator	iter = p.begin();

	while (iter != p.end())
	{
		SAFE_DELETE((iter->second));
		++iter;
	}

	p.clear();
}

template <typename T>
void Safe_Release_Map(T& p)
{
	T::iterator	iter = p.begin();

	while (iter != p.end())
	{
		iter->second->Release();
		++iter;
	}

	p.clear();
}
