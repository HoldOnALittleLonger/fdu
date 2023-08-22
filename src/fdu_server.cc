import "fdu_cs.h";
import F_SERVER;

//  thread will deatched.
static void fdu_server_worker(typename f_server::fupload_t upload)
{
  const request_header &rh = upload.readRequest();
  responding_header responding = upload.checkFile(rh);
  upload.sendFile(responding, rh);
}

int fdu_server(const std::string &listen_address, unsigned long port, unsigned int maximum_links = 3)
{
  //  typedef
  using worker = std::function<void(typename f_server::fupload_t)>;

  std::unique_ptr<f_server> local_server_ptr(nullptr);
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
  int loop_end_condition(0);
  do {
    std::unique_ptr<f_server::fupload_t> uploadPtr(nullptr);
    try {
      uploadPtr = new f_server::fupload_t(local_server_ptr->accept());
    } catch (std::bad_alloc &x) {
      cerr<<"Allocate Memory was failed.SKIP."<<endl;
    } catch (typename f_server::fupload::fupload_err &x) {
      cerr<<"Generate worker object was failed.SKIP."<<endl;
    }
    worker new_worker = fdu_server_worker;
    std::thread new_thread(new_worker, upload);
    new_thread.detach();
  } while (!loop_end_condition);

  return 0;
}

