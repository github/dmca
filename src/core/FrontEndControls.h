#pragma once

enum {
	NUM_MULTICHOICE_OPTIONS = 16,
	// 50 actual lines and 15 for spacing
	NUM_LINELISTER_LINES = 50,
	NUM_LINELISTER_LINES_TOTAL = NUM_LINELISTER_LINES + 15,
	NUM_PAGE_WIDGETS = 10,
};

class CTriggerCaller
{
	bool bHasTrigger;
	void *pTrigger;
	void (*pFunc)(void *);
	int field_C;
public:
	
	CTriggerCaller() : bHasTrigger(false), pFunc(nil)
	{}
	
	void SetTrigger(void *func, void *trigger)
	{
		if ( !bHasTrigger )
		{
			pFunc = (void (*)(void *))func;
			pTrigger = trigger;
			bHasTrigger = true;
		}
	}
	
	void CallTrigger(void)
	{
		if ( bHasTrigger && pFunc != nil )
			pFunc(pTrigger);
		
		bHasTrigger = false;
		pFunc = nil;
	}
	
	bool CanCall()
	{
		return bHasTrigger;
	}
};

class CPlaceableText
{
public:
	CVector2D m_position;
	CRGBA m_color;
	wchar *m_text;

	CPlaceableText(void)
	 : m_position(0.0f, 0.0f), m_color(255, 255, 255, 255), m_text(nil) {}
	void SetPosition(float x, float y) { m_position.x = x; m_position.y = y; }
	void SetColor(const CRGBA &color) { m_color = color; }
	CRGBA GetColor(void) { return m_color; }
	void SetAlpha(uint8 alpha) { m_color.alpha = alpha; }
};

// No trace of this in the game but it makes the other classes simpler
class CPlaceableTextTwoLines
{
public:
	CPlaceableText m_line1;
	CPlaceableText m_line2;

	void SetColor(const CRGBA &color) { m_line1.SetColor(color); m_line2.SetColor(color); }
	void SetAlpha(uint8 alpha) { m_line1.SetAlpha(alpha); m_line2.SetAlpha(alpha); }
};

// No trace of this in the game but it makes the other classes simpler
class CShadowInfo
{
public:
	bool m_bRightJustify;
	bool m_bDropShadow;
	CRGBA m_shadowColor;
	CVector2D m_shadowOffset;

	CShadowInfo(void)
	 : m_bRightJustify(false), m_bDropShadow(false),
	   m_shadowColor(255, 255, 255, 255),
	   m_shadowOffset(-1.0f, -1.0f) {}
	CRGBA GetShadowColor(void) { return m_shadowColor; }
	void SetShadows(bool bDropShadows, const CRGBA &shadowColor, const CVector2D &shadowOffset){
		m_bDropShadow = bDropShadows;
		m_shadowColor = shadowColor;
		m_shadowOffset = shadowOffset;
	}
};

// No trace of this in the game but it makes the other classes simpler
class CSelectable
{
public:
	bool m_bSelected;
	CRGBA m_selectedColor;

	CSelectable(void) : m_bSelected(false) {}
	CRGBA GetSelectedColor(void) { return m_selectedColor; }
};

class CPlaceableShText : public CPlaceableText, public CShadowInfo
{
public:
	using CPlaceableText::SetPosition;
	void SetPosition(float x, float y, bool bRightJustify) { SetPosition(x, y); m_bRightJustify = bRightJustify; }
	void SetAlpha(uint8 alpha)  { m_shadowColor.alpha = alpha; CPlaceableText::SetAlpha(alpha); }

	void Draw(float x, float y);
	void Draw(const CRGBA &color, float x, float y);
	// unused arguments it seems
	void DrawShWrap(float x, float y, float wrapX, float wrapY) { Draw(x, y); }
};

class CPlaceableShTextTwoLines : public CPlaceableTextTwoLines, public CShadowInfo
{
public:
	void SetAlpha(uint8 alpha)  { m_shadowColor.alpha = alpha; CPlaceableTextTwoLines::SetAlpha(alpha); }

	void Draw(float x, float y);
	void Draw(const CRGBA &color, float x, float y);
};

class CPlaceableShOption : public CPlaceableShText, public CSelectable
{
public:
	void SetColors(const CRGBA &normal, const CRGBA &selection) { CPlaceableShText::SetColor(normal); m_selectedColor = selection; }
	void SetAlpha(uint8 alpha)  { m_selectedColor.alpha = alpha; CPlaceableShText::SetAlpha(alpha); }

