#pragma once
#include <stdlib.h>
#include <time.h>

namespace Rand
{
	void Initialize();
	int GetRand();
	int GetRand(int min, int max);
}