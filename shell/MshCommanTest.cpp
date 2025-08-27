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

#include <MshCommands.h>
#include <Msh.h>
#include <MshCommandExecutor.h>
#include <file_stream.h>
#include <math.h>
#include <sys/socket.h>
#include <unet.h>
#include <signal.h>
#include <syslog.h>
#include <setjmp.h>

//test functions
static void recurse(int count) {
  char buffer[1024];
  if (count > 0) {
    sprintf(buffer, "\n Recurse count: %d", count);
    Msh::Instance().printf("%s", buffer);
    recurse(count - 1);
  }
}

class Num {
public:
  Num(int i) : _i(i) {}
  void Print() {
    Msh::Instance().printf("%d ", _i);
  }
  int getI() {
    return _i;
  }
private:
  int _i;
};

static void thread(void* param) {
  Num* n = static_cast<Num*>(param);
  upan::vector<Num*> nums;
  Msh::Instance().printf("\n Seed: %d", n->getI());
  for(int j = 0; j < n->getI(); ++j) {
    for (int i = 0; i < 1000; ++i) {
      nums.push_back(new Num(n->getI() + i));
    }
    int sum = 0;
    for (int i = 0; i < nums.size(); ++i) {
      sum += nums[i]->getI();
    }
    Msh::Instance().printf("\n Result: %d", sum);
    Msh::Instance().printf("\n");
    for (int i = 0; i < nums.size(); ++i) {
      delete nums[i];
    }
    nums.clear();
  }
  delete n;
}

void lock_thread(void* param) {
  btime();
}

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
  Msh::Instance().printf("Thread Completed: %d\n", getpid());
}

void print_output(const upan::string& msg) {
  upan::mutex_guard g(getM());
  Msh::Instance().printf("\n Thread : %d -> %s", getpid(), msg.c_str());
}

void reader_thread(void* stime) {
  upan::file_stream rstream("/test.log", O_RDONLY);
  rstream.seek_end(0);

  upan::string line;
  for(int i = 0; i < 2000;) {
    if (rstream.read_line(line)) {
      print_output(line);
      ++i;
    }
    //sleep(1);
  }
}

void writer_thread(void* stime) {
  upan::file_stream wstream("/test.log", O_RDWR | O_APPEND);
  char buf[128];
  for(int i = 0; i < 1000; ++i) {
    sprintf(buf, "Write line %d from Thread %d\n", btime(), getpid());
    wstream.write(buf, strlen(buf));
    //sleep(1);
  }
}

thread_local int _tls_data1 = 100;
thread_local uint8_t _tls_data2 = 200;
thread_local int _tls_global1;
thread_local uint64_t _tls_global2;
extern thread_local int _lib_data1_thread_local;
extern thread_local int _lib_global1_thread_local;

void tls_thread(void*) {
  auto pid = getpid();

  _tls_data1 += btime();
  _tls_data2 += pid;
  _tls_global2 += btime();
  _lib_data1_thread_local += getpid();
  _lib_global1_thread_local += 1;

  Msh::Instance().printf("\n %d -> data1: %d", pid, _tls_data1);
  Msh::Instance().printf("\n %d -> data2: %u", pid, _tls_data2);
  Msh::Instance().printf("\n %d -> global1: %d", pid, _tls_global1);
  Msh::Instance().printf("\n %d -> global2: %lu", pid, _tls_global2);
  Msh::Instance().printf("\n %d -> lib-data1: %d", pid, _lib_data1_thread_local);
  Msh::Instance().printf("\n %d -> lib-global1: %d", pid, _lib_global1_thread_local);
  access_thread_local_test();
  Msh::Instance().printf("\n %d -> (3)lib-data1: %d", pid, _lib_data1_thread_local);
  Msh::Instance().printf("\n %d -> (3)lib-global1: %d", pid, _lib_global1_thread_local);
  tls_xlib_test();
  Msh::Instance().printf("\n %d -> (X)lib-data1: %d", pid, _lib_data1_thread_local);
  Msh::Instance().printf("\n %d -> (X)lib-global1: %d", pid, _lib_global1_thread_local);
}

#define TEST_TCP_SERVER_PORT 8080
#define TEST_TCP_SERVER_BUFFER_SIZE 1024

