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

	void AddSource(D3DXVECTOR4 lPosition);


private:
	std::vector<SoundSource*> mSource;
};

#endif