#pragma once

#include "./vectormath/scalar_cpp/vectormath_aos.h"
#include <ngs2/ngs2_geom.h>

using namespace sce::Vectormath::Scalar::Aos;

class PS4SoundEmitter
{
public:
	friend class PS4AudioSystem;

	PS4SoundEmitter();
	~PS4SoundEmitter();

	Vector3 GetPosition() const { return position;  }
	float	GetPitch()    const { return pitch;		}
	float	GetVolume()   const { return volume;	}

protected:
	Vector3 position;
	float	pitch;
	float	volume;

	SceNgs2GeomSourceParam sourceParam;
};