static void test_tcp_server() {
  struct sockaddr_in address;
  char buffer[TEST_TCP_SERVER_BUFFER_SIZE] = {0};

  auto sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0) {
    throw upan::exception(XLOC, "socket() failed");
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
  address.sin_port = htons(TEST_TCP_SERVER_PORT);

  if (bind(sd, (struct sockaddr*) &address, sizeof(address)) < 0) {
    close(sd);
    throw upan::exception(XLOC, "bind() failed");
  }

  if (listen(sd, 1) < 0) {
    close(sd);
    throw upan::exception(XLOC, "listen() failed");
  }

  Msh::Instance().printf("\nServer is listening on port %d...", TEST_TCP_SERVER_PORT);

  // Accept a connection
  socklen_t addrlen;
  auto new_sd = accept(sd, (struct sockaddr*) &address, (socklen_t*) &addrlen);
  if (new_sd < 0) {
    close(sd);
    throw upan::exception(XLOC, "accept() failed");
  }

  int n = recv(new_sd, buffer, TEST_TCP_SERVER_BUFFER_SIZE - 1, 0);
  if (n >= 0) {
    buffer[n] = '\0'; // Null-terminate
    Msh::Instance().printf("\nReceived: %s", buffer);
  }

  strcpy(buffer, "Hello from Upanix server!");
  ssize_t bytes_sent = send(new_sd, buffer, strlen(buffer), 0);
  if (bytes_sent < 0) {
    throw upan::exception(XLOC, "send() failed");
  } else {
    Msh::Instance().printf("\nSent %d bytes: %s", bytes_sent, buffer);
  }

  close(new_sd);
  close(sd);
}

static void test_tcp_client() {
  const upan::string& host = "192.168.50.208";
  const int server_port = 12345;
  char message[1024] = "Hello, TCP Server!";

  int sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0) {
    throw upan::exception(XLOC, "socket() failed");
  }

  struct timeval timeout {};
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
  if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
    close(sd);
    throw upan::exception(XLOC, "failed to set socket option: SO_RCVTIMEO");
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  server_addr.sin_addr.s_addr = upan::net::inet_strton(host.c_str());

  if (connect(sd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    close(sd);
    throw upan::exception(XLOC, "connect() failed");
  }

  Msh::Instance().printf("\nConnected to %s:%d", host.c_str(), server_port);

  ssize_t bytes_sent = send(sd, message, strlen(message), 0);
  if (bytes_sent < 0) {
    throw upan::exception(XLOC, "send() failed");
  } else {
    Msh::Instance().printf("\nSent %d bytes: %s", bytes_sent, message);
  }

  // 5. Receive response
  ssize_t bytes_received = recv(sd, message, 1024 - 1, 0);
  if (bytes_received > 0) {
    message[bytes_received] = '\0';
    Msh::Instance().printf("\nReceived from server: %s", message);
  }

  close(sd);
}

void sig_handler(int signum) {
  printf("\nCaught signal: %d", signum);
}

void sig_action(int signum, siginfo_t* info, void* context) {
  printf("\nCaught signal: %d, SigInfo val: %d", signum, info->si_value.sival_int);
}

static sigjmp_buf segv_jmp_buf;

void sig_segv_avoid(int signum, siginfo_t* info, void* context) {
  printf("\nSegmentation fault @ %llx\n", info->si_value.sival_ptr);
  siglongjmp(segv_jmp_buf, 1);
}

