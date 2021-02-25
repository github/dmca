CTriggerCaller MemCardAccessTriggerCaller;

void InitialiseTextsInMenuControllerInCar(CMenuPictureAndText *widget, CMenuManager::CONTRCONFIG cont);
void InitialiseTextsInMenuControllerOnFoot(CMenuPictureAndText *widget, CMenuManager::CONTRCONFIG cont);
void TriggerSave_BackToMainMenu(CMenuMultiChoiceTriggered *widget);
void TriggerSave_BackToMainMenuTwoLines(CMenuMultiChoiceTwoLinesTriggered *widget);
void TriggerSave_LoadGameLoadGameSelect(CMenuMultiChoiceTwoLinesTriggered *widget);
void TriggerSave_DeleteGameDeleteGameSelect(CMenuMultiChoiceTwoLinesTriggered *widget);
void TriggerSaveZone_BackToMainMenuTwoLines(CMenuMultiChoiceTwoLinesTriggered *widget);
void TriggerSaveZone_BackToMainMenuTwoLines(CMenuMultiChoiceTwoLinesTriggered *widget);
void TriggerSaveZone_SaveSlots(CMenuMultiChoiceTwoLinesTriggered *widget);

void
DisplayWarningControllerMsg()
{
	if ( CPad::bDisplayNoControllerMessage )
	{
		CSprite2d::DrawRect(CRect(X(20.0f), Y(140.0f), X(620.0f), Y(328.0)), CRGBA(64, 16, 16, 224)); // CRect(20.0f, 160.0f, 620.0f, 374.857117f)
		
		CFont::SetFontStyle(FONT_BANK);
		CFont::SetBackgroundOff();
		CFont::SetScale(X(0.84f), Y(1.26f)); // 1.440000
		CFont::SetPropOn();
		CFont::SetCentreOff();
		CFont::SetJustifyOn();
		CFont::SetRightJustifyOff();
		CFont::SetBackGroundOnlyTextOn();
		CFont::SetWrapx(SCRW-X(40.0f)); // 600.0f
		
		CPlaceableShText text;
		text.SetPosition(X(60.0f), Y(180.0f), false); // 205.714294
		text.SetColor(CRGBA(152, 152, 152, 255));
		text.m_text = TheText.Get("NOCONTE"); // Please re-insert the analog controller (DUALSHOCK@) or analog controller (DUALSHOCK@2) in controller port 1 to continue
		text.SetShadows(true, TEXT_SHADOW_COLOR, SHADOW_VECTOR*2.0f);
		text.SetAlpha(255);
		text.DrawShWrap(0.0f, 0.0f, X(600.0f+SHADOW_VECTOR.x), YF(600.0f)); //TODO check
		
		CFont::DrawFonts();
	}
	else if ( CPad::bObsoleteControllerMessage )
	{
		CSprite2d::DrawRect(CRect(X(20.0f), Y(140.0f), X(620.0f), Y(328.0)), CRGBA(64, 16, 16, 224)); // CRect(20.0f, 160.0f, 620.0f, 374.857117f)
		CFont::SetFontStyle(FONT_BANK);
		CFont::SetBackgroundOff();
		CFont::SetScale(X(0.84f), Y(1.26f)); // 1.440000
		CFont::SetPropOn();
		CFont::SetCentreOff();
		CFont::SetJustifyOn();
		CFont::SetRightJustifyOff();
		CFont::SetBackGroundOnlyTextOn();
		CFont::SetWrapx(SCRW-X(40.0f)); // 600.0f
		
		CPlaceableShText text;
		text.SetPosition(X(60.0f), Y(180.0f), false); // 205.714294
		text.SetColor(CRGBA(152, 152, 152, 255));
		text.m_text = TheText.Get("WRCONTE"); // Please re-insert the analog controller (DUALSHOCK@) or analog controller (DUALSHOCK@2) in controller port 1 to continue
		text.SetShadows(true, TEXT_SHADOW_COLOR, SHADOW_VECTOR*2.0f);
		text.SetAlpha(255);
		text.DrawShWrap(0.0f, 0.0f, X(600.0f+SHADOW_VECTOR.x), YF(600.0f)); //TODO check
		
		CFont::DrawFonts();
	}
	
}

void
TriggerMCSUM_Yes(CMenuMultiChoiceTriggered *widget)	
{
	if ( widget )
		bMemoryCardStartUpMenus_ExitNow = true;
}

int32 nStatLinesIndex;
wchar aStatLines[50+1][50];
wchar *PrintStatLine(char const *text, void *stat, unsigned char itsFloat, void *stat2)
{
	if (text && stat && nStatLinesIndex < 50)
	{
		char  line [64];
		wchar uline[64];
		
		memset(line, 0, sizeof(line));
		memset(uline, 0, sizeof(uline));

		if (stat2)
		{
			if ( itsFloat )
				sprintf(line, "  %.2f %s %.2f", *(float*)stat, UnicodeToAscii(TheText.Get("FEST_OO")), *(float*)stat2);
			else
				sprintf(line, "  %d %s %d", *(int32*)stat, UnicodeToAscii(TheText.Get("FEST_OO")), *(int32*)stat2);
		}
		else
		{
			if (itsFloat)
				sprintf(line, "  %.2f", *(float*)stat);
			else
				sprintf(line, "  %d", *(int32*)stat);
		}
		
		wchar *pStatLine = aStatLines[nStatLinesIndex++];
		
		AsciiToUnicode(line, uline);
		UnicodeStrcpy(pStatLine, uline);
		
		return pStatLine;
	}

	return nil;
}

void
DisplayMemoryCardAccessMsg(wchar *msg, CRGBA const &color)	
{	
	CSprite2d::DrawRect(CRect(X(70.0f), Y(100.0f), X(570.0f), Y(270.0f)), color);
	
	CFont::SetFontStyle(FONT_BANK);
	CFont::SetBackgroundOff();
	CFont::SetScale(X(MEMCARD_ACCESS_MSG_SIZE_X), Y(MEMCARD_ACCESS_MSG_SIZE_Y));
	CFont::SetPropOn();
	CFont::SetJustifyOn();
	CFont::SetRightJustifyOff();
	CFont::SetBackGroundOnlyTextOn();
	CFont::SetWrapx(SCRW-X(90.0f)); // 550.0f
	CFont::SetCentreOn();
	CFont::SetCentreSize(SCRW-X(180.0f)); // 460.0f
	
	CPlaceableShText text;
	
	text.SetPosition(X(320.0f), Y(120.0f), false); // 137.142868
	text.SetColor(CRGBA(200, 200, 200, 255));
	text.m_text = msg;
		
	text.SetShadows(true, TEXT_SHADOW_COLOR, SHADOW_VECTOR);
	text.SetAlpha(255);
	text.Draw(0.0f, 0.0f);
	
	CFont::DrawFonts();
	DoRWStuffEndOfFrame();
}

