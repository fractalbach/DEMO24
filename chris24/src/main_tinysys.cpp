#ifdef PLATFORM_TINYSYS

#include "basesystem.h"
#include "core.h"
#include "vpu.h"

#include "draw.cpp" // this will define SCREEN_HEIGHT, SCREEN_WIDTH, and framebuffer

int main(int argc, char *argv[]) {

	static uint16_t* screen = (uint16_t*)VPUAllocateBuffer(SCREEN_HEIGHT*SCREEN_WIDTH*2);

	struct EVideoContext vx;
	vx.m_vmode = EVM_640_Wide;
	vx.m_cmode = ECM_16bit_RGB;
	VPUSetVMode(&vx, EVS_Enable);
	VPUSetWriteAddress(&vx, (uint32_t)screen);
	VPUSetScanoutAddress(&vx, (uint32_t)screen);
	VPUClear(&vx, 0x03030303);

	while(1) {

		// Retrieve current time from system and convert into a format I like.
		uint64_t uintTimeMs = ClockToMs(E32ReadTime());
		float iTime = float(uintTimeMs) / 1000.0f;

		// Pass in helpful data from the system, and do all of the fun stuff!
		draw(iTime);

		// Convert our framebuffer into data the screen can actually use
		for (int y = 0; y < SCREEN_HEIGHT; y++) {
			for (int x = 0; x < SCREEN_WIDTH; x++) {
				vec3 color = framebuffer[y][x];
				uint8_t r = color.x * 255;
				uint8_t g = color.y * 255;
				uint8_t b = color.z * 255;
				screen[x + y * SCREEN_HEIGHT] = MAKECOLORRGB12(r, g, b);
			}
		}

		// Finish memory writes to display buffer
		CFLUSH_D_L1;
	}

	return 0;
}

#endif