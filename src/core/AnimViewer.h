#pragma once

class CAnimViewer {
public:
	static int animTxdSlot;
	static CEntity *pTarget;
	
	static void Initialise();
	static void Render();
	static void Shutdown();
	static void Update();
};