	using CPlaceableShText::Draw;
	void Draw(const CRGBA &highlightColor, float x, float y, bool bHighlight);
};

class CPlaceableShOptionTwoLines : public CPlaceableShTextTwoLines, public CSelectable
{
public:
	void SetColors(const CRGBA &normal, const CRGBA &selection) { CPlaceableShTextTwoLines::SetColor(normal); m_selectedColor = selection; }
	void SetAlpha(uint8 alpha)  { m_selectedColor.alpha = alpha; CPlaceableShTextTwoLines::SetAlpha(alpha); }

	using CPlaceableShTextTwoLines::Draw;
	void Draw(const CRGBA &highlightColor, float x, float y, bool bHighlight);
};

class CPlaceableSprite
{
public:
	CSprite2d *m_pSprite;
	CVector2D m_position;
	CVector2D m_size;
	CRGBA m_color;

	CPlaceableSprite(void)
	 : m_pSprite(nil), m_position(0.0f, 0.0f),
	   m_size(0.0f, 0.0f), m_color(255, 255, 255, 255) {}

	void SetPosition(float x, float y) { m_position.x = x; m_position.y = y; }
	void SetAlpha(uint8 alpha) { m_color.alpha = alpha; }

	void Draw(float x, float y);
	void Draw(const CRGBA &color, float x, float y);
};

class CPlaceableShSprite
{
public:
	CPlaceableSprite m_sprite;
	CPlaceableSprite m_shadow;
	bool m_bDropShadow;

	CPlaceableShSprite(void) : m_bDropShadow(false) {}

	void SetShadows(bool bDropShadows, const CRGBA &shadowColor, const CVector2D &shadowOffset){
		m_bDropShadow = bDropShadows;
		m_shadow.m_color = shadowColor;
		m_shadow.m_position = shadowOffset;
	}
	void SetAlpha(uint8 alpha)  { m_sprite.SetAlpha(alpha); m_shadow.SetAlpha(alpha); }

	void Draw(float x, float y);
};


class CMenuBase
{
public:
	CVector2D m_position;
	bool m_bTwoState;

	CMenuBase(void)
	 : m_position(0.0f, 0.0f), m_bTwoState(false) {}
	void SetPosition(float x, float y) { m_position.x = x; m_position.y = y; }

	virtual void Draw(const CRGBA &optionHighlight, const CRGBA &titleHighlight, float x, float y) = 0;
	virtual void DrawNormal(float x, float y) = 0;
	virtual void DrawHighlighted(const CRGBA &titleHighlight, float x, float y) = 0;
	virtual void SetAlpha(uint8 alpha) = 0;
	virtual void SetShadows(bool bDropShadows, const CRGBA &shadowColor, const CVector2D &shadowOffset) = 0;
	virtual bool GoNext(void) = 0;
	virtual bool GoPrev(void) = 0;
	virtual bool GoDown(void) = 0;
	virtual bool GoUp(void) = 0;
	virtual bool GoDownStill(void) = 0;
	virtual bool GoUpStill(void) = 0;
	virtual bool GoLeft(void) = 0;
	virtual bool GoRight(void) = 0;
	virtual bool GoLeftStill(void) = 0;
	virtual bool GoRightStill(void) = 0;
	virtual bool GoFirst(void) = 0;
	virtual bool GoLast(void) = 0;
	virtual void SelectCurrentOptionUnderCursor(void) = 0;
	virtual void SelectDefaultCancelAction(void) = 0;
	virtual void ActivateMenu(bool first) = 0;
	virtual void DeactivateMenu(void) = 0;
	virtual int GetMenuSelection(void) = 0;
	virtual void SetMenuSelection(int selection) = 0;
};

class CMenuDummy : public CMenuBase
{
public:
	bool m_bActive;

