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
# include <Edit.h>
# include <stdio.h>
# include <string.h>
# include <malloc.h>
# include <mosstd.h>

MWindow* Edit_pBackWin ;
MWindow* Edit_pEditWin ;
MWindow* Edit_pMenuWin ;
MWindow* Edit_pStatusWin ;

int Edit_row, Edit_col ;
int Edit_yMin, Edit_yMax, Edit_xMin, Edit_xMax ;

byte Edit_bSave ;
char Edit_szFilename[110], Edit_szFname[10], Edit_szCheck[110] ;

int wy, wx ;
int ok ;

char *word ;
ENode *Edit_headNode, *Edit_curNode ;

/******************* Static functions ***********************/

static int Edit_FindCurOnMoveUp(int x)
{
	int in;
 
	while(Edit_curNode->blink != Edit_headNode && Edit_curNode->blink->c != '\n')
		Edit_curNode = Edit_curNode->blink ;
	
	in = Edit_xMin ;

	while(Edit_curNode->c != '\n')
	{
		Edit_curNode = Edit_curNode->flink ;
		in++;
		in = in % (Edit_xMax + 1) ;
		if(in == 0)
		   in = Edit_xMin ;
	}

	Edit_curNode = Edit_curNode->blink;

	if(in > x)
	{
		while(in > x)
		{ 
			in--;
			Edit_curNode = Edit_curNode->blink;
		}
	}
	
	return in;
}

static void Edit_DisOLine()
{
	int i, y, x ;
	ENode *temp, *temp1 ;

	MTerm_GetWinYX(Edit_pEditWin, &y, &x) ;

	temp1 = Edit_curNode ;
	Edit_Home() ;
	temp = Edit_curNode->flink ;
	Edit_curNode = temp1 ;

	MTerm_WinClearLineFromCursor(Edit_pEditWin) ;

	for(i = Edit_xMin; i <= Edit_xMax && temp != NULL && temp->c != '\n'; temp = temp->flink, i++)
		MTerm_WinPutChar(Edit_pEditWin, temp->c) ;

	MTerm_WinMove(Edit_pEditWin, y, x) ;
}

/*************************************************************/

void Edit_Init()
{
	Edit_row = 0 ;
	Edit_col = 0 ;
	Edit_yMin = 0 ;
	Edit_yMax = MTerm_MaxRows() - 5 ;
   	Edit_xMin = 0 ;
	Edit_xMax = MTerm_MaxColumns() - 3 ;

	wy = 0 ;
	wx = 0 ;
	ok = 1 ;

	Edit_bSave = true ;
	word = NULL ;

	strcpy(Edit_szFilename, "\0") ;
   	strcpy(Edit_szFname, "\0") ;
	strcpy(Edit_szCheck, "\0") ;

	Edit_headNode = Edit_CreateNode() ;
	Edit_curNode = Edit_headNode ;
}

ENode* Edit_CreateNode()
{
	ENode* x ;

	x = (ENode*)malloc(sizeof(ENode)) ;

	if(x == NULL)
	{
		printf("\n OUT OF MEM\n") ;
		return x ;
	}

	x->flink = x->blink = NULL ;
	x->c = '\0' ;

	return x ;
}

void Edit_InsertCh(char ch)
{
	ENode* temp ; 
	int i, y, x ;

	MTerm_GetWinYX(Edit_pEditWin, &y, &x) ;

	if(ch == '\t')
	{
		for(i = 0; i < 3; i++)
			Edit_InsertCh(' ') ;
		return ;
	}

	temp = Edit_CreateNode() ;
	temp->c = ch ;

	temp->flink = Edit_curNode->flink ;
	temp->blink = Edit_curNode ;
	if(Edit_curNode->flink != NULL)
		Edit_curNode->flink->blink = temp ;
	Edit_curNode->flink = temp ;

	if(x == Edit_xMax || ch == '\n')
		Edit_row++ ;
	
	if(ch == '\n')
	{
		MTerm_WinClearLineFromCursor(Edit_pEditWin) ;
		MTerm_WinNextLine(Edit_pEditWin) ;
	}
	else
		MTerm_WinPutChar(Edit_pEditWin, ch) ; 
	Edit_curNode = temp ;
}

