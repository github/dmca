#include "common.h"

#include "Pools.h"
#include "World.h"
#include "Dummy.h"

void *CDummy::operator new(size_t sz) { return CPools::GetDummyPool()->New();  }
void CDummy::operator delete(void *p, size_t sz) { CPools::GetDummyPool()->Delete((CDummy*)p); }

void
CDummy::Add(void)
{
	int x, xstart, xmid, xend;
	int y, ystart, ymid, yend;
	CSector *s;
	CPtrList *list;

	CRect bounds = GetBoundRect();
	xstart = CWorld::GetSectorIndexX(bounds.left);
	xend   = CWorld::GetSectorIndexX(bounds.right);
	xmid   = CWorld::GetSectorIndexX((bounds.left + bounds.right)/2.0f);
	ystart = CWorld::GetSectorIndexY(bounds.top);
	yend   = CWorld::GetSectorIndexY(bounds.bottom);
	ymid   = CWorld::GetSectorIndexY((bounds.top + bounds.bottom)/2.0f);
	assert(xstart >= 0);
	assert(xend < NUMSECTORS_X);
	assert(ystart >= 0);
	assert(yend < NUMSECTORS_Y);

	for(y = ystart; y <= yend; y++)
		for(x = xstart; x <= xend; x++){
			s = CWorld::GetSector(x, y);
			if(x == xmid && y == ymid)
				list = &s->m_lists[ENTITYLIST_DUMMIES];
			else
				list = &s->m_lists[ENTITYLIST_DUMMIES_OVERLAP];
			CPtrNode *node = list->InsertItem(this);
			assert(node);
			m_entryInfoList.InsertItem(list, node, s);
		}
}

void
CDummy::Remove(void)
{
	CEntryInfoNode *node, *next;
	for(node = m_entryInfoList.first; node; node = next){
		next = node->next;
		node->list->DeleteNode(node->listnode);
		m_entryInfoList.DeleteNode(node);
	}
}
