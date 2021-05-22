#include "global.h"
#include "processor.h"
#include "cpu.h"
#include "cpu_registers.h"
#include "ppu.h"
#include "ppu_registers.h"
#include "ppu_memory.h"
#include "memory.h"
#include "memory_registers.h"
#include "control_flags.h"
#include "gfx.h"
#include "mapper.h "
#include "cpu_opcodes.h"
#include "apu.h"

#define FIRST   0

extern void cpu_reset();

vuchar g_pPatternTableBuffer(0x30000);

void CPUProcess();

void draw_frame()
{
	g_bDisplayReady = g_bPatternTableReady = g_bNameTableReady = true;
	g_pMainFrame->MakeCurrent();
	SetTextureData(g_pScreenBuffer, g_txMainWindow);
}

void Process()
{
	if (!g_bCpuRunning) {
		return;
	}
	if (g_PPURegisters.scanline == FIRST) {
		g_bDisplayReady = g_bPatternTableReady = g_bNameTableReady = false;
	} 
	CPUProcess();
}


void CPUProcess()
{
	uint sn = g_PPURegisters.scanline;
	while (sn == g_PPURegisters.scanline) {
		memory_pc_process(); //should be cycle accurate
#if 0
		auto op = OpCodes[g_Registers.opCode];
		char opBuff[48] = {0};
		if (op.nParams != -1 && op.nType != -1) {
			if (op.nParams == 0) {
				sprintf(&opBuff[0], OpCodeFormats[op.nType].pcszFormat, op.sOpCode, g_Registers.byteLatch);
			} else if (op.nParams == 1) {
				sprintf(&opBuff[0], OpCodeFormats[op.nType].pcszFormat, op.sOpCode, memory_main_read(g_Registers.pc + 1), g_Registers.byteLatch);
			} else if (op.nParams == 2) {
				sprintf(&opBuff[0], OpCodeFormats[op.nType].pcszFormat, op.sOpCode, memory_main_read(g_Registers.pc + 2), memory_main_read(g_Registers.pc + 1), g_Registers.byteLatch);
			}
			VLog().AddLine( "$%04X %02X %s x:%02X a:%02X y:%02X\n", g_Registers.pc, g_Registers.opCode, &opBuff[0], g_Registers.x, g_Registers.a, g_Registers.y);
		} else {
			VLog().AddLine( "$%04X %02X %s\n", g_Registers.pc, g_Registers.opCode, "Unknown OPCODE");
		}
#endif
	}
}

void Stop()
{
	g_bCpuRunning = false;
}

void Start()
{
	cpu_reset();
	g_bCpuRunning = true;
}

void PatternTableProcess()
{
	if (!g_bPatternTableReady) {
		return;
	}

	//Unfortunetly we have to draw tp the patten table buff in here..
	//16 tiles by 16 tiles
	BYTE* Palette = &g_Palette[0];
	for(uint y = 0; y < 16; y++) {
		for(uint x = 0; x < 16; x++) {
			uint nAddress = (y * 256) + (x * 16);
			uint nBank = nAddress >> 0xA;
			uchar tile[16] = {0};
			memcpy(&tile[0], &g_PPUTable[nBank][nAddress & 0x3FF], 16);

			for(uint ty = 0; ty < 8; ty++) {//bytes
				uchar nByte1 = tile[ty];
				uchar nByte2 = tile[ty + 8];

				spritebmp_t b0;
				spritebmp_t b1;

				b0.set(nByte1);
				b1.set(nByte2);

				for(uint tx = 0; tx < 8; tx++) {//bits
					uint nBuffAddress = (((y * 8) + ty) * 768) + (((x * 8) + tx) * 3);

					uint nBit1 = b0.fetch();
					uint nBit2 = b1.fetch();
					uint nColor = (nBit2 << 1) | nBit1;

					g_pPatternTableBuffer[nBuffAddress + 0] = g_RGBPalette[Palette[nColor]][0];
					g_pPatternTableBuffer[nBuffAddress + 1] = g_RGBPalette[Palette[nColor]][1];
					g_pPatternTableBuffer[nBuffAddress + 2] = g_RGBPalette[Palette[nColor]][2];

					b0.shift();
					b1.shift();
				}
			}
		}
	}

	for(uint y = 0; y < 16; y++) {
		for( uint x = 0; x < 16; x++) {
			uint nAddress = 0x1000 + (y * 256) + (x * 16);
			uint nBank = nAddress >> 0xA;
			uchar tile[16] = {0};
			memcpy(&tile[0], &g_PPUTable[nBank][nAddress & 0x3FF], 16);
			for(uint ty = 0; ty < 8; ty++) {//bytes
				uchar nByte1 = tile[ty];
				uchar nByte2 = tile[ty + 8];
				for(uint tx = 0; tx < 8; tx++) {//bits
					uint nBuffAddress = (((y * 8) + ty) * 768) + (((x * 8) + tx + 128) * 3);
					uint nBit1 = ((nByte1 >> (7 - tx)) & 1);
					uint nBit2 = ((nByte2 >> (7 - tx)) & 1);
					uint nColor = (nBit2 << 1) | nBit1;

					g_pPatternTableBuffer[nBuffAddress + 0] = g_RGBPalette[Palette[0x10 +nColor]][0];
					g_pPatternTableBuffer[nBuffAddress + 1] = g_RGBPalette[Palette[0x10 + nColor]][1];
					g_pPatternTableBuffer[nBuffAddress + 2] = g_RGBPalette[Palette[0x10 + nColor]][2];
				}
			}
		}
	}
	g_pPatternTableFrame->MakeCurrent();
	SetTextureData(&g_pPatternTableBuffer[0], g_txPatternTable);
}

