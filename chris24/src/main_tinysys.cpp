// #define PLATFORM_TINYSYS
// #ifdef PLATFORM_TINYSYS

#include <stdlib.h>
#include <string.h>
#include "basesystem.h"
#include "core.h"
#include "vpu.h"
#include "dma.h"
#include "modplayer.cpp"

#include "mycoolfont.h"
#include "img_circle.h"
#include "waterfall_butterfly.h"
#include "waterfall_animated.h"
// #include "waterfall2_data.h"

#define W 320
#define H 240
#define FBLEN 76800

uint8_t *screen;
uint8_t *testbuf;
uint32_t prevvblankcount;
int currentStage = 0;
uint64_t programTime = 0;


// ================================================================================
// Witchcraft
// ================================================================================

// inline uint32_t witchcraft(float value)
// {
// 	uint32_t retval;
// 	asm (
// 		"mv a1, %1;"
// 		".word 0xc2058553;" // fcvtswu4sat a0, a1 // note A0==cpu.x10, A1==cpu.x11
// 		"mv %0, a0; "
// 		: "=r" (retval)
// 		: "r" (value)
// 		: "a0", "a1"
// 	);
// 	return retval;
// }


// inline void witchcraft_summon_pixel(int x, int y, float r, float g, float b)
// {
// 	uint32_t red = witchcraft(r);
// 	uint32_t green = witchcraft(g);
// 	uint32_t blue = witchcraft(b);
// 	uint16_t *pixel = (uint16_t*)( testbuf + (x+W*y) * 2 );
// 	*pixel = (red<<8) | (blue<<4) | green;
// }

void clearBuffer() {
	for (int i = 0; i < FBLEN; i++) {
		testbuf[i] = 0;
	}
}

// ================================================================================
// Image Loading and Palette Cycling
// ================================================================================

void setImagePalette( uint32_t pal[][3] ) {
	for (int i=0; i<256; i++) {
		uint32_t r = pal[i][0];
		uint32_t g = pal[i][1];
		uint32_t b = pal[i][2];
		VPUSetPal( i, r, g, b );
	}
}

void drawImage( uint8_t * imageData ) {
	for (int i=0; i<FBLEN; i++) {
		testbuf[i] = imageData[i];
	}
}

void cyclePalette( uint32_t pal[][3], int startIndex, int nColors, int cycleSpeed, bool reversed = false ) {
	int cycleTime = programTime % (cycleSpeed * nColors);
	int offset = cycleTime / cycleSpeed;
	if (reversed) {
		offset = nColors - offset;
	}
	for (int i=0; i<nColors; i++) {
		int dstIndex = startIndex + i;
		int srcIndex = startIndex + (i+offset)%nColors;
		uint32_t r = pal[srcIndex][0];
		uint32_t g = pal[srcIndex][1];
		uint32_t b = pal[srcIndex][2];
		VPUSetPal( dstIndex, r, g, b );
	}
}


// ================================================================================
// Font Rendering and Effects
// ================================================================================

// inline int mmin( int x, int y) {
// 	return x < y ? x : y;
// }

inline int mmin( int x, int y ) {
	return y ^ ((x ^ y) & -(x < y));
}

// inline int mmax( int x, int y ) {
// 	return x ^ ((x ^ y) & -(x < y));
// }

void drawChar( char c, int screenBeginX, int screenBeginY ) {
	int a = (int)c - 32;
	a = a % 60;
	int screenEndX = mmin( W , screenBeginX + FONT_W );
	int screenEndY = mmin( H , screenBeginY + FONT_H );
	int colsToDraw = screenEndX - screenBeginX;
	int rowsToDraw = screenEndY - screenBeginY;
	int atlasBeginX = ( a % FONT_CHARS_PER_ROW ) * FONT_W;
	int atlasBeginY = ( a / FONT_CHARS_PER_ROW ) * FONT_H;
	for ( int row = 0; row < rowsToDraw; row++ ) {
		for ( int col = 0; col < colsToDraw; col++ ) {
			int dataIndex = (col + atlasBeginX) + (row + atlasBeginY) * FONT_ATLAS_W;
			uint8_t data = MY_COOL_FONT_DATA[dataIndex];
			if ( data == FONT_EMPTY )
				continue;
			int screenIndex = (col + screenBeginX) + (row + screenBeginY) * W;
			testbuf[screenIndex] = data;
		}
	}
}

