#pragma once

#ifdef TIMEBARS
void tbInit();
void tbStartTimer(int32, Const char*);
void tbEndTimer(Const char*);
void tbDisplay();
#else
#define tbInit()
#define tbStartTimer(a, b)
#define tbEndTimer(a)
#define tbDisplay()
#endif
