#pragma once
#include "PS4SoundEmitter.h"
#include <vector>
#include "./ngs2/ngs2_geom.h"
#include "./vectormath/scalar_cpp/vectormath_aos.h"

#include <sys\_defines\_sce_ok.h>
#include <..\samples\sample_code\audio_video\api_libngs2\audioout.h>

using std::vector;

class PS4AudioSystem
{
public:
	PS4AudioSystem();
	~PS4AudioSystem();


	void AddSoundEmitter(PS4SoundEmitter* e);
	void RemoveSoundEmitter(PS4SoundEmitter* e);

	void Update(float msec);

public:
	void InitSystem();
	void InitListener();


	void UpdateListener();
	void UpdateSource(PS4SoundEmitter* s);

	static int32_t audioThread(uint32_t numChannels, void* data, size_t dataSize, void* arg);

	int32_t audioThread(uint32_t numChannels, void* data, size_t dataSize);

protected:
	vector<PS4SoundEmitter*> registeredEmitters;

	SceNgs2GeomListenerWork  listenerWork;
	SceNgs2GeomListenerParam listenerParams;


	SceNgs2Handle systemHandle;				// NGS2 system handle
	SceNgs2Handle masteringRackHandle;		// Mastering rack handle
	SceNgs2Handle samplerRackHandle;			// Sample rack handle
	SceNgs2Handle masteringVoiceHandle;		// Mastering voice handle
	SceNgs2Handle samplerVoiceHandle;			// Sampler voice handle

	AudioOut audioOutput;
};

