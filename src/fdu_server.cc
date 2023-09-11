#include <signal.h>
#include <cstddef>

extern "C" {
  int sigaction(int, const struct sigaction *, struct sigaction *);
};

extern int fdu_server_loop_end_condition;

static void sigint_action(int v)
{
  fdu_server_loop_end_condition = 1;
  return;
}

static inline
int setSIGINTAction(void)
{
  struct sigaction int_act;
  int_act.sa_handler = sigint_action;
  int_act.sa_sigaction = nullptr;
  int_act.sa_flags = 0;
  sigemptyset(&int_act.sa_mask);
  sigaddset(&int_act.sa_mask, SIGINT);

  return sigaction(SIGINT, &int_act, NULL);
}

//import "fdu_cs.h";
//import F_SERVER;
#include "fdu_cs.h"
#include "f_server.h"

//  thread will deatched.
static void fdu_server_worker(typename f_server::fupload_t upload)
{
  const request_header &rh = upload.readRequest();
  responding_header responding = upload.checkFile(rh);
  (void)upload.sendFile(responding, rh);
}

int fdu_server_loop_end_condition = 0;

int fdu_server(const std::string &listen_address, unsigned long port, unsigned int maximum_links = 3)
{
  //  typedef
  using worker = std::function<void(typename f_server::fupload_t)>;

  std::unique_ptr<f_server> local_server_ptr(nullptr);  //  f_server has throw because TCP/IP resource has throw
  try {
    local_server_ptr = new f_server(maximum_links);
  } catch (f_server::f_server_err &x) {
    cerr<<"Generate server object was failed."<<endl;
    return -1;
  }

  if (!local_server_ptr->setListenAddressIPv4(listen_address)) {
    cerr<<"IPv4 Address is wrong."<<endl;
    return -1;
  }
  local_server_ptr->setListenPort(port);
  if (local_server_ptr->init() != 0) {
    cerr<<"Server initialization was failed."<<endl;
    return -1;
  }

  //  if initialization is succeed,then we enter a loop wait client come.

  //  server have to handle SIGINT,that will let server stop.
  if (setSIGINTAction() < 0) {
    cerr<<"Install SIGINT action was failed."<<endl;
    return -1;
  }

  fdu_server_loop_end_condition(0);
  do {
    typename f_server::fupload_t upload(local_server_ptr->accept());  //  fupload(int) no throw
                                                                      //  but there will call to copy-constructor
                                                                      //  it only throw when copy-from has buffer allocated
    worker new_worker = fdu_server_worker;
    std::thread new_thread(new_worker, upload);
    new_thread.detach();
  } while (!fdu_server_loop_end_condition);

  return 0;
}