void InitializeProcessor()
{
	memset(&g_pScreenBuffer[0], 128, 0x30000);

	const int ciDesiredWidth = 512;
	const int ciDesiredHeight = 512;
	const int ciCXFrameSize = ::GetSystemMetrics(SM_CYFIXEDFRAME);
	const int ciCXBorderSize = ::GetSystemMetrics(SM_CXBORDER);
	const int ciCYBorderSize = ::GetSystemMetrics(SM_CYBORDER);
	const int ciCYFrameSize = ::GetSystemMetrics(SM_CXFIXEDFRAME);
	const int ciCaption = ::GetSystemMetrics(SM_CYCAPTION);

	const int ciPadding = ::GetSystemMetrics(SM_CXPADDEDBORDER);


	g_pMainFrame = new gfx::COpenGLWrapper;
	g_pMainFrame->SetMenu(VMainframeMenu().GetSafeHmenu());//platform
	g_pMainFrame->Create( ciDesiredHeight + ciCYBorderSize + ciCYFrameSize + ciCYBorderSize + ciCYFrameSize + ciCaption + ciPadding + ciPadding, ciDesiredWidth + ciCXBorderSize + ciCXBorderSize + ciPadding + ciPadding + ciCXFrameSize + ciCXFrameSize, "mnes-0.5.0"); //295, 272
	g_pMainFrame->MakeCurrent();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POINT_SMOOTH);
	glGenTextures(1, &g_txMainWindow );
	glBindTexture(GL_TEXTURE_2D, g_txMainWindow );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, g_pScreenBuffer );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
	g_pMainFrame->SetCalculateFunc( Process );
	g_pMainFrame->SetDisplayFunc( DrawFrame );

	HINSTANCE hInstance = GetModuleHandle(NULL);//platform

	cpu_initialize(hInstance, g_pMainFrame->GetWindowHandle());//platform
	ppu_initialize();
	apu_initialize();


	//g_pMainFrame->Resize(256, 256);
#if 0
	memset( &g_pPatternTableBuffer[0], 128, 0x30000);
	g_pPatternTableFrame = new gfx::COpenGLWrapper;
	g_pPatternTableFrame->Create(512,512,"Pattern Table $0000 - $1FFF");
	g_pPatternTableFrame->MakeCurrent();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &g_txPatternTable );
	glBindTexture(GL_TEXTURE_2D, g_txPatternTable );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, &g_pPatternTableBuffer[0] );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
	g_pPatternTableFrame->SetCalculateFunc(PatternTableProcess);
	g_pPatternTableFrame->SetDisplayFunc(DrawPatternTableFrame);
#endif
}