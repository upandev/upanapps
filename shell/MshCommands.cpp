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

# include <mosstd.h>
# include <fs.h>
# include <drive.h>
# include <error.h>
# include <MshCommands.h>
# include <MshCommandExecutor.h>
# include <Msh.h>

void MshCommand::printUsage() {
  Msh::Instance().printf("Usage: %s %s\n", _name.c_str(), _usage.c_str());
}

void MshCommandEcho::execute(const MshCommandExecutor& cmdExec) {
  if (!cmdExec.params().empty()) {
    Msh::Instance().printf("%s\n", cmdExec.params()[0].c_str());
  }
}

void MshCommandClearScreen::execute(const MshCommandExecutor& cmdExec) {
  Msh::Instance().clearScreen();
}

void MshCommandExit::execute(const MshCommandExecutor& cmdExec) {
  exit(0);
}

void MshCommandCreateDir::execute(const MshCommandExecutor& cmdExec) {
  for(const auto& param : cmdExec.params()) {
    if(mkdir(param.c_str(), ATTR_DIR_DEFAULT | ATTR_TYPE_DIRECTORY) < 0) {
      printerr("%s : Directory Creation Failed", param.c_str()) ;
    }
  }
}

void MshCommandDeleteFile::execute(const MshCommandExecutor& cmdExec) {
  for(const auto& param : cmdExec.params()) {
    if(unlink(param.c_str()) < 0) {
      printerr("%s : Directory Deletion Failed", param.c_str()) ;
    }
  }
}

void MshCommandListDir::execute(const MshCommandExecutor& cmdExec) {
  int size;
  struct stat_ex* dirList;

  if (!cmdExec.params().empty()) {
    bool bDisplayParentDirName = cmdExec.params().size() > 1;

    for (const auto& param : cmdExec.params()) {
      if (get_dir_content(param.c_str(), &dirList, &size) < 0) {
        Msh::Instance().printf("%s : No Such Directory/File", param.c_str());
      } else {
        if (bDisplayParentDirName)
          Msh::Instance().printf("\n\n %s :", param.c_str());
        showDirContent(cmdExec, dirList, size);
        free(dirList);
      }
    }
  } else {
    if (get_dir_content("", &dirList, &size) < 0) {
      Msh::Instance().printf("\nFailed to Get Directory Content");
    } else {
      showDirContent(cmdExec, dirList, size);
      free(dirList);
    }
  }
}

void MshCommandListDir::showDirContent(const MshCommandExecutor& cmdExec, const struct stat_ex* dirList, int size) {
  if(cmdExec.isOptPresent("-l")) {
    char rwx[12] ;
    // Attr, User, Size, Create Time, Modify Time, Access Time, Name
    for(int i = 0; i < size; i++) {
      format_dir_attr(dirList[i]._stat.st_mode, rwx) ;

      struct tm t;
      char createdTime[20];
      localtime(&t, &dirList[i]._stat.st_ctime.tv_sec);
      sprintf(createdTime, "%02d/%02d/%d %02d:%02d:%02d", t.tm_mday, t.tm_mon, t.tm_year, t.tm_hour, t.tm_min, t.tm_sec);

      char modifiedTime[20];
      localtime(&t, &dirList[i]._stat.st_mtime.tv_sec);
      sprintf(modifiedTime, "%02d/%02d/%d %02d:%02d:%02d", t.tm_mday, t.tm_mon, t.tm_year, t.tm_hour, t.tm_min, t.tm_sec);

      Msh::Instance().printf("\n%-15s%-15d%-24s%-24s%s", rwx, dirList[i]._stat.st_size, createdTime, modifiedTime, dirList[i]._name) ;
    }
  } else {
    for(int i = 0; i < size; i++)
    {
      if(!(i % 3) && i != 0) {
        Msh::Instance().printf("\n");
      }
      Msh::Instance().printf("%-20s", dirList[i]._name) ;
    }
  }
}

void MshCommandChangeDir::execute(const MshCommandExecutor& cmdExec) {
  if (!cmdExec.params().empty()) {
    if(chdir(cmdExec.params()[0].c_str()) < 0)
      printerr("%s : No Such Directory", cmdExec.params()[0].c_str());
  }
}

void MshCommandPresentWorkingDir::execute(const MshCommandExecutor& cmdExec) {
  Msh::Instance().printf("%s", getenv("PWD")) ;
}

