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
#pragma once

#include <ustring.h>
#include <map.h>
#include <Terminal.h>

class MshCommand;

class Msh : public upanui::Terminal::CommandExecutor {
private:
  Msh();

public:
  static Msh& Instance() {
    static Msh msh;
    return msh;
  }

  void start();
  void stop(int exitCode);

  typedef upan::map<upan::string, MshCommand*> CmdMap;

  const CmdMap& getCommands() {
    return _commands;
  }

  void clearScreen() {
    _terminal->clear();
  }

protected:
  void execute(const upan::string& cmdLine) override;

private:
  void updatePrompt();

private:
  const upan::string PROMPT_BASE;
  upan::string _prompt;
  CmdMap _commands;
  upanui::Terminal* _terminal;
};