#pragma once

#include "Object.h"

class CCutsceneObject : public CObject
{
public:
#ifdef PED_SKIN
	bool bRenderHead;
	bool bRenderRightHand;
	bool bRenderLeftHand;

	bool GetRenderHead(void) { return bRenderHead; }
	bool GetRenderRightHand(void) { return bRenderRightHand; }
	bool GetRenderLeftHand(void) { return bRenderLeftHand; }
	void SetRenderHead(bool render) { bRenderHead = render; }
	void SetRenderRightHand(bool render) { bRenderRightHand = render; }
	void SetRenderLeftHand(bool render) { bRenderLeftHand = render; }
#endif

	CCutsceneObject(void);

	void SetModelIndex(uint32 id);
	void ProcessControl(void);
	void PreRender(void);
	void Render(void);
	void RenderLimb(int32 bone);
	bool SetupLighting(void);
	void RemoveLighting(bool reset);
};
#ifndef PED_SKIN
VALIDATE_SIZE(CCutsceneObject, 0x198);
#endif
