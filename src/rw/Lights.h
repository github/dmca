#pragma once

extern RpLight *pAmbient;
extern RpLight *pDirect;
extern RpLight *pExtraDirectionals[4];
extern int LightStrengths[4];
extern int NumExtraDirLightsInWorld;

void SetLightsWithTimeOfDayColour(RpWorld *);
RpWorld *LightsCreate(RpWorld *world);
void LightsDestroy(RpWorld *world);
void WorldReplaceNormalLightsWithScorched(RpWorld *world, float l);
void WorldReplaceScorchedLightsWithNormal(RpWorld *world);
void AddAnExtraDirectionalLight(RpWorld *world, float dirx, float diry, float dirz, float red, float green, float blue);
void RemoveExtraDirectionalLights(RpWorld *world);
void SetAmbientAndDirectionalColours(float f);
void SetFlashyColours(float f);
void SetFlashyColours_Mild(float f);
void SetBrightMarkerColours(float f);
void ReSetAmbientAndDirectionalColours(void);
void DeActivateDirectional(void);
void ActivateDirectional(void);
void SetAmbientColours(void);
void SetAmbientColoursForPedsCarsAndObjects(void);
void SetAmbientColoursToIndicateRoadGroup(int i);
void SetAmbientColours(RwRGBAReal *color);
