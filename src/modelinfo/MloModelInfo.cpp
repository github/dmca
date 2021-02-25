#include "common.h"

#include "VisibilityPlugins.h"
#include "ModelInfo.h"

void
CMloModelInfo::ConstructClump()
{
	m_clump = RpClumpCreate();
	RwFrame *mainFrame = RwFrameCreate();
	RwFrameSetIdentity(mainFrame);
	RpClumpSetFrame(m_clump, mainFrame);

	for (int i = firstInstance; i < lastInstance; i++) {
		int modelId = CModelInfo::GetMloInstanceStore().store[i].m_modelIndex;
		RwMatrix *attMat = CModelInfo::GetMloInstanceStore().store[i].GetMatrix().m_attachment;
		CSimpleModelInfo *minfo = (CSimpleModelInfo*)CModelInfo::GetModelInfo(modelId);

		if (minfo->m_atomics[0] != nil) {
			RpAtomic *newAtomic = RpAtomicClone(minfo->m_atomics[0]);
			RwFrame *newFrame = RwFrameCreate();
			if (newAtomic != nil && newFrame != nil) {
				*RwFrameGetMatrix(newFrame) = *attMat;
				RpAtomicSetFrame(newAtomic, newFrame);
				RwFrameAddChild(mainFrame, newFrame);
				RpClumpAddAtomic(m_clump, newAtomic);
			} else {
				debug("Failed to allocate memory while creating template MLO.\n");
			}
		}
	}

	if (RpClumpGetNumAtomics(m_clump) != 0) {
		CVisibilityPlugins::SetClumpModelInfo(m_clump, this);
	} else {
		RpClumpDestroy(m_clump);
		m_clump = nil;
	}
}