void drawStringUpperLeft( const char *s, int screenBeginX, int screenBeginY ) {
	int n = strlen( s );
	for ( int i=0; i<n; i++ ) {
		drawChar( s[i], (screenBeginX + FONT_W * i), screenBeginY );
	}
}

void drawStringCentered( const char *s, int centerX, int centerY ) {
	int n = strlen( s );
	int screenStartX = centerX - ( FONT_W * n / 2 );
	int screenStartY = centerY - ( FONT_H / 2 );
	drawStringUpperLeft( s, screenStartX, screenStartY );
}

// ================================================================================
// Procedures for Each Stage of the Demo
// ================================================================================

void stage_IntroText();
void stage_OutroText();

void stage_IntroTextSetup() {
	setImagePalette( MY_COOL_FONT_PAL );
	currentStage++;
	stage_IntroText();
}

void stage_IntroText() {
	clearBuffer();
	int lh = FONT_H + 2;
	int x = W/2;
	int y = H/2 - lh;
	if ( programTime < 10000 ) {
		drawStringCentered( "WELCOME"   , x, y+lh*0 );
		drawStringCentered( "TO MY"     , x, y+lh*1 );
		drawStringCentered( "DEMO"      , x, y+lh*2 );
	} else {
		drawStringCentered( "BY"        , x, y+lh*0 );
		drawStringCentered( "CHRIS"     , x, y+lh*1 );
		drawStringCentered( "ACHENBACH" , x, y+lh*2 );
	}
	cyclePalette( MY_COOL_FONT_PAL, FONT_CYCLE_START, FONT_CYCLE_NCOLORS, FONT_CYCLE_SPEED );
	if ( programTime > 20000 ) {
		currentStage++;
	}
}

void stage_WaterfallButterflySetupPal() {
	setImagePalette( IMAGE_WATERFALL_BUTTERFLY_PALETTE );
	drawImage( IMAGE_WATERFALL_BUTTERFLY_DATA );
	currentStage++;
}

void stage_WaterfallButterflyChill() {
	drawImage( IMAGE_WATERFALL_BUTTERFLY_DATA ); // testing if multiple draws fixes something odd.
	if ( programTime > 40000 ) {
		currentStage++;
	}
}

void stage_WaterfallAnimatedSetupPal() {
	setImagePalette( WATERFALL_ANIMATED_PAL );
	drawImage( WATERFALL_ANIMATED_DATA );
	currentStage++;
}

void stage_WaterfallAnimatedCycle() {
	cyclePalette( WATERFALL_ANIMATED_PAL, WATERFALL_CYCLE_1_START, WATERFALL_CYCLE_NCOLORS, WATERFALL_CYCLE_SPEED, true );
	cyclePalette( WATERFALL_ANIMATED_PAL, WATERFALL_CYCLE_2_START, WATERFALL_CYCLE_NCOLORS, WATERFALL_CYCLE_SPEED, true );
	cyclePalette( WATERFALL_ANIMATED_PAL, WATERFALL_CYCLE_3_START, WATERFALL_CYCLE_NCOLORS, WATERFALL_CYCLE_SPEED, true );
	cyclePalette( WATERFALL_ANIMATED_PAL, WATERFALL_CYCLE_4_START, WATERFALL_CYCLE_NCOLORS, WATERFALL_CYCLE_SPEED, true );
	if ( programTime > 60000 ) {
		currentStage++;
	}
}

void stage_CircleImageSetup() {
	setImagePalette( IMG_CIRCLE_PAL );
	drawImage( IMG_CIRCLE_DATA );
	currentStage++;
}

void stage_CircleImageCycle() {
	cyclePalette( IMG_CIRCLE_PAL, CIRCLE_CYCLE_START, CIRCLE_CYCLE_NCOLORS, CIRCLE_CYCLE_SPEED );
	if ( programTime > 80000 ) {
		currentStage++;
	}
}

void stage_OutroTextSetup() {
	setImagePalette( MY_COOL_FONT_PAL );
	currentStage++;
	stage_OutroText();
}

