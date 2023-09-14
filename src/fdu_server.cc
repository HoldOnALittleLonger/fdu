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
static void fdu_server_worker(typename f_server::fupload_t &&upload)
{
  request_header rh = upload.readRequest();
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
    local_server_ptr.reset(new f_server(maximum_links));
  } catch (f_server::f_server_err &x) {
    std::cerr<<"Generate server object was failed."<<std::endl;
    return -1;
  }

  if (!local_server_ptr->setListenAddressIPv4(listen_address)) {
    std::cerr<<"IPv4 Address is wrong."<<std::endl;
    return -1;
  }
  local_server_ptr->setListenPort(port);
  if (local_server_ptr->init() != 0) {
    std::cerr<<"Server initialization was failed."<<std::endl;
    return -1;
  }

  //  if initialization is succeed,then we enter a loop wait client come.

  //  server have to handle SIGINT,that will let server stop.
  if (setSIGINTAction() < 0) {
    std::cerr<<"Install SIGINT action was failed."<<std::endl;
    return -1;
  }

  fdu_server_loop_end_condition = 0;
  do {
    typename f_server::fupload_t upload(local_server_ptr->accept());  //  fupload(int) no throw
                                                                      //  but there will call to copy-constructor
                                                                      //  it only throw when copy-from has buffer allocated
    worker new_worker = fdu_server_worker;

    //  !! NOTE : there will calls move-constructor twice.
    //  FIRST > std::move() called
    //  SECOND > thread argument binding,the std::thread template request argument is
    //           type of rvalue reference.
    //  IF new_worker doest not require rvalue reference parameter,but std::thread still
    //  create an object is type of rvalue reference,and use the rvalue ref to create
    //  a new object it is copied from the rvalue object.
    //  !!  EVEN call std::forward() to forwards argument,there is still calls move-constructor
    //      twice.
    //  the bug free invalid size,it because the data members has invalid value when move-constructor
    //  is going to be called.then free memory happened memory bug.
    //  now added member-list-initialization to move-constructor and copy-constructor to ensure
    //  the important data members(non-smart-pointer, involved to memory allocating)
    //  holds the valid but useless values.
    std::thread new_thread(new_worker, std::move(upload));

    new_thread.detach();
  } while (!fdu_server_loop_end_condition);

  return 0;
}

