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
#include <MTerm.h>
#include <Action.h>
#include <Edit.h>
#include <EView.h>
#include <kb.h>
#include <KeyboardEventHandler.h>
#include <GraphicsContext.h>

class EditorKeyboardEventHandler : public upanui::KeyboardEventHandler {
public:
  void onEvent(upanui::UIObject& uiObject, const upanui::KeyboardEvent& e) {
    const auto& key = e.getData();
    int y;

    if (EView_getCurrentPopupMenuType() != PopupMenuType::NA_Menu) {
      EView_HandleMenuActions(key);
    } else if (EView_getCurrentPopupDialogType() == PopupDialogType::HelpDialog) {
      EView_HandleHelpDialogActions(key);
    } else if(key.isCtrlPressed() && (key.getCh() == 'f' || key.getCh() == 'F')) {
      EView_OpenFileMenu() ; // Ctrl+F - FILE MENU
      return;
    } else if(key.isCtrlPressed() && (key.getCh() == 'e' || key.getCh() == 'E')) {
      EView_OpenEditMenu() ; // Ctrl+E - EDIT MENU
      return;
    } else if(key.isCtrlPressed() && (key.getCh() == 'h' || key.getCh() == 'H')) {
      EView_OpenHelpMenu() ;
      return;
    } else if(key.isCtrlPressed() && (key.getCh() == 'n' || key.getCh() == 'N'))
    {
      EView_OnFileMenu(1) ; // Ctrl+N - New
    }
    else if(key.isCtrlPressed() && (key.getCh() == 'o' || key.getCh() == 'O'))
    {
      EView_OnFileMenu(2) ; // Ctrl+O - Open
    }
    else if(key.isCtrlPressed() && (key.getCh() == 's' || key.getCh() == 'S'))
    {
      EView_OnFileMenu(3) ; // Ctrl+S - Save
    }
    else if(key.isCtrlPressed() && (key.getCh() == 'a' || key.getCh() == 'A'))
    {
      EView_OnFileMenu(4) ; // Ctrl+A - Save as
    }
    else if(key.isCtrlPressed() && (key.getCh() == 'x' || key.getCh() == 'X'))
    {
      EView_OnFileMenu(5) ; // ctrl+X - Exit
    }
    else if(key.isCtrlPressed() && (key.getCh() == 'd' || key.getCh() == 'D'))
    {
      EView_OnEditMenu(1) ; // Ctrl+D - Delete line
    }
    else if(key.isCtrlPressed() && (key.getCh() == 'r' || key.getCh() == 'R'))
    {
      EView_OnEditMenu(2) ; // Ctrl+R - Delete word
    }
    else if(key.isCtrlPressed() && (key.getCh() == 'k' || key.getCh() == 'K'))
    {
      EView_OnEditMenu(3) ; // Ctrl+K - Insert line
    }
    else if(key.isCtrlPressed() && (key.getCh() == 'w' || key.getCh() == 'W'))
    {
      EView_OnEditMenu(4) ; // Ctrl+W - Insert word
    }
    else if(key.isCtrlPressed() && (key.getCh() == 'p' || key.getCh() == 'P'))
    {
      //	options_menu();//Ctrl+P - Options Men
    }
    else if(key.getCh() == Keyboard_KEY_HOME)
    {
      Edit_Home() ;
    }
    else if(key.getCh() == Keyboard_KEY_END)
    {
      Edit_End() ;
    }
    else if(key.getCh() == Keyboard_KEY_UP)
    {
      Edit_MoveUp() ;
    }
    else if(key.getCh() == Keyboard_KEY_DOWN)
    {
      Edit_MoveDown() ;
    }
    else if(key.getCh() == Keyboard_KEY_LEFT)
    {
      Edit_MoveLeft() ;
    }
    else if(key.getCh() == Keyboard_KEY_RIGHT)
    {
      Edit_MoveRight() ;
    }
    else if(key.getCh() == Keyboard_KEY_DEL)
    {
      Edit_DeleteCh() ;
      Edit_bSave = false ;
      Edit_DisplayLine() ;
    }
    else if(key.getCh() == Keyboard_BACKSPACE)
    {
      Edit_BakSpace() ;
      Edit_bSave = false ;
      Edit_DisplayLine() ;
    }
    else if(key.getCh() == Keyboard_KEY_PG_UP)
    {
      Edit_PageUp() ;
    }
    else if(key.getCh() == Keyboard_KEY_PG_DOWN)
    {
      Edit_PageDown() ;
    }
    else
    {
      if(toascii(key.getCh()) > 30 || key.getCh() == Keyboard_ENTER || key.getCh() == '\t')
      {
        auto ch = key.getCh();
        if(ch == Keyboard_ENTER)
          ch = '\n' ;

        Edit_InsertCh(ch) ;
        Edit_bSave = false ;
        Edit_DisplayLine() ;
      }
    }

    if (EView_getCurrentPopupDialogType() == PopupDialogType::NA_Dialog
    && EView_getCurrentPopupMenuType() == PopupMenuType::NA_Menu) {
      MTerm_GetWinYX(Edit_pEditWin, &y, &Edit_col) ;
      EView_DisplayStatus() ;
      MTerm_WinRefresh(Edit_pEditWin) ;
    }
  }
};

int main(int argc, char** argv)
{
  auto consoleCanvas = MTerm_InitScr() ;

	Edit_Init() ;

 	EView_DrawInitBack() ;
	EView_DrawMenuBar() ;
	EView_DrawStatusBar() ;

	MTerm_WinMove(Edit_pEditWin, Edit_yMin, Edit_xMin) ;
	EView_Refresh() ;

	upanui::GraphicsContext::Instance().setFocus(*consoleCanvas);
	EditorKeyboardEventHandler keyboardEventHandler;
	consoleCanvas->addKeyboardEventHandler(keyboardEventHandler);

	int y;

	MTerm_GetWinYX(Edit_pEditWin, &y, &Edit_col) ;
	EView_DisplayStatus() ;
	MTerm_WinRefresh(Edit_pEditWin) ;

	upanui::GraphicsContext::Instance().eventManager().startEventLoop();

	EView_Release() ;

	return 0;
}

