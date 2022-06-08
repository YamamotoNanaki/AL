#include "FPS.h"
#include <string>

using namespace IF;

void IF::FPS::Initialize(unsigned int FPS)
{
	MIN_FREAM_TIME = 1.0f / FPS;
	QueryPerformanceCounter(&timeStart);
}

void FPS::FPSFixed() {
	QueryPerformanceCounter(&timeEnd);
	frameTime = static_cast<float>(timeEnd.QuadPart - timeStart.QuadPart) / static_cast<float>(timeFreq.QuadPart);

	

	if (frameTime < MIN_FREAM_TIME) {
		// ƒ~ƒŠ•b‚É•ÏŠ·
		DWORD sleepTime = static_cast<DWORD>((MIN_FREAM_TIME - frameTime) * 1000);
		//sleepTime = 16;

		//std::string a = std::to_string(sleepTime) + "\n";
		//OutputDebugStringA(a.c_str());

		timeBeginPeriod(1);
		Sleep(sleepTime);
		timeEndPeriod(1);

		return;
	}

	timeStart = timeEnd; // “ü‚ê‘Ö‚¦
}

