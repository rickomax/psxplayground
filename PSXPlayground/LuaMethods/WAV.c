#include <stdlib.h>
#include "include/WAV.h"
#include "Include/Common.h"
#include "libspu.h"
#include "../Include/WAV.h"
#include "../Include/PSXPlayground.h"

int lua_loadWAV(lua_State* L)
{
	const char* filename = lua_tostring(L, 1);
	const PSX_INT_TYPE doLoop = lua_toboolean(L, 2);
	int vagSize;
	PSX_INT_TYPE pitch;
	unsigned char* vagData = loadWAV(filename, doLoop, &vagSize, &pitch);

	if (vagData != 0) {
		SpuSetTransferMode(SPU_TRANSFER_BY_DMA);
		const long audioAddress = SpuMalloc(vagSize);
		SpuSetTransferStartAddr(audioAddress);
		SpuWrite(vagData, vagSize);
		SpuIsTransferCompleted(SPU_TRANSFER_WAIT);

		free(vagData);

		audios[audioCount].audioAddress = audioAddress;
		audios[audioCount].audioPitch = pitch;
		audioCount++;
	}

	lua_push_number_to_integer(L, audioCount);

	return 1;
}

int lua_playWAV(lua_State* L)
{
	const unsigned int audioIndex = lua_number_to_integer(L, 1);
	if (audioIndex < 1 || audioIndex > audioCount)
	{
		return 0;
	}

	const unsigned int channel = lua_number_to_integer(L, 2);
	if (channel > 23)
	{
		return 0;
	}

	const long finalChannel = 0x1L << channel;

	const int address = audios[audioIndex - 1].audioAddress;
	const int pitch = audios[audioIndex - 1].audioPitch;

	SpuVoiceAttr voiceAttr;
	voiceAttr.mask =
		(
			SPU_VOICE_VOLL |
			SPU_VOICE_VOLR |
			SPU_VOICE_PITCH |
			SPU_VOICE_WDSA |
			SPU_VOICE_ADSR_AMODE |
			SPU_VOICE_ADSR_SMODE |
			SPU_VOICE_ADSR_RMODE |
			SPU_VOICE_ADSR_AR |
			SPU_VOICE_ADSR_DR |
			SPU_VOICE_ADSR_SR |
			SPU_VOICE_ADSR_RR |
			SPU_VOICE_ADSR_SL
			);
	voiceAttr.voice = finalChannel;
	voiceAttr.volume.left = 0x1fff;
	voiceAttr.volume.right = 0x1fff;
	voiceAttr.pitch = pitch;//0x1000;
	voiceAttr.addr = address;
	voiceAttr.a_mode = SPU_VOICE_LINEARIncN;
	voiceAttr.s_mode = SPU_VOICE_LINEARIncN;
	voiceAttr.r_mode = SPU_VOICE_LINEARDecN;
	voiceAttr.ar = 0x0;
	voiceAttr.dr = 0x0;
	voiceAttr.sr = 0x0;
	voiceAttr.rr = 0x0;
	voiceAttr.sl = 0xf;

	SpuSetVoiceAttr(&voiceAttr);
	SpuSetKey(SpuOn, finalChannel);

	return 0;
}
