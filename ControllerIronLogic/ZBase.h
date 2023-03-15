#pragma once
//#include "pch.h"
#include <tchar.h>
#include <algorithm>

// Номер ключа контроллера ([0] - длина номера ключа)
typedef BYTE Z_KEYNUM[16];
typedef Z_KEYNUM near *PZ_KEYNUM;

using namespace std;

inline INT CompareZKeyNums(const Z_KEYNUM& _Left, const Z_KEYNUM& _Right)
{
	INT nRes;
	if (_Left[0] == _Right[0])
		nRes = (_Left[0] == 0) ? 0 : memcmp(&_Left[1], &_Right[1], _Left[0]);
	else
	{
		INT n = min(_Left[0], _Right[0]);
		if (n > 0)
			nRes = memcmp(&_Left[1], &_Right[1], n);
		else
			nRes = 0;
		if (nRes == 0)
			nRes = ((_Left[0] < _Right[0]) ? -1 : +1);
	}
	return nRes;
}

