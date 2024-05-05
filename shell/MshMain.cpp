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

#include <Msh.h>
#include <mosstd.h>
#include <vector.h>
#include <file_stream.h>

upan::mutex& getM() {
  static upan::mutex m;
  return m;
}

static void thread_loop(void* x) {
  static int counter1 = 0;
  static int counter2 = 0;
  static int counter3 = 0;
  static int counter4 = 0;

  while(true) {
    upan::mutex_guard g(getM());
    if((uint64_t)x == 1) {
      ++counter1;
      if (counter1 == 1000000) break;
    } else if((uint64_t)x == 2) {
      ++counter2;
      if (counter2 == 1000000) break;
    } else if((uint64_t)x == 3) {
      ++counter3;
      if (counter3 == 1000000) break;
    } else if((uint64_t)x == 4) {
      ++counter4;
      if (counter4 == 1000000) break;
    }
  }
  printf("Thread Completed: %d\n", getpid());
}

void print_output(const upan::string& msg) {
  upan::mutex_guard g(getM());
  printf("\n Thread : %d -> %s", getpid(), msg.c_str());
}

void reader_thread(void* stime) {
  upan::file_stream rstream("/test.log", O_RDONLY);
  rstream.seek_end(0);

  upan::string line;
  for(int i = 0; i < 10;) {
    if (rstream.read_line(line)) {
      print_output(line);
      ++i;
    }
    sleep(1);
  }
}

void writer_thread(void* stime) {
  upan::file_stream wstream("/test.log", O_RDWR | O_APPEND);
  char buf[128];
  for(int i = 0; i < 10; ++i) {
    sprintf(buf, "Write line %d from Thread %d\n", btime(), getpid());
    wstream.write(buf, strlen(buf));
    sleep(1);
  }
}

int main(int argc, char** argv) {
  for(int i = 0; i < argc; ++i) {
    printf("\n%s", argv[i]);
  }
  printf("\n Process PID: %d\n", getpid());
  upan::vector<int> threads;
  threads.push_back(exect(thread_loop, (void*)1));
  threads.push_back(exect(thread_loop, (void*)2));
  threads.push_back(exect(thread_loop, (void*)3));
  threads.push_back(exect(thread_loop, (void*)4));
  for(auto& id : threads) {
    waitpid(id);
  }

  create("/test.log", ATTR_FILE_DEFAULT);

  threads.clear();
  threads.push_back(exect(reader_thread, (void*)1));
  threads.push_back(exect(reader_thread, (void*)2));
  threads.push_back(exect(writer_thread, (void*)1));
  threads.push_back(exect(writer_thread, (void*)2));

  for(auto& id : threads) {
    waitpid(id);
  }

  return 0;

  try {
    Msh::Instance().start();
  } catch(const upan::exception& e) {
    printf("\nError: %s", e.ErrorMsg().c_str());
  }
	return 0 ;
}