void
FillMenuWithMemCardFileListing(CMenuMultiChoiceTwoLinesTriggered *widget, void (*cancelTrigger)(CMenuMultiChoiceTwoLinesTriggered *), void (*selectTrigger)(CMenuMultiChoiceTwoLinesTriggered *), wchar *text, int32 y, int32 height, int32 offset)
{
	if ( widget )
	{
		int32 selected = 0;
		if ( bMemoryCardSpecialZone )
			selected = widget->m_cursor != -1 ? widget->m_cursor : 0;
		
		widget->DeactivateMenu(); // TODO check
		widget->m_numOptions = 0;
		widget->AddTitle(nil, 0.0f, 0.0f, 0);
		
		TheMemoryCard.PopulateSlotInfo(CARD_ONE);
		
		if ( TheMemoryCard.GetError() == CMemoryCard::NO_ERR_SUCCESS)
		{
			widget->AddOption(TheText.Get("FES_CAN"), 0.0f, YF(y), cancelTrigger, 0, 0);
			
			FrontEndMenuManager.field_3C = 0;
			
			y += offset;
			
			char buff[100];
			
			for ( int32 i = 0; i < CMemoryCard::MAX_SLOTS; i++ )
			{
				// SAVE FILE
				sprintf(buff, "%s %d ", UnicodeToAscii(TheText.Get("FES_SLO")), i+1);
				AsciiToUnicode(buff, MemoryCard_FileNames[i]);
				
				wchar *datetime = nil;
				
				switch ( TheMemoryCard.GetInfoOnSpecificSlot(i) )
				{
					case CMemoryCard::SLOT_CORRUPTED:
					{
						UnicodeStrcat(MemoryCard_FileNames[i], TheText.Get("FES_ISC")); // IS CORRUPTED
						datetime = TheMemoryCard.GetDateAndTimeOfSavedGame(i);
						break;
					}
					case CMemoryCard::SLOT_PRESENT:
					{
						if ( TheMemoryCard.GetNameOfSavedGame(i) != nil )
						{
							UnicodeStrcpy(MemoryCard_FileNames[i], TheMemoryCard.GetNameOfSavedGame(i));
							datetime = TheMemoryCard.GetDateAndTimeOfSavedGame(i);
						}
						else
						{
							UnicodeStrcpy(MemoryCard_FileNames[i], TheText.Get("FES_SAG")); // PRESENT
							datetime = TheMemoryCard.GetDateAndTimeOfSavedGame(i);
						}
						break;
					}
					case CMemoryCard::SLOT_NOTPRESENT:
					{
						UnicodeStrcat(MemoryCard_FileNames[i], TheText.Get("FES_ISF"));
						datetime = TheMemoryCard.GetDateAndTimeOfSavedGame(i);
						break;
					}
				}
				
				widget->AddOption(MemoryCard_FileNames[i], 0.0f, YF(y), datetime, 0.0f, YF(float(y)+(0.44f*height)), selectTrigger, 0, 0);
				y += height;
			}
		}
		else
		{
			if ( !gErrorSampleTriggered )
			{
				DMAudio.PlayFrontEndSound(SOUND_FRONTEND_FAIL, 0);
				gErrorSampleTriggered = true;
			}
			
			// Cancel
			widget->AddOption(TheText.Get("FES_CAN"), 0.0f, YF(y+(height*2)), cancelTrigger, 0, 0);
			
			FrontEndMenuManager.field_3C = 1;
			
			y += height;
			
			TheMemoryCard.PopulateErrorMessage();
			
			// Error Reading Memory Card (PS2) in MEMORY CARD slot 1 please check and try again.
			if ( TheMemoryCard.GetErrorMessage())
				widget->AddTitle(TheMemoryCard.GetErrorMessage(), 0.0f, YF(y), 0);
			else
				widget->AddTitle(TheText.Get("FES_GME"), 0.0f, YF(y), 0);
		}
		
		widget->SetMenuSelection(0);
		widget->ActivateMenu(1);
		
		if ( bMemoryCardSpecialZone )
		{
			widget->GoFirst();
			
			for ( int32 i = 0; i < selected; i++ )
				widget->GoNext();
		}
	}
}

void
TriggerSaveZone_FormatFailedOK(CMenuMultiChoiceTriggered *widget)
{
	if ( widget )
		pActiveMenuPage = &MenuPageSaveZone_SaveGame;
}

void
TriggerSaveZone_BackToMainMenu(CMenuMultiChoiceTriggered *widget)
{
	bMemoryCardSpecialZone = false;
	bIgnoreTriangleButton = false;
	pActiveMenuPage = &MenuPageSaveZone_SaveGame;
}

void
TriggerSaveZone_QuitMenu(CMenuMultiChoiceTriggered *widget)
{
	if ( widget )
	{
		FrontEndMenuManager.m_bMenuActive = false;
		FrontEndMenuManager.m_bInSaveZone = false;
		CTimer::EndUserPause();
	}
}

