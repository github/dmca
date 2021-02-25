#pragma once

extern RwMemoryFunctions memFuncs;
void InitMemoryMgr(void);

void *MemoryMgrMalloc(size_t size);
void *MemoryMgrRealloc(void *ptr, size_t size);
void *MemoryMgrCalloc(size_t num, size_t size);
void MemoryMgrFree(void *ptr);

void *RwMallocAlign(RwUInt32 size, RwUInt32 align);
void RwFreeAlign(void *mem);