	virtual void Draw(const CRGBA &, const CRGBA &, float x, float y) {}
	virtual void DrawNormal(float x, float y) {}
	virtual void DrawHighlighted(const CRGBA &, float x, float y) {}
	virtual void SetAlpha(uint8 alpha) {}
	virtual void SetShadows(bool bDropShadows, const CRGBA &shadowColor, const CVector2D &shadowOffset) {}
	virtual bool GoNext(void) { DeactivateMenu(); return false; }
	virtual bool GoPrev(void) { DeactivateMenu(); return false; }
	virtual bool GoDown(void) { return GoNext(); }
	virtual bool GoUp(void) { return GoPrev(); }
	virtual bool GoDownStill(void) { return false; }
	virtual bool GoUpStill(void) { return false; }
	virtual bool GoLeft(void) { return true; }
	virtual bool GoRight(void) { return true; }
	virtual bool GoLeftStill(void) { return true; }
	virtual bool GoRightStill(void) { return true; }
	virtual bool GoFirst(void) { ActivateMenu(true); return true; }
	virtual bool GoLast(void) { ActivateMenu(true); return true; }
	virtual void SelectCurrentOptionUnderCursor(void) {}
	virtual void SelectDefaultCancelAction(void) {}
	virtual void ActivateMenu(bool first) { m_bActive = true; }
	virtual void DeactivateMenu(void) { m_bActive = false; }
	virtual int GetMenuSelection(void) { return -1; }
	virtual void SetMenuSelection(int) {}
};

class CMenuPictureAndText : public CMenuBase
{
public:
	int m_numSprites;
	CPlaceableShSprite m_sprites[5];
	int m_numTexts;
	CPlaceableShText m_texts[20];

	CVector2D m_oldTextScale;
	CVector2D m_textScale;
	bool m_bSetTextScale;

	float m_wrapX;
	float m_oldWrapx;
	bool m_bWrap;
	// missing some?


	CMenuPictureAndText(void)
	 : m_numSprites(0), m_numTexts(0),
	   m_bSetTextScale(false), m_bWrap(false) {}

	void SetNewOldShadowWrapX(bool bWrapX, float newWrapX, float oldWrapX);
	void SetNewOldTextScale(bool bTextScale, const CVector2D &newScale, const CVector2D &oldScale);
	void SetTextsColor(const CRGBA &color);
	void AddText(wchar *text, float positionX, float positionY, const CRGBA &color, bool bRightJustify);
	void AddPicture(CSprite2d *sprite, CSprite2d *shadow, float positionX, float positionY, float width, float height, const CRGBA &color);
	void AddPicture(CSprite2d *sprite, float positionX, float positionY, float width, float height, const CRGBA &color);

	virtual void Draw(const CRGBA &, const CRGBA &, float x, float y);
	virtual void DrawNormal(float x, float y) { Draw(CRGBA(0,0,0,0), CRGBA(0,0,0,0), x, y); }
	virtual void DrawHighlighted(const CRGBA &, float x, float y) { Draw(CRGBA(0,0,0,0), CRGBA(0,0,0,0), x, y); }
	virtual void SetAlpha(uint8 alpha);
	virtual void SetShadows(bool bDropShadows, const CRGBA &shadowColor, const CVector2D &shadowOffset);
	virtual bool GoNext(void) { return false; }
	virtual bool GoPrev(void) { return false; }
	virtual bool GoDown(void) { return GoNext(); }
	virtual bool GoUp(void) { return GoPrev(); }
	virtual bool GoDownStill(void) { return false; }
	virtual bool GoUpStill(void) { return false; }
	virtual bool GoLeft(void) { return true; }
	virtual bool GoRight(void) { return true; }
	virtual bool GoLeftStill(void) { return true; }
	virtual bool GoRightStill(void) { return true; }
	virtual bool GoFirst(void) { return false; }
	virtual bool GoLast(void) { return false; }
	virtual void SelectCurrentOptionUnderCursor(void) {}
	virtual void SelectDefaultCancelAction(void) {}
	virtual void ActivateMenu(bool first) {}
	virtual void DeactivateMenu(void) {}
	virtual int GetMenuSelection(void) { return -1; }
	virtual void SetMenuSelection(int) {}
};

class CMenuMultiChoice : public CMenuBase
{
public:
	int m_numOptions;
	CPlaceableShText m_title;
	CPlaceableShOption m_options[NUM_MULTICHOICE_OPTIONS];
	int m_cursor;
	CVector2D m_oldTextScale;
	CVector2D m_textScale;
	bool m_bSetTextScale;
	bool m_bSetTitleTextScale;

	CMenuMultiChoice(void)
	 : m_numOptions(0), m_cursor(-1),
	   m_bSetTextScale(false), m_bSetTitleTextScale(false) {}

	void AddTitle(wchar *text, float positionX, float positionY, bool bRightJustify);
	CPlaceableShOption *AddOption(wchar *text, float positionX, float positionY, bool bSelected, bool bRightJustify);
	void SetColors(const CRGBA &title, const CRGBA &normal, const CRGBA &selected);
	void SetNewOldTextScale(bool bTextScale, const CVector2D &newScale, const CVector2D &oldScale, bool bTitleTextScale);