void MshCommandCopyFile::execute(const MshCommandExecutor& cmdExec) {
  if(cmdExec.params().size() < 2)
    return ;

  const auto& szSrcFileName = cmdExec.params()[0];
  const auto& szDestFileName = cmdExec.params()[1];

  if(szSrcFileName == szDestFileName)
  {
    printerr("Src and Dest files must be different") ;
    return ;
  }

  int fdSrc ;
  if((fdSrc = open(szSrcFileName.c_str(), O_RDONLY)) < 0)
  {
    printerr("Failed to open file %s", szSrcFileName.c_str()) ;
    return ;
  }

  if(create(szDestFileName.c_str(), ATTR_FILE_DEFAULT) < 0)
  {
    printerr("Failed to create file %s", szDestFileName.c_str()) ;
    close(fdSrc) ;
    return ;
  }

  int fdDest ;
  if((fdDest = open(szDestFileName.c_str(), O_RDWR)) < 0)
  {
    printerr("Failed to open file %s", szDestFileName.c_str()) ;
    close(fdSrc) ;
    return ;
  }

  Msh::Instance().printf("\n Progress = ") ;
  //TODO:
  int cr = 0;// upanui::TerminalConsole::Instance().getCurPos();

  int progress = 0 ;

  struct stat fStat;
  if(fstat(fdSrc, &fStat) < 0)
  {
    Msh::Instance().printf("\n Failed to get stat on file: %s", szSrcFileName.c_str()) ;
    close(fdSrc) ;
    close(fdDest) ;
    return ;
  }

  unsigned fsize = fStat.st_size ;
  if(fsize == 0) fsize = 1 ;

  int n = 0 ;
  const int BUF_SIZE = 8192 ;
  char *buffer = (char*)malloc( BUF_SIZE ) ;

  if(!buffer)
  {
    printerr(" buffer malloc failed") ;
    close(fdSrc) ;
    close(fdDest) ;
    return;
  }

  while(true)
  {
    if((n = read(fdSrc, buffer, BUF_SIZE)) < 0)
    {
      printerr(" error reading file %s", szSrcFileName.c_str()) ;
      break ;
    }

    if(n < BUF_SIZE)
    {
      if(n == 0)
        break ;

      if(write(fdDest, buffer, n) < 0)
      {
        printerr(" error writing file %s", szDestFileName.c_str()) ;
        break ;
      }

      showprogress(cr, "", 100) ;
      break ;
    }

    if(write(fdDest, buffer, n) < 0)
    {
      printerr(" error writing file %s", szDestFileName.c_str()) ;
      break ;
    }

    progress++ ;
    showprogress(cr, "", (progress * BUF_SIZE * 100) / fsize) ;
  }

  free(buffer) ;
  close(fdSrc) ;
  close(fdDest) ;
}

void MshCommandShowFile::execute(const MshCommandExecutor& cmdExec) {
  if(cmdExec.params().size() < 1)
    return ;

  const auto& szFileName = cmdExec.params()[0];

  int fd ;
  if((fd = open(szFileName.c_str(), O_RDONLY)) < 0)
  {
    printerr("Failed to open file %s", szFileName.c_str()) ;
    return ;
  }

  int n = 0 ;
  char buffer[513] ;

  while(true)
  {
    if((n = read(fd, buffer, 512)) < 0)
    {
      printerr(" error reading file %s", szFileName.c_str()) ;
      break ;
    }

    if(n == 0)
      break ;

    buffer[n] = '\0' ;
    puts(buffer) ;

    if(n < 512)
      break ;
  }

  close(fd) ;
}

void MshCommandDate::execute(const MshCommandExecutor& cmdExec) {
  RTCDateTime rtcDateTime ;
  dtime(&rtcDateTime) ;

  Msh::Instance().printf("%d/%d/%d %d:%d:%d", rtcDateTime._dayOfMonth, rtcDateTime._month,
         rtcDateTime._century * 100 + rtcDateTime._year, rtcDateTime._hour, rtcDateTime._minute,
         rtcDateTime._second) ;
}

void MshCommandChangeDrive::execute(const MshCommandExecutor& cmdExec) {
  if(!cmdExec.params().empty()) {
    if(chdrive(cmdExec.params()[0].c_str()) < 0)
      printerr("Failed to switch drive to %s", cmdExec.params()[0].c_str()) ;
  }
}