void
TriggerSaveZone_FormatCard(CMenuMultiChoiceTriggered *widget)
{
	if ( widget )
	{
		FillMenuWithMemCardFileListing(&MenuSaveZoneSSL_1, TriggerSaveZone_BackToMainMenuTwoLines, TriggerSaveZone_SaveSlots, nil, 0, 34, 22);
		
		if ( TheMemoryCard.GetError() == CMemoryCard::NO_ERR_SUCCESS)
		{
			MenuSaveZoneMSG_1.m_numTexts = 0;
			MenuSaveZoneMSG_1.AddText(TheText.Get("FES_AFO"), X(-80.0f), 0.0f, TEXT_COLOR, 0);
			
			MenuSaveZoneMSG_2.m_numOptions = 0;
			MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(100.0f), YF(5.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
			
			MenuPageSaveZone_Message.ActivatePage();
			pActiveMenuPage = &MenuPageSaveZone_Message;
		}
		else if ( TheMemoryCard.GetError() != CMemoryCard::ERR_NOFORMAT)
		{
			MenuSaveZoneMSG_1.m_numTexts = 0;
			MenuSaveZoneMSG_1.AddText(TheMemoryCard.GetErrorMessage(), X(-80.0f), 0.0f, TEXT_COLOR, 0);
			
			MenuSaveZoneMSG_2.m_numOptions = 0;
			MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(100.0f), YF(15.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
			
			MenuPageSaveZone_Message.ActivatePage();
			pActiveMenuPage = &MenuPageSaveZone_Message;
		}
		else
		{
			if ( !MemCardAccessTriggerCaller.CanCall() )
				MemCardAccessTriggerCaller.SetTrigger(TriggerSaveZone_FormatCard, widget);
			else
			{
				// Formatting Memory Card (PS2) in MEMORY CARD slot 1.  Please do not remove the Memory Card (PS2), reset or switch off the console.
				DisplayMemoryCardAccessMsg(TheText.Get("FEFD_WR"), CRGBA(200, 50, 50, 192));
				TheMemoryCard.FormatCard(CARD_ONE);
				
				if ( TheMemoryCard.GetError() == CMemoryCard::NO_ERR_SUCCESS )
					pActiveMenuPage = &MenuPageSaveZone_SaveGame;
				else
				{
					TheMemoryCard.PopulateErrorMessage();
					
					wchar *error = TheText.Get("FESZ_FF"); // Format Failed! Check Memory Card (PS2) in MEMORY CARD slot 1 and please try again.
					
					// missing switch
					
					if ( !error ) error = TheText.Get("FES_GME"); // Error Reading Memory Card (PS2) in MEMORY CARD slot 1 please check and try again.
					
					MenuSaveZoneMSG_1.m_numTexts = 0;
					MenuSaveZoneMSG_1.AddText(error, X(-80.0f), 0.0f, TEXT_COLOR, 0);
					
					MenuSaveZoneMSG_2.m_numOptions = 0;
					MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(100.0f), YF(20.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
					
					MenuPageSaveZone_Message.ActivatePage();
					pActiveMenuPage = &MenuPageSaveZone_Message;
				}
				
				if ( TheMemoryCard.GetError() == CMemoryCard::NO_ERR_SUCCESS )
				{
					FillMenuWithMemCardFileListing(&MenuSaveZoneSSL_1, TriggerSaveZone_BackToMainMenuTwoLines, TriggerSaveZone_SaveSlots, nil, 0, 34, 22);
					pActiveMenuPage = &MenuPageSaveZone_SaveSlots;
					bMemoryCardSpecialZone = true;
					bIgnoreTriangleButton = true;
					pActiveMenuPage->ActivatePage();
				}
				else
				{
					TheMemoryCard.PopulateErrorMessage();
					
					// Format Failed! Check Memory Card (PS2) in MEMORY CARD slot 1 and please try again.
					wchar *error = TheText.Get("FESZ_FF");
					
					switch ( TheMemoryCard.GetError() )
					{
						case CMemoryCard::ERR_WRITEFULLDEVICE:
						case CMemoryCard::ERR_DIRFULLDEVICE:
						case CMemoryCard::ERR_SAVEFAILED:
						{
							error = TheMemoryCard.GetErrorMessage();
							break;
						}
					}
					
					// Error Reading Memory Card (PS2) in MEMORY CARD slot 1 please check and try again.
					if ( !error ) error = TheText.Get("FES_GME");
					
					MenuSaveZoneMSG_1.m_numTexts = 0;
					MenuSaveZoneMSG_1.AddText(error, X(-80.0f), 0.0f, TEXT_COLOR, 0);
					
					MenuSaveZoneMSG_2.m_numOptions = 0;
					MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(100.0f), YF(20.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
					
					MenuPageSaveZone_Message.ActivatePage();
					pActiveMenuPage = &MenuPageSaveZone_Message;
				}
			}
		}
	}
}

void
TriggerSaveZone_FormatCardSelect(CMenuMultiChoiceTriggered *widget)
{
	if ( widget )
	{
		FillMenuWithMemCardFileListing(&MenuSaveZoneSSL_1, TriggerSaveZone_BackToMainMenuTwoLines, TriggerSaveZone_SaveSlots, nil, 0, 34, 22);
		
		if ( TheMemoryCard.GetError() == CMemoryCard::NO_ERR_SUCCESS )
		{
			// This Memory Card (PS2) is already formatted.
			MenuSaveZoneMSG_1.m_numTexts = 0;
			MenuSaveZoneMSG_1.AddText(TheText.Get("FES_AFO"), X(-80.0f), 0.0f, TEXT_COLOR, 0);
			
			MenuSaveZoneMSG_2.m_numOptions = 0;
			MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(100.0f), YF(5.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
			
			MenuPageSaveZone_Message.ActivatePage();
			pActiveMenuPage = &MenuPageSaveZone_Message;
		}
		else if ( TheMemoryCard.GetError() != CMemoryCard::ERR_NOFORMAT )
		{
			MenuSaveZoneMSG_1.m_numTexts = 0;
			MenuSaveZoneMSG_1.AddText(TheMemoryCard.GetErrorMessage(), X(-80.0f), 0.0f, TEXT_COLOR, 0);
			
			MenuSaveZoneMSG_2.m_numOptions = 0;
			MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(100.0f), YF(15.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
			
			MenuPageSaveZone_Message.ActivatePage();
			pActiveMenuPage = &MenuPageSaveZone_Message;
		}
		else
		{
			// Are you sure you wish to format the Memory Card (PS2) in MEMORY CARD slot 1? 
			MenuSaveZoneQYN_1.m_numTexts = 0;
			MenuSaveZoneQYN_1.AddText(TheText.Get("FESZ_QF"), X(-40.0f), 0.0f, TEXT_COLOR, 0);
			
			MenuSaveZoneQYN_2.m_numOptions = 0;
			MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_NO"),  X(80.0f), YF(20.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
			MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_YES"), X(80.0f), 0.0f,  TriggerSaveZone_FormatCard,     0, 0);
			
			MenuPageSaveZone_QuestionYesNo.ActivatePage();
			pActiveMenuPage = &MenuPageSaveZone_QuestionYesNo;
		}
	}
}

void
TriggerSaveZone_DeleteSaveGame(CMenuMultiChoiceTriggered *widget)
{	
	if ( widget )
	{
		bMemoryCardSpecialZone = false;
		bIgnoreTriangleButton = false;
		
		if ( !MemCardAccessTriggerCaller.CanCall() )
			MemCardAccessTriggerCaller.SetTrigger(TriggerSaveZone_DeleteSaveGame, widget);
		else
		{
			// Overwriting data. Please do not remove the Memory Card (PS2) in MEMORY CARD slot 1, reset or switch off the console.
			DisplayMemoryCardAccessMsg(TheText.Get("FESZ_OW"), CRGBA(200, 50, 50, 192));
			
			TheMemoryCard.DeleteSlot(MemoryCardSlotSelected);
			
			if ( TheMemoryCard.GetError() != CMemoryCard::NO_ERR_SUCCESS )
			{
				TheMemoryCard.PopulateErrorMessage();
				
				wchar *error = TheText.Get("FES_DEE"); // Deleting Failed! Check Memory Card (PS2) in MEMORY CARD slot 1 and please try again.
				
				// switch missing
				
				if ( !error ) error = TheText.Get("FES_GME"); // Error Reading Memory Card (PS2) in MEMORY CARD slot 1 please check and try again.
				
				MenuSaveZoneMSG_1.m_numTexts = 0;
				MenuSaveZoneMSG_1.AddText(error, X(-80.0f), 0.0f, TEXT_COLOR, 0);
				
				MenuSaveZoneMSG_2.m_numOptions = 0;
				MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(100.0f), YF(20.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
				
				MenuPageSaveZone_Message.ActivatePage();
				pActiveMenuPage = &MenuPageSaveZone_Message;
			}
			else
			{
				TheMemoryCard.SaveSlot(MemoryCardSlotSelected);
				
				if ( TheMemoryCard.GetError() == CMemoryCard::NO_ERR_SUCCESS )
				{
					// Game saved successfully!
					MenuSaveZoneMSG_1.m_numTexts = 0;
					MenuSaveZoneMSG_1.AddText(TheText.Get("FESZ_L1"), X(-20.0f), YF(10.0f), TEXT_COLOR, 0);
					
					MenuSaveZoneMSG_2.m_numOptions = 0;
					MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(110.0f), 0.0f, TriggerSaveZone_QuitMenu, 0, 0);
					
					MenuPageSaveZone_Message.ActivatePage();
					pActiveMenuPage = &MenuPageSaveZone_Message;
				}
				else
				{
					TheMemoryCard.PopulateErrorMessage();
					
					wchar *error = TheText.Get("FESZ_SR"); // Save Failed! Check Memory Card (PS2) in MEMORY CARD slot 1 and please try again.
					
					switch ( TheMemoryCard.GetError() )
					{
						case CMemoryCard::ERR_WRITEFULLDEVICE:
						case CMemoryCard::ERR_DIRFULLDEVICE:
						case CMemoryCard::ERR_SAVEFAILED:
						{
							error = TheMemoryCard.GetErrorMessage();
							break;
						}
					}
					
					if ( !error ) error = TheText.Get("FES_GME"); // Error Reading Memory Card (PS2) in MEMORY CARD slot 1 please check and try again.
					
					MenuSaveZoneMSG_1.m_numTexts = 0;
					MenuSaveZoneMSG_1.AddText(error, X(-80.0f), 0.0f, TEXT_COLOR, 0);
					
					MenuSaveZoneMSG_2.m_numOptions = 0;
					MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(120.0f), YF(30.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
					
					MenuPageSaveZone_Message.ActivatePage();
					pActiveMenuPage = &MenuPageSaveZone_Message;
				}
			}
		}
	}
}

void
TriggerSaveZone_SaveGame(CMenuMultiChoiceTriggered *widget)
{	
	if ( widget )
	{
		bMemoryCardSpecialZone = false;
		bIgnoreTriangleButton  = false;
		if ( !MemCardAccessTriggerCaller.CanCall() )
			MemCardAccessTriggerCaller.SetTrigger(TriggerSaveZone_SaveGame, widget);
		else
		{
			DisplayMemoryCardAccessMsg(TheText.Get("FESZ_WR"), CRGBA(200, 50, 50, 192));
			
			TheMemoryCard.SaveSlot(MemoryCardSlotSelected);
			
			if ( TheMemoryCard.GetError() == CMemoryCard::NO_ERR_SUCCESS )
			{
				// Game saved successfully!
				MenuSaveZoneMSG_1.m_numTexts = 0;
				MenuSaveZoneMSG_1.AddText(TheText.Get("FESZ_L1"), X(-20.0f), YF(10.0f), TEXT_COLOR, 0);
				
				MenuSaveZoneMSG_2.m_numOptions = 0;
				MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(110.0f), 0.0f, TriggerSaveZone_QuitMenu, 0, 0);
				
				MenuPageSaveZone_Message.ActivatePage();
				pActiveMenuPage = &MenuPageSaveZone_Message;
			}
			else
			{
				TheMemoryCard.PopulateErrorMessage();
				
				wchar *error = TheText.Get("FESZ_SR"); // Save Failed! Check Memory Card (PS2) in MEMORY CARD slot 1 and please try again.
				
				switch ( TheMemoryCard.GetError() )
				{
					case CMemoryCard::ERR_WRITEFULLDEVICE:
					case CMemoryCard::ERR_DIRFULLDEVICE:
					case CMemoryCard::ERR_SAVEFAILED:
					{
						error = TheMemoryCard.GetErrorMessage();
						break;
					}
				}
				
				if ( !error ) error = TheText.Get("FES_GME"); // Error Reading Memory Card (PS2) in MEMORY CARD slot 1 please check and try again.
				
				MenuSaveZoneMSG_1.m_numTexts = 0;
				MenuSaveZoneMSG_1.AddText(error, X(-80.0f), 0.0f, TEXT_COLOR, 0);
				
				MenuSaveZoneMSG_2.m_numOptions = 0;
				MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(120.0f), YF(30.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
				
				MenuPageSaveZone_Message.ActivatePage();
				pActiveMenuPage = &MenuPageSaveZone_Message;
			}
		}
	}
}

void
TriggerSaveZone_SaveSlots(CMenuMultiChoiceTwoLinesTriggered *widget)
{
	if ( widget )
	{
		if ( widget->GetMenuSelection() > 0 )
		{
			MemoryCardSlotSelected = widget->GetMenuSelection() - 1;
			
			switch ( TheMemoryCard.GetInfoOnSpecificSlot(MemoryCardSlotSelected) )
			{
				case CMemoryCard::SLOT_PRESENT:
				case CMemoryCard::SLOT_CORRUPTED:
				{
					// Proceed with overwriting this saved game? 
					MenuSaveZoneQYN_1.m_numTexts = 0;
					MenuSaveZoneQYN_1.AddText(TheText.Get("FESZ_QO"), X(-40.0f), 0.0f, TEXT_COLOR, 0);
					
					MenuSaveZoneQYN_2.m_numOptions = 0;
					MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_NO"),  X(80.0f), YF(20.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
					MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_YES"), X(80.0f), 0.0f,  TriggerSaveZone_DeleteSaveGame, 0, 0);
					
					MenuPageSaveZone_QuestionYesNo.ActivatePage();
					bMemoryCardSpecialZone = false;
					pActiveMenuPage = &MenuPageSaveZone_QuestionYesNo;
					break;
				}
				
				case CMemoryCard::SLOT_NOTPRESENT:
				{
					// PROCEED WITH SAVE ?
					MenuSaveZoneQYN_1.m_numTexts = 0;
					MenuSaveZoneQYN_1.AddText(TheText.Get("FESZ_QS"), X(-40.0f), 0.0f, TEXT_COLOR, 0);
					
					MenuSaveZoneQYN_2.m_numOptions = 0;
					MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_NO"),  X(80.0f), YF(20.0f), TriggerSaveZone_BackToMainMenu, 0, 0);
					MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_YES"), X(80.0f), 0.0f,  TriggerSaveZone_SaveGame,       0, 0);
					
					MenuPageSaveZone_QuestionYesNo.ActivatePage();
					bMemoryCardSpecialZone = false;
					pActiveMenuPage = &MenuPageSaveZone_QuestionYesNo;
					break;
				}
			}
		}
	}
}

void
TriggerSaveZone_SaveGameSelect(CMenuMultiChoiceTriggered *widget)
{	
	if ( widget )
	{
		FillMenuWithMemCardFileListing(&MenuSaveZoneSSL_1, TriggerSaveZone_BackToMainMenuTwoLines, TriggerSaveZone_SaveSlots, nil, 0, 34, 22);
		
		if ( TheMemoryCard.GetError() == CMemoryCard::ERR_NOFORMAT)
		{
			gErrorSampleTriggered = false;
			pActiveMenuPage = &MenuPageSaveZone_FormatCard;
		}
		else
		{
			bMemoryCardSpecialZone = true;
			bIgnoreTriangleButton  = true;
			pActiveMenuPage = &MenuPageSaveZone_SaveSlots;
		}
		
		pActiveMenuPage->ActivatePage();
	}
}

void
TriggerControls_Vibrations(CMenuOnOffTriggered *widget)
{
	if ( widget )
	{
		CMenuManager::m_PrefsUseVibration = widget->GetMenuSelection();
		if ( CMenuManager::m_PrefsUseVibration )
		{
			CPad::GetPad(0)->StartShake(300, 150);
			TimeToStopPadShaking = CTimer::GetTimeInMillisecondsPauseMode() + 500;
		}
	}
}

void
TriggerControls_ContrDisplay(CMenuMultiChoiceTriggeredAlways *widget)
{
	if ( widget )
	{
		int32 conf = MenuControls_1.GetMenuSelection();
		int32 i  = MenuControls_2.GetMenuSelection();
		if ( i == 1 )
		{
			if ( conf == CMenuManager::CONFIG_2 )
				MenuPage_Controls.m_controls[0] = &MenuControls_7;
			else
				MenuPage_Controls.m_controls[0] = &MenuControls_4;
		}
		else if ( i == 0 )
		{
			if ( conf == CMenuManager::CONFIG_2 )
				MenuPage_Controls.m_controls[0] = &MenuControls_6;
			else
				MenuPage_Controls.m_controls[0] = &MenuControls_3;
		}
	}
}

void
TriggerControls_DrawHNContrConfig(CMenuMultiChoiceTriggeredAlways *widget)
{
	if ( widget )
	{
		int32 conf = widget->GetMenuSelection();
		
		InitialiseTextsInMenuControllerOnFoot(&MenuControls_3, (CMenuManager::CONTRCONFIG)conf);
		InitialiseTextsInMenuControllerInCar (&MenuControls_4, (CMenuManager::CONTRCONFIG)conf);
		
		int32 i = MenuControls_2.GetMenuSelection();
		if ( i == 1 )
		{
			if ( conf == CMenuManager::CONFIG_2 )
				MenuPage_Controls.m_controls[0] = &MenuControls_7;
			else
				MenuPage_Controls.m_controls[0] = &MenuControls_4;
		}
		else if ( i == 0 )
		{
			if ( conf == CMenuManager::CONFIG_2 )
				MenuPage_Controls.m_controls[0] = &MenuControls_6;
			else
				MenuPage_Controls.m_controls[0] = &MenuControls_3;
		}
	}
}

void
TriggerControls_DrawContrConfig(CMenuMultiChoiceTriggeredAlways *widget)
{
	if ( widget )
	{
		int32 conf = widget->GetMenuSelection();
		if ( widget->m_cursor != -1 )
			conf = widget->m_cursor;
		
		InitialiseTextsInMenuControllerOnFoot(&MenuControls_3, (CMenuManager::CONTRCONFIG)conf);
		InitialiseTextsInMenuControllerInCar(&MenuControls_4, (CMenuManager::CONTRCONFIG)conf);
		
		int32 i = MenuControls_2.GetMenuSelection();
		if ( i == 1 )
		{
			if ( conf == CMenuManager::CONFIG_2 )
				MenuPage_Controls.m_controls[0] = &MenuControls_7;
			else
				MenuPage_Controls.m_controls[0] = &MenuControls_4;
		}
		else if ( i == 0 )
		{
			if ( conf == CMenuManager::CONFIG_2 )
				MenuPage_Controls.m_controls[0] = &MenuControls_6;
			else
				MenuPage_Controls.m_controls[0] = &MenuControls_3;
		}
	}
}

void
TriggerControls_ContrConfig(CMenuMultiChoiceTriggered *widget)
{
	if ( widget )
	{
		int32 conf = widget->GetMenuSelection();
		
		InitialiseTextsInMenuControllerOnFoot(&MenuControls_3, (CMenuManager::CONTRCONFIG)conf);
		InitialiseTextsInMenuControllerInCar(&MenuControls_4, (CMenuManager::CONTRCONFIG)conf);
		
		int32 i = MenuControls_2.GetMenuSelection();
		if ( i == 1 )
		{
			if ( conf == CMenuManager::CONFIG_2 )
				MenuPage_Controls.m_controls[0] = &MenuControls_7;
			else
				MenuPage_Controls.m_controls[0] = &MenuControls_4;
		}
		else if ( i == 0 )
		{
			if ( conf == CMenuManager::CONFIG_2 )
				MenuPage_Controls.m_controls[0] = &MenuControls_6;
			else
				MenuPage_Controls.m_controls[0] = &MenuControls_3;
		}
	}
}

void
TriggerLanguage_Language(CMenuMultiChoiceTriggered *widget)
{	
	if ( widget )
	{
		if ( CMenuManager::m_PrefsLanguage != widget->GetMenuSelection() )
		{
			CMenuManager::m_PrefsLanguage = widget->GetMenuSelection();
			FrontEndMenuManager.m_bInitialised = false;
			bFrontEnd_ReloadObrTxtGxt = true;
		}
	}
}

void
TriggerAudio_RadioStation(CMenuMultiChoicePicturedTriggered *widget)
{
	if ( widget )
	{
		if ( CMenuManager::m_PrefsRadioStation != widget->GetMenuSelection() )
		{
			CMenuManager::m_PrefsRadioStation = widget->GetMenuSelection();
			DMAudio.PlayFrontEndTrack(CMenuManager::m_PrefsRadioStation, 1);
			DMAudio.SetRadioInCar(CMenuManager::m_PrefsRadioStation);
		}
	}
}

void
TriggerAudio_StereoMono(CMenuMultiChoiceTriggered *widget)
{
	if ( widget )
	{	
		if (widget->GetMenuSelection() == 1)
		{
			DMAudio.SetMonoMode(true);
			DMAudio.PlayFrontEndSound(SOUND_FRONTEND_MONO, 0);
		}
		else
		{
			DMAudio.SetMonoMode(false);
			DMAudio.PlayFrontEndSound(SOUND_FRONTEND_STEREO, 0);
		}
	}
}

void
TriggerAudio_MusicVolumeAlways(CMenuSliderTriggered *widget)
{
	;
}

void
TriggerAudio_SfxVolumeAlways(CMenuSliderTriggered *widget)
{
	if ( widget )
	{
		static bool bTriggerTest = false;
		
		CMenuManager::m_PrefsSfxVolume = float(widget->GetMenuSelection()) / 100.0f * 127.0f + 0.5f;
		
		if ( CMenuManager::m_PrefsSfxVolume == 102 && !CPad::GetPad(0)->GetDPadLeft()&& !CPad::GetPad(0)->GetDPadRight() )
		{
			if ( bTriggerTest )
			{
				DMAudio.PlayFrontEndSound(SOUND_FRONTEND_AUDIO_TEST, 0);
				bTriggerTest = false;
			}
		}
		else
			bTriggerTest = true;
		
		FrontEndMenuManager.SetSoundLevelsForMusicMenu();
	}
}

void
TriggerAudio_MusicVolume(CMenuSliderTriggered *widget)
{	
	if ( widget )
	{
		CMenuManager::m_PrefsMusicVolume = float(widget->GetMenuSelection()) / 100.0f * 127.0f + 0.5f;
		FrontEndMenuManager.SetSoundLevelsForMusicMenu();
	}
}

void
TriggerAudio_SfxVolume(CMenuSliderTriggered *widget)
{
	;
}

void
TriggerSave_NewGameNewGame(CMenuMultiChoiceTriggered *widget)
{	
	FrontEndMenuManager.m_bWantToRestart = true;
	FrontEndMenuManager.m_bMenuActive    = false;
	FrontEndMenuManager.m_bInSaveZone    = false;
	bIgnoreTriangleButton = false;
	
	CTimer::EndUserPause();
	
	FrontEndMenuManager.AnaliseMenuContents();
	
	DMAudio.SetEffectsFadeVol(0);
	DMAudio.SetMusicFadeVol(0);
	DMAudio.ResetTimers(CTimer::GetTimeInMilliseconds());
}

void
TriggerSave_NewGameSelectYes(CMenuMultiChoiceTriggered *widget)
{
	// Are you sure you want to start a new game? All progress since the last save game will be lost. Proceed?
	MenuSaveZoneQYN_1.m_numTexts = 0;
	MenuSaveZoneQYN_1.AddText(TheText.Get("FESZ_QR"), X(-100.0f), 0.0f, TEXT_COLOR, 0);
	
	MenuSaveZoneQYN_2.m_numOptions = 0;
	MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_NO"),  X(80.0f), YF(30.0f), TriggerSave_BackToMainMenu, 0, 0);
	MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_YES"), X(80.0f), YF(10.0f), TriggerSave_NewGameNewGame, 0, 0);
	
	MenuPageSaveZone_QuestionYesNo.ActivatePage();
	pMenuSave = &MenuPageSaveZone_QuestionYesNo;
	bIgnoreTriangleButton = true;
}

void
TriggerSave_DeleteGameDeleteGame(CMenuMultiChoiceTriggered *widget)
{
	if ( widget )
	{
		bMemoryCardSpecialZone = false;
		bIgnoreTriangleButton  = false;
		
		if ( !MemCardAccessTriggerCaller.CanCall() )
			MemCardAccessTriggerCaller.SetTrigger(TriggerSave_DeleteGameDeleteGame, widget);
		else
		{
			// Deleting data. Please do not remove the Memory Card (PS2) in MEMORY CARD slot 1, reset or switch off the console.
			DisplayMemoryCardAccessMsg(TheText.Get("FEDL_WR"), CRGBA(200, 50, 50, 192));
			
			TheMemoryCard.DeleteSlot(MemoryCardSlotSelected);
			
			if ( TheMemoryCard.GetError() != CMemoryCard::NO_ERR_SUCCESS)
			{
				// Deleting Failed! Check Memory Card (PS2) in MEMORY CARD slot 1 and please try again.
				MenuSaveZoneMSG_1.m_numTexts = 0;
				MenuSaveZoneMSG_1.AddText(TheText.Get("FES_DEE"), X(-80.0f), YF(20.0f), TEXT_COLOR, 0);
				
				MenuSaveZoneMSG_2.m_numOptions = 0;
				MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(100.0f), YF(15.0f), TriggerSave_BackToMainMenu, 0, 0);
				
				MenuPageSaveZone_Message.ActivatePage();
				pMenuSave = &MenuPageSaveZone_Message;
				
				bMemoryCardSpecialZone = false;
				bIgnoreTriangleButton  = true;
			}
			else
			{
				FillMenuWithMemCardFileListing(&MenuSaveLG_2, TriggerSave_BackToMainMenuTwoLines, TriggerSave_LoadGameLoadGameSelect,     nil, 0, 34, 22);
				FillMenuWithMemCardFileListing(&MenuSaveDG_2, TriggerSave_BackToMainMenuTwoLines, TriggerSave_DeleteGameDeleteGameSelect, nil, 0, 34, 22);
				
				pMenuSave = &MenuPage_SaveBasic;
				pMenuSave->ActivatePage();
			}
		}
	}
}

void
TriggerSave_DeleteGameDeleteGameSelect(CMenuMultiChoiceTwoLinesTriggered *widget)
{
	if ( widget )
	{
		if ( widget->GetMenuSelection() > 0 )
		{
			MemoryCardSlotSelected = widget->GetMenuSelection() - 1;
			
			switch ( TheMemoryCard.GetInfoOnSpecificSlot(MemoryCardSlotSelected) )
			{
				case CMemoryCard::SLOT_NOTPRESENT:
				{
					break;
				}
				case CMemoryCard::SLOT_CORRUPTED:
				case CMemoryCard::SLOT_PRESENT:
				{
					// Proceed with deleting this saved game? 
					MenuSaveZoneQYN_1.m_numTexts = 0;
					MenuSaveZoneQYN_1.AddText(TheText.Get("FESZ_QD"), X(-40.0f), 0.0f, TEXT_COLOR, 0);
					
					MenuSaveZoneQYN_2.m_numOptions = 0;
					MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_NO"), X(80.0f), YF(20.0f), TriggerSave_BackToMainMenu, 0, 0);
					MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_YES"), X(80.0f), 0.0f, TriggerSave_DeleteGameDeleteGame, 0, 0);
					
					MenuPageSaveZone_QuestionYesNo.ActivatePage();
					pMenuSave = &MenuPageSaveZone_QuestionYesNo;
					bMemoryCardSpecialZone = false;
					break;
				}
			}
		}
	}
}

void
TriggerSave_DeleteGameSelect(CMenuMultiChoiceTriggered *widget)
{	
	FillMenuWithMemCardFileListing(&MenuSaveDG_2, TriggerSave_BackToMainMenuTwoLines, TriggerSave_DeleteGameDeleteGameSelect, nil, 0, 34, 22);
	FillMenuWithMemCardFileListing(&MenuSaveLG_2, TriggerSave_BackToMainMenuTwoLines, TriggerSave_LoadGameLoadGameSelect,     nil, 0, 34, 22);
	
	pMenuSave = &MenuPage_SaveDeleteGame;
	pMenuSave->ActivatePage();
	
	gErrorSampleTriggered  = false;
	bMemoryCardSpecialZone = true;
	bIgnoreTriangleButton  = true;
}

void
TriggerSave_LoadGameLoadGame(CMenuMultiChoiceTriggered *widget)
{	
	if ( widget )
	{
		bMemoryCardSpecialZone = false;
		bIgnoreTriangleButton  = false;
		
		if ( !MemCardAccessTriggerCaller.CanCall() )
			MemCardAccessTriggerCaller.SetTrigger(TriggerSave_LoadGameLoadGame, widget);
		else
		{
			// Loading data. Please do not remove the Memory Card (PS2) in MEMORY CARD slot 1, reset or switch off the console.
			DisplayMemoryCardAccessMsg(TheText.Get("FELD_WR"), CRGBA(200, 50, 50, 192));
			TheMemoryCard.LoadSlotToBuffer(MemoryCardSlotSelected);
			
			if ( TheMemoryCard.GetError() == CMemoryCard::NO_ERR_SUCCESS)
			{
				FrontEndMenuManager.m_bWantToRestart = true;
				FrontEndMenuManager.AnaliseMenuContents();
				FrontEndMenuManager.m_bMenuActive = false;
				FrontEndMenuManager.m_bInSaveZone = false;
				
				CTimer::EndUserPause();
				
				TheMemoryCard.m_bWantToLoad = true;
				
				DMAudio.SetEffectsFadeVol(0);
				DMAudio.SetMusicFadeVol(0);
				DMAudio.ResetTimers(CTimer::GetTimeInMilliseconds());
			}
			else
			{
				// Load Failed! Check Memory Card (PS2) in MEMORY CARD slot 1 and please try again.
				MenuSaveZoneMSG_1.m_numTexts = 0;
				MenuSaveZoneMSG_1.AddText(TheText.Get("FES_LOE"), X(-80.0f), YF(20.0f), TEXT_COLOR, 0);
				
				MenuSaveZoneMSG_2.m_numOptions = 0;
				MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(100.0f), YF(25.0f), TriggerSave_BackToMainMenu, 0, 0);
				
				pMenuSave = &MenuPageSaveZone_Message;
				pMenuSave->ActivatePage();
				
				bMemoryCardSpecialZone = false;
				bIgnoreTriangleButton = true;
			}
		}
	}
}

void
TriggerSave_LoadGameLoadGameSelect(CMenuMultiChoiceTwoLinesTriggered *widget)
{	
	if ( widget )
	{
		if ( widget->GetMenuSelection() > 0 )
		{
			MemoryCardSlotSelected = widget->GetMenuSelection() - 1;
			
			switch ( TheMemoryCard.GetInfoOnSpecificSlot(MemoryCardSlotSelected) )
			{
				case CMemoryCard::SLOT_NOTPRESENT:
				{
					break;
				}
				case CMemoryCard::SLOT_CORRUPTED:
				{
					// Load Failed.
					MenuSaveZoneMSG_1.m_numTexts = 0;
					MenuSaveZoneMSG_1.AddText(TheText.Get("FES_LOF"), X(50.0f), YF(20.0f), TEXT_COLOR, 0);
					
					MenuSaveZoneMSG_2.m_numOptions = 0;
					MenuSaveZoneMSG_2.AddOption(TheText.Get("FESZ_OK"), X(100.0f), 0.0f, TriggerSave_BackToMainMenu, 0, 0);
					
					MenuPageSaveZone_Message.ActivatePage();
					pMenuSave = &MenuPageSaveZone_Message;
					bMemoryCardSpecialZone = false;
					break;
				}
				case CMemoryCard::SLOT_PRESENT:
				{
					// All unsaved progress in your current game will be lost. Proceed with loading? 
					MenuSaveZoneQYN_1.m_numTexts = 0;
					MenuSaveZoneQYN_1.AddText(TheText.Get("FESZ_QL"), X(-40.0f), 0.0f, TEXT_COLOR, 0);
					
					MenuSaveZoneQYN_2.m_numOptions = 0;
					MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_NO"),  X(80.0f), YF(20.0f), TriggerSave_BackToMainMenu,   0, 0);
					MenuSaveZoneQYN_2.AddOption(TheText.Get("FEM_YES"), X(80.0f), 0.0f,  TriggerSave_LoadGameLoadGame, 0, 0);
					
					MenuPageSaveZone_QuestionYesNo.ActivatePage();
					pMenuSave = &MenuPageSaveZone_QuestionYesNo;
					bMemoryCardSpecialZone = false;
					break;
				}
			}
		}
	}
}

void
TriggerSave_LoadGameSelect(CMenuMultiChoiceTriggered *widget)
{
	FillMenuWithMemCardFileListing(&MenuSaveLG_2, TriggerSave_BackToMainMenuTwoLines, TriggerSave_LoadGameLoadGameSelect,     nil, 0, 34, 22);
	FillMenuWithMemCardFileListing(&MenuSaveDG_2, TriggerSave_BackToMainMenuTwoLines, TriggerSave_DeleteGameDeleteGameSelect, nil, 0, 34, 22);
	
	pMenuSave = &MenuPage_SaveLoadGame;
	pMenuSave->ActivatePage();
	
	gErrorSampleTriggered  = false;
	bMemoryCardSpecialZone = true;
	bIgnoreTriangleButton  = true;
}

void
TriggerSave_BackToMainMenu(CMenuMultiChoiceTriggered *widget)
{
	pMenuSave = &MenuPage_SaveBasic;
	pMenuSave->ActivatePage();
	bMemoryCardSpecialZone = false;
	bIgnoreTriangleButton  = false;
}

void InitialiseTextsInMenuControllerInCar(CMenuPictureAndText *widget, CMenuManager::CONTRCONFIG cont)
{
	if ( widget )
	{
		widget->m_numTexts = 0;
		
		switch ( cont )
		{
			case CMenuManager::CONFIG_1:
			{
				widget->AddText(TheText.Get("FEC_LL"),  X(50.0f),  Y(-14.0f), PAD_TEXT_COLOR, true);  // -16.0f
				widget->AddText(TheText.Get("FEC_RSC"), X(-4.0f),  Y(29.0f),  PAD_TEXT_COLOR, true);  // 33.142860f
				widget->AddText(TheText.Get("FEC_VES"), X(-4.0f),  Y(65.0f),  PAD_TEXT_COLOR, true);  // 74.285721f
				widget->AddText(TheText.Get("FEC_VES"), X(-4.0f),  Y(97.0f),  PAD_TEXT_COLOR, true);  // 110.857147f
				widget->AddText(TheText.Get("FEC_HO3"), X(84.0f),  Y(162.0f), PAD_TEXT_COLOR, false); // 185.142868f
				widget->AddText(TheText.Get("FEC_CAM"), X(103.0f), Y(141.0f), PAD_TEXT_COLOR, false); // 161.142868f
				widget->AddText(TheText.Get("FEC_PAU"), X(130.0f), Y(128.0f), PAD_TEXT_COLOR, false); // 146.285721f
				widget->AddText(TheText.Get("FEC_LB"),  X(68.0f),  Y(-6.0f),  PAD_TEXT_COLOR, false); // -6.857143f
				widget->AddText(TheText.Get("FEC_LR"),  X(184.0f), Y(-14.0f), PAD_TEXT_COLOR, false); // -16.0f
				widget->AddText(TheText.Get("FEC_HAB"), X(238.0f), Y(25.0f),  PAD_TEXT_COLOR, false); // 28.571430f
				widget->AddText(TheText.Get("FEC_BRA"), X(155.0f), Y(18.0f),  PAD_TEXT_COLOR, true);  //  20.571430f
				widget->AddText(TheText.Get("FEC_EXV"), X(238.0f), Y(52.0f),  PAD_TEXT_COLOR, false); // 59.428574f
				widget->AddText(TheText.Get("FEC_CAW"), X(238.0f), Y(65.0f),  PAD_TEXT_COLOR, false); //  74.285721f
				widget->AddText(TheText.Get("FEC_ACC"), X(238.0f), Y(78.0f),  PAD_TEXT_COLOR, false); // 89.142860f
				widget->AddText(TheText.Get("FEC_TUC"), X(238.0f), Y(94.0f),  PAD_TEXT_COLOR, false); // 107.428574f
				widget->AddText(TheText.Get("FEC_SM3"), X(238.0f), Y(109.0f), PAD_TEXT_COLOR, false); // 124.571434f
				
				break;
			}
			
			case CMenuManager::CONFIG_2:
			{
				widget->AddText(TheText.Get("FEC_LL"),  X(50.0f),  Y(-14.0f), PAD_TEXT_COLOR, true);  // -16.0f
				widget->AddText(TheText.Get("FEC_HOR"), X(-4.0f),  Y(29.0f),  PAD_TEXT_COLOR, true);  // 33.142860f
				widget->AddText(TheText.Get("FEC_CAM"), X(-4.0f),  Y(65.0f),  PAD_TEXT_COLOR, true);  // 74.285721f
				widget->AddText(TheText.Get("FEC_VES"), X(-4.0f),  Y(97.0f),  PAD_TEXT_COLOR, true);  // 110.857147f
				widget->AddText(TheText.Get("FEC_NA"),  X(84.0f),  Y(162.0f), PAD_TEXT_COLOR, false); // 185.142868f
				widget->AddText(TheText.Get("FEC_RSC"), X(103.0f), Y(141.0f), PAD_TEXT_COLOR, false); // 161.142868f
				widget->AddText(TheText.Get("FEC_PAU"), X(130.0f), Y(128.0f), PAD_TEXT_COLOR, false); // 146.285721f
				widget->AddText(TheText.Get("FEC_LB"),  X(68.0f),  Y(-6.0f),  PAD_TEXT_COLOR, false); // -6.857143f
				widget->AddText(TheText.Get("FEC_LR"),  X(184.0f), Y(-14.0f), PAD_TEXT_COLOR, false); // -16.0f
				widget->AddText(TheText.Get("FEC_HAB"), X(238.0f), Y(25.0f),  PAD_TEXT_COLOR, false); // 28.571430f
				widget->AddText(TheText.Get("FEC_BRA"), X(155.0f), Y(18.0f),  PAD_TEXT_COLOR, true);  //  20.571430f
				widget->AddText(TheText.Get("FEC_EXV"), X(238.0f), Y(52.0f),  PAD_TEXT_COLOR, false); // 59.428574f
				widget->AddText(TheText.Get("FEC_CAW"), X(238.0f), Y(65.0f),  PAD_TEXT_COLOR, false); //  74.285721f
				widget->AddText(TheText.Get("FEC_ACC"), X(238.0f), Y(78.0f),  PAD_TEXT_COLOR, false); // 89.142860f
				widget->AddText(TheText.Get("FEC_TUC"), X(238.0f), Y(94.0f),  PAD_TEXT_COLOR, false); // 107.428574f
				widget->AddText(TheText.Get("FEC_SM3"), X(238.0f), Y(109.0f), PAD_TEXT_COLOR, false); // 124.571434f
				
				break;
			}
			
			case CMenuManager::CONFIG_3:
			{
				widget->AddText(TheText.Get("FEC_LL"),  X(50.0f),  Y(-14.0f), PAD_TEXT_COLOR, true);  // -16.0f
				widget->AddText(TheText.Get("FEC_EXV"), X(-4.0f),  Y(29.0f),  PAD_TEXT_COLOR, true);  // 33.142860f
				widget->AddText(TheText.Get("FEC_VES"), X(-4.0f),  Y(65.0f),  PAD_TEXT_COLOR, true);  // 74.285721f
				widget->AddText(TheText.Get("FEC_VES"), X(-4.0f),  Y(97.0f),  PAD_TEXT_COLOR, true);  // 110.857147f
				widget->AddText(TheText.Get("FEC_RS3"), X(84.0f),  Y(162.0f), PAD_TEXT_COLOR, false); // 185.142868f
				widget->AddText(TheText.Get("FEC_CAM"), X(103.0f), Y(141.0f), PAD_TEXT_COLOR, false); // 161.142868f
				widget->AddText(TheText.Get("FEC_PAU"), X(130.0f), Y(128.0f), PAD_TEXT_COLOR, false); // 146.285721f
				widget->AddText(TheText.Get("FEC_LB"),  X(68.0f),  Y(-6.0f),  PAD_TEXT_COLOR, false); // -6.857143f
				widget->AddText(TheText.Get("FEC_LR"),  X(184.0f), Y(-14.0f), PAD_TEXT_COLOR, false); // -16.0f
				widget->AddText(TheText.Get("FEC_HOR"), X(238.0f), Y(25.0f),  PAD_TEXT_COLOR, false); // 28.571430f
				widget->AddText(TheText.Get("FEC_BRA"), X(155.0f), Y(18.0f),  PAD_TEXT_COLOR, true);  //  20.571430f
				widget->AddText(TheText.Get("FEC_HAB"), X(238.0f), Y(52.0f),  PAD_TEXT_COLOR, false); // 59.428574f
				widget->AddText(TheText.Get("FEC_CAW"), X(238.0f), Y(65.0f),  PAD_TEXT_COLOR, false); //  74.285721f
				widget->AddText(TheText.Get("FEC_ACC"), X(238.0f), Y(78.0f),  PAD_TEXT_COLOR, false); // 89.142860f
				widget->AddText(TheText.Get("FEC_TUC"), X(238.0f), Y(94.0f),  PAD_TEXT_COLOR, false); // 107.428574f
				widget->AddText(TheText.Get("FEC_SM3"), X(238.0f), Y(109.0f), PAD_TEXT_COLOR, false); // 124.571434f
				
				break;
			}
			
			case CMenuManager::CONFIG_4:
			{
				widget->AddText(TheText.Get("FEC_LL"),  X(50.0f),  Y(-14.0f), PAD_TEXT_COLOR, true);  // -16.0f
				widget->AddText(TheText.Get("FEC_HAB"), X(-4.0f),  Y(29.0f),  PAD_TEXT_COLOR, true);  // 33.142860f
				widget->AddText(TheText.Get("FEC_TUC"), X(-4.0f),  Y(65.0f),  PAD_TEXT_COLOR, true);  // 74.285721f
				widget->AddText(TheText.Get("FEC_VES"), X(-4.0f),  Y(97.0f),  PAD_TEXT_COLOR, true);  // 110.857147f
				widget->AddText(TheText.Get("FEC_HO3"), X(84.0f),  Y(162.0f), PAD_TEXT_COLOR, false); //  185.142868f
				widget->AddText(TheText.Get("FEC_CAM"), X(103.0f), Y(141.0f), PAD_TEXT_COLOR, false); //  161.142868f
				widget->AddText(TheText.Get("FEC_PAU"), X(130.0f), Y(128.0f), PAD_TEXT_COLOR, false); // 146.285721f
				widget->AddText(TheText.Get("FEC_LB"),  X(68.0f),  Y(-6.0f),  PAD_TEXT_COLOR, false); //  -6.857143f
				widget->AddText(TheText.Get("FEC_LR"),  X(184.0f), Y(-14.0f), PAD_TEXT_COLOR, false); // -16.0f
				widget->AddText(TheText.Get("FEC_CAW"), X(238.0f), Y(25.0f),  PAD_TEXT_COLOR, false); //  28.571430f
				widget->AddText(TheText.Get("FEC_SMT"), X(155.0f), Y(18.0f),  PAD_TEXT_COLOR, true);  //  20.571430f
				widget->AddText(TheText.Get("FEC_EXV"), X(238.0f), Y(52.0f),  PAD_TEXT_COLOR, false); // 59.428574f
				widget->AddText(TheText.Get("FEC_RSC"), X(238.0f), Y(65.0f),  PAD_TEXT_COLOR, false); //  74.285721f
				widget->AddText(TheText.Get("FEC_NA"),  X(238.0f), Y(78.0f),  PAD_TEXT_COLOR, false); // 89.142860f
				widget->AddText(TheText.Get("FEC_ACC"), X(238.0f), Y(94.0f),  PAD_TEXT_COLOR, false); // 107.428574f
				widget->AddText(TheText.Get("FEC_BRA"), X(238.0f), Y(109.0f), PAD_TEXT_COLOR, false); // 124.571434f
				
				break;
			}
		}
	}
}

void InitialiseTextsInMenuControllerOnFoot(CMenuPictureAndText *widget, CMenuManager::CONTRCONFIG cont)
{
	if ( widget )
	{
		widget->m_numTexts = 0;
		
		
		switch ( cont )
		{
			case CMenuManager::CONFIG_1:
			{
				widget->AddText(TheText.Get("FEC_CWL"),  X(50.0f),   Y(-14.0f), PAD_TEXT_COLOR, true);  // -16.0f
				widget->AddText(TheText.Get("FEC_LOF"),  X(-4.0f),   Y(25.0f),  PAD_TEXT_COLOR, true);  // 28.571430f
				widget->AddText(TheText.Get("FEC_MOV"),  X(-4.0f),   Y(65.0f),  PAD_TEXT_COLOR, true);  // 74.285721f
				widget->AddText(TheText.Get("FEC_MOV"),  X(-4.0f),   Y(97.0f),  PAD_TEXT_COLOR, true);  // 110.857147f
				widget->AddText(TheText.Get("FEC_CAM"),  X(103.0f),  Y(141.0f), PAD_TEXT_COLOR, false); // 161.142868f
				widget->AddText(TheText.Get("FEC_PAU"),  X(130.0f),  Y(128.0f), PAD_TEXT_COLOR, false); // 146.285721f
				widget->AddText(TheText.Get("FEC_CWR"),  X(184.0f),  Y(-14.0f), PAD_TEXT_COLOR, false); // -16.0f
				widget->AddText(TheText.Get("FEC_TAR"),  X(238.0f),  Y(25.0f),  PAD_TEXT_COLOR, false); // 28.571430f
				widget->AddText(TheText.Get("FEC_JUM"),  X(144.0f),  Y(18.0f),  PAD_TEXT_COLOR, true);  // 20.571430f
				widget->AddText(TheText.Get("FEC_ENV"),  X(238.0f),  Y(52.0f),  PAD_TEXT_COLOR, false); // 59.428574f
				widget->AddText(TheText.Get("FEC_ATT"),  X(238.0f),  Y(65.0f),  PAD_TEXT_COLOR, false); // 74.285721f
				widget->AddText(TheText.Get("FEC_RUN"),  X(238.0f),  Y(78.0f),  PAD_TEXT_COLOR, false); // 89.142860f
				widget->AddText(TheText.Get("FEC_FPC"),  X(238.0f),  Y(94.0f),  PAD_TEXT_COLOR, false); // 107.428574f
				widget->AddText(TheText.Get("FEC_LB3"),  X(238.0f),  Y(109.0f), PAD_TEXT_COLOR, false); // 124.571434f
				widget->AddText(TheText.Get("FEC_R3"),   X(238.0f),  Y(122.0f), PAD_TEXT_COLOR, false); // 139.428574f
				
				break;
			}
		
			case CMenuManager::CONFIG_2:
			{
				widget->AddText(TheText.Get("FEC_CWL"),  X(50.0f),   Y(-14.0f), PAD_TEXT_COLOR, true);  // -16.0f
				widget->AddText(TheText.Get("FEC_LOF"),  X(-4.0f),   Y(25.0f),  PAD_TEXT_COLOR, true);  // 28.571430f
				widget->AddText(TheText.Get("FEC_CAM"),  X(-4.0f),   Y(65.0f),  PAD_TEXT_COLOR, true);  // 74.285721f
				widget->AddText(TheText.Get("FEC_MOV"),  X(-4.0f),   Y(97.0f),  PAD_TEXT_COLOR, true);  // 110.857147f
				widget->AddText(TheText.Get("FEC_NA"),   X(103.0f),  Y(141.0f), PAD_TEXT_COLOR, false); // 161.142868f
				widget->AddText(TheText.Get("FEC_PAU"),  X(130.0f),  Y(128.0f), PAD_TEXT_COLOR, false); // 146.285721f
				widget->AddText(TheText.Get("FEC_CWR"),  X(184.0f),  Y(-14.0f), PAD_TEXT_COLOR, false); // -16.0f
				widget->AddText(TheText.Get("FEC_TAR"),  X(238.0f),  Y(25.0f),  PAD_TEXT_COLOR, false); // 28.571430f
				widget->AddText(TheText.Get("FEC_JUM"),  X(144.0f),  Y(18.0f),  PAD_TEXT_COLOR, true);  // 20.571430f
				widget->AddText(TheText.Get("FEC_ENV"),  X(238.0f),  Y(52.0f),  PAD_TEXT_COLOR, false); // 59.428574f
				widget->AddText(TheText.Get("FEC_ATT"),  X(238.0f),  Y(65.0f),  PAD_TEXT_COLOR, false); // 74.285721f
				widget->AddText(TheText.Get("FEC_RUN"),  X(238.0f),  Y(78.0f),  PAD_TEXT_COLOR, false); // 89.142860f
				widget->AddText(TheText.Get("FEC_FPC"),  X(238.0f),  Y(94.0f),  PAD_TEXT_COLOR, false); // 107.428574f
				widget->AddText(TheText.Get("FEC_LB3"),  X(238.0f),  Y(109.0f), PAD_TEXT_COLOR, false); // 124.571434f
				widget->AddText(TheText.Get("FEC_R3"),   X(238.0f),  Y(122.0f), PAD_TEXT_COLOR, false); // 139.428574f
				
				break;
			}
		
			case CMenuManager::CONFIG_3:
			{
				widget->AddText(TheText.Get("FEC_CWL"),  X(50.0f),   Y(-14.0f), PAD_TEXT_COLOR, true);  // -16.0f
				widget->AddText(TheText.Get("FEC_ENV"),  X(-4.0f),   Y(25.0f),  PAD_TEXT_COLOR, true);  // 28.571430f
				widget->AddText(TheText.Get("FEC_MOV"),  X(-4.0f),   Y(65.0f),  PAD_TEXT_COLOR, true);  // 74.285721f
				widget->AddText(TheText.Get("FEC_MOV"),  X(-4.0f),   Y(97.0f),  PAD_TEXT_COLOR, true);  // 110.857147f
				widget->AddText(TheText.Get("FEC_CAM"),  X(103.0f),  Y(141.0f), PAD_TEXT_COLOR, false); // 161.142868f
				widget->AddText(TheText.Get("FEC_PAU"),  X(130.0f),  Y(128.0f), PAD_TEXT_COLOR, false); // 146.285721f
				widget->AddText(TheText.Get("FEC_CWR"),  X(184.0f),  Y(-14.0f), PAD_TEXT_COLOR, false); // -16.0f
				widget->AddText(TheText.Get("FEC_TAR"),  X(238.0f),  Y(25.0f),  PAD_TEXT_COLOR, false); // 28.571430f
				widget->AddText(TheText.Get("FEC_JUM"),  X(144.0f),  Y(18.0f),  PAD_TEXT_COLOR, true);  // 20.571430f
				widget->AddText(TheText.Get("FEC_LOF"),  X(238.0f),  Y(52.0f),  PAD_TEXT_COLOR, false); // 59.428574f
				widget->AddText(TheText.Get("FEC_RUN"),  X(238.0f),  Y(65.0f),  PAD_TEXT_COLOR, false); // 74.285721f
				widget->AddText(TheText.Get("FEC_ATT"),  X(238.0f),  Y(78.0f),  PAD_TEXT_COLOR, false); // 89.142860f
				widget->AddText(TheText.Get("FEC_FPC"),  X(238.0f),  Y(94.0f),  PAD_TEXT_COLOR, false); // 107.428574f
				widget->AddText(TheText.Get("FEC_LB3"),  X(238.0f),  Y(109.0f), PAD_TEXT_COLOR, false); // 124.571434f
				widget->AddText(TheText.Get("FEC_R3"),   X(238.0f),  Y(122.0f), PAD_TEXT_COLOR, false); // 139.428574f
				
				break;
			}
			
			case CMenuManager::CONFIG_4:
			{
				widget->AddText(TheText.Get("FEC_CWL"),  X(50.0f),   Y(-14.0f), PAD_TEXT_COLOR, true);  // -16.0f
				widget->AddText(TheText.Get("FEC_TAR"),  X(-4.0f),   Y(25.0f),  PAD_TEXT_COLOR, true);  // 28.571430f
				widget->AddText(TheText.Get("FEC_NA"),   X(-4.0f),   Y(65.0f),  PAD_TEXT_COLOR, true);  // 74.285721f
				widget->AddText(TheText.Get("FEC_MOV"),  X(-4.0f),   Y(97.0f),  PAD_TEXT_COLOR, true);  // 110.857147f
				widget->AddText(TheText.Get("FEC_CAM"),  X(103.0f),  Y(141.0f), PAD_TEXT_COLOR, false); // 161.142868f
				widget->AddText(TheText.Get("FEC_PAU"),  X(130.0f),  Y(128.0f), PAD_TEXT_COLOR, false); // 146.285721f
				widget->AddText(TheText.Get("FEC_CWR"),  X(184.0f),  Y(-14.0f), PAD_TEXT_COLOR, false); // -16.0f
				widget->AddText(TheText.Get("FEC_ATT"),  X(238.0f),  Y(25.0f),  PAD_TEXT_COLOR, false); // 28.571430f
				widget->AddText(TheText.Get("FEC_JUM"),  X(144.0f),  Y(18.0f),  PAD_TEXT_COLOR, true);  // 20.571430f
				widget->AddText(TheText.Get("FEC_ENV"),  X(238.0f),  Y(52.0f),  PAD_TEXT_COLOR, false); // 59.428574f
				widget->AddText(TheText.Get("FEC_LOF"),  X(238.0f),  Y(65.0f),  PAD_TEXT_COLOR, false); // 74.285721f
				widget->AddText(TheText.Get("FEC_RUN"),  X(238.0f),  Y(78.0f),  PAD_TEXT_COLOR, false); // 89.142860f
				widget->AddText(TheText.Get("FEC_FPC"),  X(238.0f),  Y(94.0f),  PAD_TEXT_COLOR, false); // 107.428574f
				widget->AddText(TheText.Get("FEC_LB3"),  X(238.0f),  Y(109.0f), PAD_TEXT_COLOR, false); // 124.571434f
				widget->AddText(TheText.Get("FEC_R3"),   X(238.0f),  Y(122.0f), PAD_TEXT_COLOR, false); // 139.428574f
				
				break;
			}
		}
	}
}

void
TriggerSaveZone_BackToMainMenuTwoLines(CMenuMultiChoiceTwoLinesTriggered *widget)
{
	bMemoryCardSpecialZone = false;
	bIgnoreTriangleButton = false;
	pActiveMenuPage = &MenuPageSaveZone_SaveGame;
}

void
TriggerSave_BackToMainMenuTwoLines(CMenuMultiChoiceTwoLinesTriggered *widget)
{
	pMenuSave = &MenuPage_SaveBasic;
	pMenuSave->ActivatePage();
	bMemoryCardSpecialZone = false;
	bIgnoreTriangleButton = false;
}

void
SetRandomActiveTextlineColor(uint8 bText)
{
	if ( bMemoryCardSpecialZone )
		rgbaATC = SELECTED_TEXT_COLOR;
	else
	{
		bool bSelected = false;
		bool bHighlignted = false;
		
		switch ( FrontEndMenuManager.m_pageState )
		{
			case PAGESTATE_NORMAL:
				break;
			case PAGESTATE_HIGHLIGHTED:
				bHighlignted = true;
				break;
			case PAGESTATE_SELECTED:
				bSelected = true;
				break;
		}
		
		if ( FrontEndMenuManager.m_bInSaveZone )
			bSelected = true;
		
		if ( bSelected || bText )
		{
			static uint32 delayTime = 0;
			static bool bAddVal = true;
			
			if ( delayTime < CTimer::GetTimeInMillisecondsPauseMode() )
			{
				delayTime = CTimer::GetTimeInMillisecondsPauseMode() + 200;
				
				if ( bAddVal )
					rgbaATC = TEXT_COLOR;
				else
					rgbaATC = SELECTED_TEXT_COLOR;
				
				bAddVal = !bAddVal;
			}
		}
		
		if ( bHighlignted )
		{
			static uint32 delayTime = 0;
			static bool bAddVal = true;
			
			if ( delayTime < CTimer::GetTimeInMillisecondsPauseMode() )
			{
				delayTime = CTimer::GetTimeInMillisecondsPauseMode() + 200;
				
				if ( bAddVal )
					rgbaATC = TITLE_TEXT_COLOR;
				else
					rgbaATC = MENU_SELECTED_COLOR;
				
				bAddVal = !bAddVal;
			}
		}
	}
}

#ifdef GTA_PC

void
TriggerDisplay_Trails(CMenuOnOffTriggered *widget)
{
	if ( widget )
	{
		CMenuManager::m_PrefsShowTrails = widget->GetMenuSelection();
		CMBlur::BlurOn = CMenuManager::m_PrefsShowTrails;
		
		if ( CMBlur::BlurOn )
			CMBlur::MotionBlurOpen(Scene.camera);
		else
			CMBlur::MotionBlurClose();
	}
}

#endif