	virtual void Draw(const CRGBA &optionHighlight, const CRGBA &titleHighlight, float x, float y);
	virtual void DrawNormal(float x, float y);
	virtual void DrawHighlighted(const CRGBA &titleHighlight, float x, float y);
	virtual void SetAlpha(uint8 alpha);
	virtual void SetShadows(bool bDropShadows, const CRGBA &shadowColor, const CVector2D &shadowOffset);
	virtual bool GoNext(void);
	virtual bool GoPrev(void);
	virtual bool GoDown(void) { return GoNext(); }
	virtual bool GoUp(void) { return GoPrev(); }
	virtual bool GoDownStill(void) { return false; }
	virtual bool GoUpStill(void) { return false; }
	virtual bool GoLeft(void) { return GoPrev(); }
	virtual bool GoRight(void) { return GoNext(); }
	virtual bool GoLeftStill(void) { return true; }
	virtual bool GoRightStill(void) { return true; }
	virtual bool GoFirst(void) { m_cursor = 0; return true; }
	virtual bool GoLast(void) { m_cursor = m_numOptions-1; return true; }
	virtual void SelectCurrentOptionUnderCursor(void);
	virtual void SelectDefaultCancelAction(void) {}
	virtual void ActivateMenu(bool first) { m_cursor = first ? 0 : m_numOptions-1; }
	virtual void DeactivateMenu(void) { m_cursor = -1; }
	virtual int GetMenuSelection(void);
	virtual void SetMenuSelection(int selection);
};

class CMenuMultiChoiceTriggered : public CMenuMultiChoice
{
public:
	typedef void (*Trigger)(CMenuMultiChoiceTriggered *);

	Trigger m_triggers[NUM_MULTICHOICE_OPTIONS];
	Trigger m_defaultCancel;

	CMenuMultiChoiceTriggered(void) { Initialise(); }

	void Initialise(void);
	CPlaceableShOption *AddOption(wchar *text, float positionX, float positionY, Trigger trigger, bool bSelected, bool bRightJustify);

	virtual void SelectCurrentOptionUnderCursor(void);
	virtual void SelectDefaultCancelAction(void);
};

class CMenuMultiChoiceTriggeredAlways : public CMenuMultiChoiceTriggered
{
public:
	Trigger m_alwaysNormalTrigger;
	Trigger m_alwaysHighlightTrigger;
	Trigger m_alwaysTrigger;

	CMenuMultiChoiceTriggeredAlways(void)
	 : m_alwaysNormalTrigger(nil), m_alwaysHighlightTrigger(nil), m_alwaysTrigger(nil) {}

	virtual void Draw(const CRGBA &optionHighlight, const CRGBA &titleHighlight, float x, float y);
	virtual void DrawNormal(float x, float y);
	virtual void DrawHighlighted(const CRGBA &titleHighlight, float x, float y);
};

class CMenuMultiChoicePictured : public CMenuMultiChoice
{
public:
	CPlaceableSprite m_sprites[NUM_MULTICHOICE_OPTIONS];
	bool m_bHasSprite[NUM_MULTICHOICE_OPTIONS];

	CMenuMultiChoicePictured(void) { Initialise(); }
	void Initialise(void);
	using CMenuMultiChoice::AddOption;
	CPlaceableShOption *AddOption(CSprite2d *sprite, float positionX, float positionY, const CVector2D &size, bool bSelected);

	virtual void Draw(const CRGBA &optionHighlight, const CRGBA &titleHighlight, float x, float y);
	virtual void DrawNormal(float x, float y);
	virtual void DrawHighlighted(const CRGBA &titleHighlight, float x, float y);
	virtual void SetAlpha(uint8 alpha);
	// unnecessary - same as base class
//	virtual void SetShadows(bool bDropShadows, const CRGBA &shadowColor, const CVector2D &shadowOffset);
};

class CMenuMultiChoicePicturedTriggered : public CMenuMultiChoicePictured
{
public:
	typedef void (*Trigger)(CMenuMultiChoicePicturedTriggered *);

	Trigger m_triggers[NUM_MULTICHOICE_OPTIONS];
	Trigger m_defaultCancel;

	CMenuMultiChoicePicturedTriggered(void) { Initialise(); }