void MshCommandTest::execute(const MshCommandExecutor& cmdExec) {
  if (cmdExec.params().size()) {
    const upan::string& type = cmdExec.params()[0];
    if (type == "sig_segv") {
      struct sigaction sa;
      memset(&sa, 0, sizeof(struct sigaction));
      sa.sa_flags = SA_SIGINFO;
      sa.sa_sigaction = sig_segv_avoid;
      sigaction(SIGSEGV, &sa, nullptr);
      if (sigsetjmp(segv_jmp_buf, 1) == 0) {
        int* p = (int*)0x123;
        *p = 10;
      } else {
        Msh::Instance().printf("\nGracefully recovered from Segmentation fault!");
      }
    } else if (type == "sig_handler") {
      int signal = atoi(cmdExec.params()[1].c_str());
      struct sigaction sa;
      memset(&sa, 0, sizeof(struct sigaction));
      sa.sa_handler = sig_handler;
      if (sigaction(signal, &sa, nullptr)) {
        Msh::Instance().printf("\nFailed to set signal handler");
      } else {
        Msh::Instance().printf("\nRegistered set signal handler");
      }
    } else if (type == "sig_action") {
      int signal = atoi(cmdExec.params()[1].c_str());
      struct sigaction sa;
      memset(&sa, 0, sizeof(struct sigaction));
      sa.sa_flags = SA_SIGINFO;
      sa.sa_sigaction = sig_action;

      struct sigaction old_sa;
      if (sigaction(signal, &sa, &old_sa)) {
        Msh::Instance().printf("\nFailed to set signal action handler");
      } else {
        Msh::Instance().printf("\nRegistered set signal action handler - old_sa: %d", old_sa.sa_flags);
      }
    } else if (type == "tcp-server") {
      test_tcp_server();
    } else if (type == "tcp-client") {
      test_tcp_client();
    } else if (type == "recurse") {
      int count = atoi(cmdExec.params()[1].c_str());
      Msh::Instance().printf("\n Stack Limit Test - Recurse %d times", count);
      recurse(count);
    } else if (type == "thread") {
      const char* seed = cmdExec.params().size() > 1 ? cmdExec.params()[1].c_str() : "0";
      Num* n = new Num(atoi(seed));
      int threadID = exect(thread, (void*)n);
      waitpid(threadID);
      Msh::Instance().printf("\n Thread ID: %d", threadID);

      upan::vector<int> threads;
      threads.push_back(exect(thread_loop, (void*)1));
      threads.push_back(exect(thread_loop, (void*)2));
      threads.push_back(exect(thread_loop, (void*)3));
      threads.push_back(exect(thread_loop, (void*)4));
      for(auto id : threads) {
        waitpid(id);
      }
    } else if (type == "mem") {
      uint32_t s = btime();
      upan::map<int, int> m;
      for(int i = 0; i < 1000000; ++i) {
        m.insert(upan::map<int, int>::value_type(i, i * 10));
      }
      uint32_t e = btime();;
      Msh::Instance().printf("\n Map size: %d, Time Taken: %u", m.size(), e - s);
    } else if (type == "lock") {
      int threadID = exect(lock_thread, nullptr);
      btime();
    } else if (type == "fs") {
      create("/test.log", ATTR_FILE_DEFAULT);

      upan::vector<int> threads;
      threads.push_back(exect(reader_thread, (void*)1));
      threads.push_back(exect(reader_thread, (void*)2));
      threads.push_back(exect(writer_thread, (void*)1));
      threads.push_back(exect(writer_thread, (void*)2));

      for(auto& id : threads) {
        waitpid(id);
      }
    } else if (type == "tls") {
      upan::vector<int> threads;
      threads.push_back(exect(tls_thread, nullptr));
      threads.push_back(exect(tls_thread, nullptr));
      threads.push_back(exect(tls_thread, nullptr));
      threads.push_back(exect(tls_thread, nullptr));

      for(auto& id : threads) {
        waitpid(id);
      }

      Msh::Instance().printf("\n TLS data1: %d", _tls_data1);
      Msh::Instance().printf("\n TLS data2 %u", _tls_data2);
      Msh::Instance().printf("\n TLS global1: %d", _tls_global1);
      Msh::Instance().printf("\n TLS global2: %lu", _tls_global2);
      Msh::Instance().printf("\n TLS lib-data1: %d", _lib_data1_thread_local);
      Msh::Instance().printf("\n TLS lib-global1: %d", _lib_global1_thread_local);
      access_thread_local_test();
      Msh::Instance().printf("\n TLS (3)lib-data1: %d", _lib_data1_thread_local);
      Msh::Instance().printf("\n TLS (3)lib-global1: %d", _lib_global1_thread_local);
      tls_xlib_test();
      Msh::Instance().printf("\n TLS (X)lib-data1: %d", _lib_data1_thread_local);
      Msh::Instance().printf("\n TLS (X)lib-global1: %d", _lib_global1_thread_local);
    } else if (type == "net") {
      Msh::Instance().printf("\n BROADCAST MAC %02x:%02x:%02x:%02x:%02x:%02x",
                             INADDR_MAC_BROADCAST[0],
                             INADDR_MAC_BROADCAST[1],
                             INADDR_MAC_BROADCAST[2],
                             INADDR_MAC_BROADCAST[3],
                             INADDR_MAC_BROADCAST[4],
                             INADDR_MAC_BROADCAST[5]);
      test_initialized_globals();
    }
  }
}