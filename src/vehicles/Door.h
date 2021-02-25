#pragma once

class CVehicle;

enum eDoorState
{
	DOORST_SWINGING,
	// actually wrong though,
	// OPEN is really MAX_ANGLE and CLOSED is MIN_ANGLE
	DOORST_OPEN,
	DOORST_CLOSED
};

class CDoor
{
public:
	float m_fMaxAngle;
	float m_fMinAngle;
	// direction of rotation for air resistance
	int8 m_nDirn;
	// axis in which this door rotates
	int8 m_nAxis;
	int8 m_nDoorState;
	float m_fAngle;
	float m_fPrevAngle;
	float m_fAngVel;
	CVector m_vecSpeed;

	CDoor(void);
	void Init(float minAngle, float maxAngle, int8 dir, int8 axis) {
		m_fMinAngle = minAngle;
		m_fMaxAngle = maxAngle;
		m_nDirn = dir;
		m_nAxis = axis;
	}
	void Open(float ratio);
	void Process(CVehicle *veh);
	float RetAngleWhenClosed(void);	// dead
	float RetAngleWhenOpen(void);
	float GetAngleOpenRatio(void);
	bool IsFullyOpen(void);
	bool IsClosed(void);	// dead
};

class CTrainDoor
{
public:
	float m_fClosedPosn;
	float m_fOpenPosn;
	int8 m_nDirn;
	int8 m_nDoorState;	// same enum as above?
	int8 m_nAxis;
	float m_fPosn;
	float m_fPrevPosn;
	int field_14;	// unused?

	CTrainDoor(void);
	void Init(float open, float closed, int8 dir, int8 axis) {
		m_fOpenPosn = open;
		m_fClosedPosn = closed;
		m_nDirn = dir;
		m_nAxis = axis;
	}
	bool IsClosed(void);
	bool IsFullyOpen(void);
	float RetTranslationWhenClosed(void);
	float RetTranslationWhenOpen(void);
	void Open(float ratio);
};