	void Initialise(void);
	using CMenuMultiChoicePictured::AddOption;
	CPlaceableShOption *AddOption(CSprite2d *sprite, float positionX, float positionY, const CVector2D &size, Trigger trigger, bool bSelected);

	virtual void SelectCurrentOptionUnderCursor(void);
	virtual void SelectDefaultCancelAction(void);
};

struct FEC_MOVETAB
{
	int8 right;
	int8 left;
	int8 down;
	int8 up;
};

class CMenuMultiChoicePicturedTriggeredAnyMove : public CMenuMultiChoicePicturedTriggered
{
public:
	FEC_MOVETAB m_moveTab[NUM_MULTICHOICE_OPTIONS];

	CMenuMultiChoicePicturedTriggeredAnyMove(void) { Initialise(); }

	void Initialise(void);
	using CMenuMultiChoicePicturedTriggered::AddOption;
	CPlaceableShOption *AddOption(CSprite2d *sprite, FEC_MOVETAB *moveTab, float positionX, float positionY, const CVector2D &size, Trigger trigger, bool bSelected);

	virtual bool GoDown(void);
	virtual bool GoUp(void);
	virtual bool GoLeft(void);
	virtual bool GoRight(void);
};

// copy of CMenuMultiChoice pretty much except for m_options type
class CMenuMultiChoiceTwoLines : public CMenuBase
{
public:
	int m_numOptions;
	CPlaceableShText m_title;
	CPlaceableShOptionTwoLines m_options[NUM_MULTICHOICE_OPTIONS];
	int m_cursor;
	CVector2D m_oldTextScale;
	CVector2D m_textScale;
	bool m_bSetTextScale;
	bool m_bSetTitleTextScale;

	CMenuMultiChoiceTwoLines(void)
	 : m_numOptions(0), m_cursor(-1),
	   m_bSetTextScale(false), m_bSetTitleTextScale(false) {}

	void AddTitle(wchar *text, float positionX, float positionY, bool bRightJustify);
	CPlaceableShOptionTwoLines *AddOption(wchar *text, float positionX, float positionY, bool bSelected, bool bRightJustify);
	CPlaceableShOptionTwoLines *AddOption(wchar *text1, float positionX1, float positionY1, wchar *text2, float positionX2, float positionY2, bool bSelected, bool bRightJustify);
	void SetColors(const CRGBA &title, const CRGBA &normal, const CRGBA &selected);
	void SetNewOldTextScale(bool bTextScale, const CVector2D &newScale, const CVector2D &oldScale, bool bTitleTextScale);

	virtual void Draw(const CRGBA &optionHighlight, const CRGBA &titleHighlight, float x, float y);
	virtual void DrawNormal(float x, float y);
	virtual void DrawHighlighted(const CRGBA &titleHighlight, float x, float y);
	virtual void SetAlpha(uint8 alpha);
	virtual void SetShadows(bool bDropShadows, const CRGBA &shadowColor, const CVector2D &shadowOffset);
	virtual bool GoNext(void);
	virtual bool GoPrev(void);
	virtual bool GoDown(void) { return GoNext(); }
	virtual bool GoUp(void) { return GoPrev(); }
	virtual bool GoDownStill(void) { return true; }
	virtual bool GoUpStill(void) { return true; }
	virtual bool GoLeft(void) { return GoPrev(); }
	virtual bool GoRight(void) { return GoNext(); }
	virtual bool GoLeftStill(void) { return true; }
	virtual bool GoRightStill(void) { return true; }
	virtual bool GoFirst(void) { m_cursor = 0; return true; }
	virtual bool GoLast(void) { m_cursor = m_numOptions-1; return true; }
	virtual void SelectCurrentOptionUnderCursor(void);
	virtual void SelectDefaultCancelAction(void) {}
	virtual void ActivateMenu(bool first) { m_cursor = first ? 0 : m_numOptions-1; }
	virtual void DeactivateMenu(void) { m_cursor = -1; }
	virtual int GetMenuSelection(void);
	virtual void SetMenuSelection(int selection);
};

// copy of CMenuMultiChoiceTriggered except for m_options
class CMenuMultiChoiceTwoLinesTriggered : public CMenuMultiChoiceTwoLines
{
public:
	typedef void (*Trigger)(CMenuMultiChoiceTwoLinesTriggered *);

	Trigger m_triggers[NUM_MULTICHOICE_OPTIONS];
	Trigger m_defaultCancel;

	CMenuMultiChoiceTwoLinesTriggered(void) { Initialise(); }

