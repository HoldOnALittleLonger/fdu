//import "f_server.h";
//module F_SERVER;

#include "f_server.h"

request_header f_server::fupload::readRequest(void)
{
  request_header rh = {0};  //  static is not multi-thread safe
  recv(_communicateSocket, &rh.fp_length, sizeof(rh.fp_length), 0);
  if (rh.fp_length > 0 && rh.fp_length < FP_LENGTH)
    recv(_communicateSocket, rh.file_path, rh.fp_length, 0);
  return rh;
}

responding_header f_server::fupload::checkFile(const request_header &r)
{
  responding_header responding = {0};
  if (r.fp_length == 0 || r.fp_length >= FP_LENGTH)
    responding.state = FDU_FILE_NOEXIST;
  else {
    responding.file_length = getFileLength(r.file_path);
    responding.state = (responding.file_length > 0) ? FDU_FILE_READY : FDU_FILE_NOEXIST;
  }
  return responding;
}

unsigned short f_server::fupload::sendFile(responding_header &responding, const request_header &request)
{
  int retv(FUPLOAD_ERR_FILE_NOEXIST);
 __cannt_satisfy_request:
  //  if file is not existed,then return responding and exit function
  if (responding.state != FDU_FILE_READY) {
    send(_communicateSocket, &responding, sizeof(responding_header), 0);
    return retv;
  }

  if (!_upload_buffer)
    try {
      __allocateUploadBuffer();
    } catch (std::bad_alloc &x) {
      responding.state = FDU_FILE_NOEXIST;
      retv = FUPLOAD_ERR_MEMORY;
      goto __cannt_satisfy_request;
    }


  std::fstream *f(open(std::string{request.file_path}, std::ios_base::in | std::ios_base::binary));
  if (!f) {
    responding.state = FDU_FILE_NOEXIST;
    retv = FUPLOAD_ERR_OPEN;
    goto __cannt_satisfy_request;
  }

  int fstream_fd(retriveFdForFstream(*f));
  getRDFileLock(fstream_fd);
  ssize_t readed(0);
  send(_communicateSocket, &responding, sizeof(responding_header), 0);
  while ((readed = read(*f, _upload_buffer, _ub_size)) > 0)
    send(_communicateSocket, _upload_buffer, readed, 0);
  releaseFileLock(fstream_fd);
  close(f);
  return 0;
}

f_server::fupload_t f_server::accept(void)
{
  int peer(general_api::accept(_listenSocket, NULL, NULL));
  return fupload_t(peer);
}
