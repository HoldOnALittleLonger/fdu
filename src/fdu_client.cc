//import "fdu_cs.h";
//import F_CLIENT;

#include "fdu_cs.h"
#include "f_client.h"

int fdu_client(const std::string &file_path_fully, const std::string &download_directory,
	       const std::string &server_address, unsigned long server_port)
{
  std::unique_ptr<f_client> local_client_ptr(nullptr);  //  RAII
  try {
    local_client_ptr.reset(new f_client);
  } catch (std::bad_alloc &x) {
    std::cerr<<"Allocate memory failed."<<std::endl;
    return -1;
  } catch (f_client::f_client_err &x) {
    std::cerr<<"Generate client object was failed."<<std::endl;
    return -1;
  }

  int ret(-1);
  if (!local_client_ptr->setPeerAddressIPv4(server_address))
    return ret;
  local_client_ptr->setPeerPort(server_port);

  std::unique_ptr<typename f_client::fdownload_t> downloadPtr(nullptr);
  //  copy and construct both throw,so have to deal with these exceptions.
  try {
    downloadPtr.reset(new typename f_client::fdownload_t(local_client_ptr->
							 connect(AF_INET, SOCK_STREAM, IPPROTO_TCP)));
  } catch (std::bad_alloc &x) {
    std::cerr<<"Allocate memory failed."<<std::endl;
    return -1;
  } catch (typename f_client::f_client_err &x) {
    std::cerr<<"Connect to server was failed."<<std::endl;
    return -1;
  } catch (typename f_client::fdownload::fdown_err &x) {
    std::cerr<<"Generate local download object was failed."<<std::endl;
    return -1;
  }

  downloadPtr->setDownloadPath(download_directory);
  downloadPtr->setFileName(file_path_fully);
  if (!downloadPtr->checkFileIsExisted()) {
    ret = downloadPtr->getFile();
    if (ret != 0) {
      std::cerr<<"Unknown error occurred when downloading."<<std::endl;
      std::cerr<<"Download path : "<<download_directory<<std::endl;
      std::cerr<<"Dentry : "<<downloadPtr->getFileName()<<std::endl;
      std::cerr<<"Required file : "<<file_path_fully<<std::endl;
      std::cerr<<"Error Code : "<<ret<<std::endl;
      ret = -1;
    } else {
      ret = 0;
    }
  } else {
    std::cerr<<"The file : "<<downloadPtr->getFileName()<<" is existed."<<std::endl;
  }

  return ret;
}
