#include "stdafx.h"
#include <chrono>
#include <thread>
#include "SKCModLoader.h"
#include "FramerateFix.h"

using namespace std::chrono;

using FrameRatio = duration<double, std::ratio<1, 60>>;

static const auto FRAME_RATIO = FrameRatio(1);
static const auto FRAME_PORTION_MS = duration_cast<milliseconds>(FRAME_RATIO) - milliseconds(1);

static auto frame_start = system_clock::now();

DataPointer(uint32_t, framerate, 0x0083114C);
DataPointer(uint32_t, framerate_target, 0x00831158);

static int __cdecl should_sleep_r()
{
	framerate_target = 60;
	auto delta = system_clock::now() - frame_start;

	if (delta < FRAME_RATIO)
	{
		framerate = 60;

		// sleep for a portion of the frame time to free up cpu time
		std::this_thread::sleep_for(FRAME_PORTION_MS - delta);

		while ((delta = system_clock::now() - frame_start) < FRAME_RATIO)
		{
			// spin for the remainder of the time
		}
	}

	framerate = static_cast<uint32_t>(std::round(60.0 / FrameRatio(delta).count()));
	frame_start = system_clock::now();
	return 0;
}

void InitFramerateFix()
{
	WriteCall(reinterpret_cast<void*>(0x00403256), should_sleep_r);
	WriteJump(reinterpret_cast<void*>(0x00403243), reinterpret_cast<void*>(0x00403256));
}