	void Initialise(void);
	CPlaceableShOptionTwoLines *AddOption(wchar *text, float positionX, float positionY, Trigger trigger, bool bSelected, bool bRightJustify);
	CPlaceableShOptionTwoLines *AddOption(wchar *text1, float positionX1, float positionY1, wchar *text2, float positionX2, float positionY2, Trigger trigger, bool bSelected, bool bRightJustify);

	virtual void SelectCurrentOptionUnderCursor(void);
	virtual void SelectDefaultCancelAction(void);
};


class CMenuOnOff : public CMenuBase
{
public:
	CPlaceableShOption m_title;
	CPlaceableShText m_options[2];
	bool m_bActive;
	bool m_bSetTextScale;
	bool m_bSetTitleTextScale;
	CVector2D m_textScale;
	CVector2D m_oldTextScale;
	int m_type;	// 0: on/off 1: yes/no

	void SetColors(const CRGBA &title, const CRGBA &options);
	void SetNewOldTextScale(bool bTextScale, const CVector2D &newScale, const CVector2D &oldScale, bool bTitleTextScale);
	void SetOptionPosition(float x, float y, bool bRightJustify);
	void AddTitle(wchar *text, bool bSelected, float positionX, float positionY, bool bRightJustify);

	virtual void Draw(const CRGBA &optionHighlight, const CRGBA &titleHighlight, float x, float y);
	virtual void DrawNormal(float x, float y);
	virtual void DrawHighlighted(const CRGBA &titleHighlight, float x, float y);
	virtual void SetAlpha(uint8 alpha);
	virtual void SetShadows(bool bDropShadows, const CRGBA &shadowColor, const CVector2D &shadowOffset);
	virtual bool GoNext(void) { DeactivateMenu(); return false; }
	virtual bool GoPrev(void) { DeactivateMenu(); return false; }
	virtual bool GoDown(void) { return GoNext(); }
	virtual bool GoUp(void) { return GoPrev(); }
	virtual bool GoDownStill(void) { return false; }
	virtual bool GoUpStill(void) { return false; }
	virtual bool GoLeft(void) { SelectCurrentOptionUnderCursor(); return true; }
	virtual bool GoRight(void) { SelectCurrentOptionUnderCursor(); return true; }
	virtual bool GoLeftStill(void) { return true; }
	virtual bool GoRightStill(void) { return true; }
	virtual bool GoFirst(void) { ActivateMenu(true); return true; }
	virtual bool GoLast(void) { ActivateMenu(true); return true; }
	virtual void SelectCurrentOptionUnderCursor(void) { m_title.m_bSelected ^= 1; }
	virtual void SelectDefaultCancelAction(void) {}
	virtual void ActivateMenu(bool first) { m_bActive = true; }
	virtual void DeactivateMenu(void) { m_bActive = false; }
	virtual int GetMenuSelection(void) { return m_title.m_bSelected; }
	virtual void SetMenuSelection(int selection) { m_title.m_bSelected = selection; }
};

class CMenuOnOffTriggered : public CMenuOnOff
{
public:
	typedef void (*Trigger)(CMenuOnOffTriggered *);

	Trigger m_trigger;

	void SetOptionPosition(float x, float y, Trigger trigger, bool bRightJustify);

	virtual void SelectCurrentOptionUnderCursor(void);
};

class CMenuSlider : public CMenuBase
{
public:
	CPlaceableShText m_title;
	CPlaceableShText m_box;	// not really a text
	CRGBA m_colors[2];	// left and right
	CVector2D m_size[2];	// left and right
	int m_value;
	CPlaceableShText m_percentageText;
	bool m_bDrawPercentage;
//	char field_8D;
//	char field_8E;
//	char field_8F;
	uint8 m_someAlpha;
//	char field_91;
//	char field_92;
//	char field_93;
	bool m_bActive;
	int m_style;

	static char Buf8[8];
	static wchar Buf16[8];

	CMenuSlider(void)
	 : m_value(0), m_bDrawPercentage(false), m_bActive(false), m_style(0)
	{
		AddTickBox(0.0f, 0.0f, 100.0f, 10.0f, 10.0f); //todo
	}

