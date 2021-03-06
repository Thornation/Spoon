#pragma once

#include <string>
#include <list>
#include <map>

#include <AL/al.h>
#include <AL/alc.h>

#include <glm/glm.hpp>

#include "AudioInstance.h"
#include "AudioObject.h"
#include "Buffer.h"
#include "Source.h"

class AudioSystem
{
public:

	AudioSystem();
	~AudioSystem();

	void Stop(AudioObject* object);

	void Update(glm::vec3 pos, glm::vec3 forward, glm::vec3 up, glm::vec3 vel);

private:

	ALCdevice *a_device;
	ALCcontext *a_context;

	std::map<std::string, AudioObject *> a_sounds;
};