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

#include <ctype.h>
#include <ustring.h>
#include <fs.h>
#include <dirent.h>

class MshCommandExecutor;

class MshCommand {
public:
  MshCommand(const upan::string& name, const upan::string& usage, int minParamCount, int maxParamCount) :
   _name(name), _usage(usage), _minParamCount(minParamCount), _maxParamCount(maxParamCount) {}
  virtual void execute(const MshCommandExecutor&) = 0;

  const upan::string& name() {
    return _name;
  }

  const upan::string& usage() {
    return _usage;
  }

  int minParamCount() {
    return _minParamCount;
  }

  int maxParamCount() {
    return _maxParamCount;
  }

  void printUsage();

private:
  const upan::string _name;
  const upan::string _usage;
  const int _minParamCount;
  const int _maxParamCount;
};

class MshCommandTest : public MshCommand {
public:
  MshCommandTest() : MshCommand("test", "", 0, 20) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandEcho : public MshCommand {
public:
  MshCommandEcho() : MshCommand("echo", "[\"]<string to be displayed>[\"] - Use \" if msg conatains whitespace", 1, 1) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandClearScreen : public MshCommand {
public:
  MshCommandClearScreen() : MshCommand("clear", "", 0, 0) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandExit : public MshCommand {
public:
  MshCommandExit() : MshCommand("exit", "", 0, 0) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandCreateDir : public MshCommand {
public:
  MshCommandCreateDir() : MshCommand("mkdir", "<new_dir_name> [<new_dir_name> ...]", 1, 10) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandDeleteFile : public MshCommand {
public:
  MshCommandDeleteFile() : MshCommand("rm", "<file_name> [<file_name> ...]", 1, 10) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandListDir : public MshCommand {
public:
  MshCommandListDir() : MshCommand("ls", "<file_name> [<file_name> ...]", 0, 20) {
  }
  virtual void execute(const MshCommandExecutor&) override;
private:
  void showDirContent(const MshCommandExecutor& cmdExec, DIR* dirp);
};

class MshCommandChangeDir : public MshCommand {
public:
  MshCommandChangeDir() : MshCommand("cd", "<dir_name>", 1, 1) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandPresentWorkingDir : public MshCommand {
public:
  MshCommandPresentWorkingDir() : MshCommand("pwd", "", 0, 0) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandCopyFile : public MshCommand {
public:
  MshCommandCopyFile() : MshCommand("cp", "<src_file_name> <dest_file_name>", 2, 2) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandShowFile : public MshCommand {
public:
  MshCommandShowFile() : MshCommand("see", "<text_file_name>", 1, 1) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandDate : public MshCommand {
public:
  MshCommandDate() : MshCommand("date", "", 0, 0) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandChangeDrive : public MshCommand {
public:
  MshCommandChangeDrive() : MshCommand("chd", "<Drive name> - Use the one listed by \"shd\" command", 1, 1) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandShowDrives : public MshCommand {
public:
  MshCommandShowDrives() : MshCommand("shd", "", 0, 0) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandMountDrive : public MshCommand {
public:
  MshCommandMountDrive() : MshCommand("mount", "<Drive name> - Use the one listed by \"shd\" command", 1, 1) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandUnMountDrive : public MshCommand {
public:
  MshCommandUnMountDrive() : MshCommand("umount", "<Drive name> - Use the one listed by \"shd\" command", 1, 1) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandFormatDrive : public MshCommand {
public:
  MshCommandFormatDrive() : MshCommand("format", "<Drive name> - Use the one listed by \"shd\" command", 1, 1) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandShowCurrentDrive : public MshCommand {
public:
  MshCommandShowCurrentDrive() : MshCommand("cud", "", 0, 0) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandReboot : public MshCommand {
public:
  MshCommandReboot() : MshCommand("reboot", "", 0, 0) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandHelp : public MshCommand {
public:
  MshCommandHelp() : MshCommand("help", "", 0, 0) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandGetProcessDetails : public MshCommand {
public:
  MshCommandGetProcessDetails() : MshCommand("ps", "", 0, 0) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandExportEvnVar : public MshCommand {
public:
  MshCommandExportEvnVar() : MshCommand("export", "<var>=<val>", 1, 1) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

class MshCommandListEnv : public MshCommand {
public:
  MshCommandListEnv() : MshCommand("env", "env", 0, 0) {
  }
  virtual void execute(const MshCommandExecutor&) override;
};