void Edit_DisplayLine()
{
	ENode* temp ;
	int y, x ;

	MTerm_GetWinYX(Edit_pEditWin, &wy, &wx) ;
	MTerm_WinScrollLock(Edit_pEditWin, true) ;
 
	MTerm_WinClearFromCursor(Edit_pEditWin) ;

	temp = Edit_curNode->flink ;
	y = wy ;
	x = wx ;
  
	for(; y <= Edit_yMax && temp != NULL; y++) 
	{
		while(x <= Edit_xMax && temp != NULL)
		{
			x++ ;
			if(temp->c == '\n')
			{
				MTerm_WinNextLine(Edit_pEditWin) ;
				temp = temp->flink ;
				break ;
			}
			MTerm_WinPutChar(Edit_pEditWin, temp->c) ; 
			temp = temp->flink ;
		}
		x = Edit_xMin ;
	}

	MTerm_WinMove(Edit_pEditWin, wy, wx) ;
	MTerm_WinScrollLock(Edit_pEditWin, false) ;
}

void Edit_Home()
{
	int y, x ;

	MTerm_GetWinYX(Edit_pEditWin, &y, &x) ;

	for(; x > Edit_xMin; x--)
		Edit_curNode = Edit_curNode->blink;

	MTerm_WinMove(Edit_pEditWin, y, Edit_xMin) ;
}

void Edit_End()
{
	int i, y, x ;

	MTerm_GetWinYX(Edit_pEditWin, &y, &x) ;

	for(i =x; i < Edit_xMax && Edit_curNode->flink != NULL && Edit_curNode->flink->c != '\n'; i++)
		Edit_curNode = Edit_curNode->flink;

	MTerm_WinMove(Edit_pEditWin, y, i) ;
}

void Edit_MoveUp()
{
	int i, y, x ;

	MTerm_GetWinYX(Edit_pEditWin, &y, &x) ;

	Edit_Home() ;

	if(Edit_curNode == Edit_headNode)
		return ;

	Edit_row-- ;

	if(Edit_curNode->c != '\n')
	{
		for(i = Edit_xMax; i >= x; i--)
			Edit_curNode = Edit_curNode->blink ;
	}
	else
	{
		x = Edit_FindCurOnMoveUp(x) ;
	}

	if(y == Edit_yMin)
	{
		MTerm_ScrollUp(Edit_pEditWin) ;
		MTerm_WinMove(Edit_pEditWin, y, x) ;
		Edit_DisOLine() ;
	}
	else
		MTerm_WinMove(Edit_pEditWin, y - 1, x) ;
}

void Edit_MoveDown()
{
	int i, y, x ;
 
	MTerm_GetWinYX(Edit_pEditWin, &y, &x) ;
	MTerm_WinScrollLock(Edit_pEditWin, true) ;
	Edit_End() ;

	if(Edit_curNode->flink == NULL)
		return ;

	Edit_row++ ;

	Edit_curNode = Edit_curNode->flink ;

	for(i = Edit_xMin; i < x && Edit_curNode->flink != NULL && Edit_curNode->flink->c != '\n'; i++)
		Edit_curNode = Edit_curNode->flink ;

	if(y == Edit_yMax)
	{
		MTerm_ScrollDown(Edit_pEditWin) ;
		MTerm_WinMove(Edit_pEditWin, y, i) ;
		Edit_DisOLine() ;
	}
	else
		MTerm_WinMove(Edit_pEditWin, y + 1, i) ;

	MTerm_WinScrollLock(Edit_pEditWin, false) ;
}

