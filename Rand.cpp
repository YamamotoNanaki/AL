#include "Rand.h"

void Rand::Initialize()
{
	srand(time(nullptr));
}

int Rand::GetRand()
{
	return rand();
}

int Rand::GetRand(int min, int max)
{
	return min + rand() % (max - min);
}
