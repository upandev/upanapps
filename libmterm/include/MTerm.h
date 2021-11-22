/*
 *  Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
 *
 *  I am making my contributions/submissions to this project solely in
 *  my personal capacity and am not conveying any rights to any
 *  intellectual property of any third parties.
 *                                                                          
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                          
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                          
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/
 */
#ifndef _MTERM_H_
#define _MTERM_H_

# include <cdisplay.h>
# include <stdlib.h>
# include <ColorPalettes.h>
# include <ConsoleCanvas.h>

#define WIN_SIZE(Win)		( Win->Rows * Win->Cols )

#define MTERM_LINE_END (-1)
#define MTERM_WIN_END (-2)

// Blink
#define ATTR_NOBLINK		0x00
#define ATTR_BLINK			0x80

#define		DEF_ATTR	( ATTR_NOBLINK | ColorPalettes::CP16::FG_WHITE | ColorPalettes::CP16::BG_BLACK )

typedef enum
{
	// TODO: Need to properly defined yet
	RED,
	WHITE,
	YELLOW,
	GREEN,
} MColor ;

typedef struct
{
	byte		Rows ;
	byte		Cols ;

	byte		ERows ;
	byte		ECols ;
	
	byte		WinCharAttr ;

	MChar*	CharContent;

	int			CursorPos ;
	int			PilotPoint ;

	byte		Focus ;
	byte		InstantUpdate ;
	byte 		ScrollLock ;
} MWindow ;


MWindow* MTerm_CreateWindow(byte nRows, byte nCols) ;
void MTerm_DestroyWindow(MWindow* pMWindow) ;
upanui::ConsoleCanvas* MTerm_InitScr() ;
unsigned MTerm_MaxRows();
unsigned MTerm_MaxColumns();
void MTerm_WinRefresh(const MWindow* pMWindow) ;
void MTerm_Refresh(const MWindow* pMWindow) ;
void MTerm_WinPutCharSpec(MWindow* pMWindow, char Ch, byte Attr) ;
void MTerm_WinPutChar(MWindow* pMWindow, char Ch) ;
void MTerm_WinPutStringSpec(MWindow* pMWindow, const char* szStr, int len, byte Attr) ;
void MTerm_WinPutString(MWindow* pMWindow, const char* szStr, int len) ;
void MTerm_ScrollDown(MWindow* pMWindow) ;
void MTerm_ScrollUp(MWindow* pMWindow) ;
void MTerm_WinClear(MWindow* pMWindow) ;
void MTerm_WinClearFromCursor(MWindow* pMWindow) ;
void MTerm_WinClearLineFromCursor(MWindow* pMWindow) ;
void MTerm_WinMoveCursor(MWindow* pMWindow, int iCurOffset) ;

void MTerm_WinSetCursor(MWindow* pMWindow, int iCurPos) ;
void MTerm_SetAttr(MWindow* pMWindow, int row, int col, int iLen, byte Attr) ;
void MTerm_SetWinAttr(MWindow* pMWindow, byte Attr) ;
void MTerm_WinDimension(MWindow* pMWindow, byte nRows, byte nCols) ;
void MTerm_SetWinPilotPoint(MWindow* pMWindow, int iPilotPoint) ;
void MTerm_WinInstantUpdate(MWindow* pMWindow, byte bUpdate) ;
void MTerm_WinScrollLock(MWindow* pMWindow, byte bLock) ;
void MTerm_WinFocus(MWindow* pMWindow, byte bFocus) ;

void MTerm_WinPutCharInRange(MWindow* pMWindow, char Ch, int row, int col, int iLen) ;
void MTerm_WinMovePutString(MWindow* pMWindow, int row, int col, const char* szStr) ;
void MTerm_WinMove(MWindow* pMWindow, int row, int col) ;
void MTerm_GetWinYX(MWindow* pMWindow, int* y, int* x) ;
void MTerm_WinNextLine(MWindow* pMWindow) ;

#endif