	void SetColors(const CRGBA &title, const CRGBA &percentage, const CRGBA &left, const CRGBA &right);
	void DrawTicks(const CVector2D &position, const CVector2D &size, float heightRight, float level, const CRGBA &leftCol, const CRGBA &selCol, const CRGBA &rightCol, bool bShadow, const CVector2D &shadowOffset, const CRGBA &shadowColor);
	void DrawTicks(const CVector2D &position, const CVector2D &size, float heightRight, float level, const CRGBA &leftCol, const CRGBA &rightCol,                      bool bShadow, const CVector2D &shadowOffset, const CRGBA &shadowColor);
	void AddTickBox(float positionX, float positionY, float width, float heigthLeft, float heightRight);
	void AddTitle(wchar *text, float positionX, float positionY);

	virtual void Draw(const CRGBA &optionHighlight, const CRGBA &titleHighlight, float x, float y);
	virtual void DrawNormal(float x, float y);
	virtual void DrawHighlighted(const CRGBA &titleHighlight, float x, float y);
	virtual void SetAlpha(uint8 alpha);
	virtual void SetShadows(bool bDropShadows, const CRGBA &shadowColor, const CVector2D &shadowOffset);
	virtual bool GoNext(void) { DeactivateMenu(); return false; }
	virtual bool GoPrev(void) { DeactivateMenu(); return false; }
	virtual bool GoDown(void) { return GoNext(); }
	virtual bool GoUp(void) { return GoPrev(); }
	virtual bool GoDownStill(void) { return false; }
	virtual bool GoUpStill(void) { return false; }
	virtual bool GoLeft(void) { if(m_value < 0) m_value = 0; return true; }
	virtual bool GoRight(void) { if(m_value > 1000) m_value = 1000; return true; }
	virtual bool GoLeftStill(void) { m_value -= 8; if(m_value < 0) m_value = 0; return true; }
	virtual bool GoRightStill(void) { m_value += 8; if(m_value > 1000) m_value = 1000; return true; }
	virtual bool GoFirst(void) { ActivateMenu(true); return true; }
	virtual bool GoLast(void) { ActivateMenu(true); return true; }
	virtual void SelectCurrentOptionUnderCursor(void) {}
	virtual void SelectDefaultCancelAction(void) {}
	virtual void ActivateMenu(bool first) { m_bActive = true; }
	virtual void DeactivateMenu(void) { m_bActive = false; }
	virtual int GetMenuSelection(void) { return m_value/10; }
	virtual void SetMenuSelection(int selection) { m_value = selection*10; }
};

class CMenuSliderTriggered : public CMenuSlider
{
public:
	typedef void (*Trigger)(CMenuSliderTriggered *);

	Trigger m_trigger;
	Trigger m_alwaysTrigger;

	CMenuSliderTriggered(void)
	 : m_trigger(nil), m_alwaysTrigger(nil) {}

	void AddTickBox(float positionX, float positionY, float width, float heigthLeft, float heightRight, Trigger trigger, Trigger alwaysTrigger);

	virtual void Draw(const CRGBA &optionHighlight, const CRGBA &titleHighlight, float x, float y);
	virtual bool GoLeft(void);
	virtual bool GoRight(void);
	virtual bool GoLeftStill(void);
	virtual bool GoRightStill(void);
};


class CMenuLineLister : public CMenuBase
{
public:
	float m_width;
	float m_height;
	int m_numLines;
	CPlaceableShText m_linesLeft[NUM_LINELISTER_LINES_TOTAL];
	CPlaceableShText m_linesRight[NUM_LINELISTER_LINES_TOTAL];
	uint8 m_lineAlphas[NUM_LINELISTER_LINES_TOTAL];
	int8 m_lineFade[NUM_LINELISTER_LINES_TOTAL];
	float m_scrollPosition;
	float m_scrollSpeed;
	int field_10E8;
	float m_lineSpacing;

	CMenuLineLister(void);

	void SetLinesColor(const CRGBA &color);
	void ResetNumberOfTextLines(void);
	bool AddTextLine(wchar *left, wchar *right);

	CPlaceableShText *GetLeftLine(int i) { return &m_linesLeft[(i%NUM_LINELISTER_LINES) + 15]; };
	CPlaceableShText *GetRightLine(int i) { return &m_linesRight[(i%NUM_LINELISTER_LINES) + 15]; };