void stage_OutroText() {
	clearBuffer();
	int stageTime = programTime % 10000;
	int lh = FONT_H + 2;
	int yi = H + 2*lh;
	int x = W/2;
	int y = yi - ( 2 * H * stageTime / 10000 );

	drawStringCentered( "THESE"               , x , (y + lh * 0) );
	drawStringCentered( "ARE"                 , x , (y + lh * 1) );
	drawStringCentered( "THE"                 , x , (y + lh * 2) );
	drawStringCentered( "CREDITS"             , x , (y + lh * 3) );

	drawStringCentered( "THANKS"              , x , (y + lh * 5) );
	drawStringCentered( "FOR"                 , x , (y + lh * 6) );
	drawStringCentered( "EVERYTHING"          , x , (y + lh * 7) );
	drawStringCentered( "EVERYBODY"           , x , (y + lh * 8) );

	cyclePalette( MY_COOL_FONT_PAL, FONT_CYCLE_START, FONT_CYCLE_NCOLORS, FONT_CYCLE_SPEED );
	if ( programTime > 100000 ) {
		currentStage++;
	}
}

void stage_ChillForever() {
	clearBuffer();
	drawStringCentered( "THE END", W/2, H/2 );
	cyclePalette( MY_COOL_FONT_PAL, FONT_CYCLE_START, FONT_CYCLE_NCOLORS, FONT_CYCLE_SPEED );
}


static void ( *stageFunctionPointer[ 11 ] ) ( ) = {
	stage_IntroTextSetup,
	stage_IntroText,
	stage_WaterfallButterflySetupPal,
	stage_WaterfallButterflyChill,
	stage_WaterfallAnimatedSetupPal,
	stage_WaterfallAnimatedCycle,
	stage_CircleImageSetup,
	stage_CircleImageCycle,
	stage_OutroTextSetup,
	stage_OutroText,
	stage_ChillForever
};


// ================================================================================
// Main and Core Loop
// ================================================================================

int main(int argc, char *argv[])
{
	int frame = 0;

	screen = VPUAllocateBuffer( FBLEN );
	testbuf = VPUAllocateBuffer( FBLEN );


	for (int i = 0; i < FBLEN; i++) {
		screen[i] = 0;
	}
	clearBuffer();

	struct EVideoContext vx;
	vx.m_vmode = EVM_320_Wide;
	vx.m_cmode = ECM_8bit_Indexed;

	VPUSetVMode( &vx, EVS_Enable );
	VPUSetWriteAddress( &vx, (uint32_t)screen );
	VPUSetScanoutAddress( &vx, (uint32_t)screen );
	VPUSetDefaultPalette( &vx );
	// VPUClear(&vx, 0x03030303);
	prevvblankcount = VPUReadVBlankCounter();

	const uint32_t blockCount = FBLEN / DMA_BLOCK_SIZE;
	const uint32_t leftoverDMA = blockCount%256;
	const uint32_t fullDMAs = blockCount/256;
	uint64_t startTime = ClockToMs(E32ReadTime());

	// apubuffer is declared in modplayer.cpp
	apubuffer = (short*)APUAllocateBuffer(BUFFER_SAMPLES*NUM_CHANNELS*sizeof(short));

	PlayMODFile("sd://chris24.mod");

	while(1)
	{
		// Update per-frame variables
		programTime = ClockToMs(E32ReadTime()) - startTime;

		// Call the procedure that we currently want to do in this demo
		stageFunctionPointer[ currentStage ]();

		// Finish memory writes to display buffer
		CFLUSH_D_L1;

		// Wait until there are no more DMA operations in flight
		DMAWait();

		// Do the full DMAs first
		uint32_t fulloffset = 0;
		for ( uint32_t full = 0; full < fullDMAs; full++ ) {
			DMACopy4K( (uint32_t)testbuf+fulloffset, (uint32_t)screen+fulloffset );
			fulloffset += 256*16; // 16 bytes for each 256-block, total of 4K
		}

		// Partial DMA next
		if ( leftoverDMA != 0 ) {
			DMACopy( (uint32_t)testbuf+fulloffset, (uint32_t)screen+fulloffset, leftoverDMA );
		}

		// Tag for sync (essentially an item in FIFO after last DMA so we can check if DMA is complete when this drains)
		DMATag(0x0);

		// Finish memory writes to display buffer
		// CFLUSH_D_L1;

		// Update Audio stuff to continue playing music
		playModuleRunThisEveryFrame();

		frame++;
	}

	return 0;
}

// #endif