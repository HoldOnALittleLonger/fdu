//import "f_client.h";
//import "misc.h";

//module F_CLIENT;

#include "f_client.h"

typename f_client::fdownload_t
f_client::connect(int domain, int type, int protocol) noexcept(false)
{
  int sockfd(socket(domain, type, protocol));
  if (sockfd < 0)
    throw F_CLIENT_ERR_SOCKET;
  auto peerAddress(getPeerAddressIPv4());
  if (general_api::connect(sockfd, &peerAddress, sizeof(peerAddress)) < 0) {
    shutdown(sockfd, SHUT_RDWR);
    throw F_CLIENT_ERR_CONNECT;
  }
  return fdownload_t(sockfd);
}

unsigned short f_client::fdownload::getFile(void)
{
  if (!_download_buffer)
    try {
      __allocateDownloadBuffer();
    } catch (std::bad_alloc &x) {
      return FDOWNLOAD_ERR_MEMORY;
    }

  request_header rh = {0};
  rh.fp_length = _filename->length();
  strncpy(rh.file_path, _filename->c_str(), rh.fp_length);
  send(_socket, &rh, sizeof(rh.fp_length) + rh.fp_length, 0);

  responding_header respond = {0};
  ssize_t returned = recv(_socket, &respond, sizeof(responding_header), 0);

  if (returned != sizeof(responding_header))
    return FDOWNLOAD_ERR_RESPOND;

  if (respond.state != FDU_FILE_READY)
    return FDOWNLOAD_ERR_FILE;

  std::fstream *local_file = open(*_dentry, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
  if (!local_file) 
    return FDOWNLOAD_ERR_OPENFILE;

  int local_file_fd(retriveFdForFstream(*local_file));
  getWRFileLock(local_file_fd);

  std::size_t all_received = 0;
  while ((returned = recv(_socket, _download_buffer, _db_size, 0)) > 0) {
    all_received += returned;
    local_file->write(_download_buffer, returned);
  }
  releaseFileLock(local_file_fd);

  close(local_file);
  return (all_received != respond.file_length) ? FDOWNLOAD_ERR_FILE : 0;
}


