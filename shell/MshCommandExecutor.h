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
#include <option.h>
#include <vector.h>
#include <set.h>

class MshCommand;

class MshCommandExecutor {
public:
  MshCommandExecutor(const upan::string& cmdLine);

  void execute();

  bool isOptPresent(const upan::string& opt) const {
    return _options.exists(opt);
  }

  const upan::vector<upan::string>& params() const {
    return _params;
  }

  void addCmdToken(const upan::string& token) {
    _cmdLineTokens.push_back(token);
  }

private:
  void parse(const upan::string& cmdLine);
  void expandCmdLine();
  void executeInternalCommand(MshCommand& cmd);
  bool executeProcess();

private:
  upan::string _cmdName;
  upan::option<MshCommand*> _cmd;
  upan::vector<upan::string> _cmdLineTokens;
  upan::vector<upan::string> _params;
  upan::set<upan::string> _options;
};
