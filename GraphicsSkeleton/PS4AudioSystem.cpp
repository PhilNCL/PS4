#include "PS4AudioSystem.h"
#include <libsysmodule.h>
#include <ngs2.h>
// Allocator

static int32_t bufferAlloc(SceNgs2ContextBufferInfo *bufferInfo)
{
	bufferInfo->hostBuffer = malloc(bufferInfo->hostBufferSize);
	printf("# Allocate (%p,%zd[byte])\n", bufferInfo->hostBuffer, bufferInfo->hostBufferSize);
	return (bufferInfo->hostBuffer) ? SCE_OK : SCE_NGS2_ERROR_EMPTY_BUFFER;
}

static int32_t bufferFree(SceNgs2ContextBufferInfo *bufferInfo)
{
	printf("# Free (%p,%zd[byte])\n", bufferInfo->hostBuffer, bufferInfo->hostBufferSize);
	free(bufferInfo->hostBuffer);
	return SCE_OK;
}


PS4AudioSystem::PS4AudioSystem()
{
	sceSysmoduleLoadModule(SCE_SYSMODULE_NGS2);

	systemHandle			= SCE_NGS2_HANDLE_INVALID;
	masteringRackHandle		= SCE_NGS2_HANDLE_INVALID;
	samplerRackHandle		= SCE_NGS2_HANDLE_INVALID;
	masteringVoiceHandle	= SCE_NGS2_HANDLE_INVALID;
	samplerVoiceHandle		= SCE_NGS2_HANDLE_INVALID;

	SceNgs2BufferAllocator allocator;
	allocator.allocHandler	= bufferAlloc;
	allocator.freeHandler	= bufferFree;
	allocator.userData		= 0;

	sceNgs2SystemCreateWithAllocator(NULL, &allocator, &systemHandle);

	sceNgs2RackCreateWithAllocator(systemHandle, SCE_NGS2_RACK_ID_MASTERING, NULL, &allocator, &masteringRackHandle);

	sceNgs2RackCreateWithAllocator(systemHandle, SCE_NGS2_RACK_ID_SAMPLER, NULL, &allocator, &samplerRackHandle);


	sceNgs2RackGetVoiceHandle(masteringRackHandle, 0, &masteringVoiceHandle);
	sceNgs2MasteringVoiceSetup(masteringVoiceHandle, SCE_NGS2_CHANNELS_7_1CH, 0);
	sceNgs2VoiceKickEvent(masteringVoiceHandle, SCE_NGS2_VOICE_EVENT_PLAY);


	// Start audio out service
	audioOutput.start(
		48000,	// [Hz]
		8,		// [ch]
		256,	// [samples]
		PS4AudioSystem::audioThread,
		this);
}


PS4AudioSystem::~PS4AudioSystem()
{
	audioOutput.stop();

	sceNgs2RackDestroy(samplerRackHandle, NULL);
	sceNgs2RackDestroy(masteringRackHandle, NULL);
	sceNgs2SystemDestroy(systemHandle, NULL);

	sceSysmoduleUnloadModule(SCE_SYSMODULE_NGS2);
}

void PS4AudioSystem::Update(float msec) {
	UpdateListener();

	for (auto i : registeredEmitters) {
		UpdateSource(i);
	}

}

void PS4AudioSystem::InitSystem() {

}

void PS4AudioSystem::InitListener() {
	sceNgs2GeomResetListenerParam(&listenerParams);
}

void PS4AudioSystem::UpdateListener() {
	sceNgs2GeomCalcListener(&listenerParams, &listenerWork, 0);
}

void PS4AudioSystem::UpdateSource(PS4SoundEmitter* s) {
	sceNgs2GeomResetSourceParam(&s->sourceParam);
}



int32_t PS4AudioSystem::audioThread(uint32_t numChannels, void* data, size_t dataSize, void* arg) {
	return ((PS4AudioSystem*)arg)->audioThread(numChannels, data, dataSize);
}

int32_t PS4AudioSystem::audioThread(uint32_t numChannels, void* data, size_t dataSize) {
	SceNgs2RenderBufferInfo aInfo[1];
	int32_t result;

	// Rendering

	aInfo[0].buffer = data;
	aInfo[0].bufferSize = dataSize;
	aInfo[0].numChannels = numChannels;
	aInfo[0].waveformType = SCE_NGS2_WAVEFORM_TYPE_PCM_F32L;

	result = sceNgs2SystemRender(systemHandle, aInfo, 1);
	if (result < 0) {
		printf("Error: sceNgs2SystemRender() %X\n", result);
		return result;
	}

	return 0;
}