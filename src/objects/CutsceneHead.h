#pragma once

#include "CutsceneObject.h"

class CCutsceneHead : public CCutsceneObject
{
public:
	RwFrame *m_pHeadNode;
#ifdef PED_SKIN
	int32 unk1;
	CCutsceneObject *m_parentObject;
	int32 unk2;
	int32 bIsSkinned;
#endif

	CCutsceneHead(CObject *obj);

	void CreateRwObject(void);
	void DeleteRwObject(void);
	void ProcessControl(void);
	void Render(void);
	void RenderLimb(int32 bone);

	void PlayAnimation(const char *animName);
};
#ifndef PED_SKIN
VALIDATE_SIZE(CCutsceneHead, 0x19C);
#endif
