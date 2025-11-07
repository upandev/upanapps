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
# include <string.h>
# include <stdio.h>
# include <fs.h>
# include <Msh.h>
# include <MshCommandExecutor.h>
# include <MshCommands.h>
# include <GraphicsContext.h>
# include <UIObjectFactory.h>
# include <VerticalScroller.h>
# include <IconButton.h>

Msh::Msh() : PROMPT_BASE("msh:"), _terminal(nullptr) {
  auto add_cmd = [&](MshCommand* cmd) {
    auto r = _commands.insert(CmdMap::value_type(cmd->name(), cmd));
    if (!r.second) {
      throw upan::exception(XLOC, "duplicate command: %s", cmd->name().c_str());
    }
  };
  add_cmd(new MshCommandTest());
  add_cmd(new MshCommandEcho());
  add_cmd(new MshCommandClearScreen());
  add_cmd(new MshCommandExit());
  add_cmd(new MshCommandCreateDir());
  add_cmd(new MshCommandDeleteFile());
  add_cmd(new MshCommandListDir());
  add_cmd(new MshCommandChangeDir());
  add_cmd(new MshCommandPresentWorkingDir());
  add_cmd(new MshCommandCopyFile());
  add_cmd(new MshCommandShowFile());
  add_cmd(new MshCommandDate());
  add_cmd(new MshCommandChangeDrive());
  add_cmd(new MshCommandShowDrives());
  add_cmd(new MshCommandMountDrive());
  add_cmd(new MshCommandUnMountDrive());
  add_cmd(new MshCommandFormatDrive());
  add_cmd(new MshCommandShowCurrentDrive());
  add_cmd(new MshCommandReboot());
  add_cmd(new MshCommandHelp());
  add_cmd(new MshCommandGetProcessDetails());
  add_cmd(new MshCommandExportEvnVar());
  add_cmd(new MshCommandListEnv());

  updatePrompt();
}

void Msh::updatePrompt() {
  _prompt = PROMPT_BASE + getenv("PWD");
}

void Msh::execute(const upan::string& cmdLine) {
  MshCommandExecutor cmdExec(cmdLine);
  try {
    cmdExec.execute();
  } catch(const upan::exception& e) {
    printf("\n%s", e.ErrorMsg().c_str());
  }
  updatePrompt();
  if (_terminal) {
    _terminal->setPrompt(_prompt);
  }
}

class PassThroughMouseHandler : public upanui::MouseEventHandler {
public:
  void onEvent(upanui::UIObject& uiObject, const upanui::MouseEvent& event) override {
    const upanui::MouseData& data = event.getData();
    if (data.leftButtonState() == upanui::MouseData::HOLD) {
      uiObject.parent().xy(uiObject.parent().x() + data.deltaX(), uiObject.parent().y() + data.deltaY());
    }
  }
};

class CloseButtonMouseHandler : public upanui::MouseEventHandler {
public:
  void onEvent(upanui::UIObject& uiObject, const upanui::MouseEvent& event) override {
    const upanui::MouseData& data = event.getData();
    if (data.leftButtonState() == upanui::MouseData::RELEASED) {
      exit(0);
    }
  }
};

void Msh::start() {
  upanui::GraphicsContext::Init();
  auto& gc = upanui::GraphicsContext::Instance();

  const int appWidth = 600;
  const int mainHeight = 500;
  const int menuBarHeight = 30;

  auto& uiRoot = gc.initUIRoot(100, 100, appWidth, mainHeight + menuBarHeight, true);
  uiRoot.setResizable(true, true);

  auto& uiMenuBar = upanui::UIObjectFactory::createRectangleCanvas(uiRoot, 0, 0, appWidth, menuBarHeight, upanui::HorizontalPlacementType::STRETCHED, upanui::VerticalPlacementType::TOP_FIXED);
  uiMenuBar.backgroundColor(0xA59E9D);

  auto& closeBt = upanui::UIObjectFactory::createIconButton(uiMenuBar, upanui::PngImageResource::CLOSE, appWidth - menuBarHeight, 0, menuBarHeight, menuBarHeight, upanui::HorizontalPlacementType::RIGHT_FIXED, upanui::VerticalPlacementType::TOP_FIXED);

  const int scrollBarWidth = 20;
  auto& vScroller = upanui::UIObjectFactory::createVerticalScroller(uiRoot, 0, menuBarHeight, appWidth, mainHeight, scrollBarWidth, upanui::HorizontalPlacementType::STRETCHED, upanui::VerticalPlacementType::STRETCHED);

  _terminal = &upanui::UIObjectFactory::createTerminal(vScroller, 0, 0, appWidth - scrollBarWidth, mainHeight, _prompt, *this, upanui::HorizontalPlacementType::STRETCHED, upanui::VerticalPlacementType::STRETCHED);

  PassThroughMouseHandler passThroughMouseHandler;
  uiMenuBar.registerMouseEventHandler(passThroughMouseHandler);

  CloseButtonMouseHandler closeButtonMouseHandler;
  closeBt.registerMouseEventHandler(closeButtonMouseHandler);

  gc.eventManager().startTerminalEventLoop();

  exit(0);
}
