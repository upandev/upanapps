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
#include <syslog.h>
#include <signal.h>
#include <pthread.h>
#include <setjmp.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <ussl.h>
#include <sys/wait.h>

//test functions
static void recurse(int count) {
  char buffer[1024];
  if (count > 0) {
    sprintf(buffer, "\n Recurse count: %d", count);
    printf("%s", buffer);
    recurse(count - 1);
  }
}

class Num {
public:
  Num(int i) : _i(i) {}
  void Print() {
    printf("%d ", _i);
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
  printf("\n Seed: %d", n->getI());
  for(int j = 0; j < n->getI(); ++j) {
    for (int i = 0; i < 1000; ++i) {
      nums.push_back(new Num(n->getI() + i));
    }
    int sum = 0;
    for (int i = 0; i < nums.size(); ++i) {
      sum += nums[i]->getI();
    }
    printf("\n Result: %d", sum);
    printf("\n");
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

static int counter1 = 0;
static int counter2 = 0;
static int counter3 = 0;
static int counter4 = 0;

static void thread_loop(void* x) {
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

pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

static void pthread_key_value_destructor(void* v) {
  auto val = static_cast<uint64_t*>(v);
  printf("pthread_key_value_destructor: %lu\n", *val);;
  delete val;
}

static void* pthread_loop(void* x) {
  int key_count = 0;
  int key_delete_count = 0;
  int counter = 0;
  pthread_key_t key;

  while(true) {
    ++counter;
    if (key_count < 20 && counter % 1000 == 0) {
      pthread_key_create(&key, pthread_key_value_destructor);
      auto val = new uint64_t(counter);
      pthread_setspecific(key, val);
      ++key_count;
    }

    pthread_mutex_lock(&pmutex);

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

    pthread_mutex_unlock(&pmutex);

    if (key_delete_count < 5 && counter % 2000 == 0) {
      pthread_key_delete(key);
      ++key_delete_count;
    }
  }
  for(int i = 0; i < 20; ++i) {
    uint64_t* val = static_cast<uint64_t*>(pthread_getspecific(i));
    if (val == nullptr) {
      printf("%d: pthread key %d -> null\n", getpid(), i);
    } else {
      printf("%d: pthread key %d -> %lu\n", getpid(), i, *val);
    }
  }
  printf("(p)Thread Completed: %d\n", pthread_self());

  return (void*)((uint64_t)getpid());
}

void reader_thread(void* stime) {
  upan::file_stream rstream("/test.log", O_RDONLY, 0);
  rstream.seek_end(0);

  upan::string line;
  for(int i = 0; i < 2000;) {
    if (rstream.read_line(line)) {
      if ( (i % 100) == 0) {
        printf("\n Read 100 lines -> Thread : %d -> %s", getpid(), line.c_str());
      }
      ++i;
    }
    //sleep(1);
  }
}

void writer_thread(void* stime) {
  upan::file_stream wstream("/test.log", O_RDWR | O_APPEND, ATTR_FILE_DEFAULT);
  char buf[128];
  for(int i = 0; i < 1000; ++i) {
    sprintf(buf, "Write line %ld from Thread %d\n", btime(), getpid());
    wstream.write(buf, strlen(buf));
    if ((i % 100) == 0) {
      printf("\n Wrote %d lines so far (%d)", i, getpid());
    }
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

  printf("\n %d -> data1: %d", pid, _tls_data1);
  printf("\n %d -> data2: %u", pid, _tls_data2);
  printf("\n %d -> global1: %d", pid, _tls_global1);
  printf("\n %d -> global2: %lu", pid, _tls_global2);
  printf("\n %d -> lib-data1: %d", pid, _lib_data1_thread_local);
  printf("\n %d -> lib-global1: %d", pid, _lib_global1_thread_local);
  access_thread_local_test();
  printf("\n %d -> (3)lib-data1: %d", pid, _lib_data1_thread_local);
  printf("\n %d -> (3)lib-global1: %d", pid, _lib_global1_thread_local);
  tls_xlib_test();
  printf("\n %d -> (X)lib-data1: %d", pid, _lib_data1_thread_local);
  printf("\n %d -> (X)lib-global1: %d", pid, _lib_global1_thread_local);
}

#define TEST_TCP_SERVER_PORT 8080
#define TEST_TCP_TLS_SERVER_PORT 8443
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

  printf("\nServer is listening on port %d...", TEST_TCP_SERVER_PORT);

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
    printf("\nReceived: %s", buffer);
  }

  strcpy(buffer, "Hello from Upanix server!");
  ssize_t bytes_sent = send(new_sd, buffer, strlen(buffer), 0);
  if (bytes_sent < 0) {
    throw upan::exception(XLOC, "send() failed");
  } else {
    printf("\nSent %d bytes: %s", bytes_sent, buffer);
  }

  close(new_sd);
  close(sd);
}

static void test_tcp_client(const upan::string& host) {
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

  printf("\nConnected to %s:%d", host.c_str(), server_port);

  ssize_t bytes_sent = send(sd, message, strlen(message), 0);
  if (bytes_sent < 0) {
    throw upan::exception(XLOC, "send() failed");
  } else {
    printf("\nSent %d bytes: %s", bytes_sent, message);
  }

  // 5. Receive response
  ssize_t bytes_received = recv(sd, message, 1024 - 1, 0);
  if (bytes_received > 0) {
    message[bytes_received] = '\0';
    printf("\nReceived from server: %s", message);
  }

  close(sd);
}

static void test_tcp_tls_server() {
  struct sockaddr_in address;

  auto sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0) {
    throw upan::exception(XLOC, "socket() failed");
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
  address.sin_port = htons(TEST_TCP_TLS_SERVER_PORT);

  if (bind(sd, (struct sockaddr*) &address, sizeof(address)) < 0) {
    close(sd);
    throw upan::exception(XLOC, "bind() failed");
  }

  if (listen(sd, 1) < 0) {
    close(sd);
    throw upan::exception(XLOC, "listen() failed");
  }

  printf("\nTLS Server is listening on port %d...", TEST_TCP_TLS_SERVER_PORT);

  // Accept a connection
  socklen_t addrlen;
  auto new_sd = accept(sd, (struct sockaddr*) &address, (socklen_t*) &addrlen);
  if (new_sd < 0) {
    close(sd);
    throw upan::exception(XLOC, "accept() failed");
  }

  SSL *ssl = SSL_new(upan::net::ssl_context::instance().getServerCtx());
  SSL_set_fd(ssl, new_sd);

  if (SSL_accept(ssl) <= 0) {
    ERR_print_errors_fp(stderr);
  } else {
    char buffer[1024] = {0};
    SSL_read(ssl, buffer, sizeof(buffer));
    printf("Client says: %s\n", buffer);

    const char msg[] = "Hello from Upanix TLS Server!";
    SSL_write(ssl, msg, strlen(msg) + 1);
  }

  SSL_shutdown(ssl);
  SSL_free(ssl);
  close(new_sd);
  close(sd);
}

static void test_tcp_tls_client(const upan::string& host) {
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
  server_addr.sin_port = htons(TEST_TCP_TLS_SERVER_PORT);
  server_addr.sin_addr.s_addr = upan::net::inet_strton(host.c_str());

  if (connect(sd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    close(sd);
    throw upan::exception(XLOC, "connect() failed");
  }

  printf("\nConnected to %s:%d", host.c_str(), TEST_TCP_TLS_SERVER_PORT);

  SSL *ssl = SSL_new(upan::net::ssl_context::instance().getClientCtx());
  SSL_set_fd(ssl, sd);

  if (SSL_connect(ssl) <= 0) {
    ERR_print_errors_fp(stderr);
  } else {
    char buffer[1024] = "Hello from Upanix TLS Client!";
    SSL_write(ssl, buffer, strlen(buffer) + 1);

    buffer[0] = '\0';
    SSL_read(ssl, buffer, sizeof(buffer));
    printf("\nTLS Server replied: %s\n", buffer);
  }

  SSL_shutdown(ssl);
  SSL_free(ssl);
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
    if (type == "sigalarm") {
      printf("\n setting alarm for 5 seconds");
      alarm(5);
    } else if (type == "sig_segv") {
      struct sigaction sa;
      memset(&sa, 0, sizeof(struct sigaction));
      sa.sa_flags = SA_SIGINFO;
      sa.sa_sigaction = sig_segv_avoid;
      sigaction(SIGSEGV, &sa, nullptr);
      if (sigsetjmp(segv_jmp_buf, 1) == 0) {
        int* p = (int*)0x123;
        *p = 10;
      } else {
        printf("\nGracefully recovered from Segmentation fault!");
      }
    } else if (type == "sig_handler") {
      int signal = atoi(cmdExec.params()[1].c_str());
      struct sigaction sa;
      memset(&sa, 0, sizeof(struct sigaction));
      sa.sa_handler = sig_handler;
      if (sigaction(signal, &sa, nullptr)) {
        printf("\nFailed to set signal handler");
      } else {
        printf("\nRegistered set signal handler");
      }
    } else if (type == "sig_action") {
      int signal = atoi(cmdExec.params()[1].c_str());
      struct sigaction sa;
      memset(&sa, 0, sizeof(struct sigaction));
      sa.sa_flags = SA_SIGINFO;
      sa.sa_sigaction = sig_action;

      struct sigaction old_sa;
      if (sigaction(signal, &sa, &old_sa)) {
        printf("\nFailed to set signal action handler");
      } else {
        printf("\nRegistered set signal action handler - old_sa: %d", old_sa.sa_flags);
      }
    } else if (type == "tcp-server") {
      test_tcp_server();
    } else if (type == "tcp-client") {
      test_tcp_client(cmdExec.params()[1]);
    } else if (type == "tcp-tls-server") {
      test_tcp_tls_server();
    } else if (type == "tcp-tls-client") {
      test_tcp_tls_client(cmdExec.params()[1]);
    } else if (type == "recurse") {
      int count = atoi(cmdExec.params()[1].c_str());
      printf("\n Stack Limit Test - Recurse %d times", count);
      recurse(count);
    } else if (type == "thread") {
      getM(); //to init the mutex;
      const char* seed = cmdExec.params().size() > 1 ? cmdExec.params()[1].c_str() : "0";
      Num* n = new Num(atoi(seed));
      int threadID = exect(thread, (void*)n, true);
      int exitStatus;
      waitpid(threadID, &exitStatus, 0);
      printf("\n Thread ID: %d", threadID);

      upan::vector<int> threads;
      counter1 = 0;
      counter2 = 0;
      counter3 = 0;
      counter4 = 0;

      threads.push_back(exect(thread_loop, (void*)1, true));
      threads.push_back(exect(thread_loop, (void*)2, true));
      threads.push_back(exect(thread_loop, (void*)3, true));
      threads.push_back(exect(thread_loop, (void*)4, true));
      for(auto id : threads) {
        int exitStatus;
        waitpid(id, &exitStatus, 0);
      }
    } else if (type == "pthreads") {
      pthread_mutex_init(&pmutex, nullptr);

      upan::vector<pthread_t> threads;
      threads.push_back(pthread_t());
      threads.push_back(pthread_t());
      threads.push_back(pthread_t());
      threads.push_back(pthread_t());

      counter1 = 0;
      counter2 = 0;
      counter3 = 0;
      counter4 = 0;

      pthread_create(&threads[0], nullptr, pthread_loop, (void*)1);
      pthread_create(&threads[1], nullptr, pthread_loop, (void*)2);
      pthread_create(&threads[2], nullptr, pthread_loop, (void*)3);
      pthread_create(&threads[3], nullptr, pthread_loop, (void*)4);

      for(auto id : threads) {
        void* ret;
        pthread_join(id, &ret);
        printf("\n (p)Thread Joined: %lu", (uint64_t)ret);
      }

      pthread_mutex_lock(&pmutex);
    } else if (type == "mem") {
      uint32_t s = btime();
      upan::map<int, int> m;
      for (int i = 0; i < 1000000; ++i) {
        m.insert(upan::map<int, int>::value_type(i, i * 10));
      }
      uint32_t e = btime();;
      printf("\n Map size: %d, Time Taken: %u", m.size(), e - s);
    } else if (type == "lock") {
      __attribute__((unused)) int threadID = exect(lock_thread, nullptr, true);
      btime();
    } else if (type == "fs") {
      create("/test.log", ATTR_FILE_DEFAULT);

      upan::vector<int> threads;
      threads.push_back(exect(reader_thread, (void*)1, true));
      threads.push_back(exect(reader_thread, (void*)2, true));
      threads.push_back(exect(writer_thread, (void*)1, true));
      threads.push_back(exect(writer_thread, (void*)2, true));

      for(auto& id : threads) {
        int exitStatus;
        waitpid(id, &exitStatus, 0);
      }
    } else if (type == "tls") {
      upan::vector<int> threads;
      threads.push_back(exect(tls_thread, nullptr, true));
      threads.push_back(exect(tls_thread, nullptr, true));
      threads.push_back(exect(tls_thread, nullptr, true));
      threads.push_back(exect(tls_thread, nullptr, true));

      for(auto& id : threads) {
        int exitStatus;
        waitpid(id, &exitStatus, 0);
      }

      printf("\n TLS data1: %d", _tls_data1);
      printf("\n TLS data2 %u", _tls_data2);
      printf("\n TLS global1: %d", _tls_global1);
      printf("\n TLS global2: %lu", _tls_global2);
      printf("\n TLS lib-data1: %d", _lib_data1_thread_local);
      printf("\n TLS lib-global1: %d", _lib_global1_thread_local);
      access_thread_local_test();
      printf("\n TLS (3)lib-data1: %d", _lib_data1_thread_local);
      printf("\n TLS (3)lib-global1: %d", _lib_global1_thread_local);
      tls_xlib_test();
      printf("\n TLS (X)lib-data1: %d", _lib_data1_thread_local);
      printf("\n TLS (X)lib-global1: %d", _lib_global1_thread_local);
    } else if (type == "net") {
      printf("\n BROADCAST MAC %02x:%02x:%02x:%02x:%02x:%02x",
                             INADDR_MAC_BROADCAST[0],
                             INADDR_MAC_BROADCAST[1],
                             INADDR_MAC_BROADCAST[2],
                             INADDR_MAC_BROADCAST[3],
                             INADDR_MAC_BROADCAST[4],
                             INADDR_MAC_BROADCAST[5]);
      test_initialized_globals();
    } else if (type == "fork") {
      printf("\n Forking");

      int fd_ptoc[2];

      if (pipe(fd_ptoc) < 0) {
        throw upan::exception(XLOC, "pipe() failed - ptoc");
      }

      int fd_ctop[2];
      if (pipe(fd_ctop) < 0) {
        throw upan::exception(XLOC, "pipe() failed - ctop");
      }

      char buf[512];

      int pid = fork();
      if (pid == 0) {
        close (fd_ptoc[1]);
        close (fd_ctop[0]);

        strcpy(buf, "Hello from child");
        write(fd_ctop[1], buf, strlen(buf));

        int n = read(fd_ptoc[0], buf, 512);
        if (n < 0) {
          throw upan::exception(XLOC, "read() failed");
        }
        buf[n] = '\0';

        printf("\n Child process - received: %s", buf);

        close(fd_ptoc[0]);
        close(fd_ctop[1]);
        exit(0);
      } else {
        close (fd_ptoc[0]);
        close (fd_ctop[1]);

        int n = read(fd_ctop[0], buf, 512);
        if (n < 0) {
          throw upan::exception(XLOC, "read() failed");
        }
        buf[n] = '\0';

        printf("\n Parent process - received: %s", buf);
        strcpy(buf, "Hello from parent");
        write(fd_ptoc[1], buf, strlen(buf));

        close (fd_ptoc[1]);
        close (fd_ctop[0]);

        printf("\n Parent process - waiting for child to exit");
        waitpid(pid, nullptr, 0);
      }
    } else if (type == "misc") {
      printf("\n RDTSC: %llu", rdtsc());
    }
  }
}