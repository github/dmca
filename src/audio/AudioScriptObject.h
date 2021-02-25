#pragma once

class cAudioScriptObject
{
public:
	int16 AudioId;
	CVector Posn;
	int32 AudioEntity;

	cAudioScriptObject();
	~cAudioScriptObject();

	void Reset(); /// ok

	static void* operator new(size_t);
	static void* operator new(size_t, int);
	static void operator delete(void*, size_t);
	static void operator delete(void*, int);

	static void LoadAllAudioScriptObjects(uint8 *buf, uint32 size);
	static void SaveAllAudioScriptObjects(uint8 *buf, uint32 *size);
};

VALIDATE_SIZE(cAudioScriptObject, 20);

extern void PlayOneShotScriptObject(uint8 id, CVector const &pos);