#ifndef SOUNDHANDLER_H
#define SOUNDHANDLER_H

#include "SoundSource.h"
#include <vector>

class SoundHandler
{
public:
	SoundHandler();
	~SoundHandler();
	void Initialize();

	void AddSource(char* lPath, D3DXVECTOR4 lPosition, SoundType lSoundType);

	void PlaySoundCustom();
	void Update();


private:
	std::vector<SoundSource*> mSource;
};

#endif