void Edit_MoveLeft()
{
	int y, x ;

	MTerm_GetWinYX(Edit_pEditWin, &y, &x) ;

	if(x > Edit_xMin)
	{
		Edit_curNode = Edit_curNode->blink ;
		MTerm_WinMove(Edit_pEditWin, y, x - 1) ;
	}
	else if(Edit_curNode == Edit_headNode)
	{
	}
	else
	{
		Edit_MoveUp() ;
		Edit_End() ;
	}
}

void Edit_MoveRight()
{
	int y, x ;

	MTerm_GetWinYX(Edit_pEditWin, &y, &x) ;
 
	if(x < Edit_xMax && Edit_curNode->flink != NULL && Edit_curNode->flink->c != '\n')
	{
		MTerm_WinMove(Edit_pEditWin, y, x + 1) ;
		Edit_curNode = Edit_curNode->flink ;
	}
	else if(Edit_curNode != NULL)
	{
		Edit_Home() ;
		Edit_MoveDown() ;
	}
}

void Edit_DeleteCh()
{
	ENode* bakup ;
 
	if(Edit_curNode->flink == NULL)
		return ;

	bakup = Edit_curNode->flink ;
	Edit_curNode->flink = bakup->flink ;

	if(bakup->flink != NULL)
		bakup->flink->blink = Edit_curNode ;

	free(bakup) ;
}

void Edit_BakSpace()
{
	if(Edit_curNode == Edit_headNode)
	{
		//beep() ;
	}
	else
	{
		Edit_MoveLeft() ;
		Edit_DeleteCh() ;
	}
}

void Edit_PageUp()
{
	int i ;

	for(i = 0; i < 15; i++)
		Edit_MoveUp() ;
}

void Edit_PageDown()
{
	int i ;

	for(i = 0; i < 15; i++)
		Edit_MoveDown() ;
}

void Edit_ClearData(ENode* temp)
{
	ENode* extra;

	while(temp != NULL)
	{
		extra = temp;
		temp = temp->flink;
		free(extra);
	}
}

void Edit_DeleteLine()
{
	int x ;
	Edit_Home() ;

	MTerm_GetWinYX(Edit_pEditWin, &wy, &wx) ;

	for(x = Edit_xMin; Edit_curNode->flink != NULL && Edit_curNode->flink->c != '\n' && x < Edit_xMax; x++)
		Edit_DeleteCh() ;
	Edit_DeleteCh() ;
	
	Edit_DisplayLine() ;
}

void Edit_DeleteWord()
{
	MTerm_GetWinYX(Edit_pEditWin, &wy, &wx) ;

	while(Edit_curNode->flink != NULL && Edit_curNode != Edit_headNode && Edit_curNode->flink->c != ' ' 
			&& Edit_curNode->flink->c != '\n' && Edit_curNode->c != '\n')
		Edit_MoveLeft() ;
 
	Edit_DeleteCh() ;

	while(Edit_curNode->flink != NULL && Edit_curNode->flink->c != '\n' && Edit_curNode->flink->c != ' ')
		Edit_DeleteCh() ;

	Edit_DisplayLine() ;
}

void Edit_InsertLine()
{
	Edit_Home() ;

	Edit_InsertCh('\n') ;
	Edit_DisplayLine() ;
}

void Edit_InsertWord()
{
	int i, flag = 0 ;

	while(Edit_curNode->flink != NULL && Edit_curNode != Edit_headNode && Edit_curNode->flink->c != ' ' 
			&& Edit_curNode->flink->c != '\n'&& Edit_curNode->c != '\n')
		Edit_MoveLeft() ;

	if(Edit_curNode != Edit_headNode && Edit_curNode->c != '\n')    
		Edit_InsertCh(' ') ;
	else
		flag = 1 ;

	for(i = 0; word[i] != '\0'; i++)
		Edit_InsertCh(word[i]) ;

	if(flag)
		Edit_InsertCh(' ') ;
	
	Edit_DisplayLine() ;

	free(word) ;
	word = NULL ;
}