	virtual void Draw(const CRGBA &optionHighlight, const CRGBA &titleHighlight, float x, float y);
	virtual void DrawNormal(float x, float y) { Draw(CRGBA(0,0,0,0), CRGBA(0,0,0,0), x, y); }
	virtual void DrawHighlighted(const CRGBA &titleHighlight, float x, float y) { Draw(CRGBA(0,0,0,0), CRGBA(0,0,0,0), x, y); }
	virtual void SetAlpha(uint8 alpha);
	virtual void SetShadows(bool bDropShadows, const CRGBA &shadowColor, const CVector2D &shadowOffset);
	virtual bool GoNext(void) { return false; }
	virtual bool GoPrev(void) { return false; }
	virtual bool GoDown(void) { return GoNext(); }
	virtual bool GoUp(void) { return GoPrev(); }
	virtual bool GoDownStill(void) { m_scrollSpeed = 0.0f; return true; }
	virtual bool GoUpStill(void) { m_scrollSpeed *= 6.0f; return true; }
	virtual bool GoLeft(void) { return true; }
	virtual bool GoRight(void) { return true; }
	virtual bool GoLeftStill(void) { return true; }
	virtual bool GoRightStill(void) { return true; }
	virtual bool GoFirst(void) { return true; }
	virtual bool GoLast(void) { return true; }
	virtual void SelectCurrentOptionUnderCursor(void) {}
	virtual void SelectDefaultCancelAction(void) {}
	virtual void ActivateMenu(bool first) {}
	virtual void DeactivateMenu(void) {}
	virtual int GetMenuSelection(void) { return -1; }
	virtual void SetMenuSelection(int selection) {}
};

class CMenuPage
{
public:
	CMenuBase *m_controls[NUM_PAGE_WIDGETS];
	int m_numControls;
	CMenuBase *m_pCurrentControl;
	int m_cursor;

	CMenuPage(void) { Initialise(); }
	void Initialise(void);
	bool AddMenu(CMenuBase *widget);

	bool IsActiveMenuTwoState(void);
	void ActiveMenuTwoState_SelectNextPosition(void);
	void Draw(const CRGBA &,const CRGBA &, float, float);
	void DrawHighlighted(const CRGBA &titleHighlight, float x, float y);
	void DrawNormal(float x, float y);
	void ActivatePage(void);
	void SetAlpha(uint8 alpha);
	void SetShadows(bool, const CRGBA &, const CVector2D &);
	void GoPrev(void) { if(m_pCurrentControl) { if(!m_pCurrentControl->GoPrev()) m_pCurrentControl->GoLast(); } }
	void GoNext(void) { if(m_pCurrentControl) { if(!m_pCurrentControl->GoNext()) m_pCurrentControl->GoFirst(); } }
	void GoLeft(void) { if(m_pCurrentControl) { if(!m_pCurrentControl->GoLeft()) m_pCurrentControl->GoLast(); } }
	void GoRight(void) { if(m_pCurrentControl) { if(!m_pCurrentControl->GoRight()) m_pCurrentControl->GoFirst(); } }
	void GoUp(void) { if(m_pCurrentControl) { if(!m_pCurrentControl->GoUp()) m_pCurrentControl->GoLast(); } }
	void GoDown(void) { if(m_pCurrentControl) { if(!m_pCurrentControl->GoDown()) m_pCurrentControl->GoFirst(); } }
	void GoLeftStill(void) { if(m_pCurrentControl) m_pCurrentControl->GoLeftStill(); }
	void GoRightStill(void) { if(m_pCurrentControl) m_pCurrentControl->GoRightStill(); }
	void GoUpStill(void) { if(m_pCurrentControl) m_pCurrentControl->GoUpStill(); }
	void GoDownStill(void) { if(m_pCurrentControl) m_pCurrentControl->GoDownStill(); }
	void SelectDefaultCancelAction(void) { if(m_pCurrentControl) m_pCurrentControl->SelectDefaultCancelAction(); }
	void SelectCurrentOptionUnderCursor(void) { if(m_pCurrentControl) m_pCurrentControl->SelectCurrentOptionUnderCursor(); }

	virtual void GoUpMenuOnPage(void);
	virtual void GoDownMenuOnPage(void);
	virtual void GoLeftMenuOnPage(void);
	virtual void GoRightMenuOnPage(void);
};

class CMenuPageAnyMove : public CMenuPage
{
public:
	FEC_MOVETAB m_moveTab[NUM_PAGE_WIDGETS];

	CMenuPageAnyMove(void) { Initialise(); }
	void Initialise(void);
	using CMenuPage::AddMenu;
	bool AddMenu(CMenuBase *widget, FEC_MOVETAB *moveTab);

	virtual void GoUpMenuOnPage(void);
	virtual void GoDownMenuOnPage(void);
	virtual void GoLeftMenuOnPage(void);
	virtual void GoRightMenuOnPage(void);
};