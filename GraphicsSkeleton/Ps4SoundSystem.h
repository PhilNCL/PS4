#pragma once

#include <thread>
#include <atomic>

#include <audio3d.h>
#include "PS4AudioSystem.h"
#include "PS4AudioSource.h"
#include "Ps4AudioEmitter.h"

class Ps4SoundSystem
{
public:
	Ps4SoundSystem(int audioSources = 32);
	~Ps4SoundSystem();

protected:
	void AudioThread();
	void UpdateSound();

	std::thread			audioThread;
	std::atomic<bool>	threadFinished;

	//libaudio3D
	SceAudio3dPortId	audioPort;

	vector<PS4AudioSource*> audioSources;

	Ps4AudioEmitter*	testEmitter;
	Sound*				testSound;
};

