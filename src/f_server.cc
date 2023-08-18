import "f_server.h";
module F_SERVER;

const request_header *f_server::fupload::readRequest(void)
{
  static request_header rh = {0};
  memset(rh.file_path, '\0', FP_LENGTH);
  recv(_communicateSocket, &rh.fp_length, sizeof(std::size_t), 0);
  if (rh.fp_length > 0 && rh.fp_length < FP_LENGTH)
    recv(_communicateSocket, rh.file_path, rh.fp_length, 0);
  return &rh;
}

responding_header f_server::fupload::checkFile(const request_header &r)
{
  responding_header responding = {0};
  if (r.fp_length == 0)
    responding.state = FDU_FILE_NOEXIST;
  else {
    responding.file_length = getFileLength(r.file_path);
    responding.state = FDU_FILE_READY;
  }
  return responding;
}

void f_server::fupload::sendFile(responding_header &responding, const request_header *request)
{
 __cannt_satisfy_request:
  //  if file is not existed,then return responding and exit function
  if (responding.state != FDU_FILE_READY) {
    send(_communicateSocket, &responding, sizeof(responding_header), 0);
    return;
  }

  std::fstream *f(open(std::string{request->file_path)), ios_base::in | ios_base::binary);
  responding.state = FDU_FILE_NOEXIST;
  goto __cannt_satisfy_request;

  int fstream_fd(retriveFdForFstream(*f));
  getRDFileLock(fstream_fd);
  std::ssize_t readed(0);
  while ((readed = read(*f, _upload_buffer, _ub_size)) > 0)
    send(_communicateSocket, _upload_buffer, readed, 0);
  releaseFileLock(fstream_fd);
  close(f);
}

fupload_t f_server::accept(void)
{
  int peer(accept(_listenSocket, NULL, NULL));
  return fupload_t(peer);
}
