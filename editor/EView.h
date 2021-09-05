/*
 *  Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
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
#ifndef _EVIEW_H_
#define _EVIEW_H_

enum PopupMenuType {
  File, Edit, Help, NA_Menu
};

enum PopupDialogType {
  HelpDialog, WarningDialog, OKCancelWarningDialog, NA_Dialog
};

void EView_DrawInitBack() ;
void EView_DrawMenuBar() ;
void EView_DrawStatusBar() ;
void EView_DisplayStatus() ;
void EView_Release() ;
void EView_Refresh() ;
void EView_OpenFileMenu() ;
void EView_OnFileMenu(int key) ;
void EView_OpenEditMenu() ;
void EView_OnEditMenu(int key) ;
void EView_OpenHelpMenu() ;
void EView_OnHelpMenu(int key) ;

namespace upanui {
  class KeyboardData;
}
void EView_HandleHelpDialogActions(const upanui::KeyboardData& key);
void EView_HandleMenuActions(const upanui::KeyboardData& ch);

PopupMenuType EView_getCurrentPopupMenuType();
PopupDialogType EView_getCurrentPopupDialogType();

#endif