void MshCommandShowDrives::execute(const MshCommandExecutor& cmdExec) {
  DriveStat* pDriveList ;
  int iListSize ;

  if(get_drive_list(&pDriveList, &iListSize) < 0)
  {
    printerr("Failed to get drive list") ;
    return ;
  }

  int i ;

  Msh::Instance().printf("%-4s | %-10s| %-10s| %-10s| %-10s| %-10s| %-8s\n",
         "SlNo", "DriveName", "Size(Phy)", "Total(fs)", "Used(fs)", "Free(fs)", "Mounted") ;
  for(i = 0; i < 4*1 + 10*6 + 3*5; i++)
    putc('-', stdout) ;

  for(i = 0; i < iListSize; i++)
  {
    //TODO: use float after coprocessor support
    //float fSize = (pDriveList[i].uiSizeInSectors * 512) / (1024 * 1024) ;
    //Msh::Instance().printf("\n[Drive %d] %s (%f MB)", i + 1, pDriveList[i].driveName, fSize) ;

    Msh::Instance().printf("\n%-4d   %-10s  %-10d  %-10d  %-10d  %-10d  %-8s", i + 1,
           pDriveList[i].driveName, (pDriveList[i].uiSizeInSectors * 512),
           pDriveList[i].ulTotalSize, pDriveList[i].ulUsedSize,
           (pDriveList[i].ulTotalSize - pDriveList[i].ulUsedSize),
           (pDriveList[i].bMounted) ? "Yes" : "No") ;
  }

  free(pDriveList) ;
}

void MshCommandMountDrive::execute(const MshCommandExecutor& cmdExec) {
  if(cmdExec.params().size() < 1)
    return ;

  const auto& szDriveName = cmdExec.params()[0];

  if(mount(szDriveName.c_str()) < 0)
    printerr("Failed to mount drive %s", szDriveName.c_str()) ;
}

void MshCommandUnMountDrive::execute(const MshCommandExecutor& cmdExec) {
  if(cmdExec.params().size() < 1)
    return ;

  const auto& szDriveName = cmdExec.params()[0];

  if(umount(szDriveName.c_str()) < 0)
    printerr("Failed to unmount drive %s", szDriveName.c_str()) ;
}

void MshCommandFormatDrive::execute(const MshCommandExecutor& cmdExec) {
  if(cmdExec.params().size() < 1)
    return ;

  const auto& szDriveName = cmdExec.params()[0];

  if(format(szDriveName.c_str()) < 0)
    printerr("Failed to format drive %s", szDriveName.c_str()) ;
}

void MshCommandShowCurrentDrive::execute(const MshCommandExecutor& cmdExec) {
  DriveStat driveStat;

  if(getcurdrive(&driveStat) < 0)
    printerr("Failed to get current drive") ;

  Msh::Instance().printf("%s (%d B) %s", driveStat.driveName, driveStat.uiSizeInSectors * 512,
         (driveStat.bMounted) ? "-- mounted" : "not mounted") ;
}

void MshCommandReboot::execute(const MshCommandExecutor& cmdExec) {
  reboot();
}

void MshCommandHelp::execute(const MshCommandExecutor& cmdExec) {
  int i = 0;
  for(auto cmd : Msh::Instance().getCommands()) {
    if(!(i % 3) && i != 0)
      putc('\n', stdout) ;
    Msh::Instance().printf("%-20s", cmd.second->name().c_str()) ;
    ++i;
  }
}

void MshCommandGetProcessDetails::execute(const MshCommandExecutor& cmdExec) {
  PS* pPS ;
  unsigned uiSize ;

  if(getpslist(&pPS, &uiSize) < 0)
  {
    Msh::Instance().printf("\n Error\n", ' ') ;
    return ;
  }

  Msh::Instance().printf("\n %-7s%-7s%-7s%-18s%-7s%-15s", "Pid", "PPid", "PGid", "Status", "Uid", "Proc Name") ;
  Msh::Instance().printf("\n ------------------------------------------------------------\n") ;
  unsigned i ;
  for(i = 0; i < uiSize; i++)
  {
    Msh::Instance().printf("\n %-7d%-7d%-7d%-18s%-7d%-15s", pPS[i].pid, pPS[i].iParentProcessID, pPS[i].iProcessGroupID, get_proc_status_desc(pPS[i].status),
           pPS[i].iUserID,	pPS[i].pname) ;
  }
  putc('\n', stdout) ;

  freepslist(pPS, uiSize) ;
}

void MshCommandExportEvnVar::execute(const MshCommandExecutor& cmdExec) {
  if(cmdExec.params().size() < 1)
    return ;

  const auto& szExp = cmdExec.params()[0];
  upan::vector<upan::string> tokens;
  szExp.tokenize("=", true, tokens);
  if (tokens.size() < 2) {
    printUsage();
    return ;
  }

  if(setenv(tokens[0].c_str(), tokens[1].c_str(), 1))
  {
    Msh::Instance().printf("\n Failed to set env variable\n") ;
    return ;
  }
}

void MshCommandListEnv::execute(const MshCommandExecutor& cmdExec) {
  const char** environ = getenviron();
  if (environ) {
    for(int i = 0; environ[i]; ++i) {
      Msh::Instance().printf("%s\n", environ[i]);
    }
  }
}