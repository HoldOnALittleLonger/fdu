import "fdu_cs.h";
import F_CLIENT;

int fdu_client(const std::string &file_path_fully, const std::string &download_directory,
	       const std::string &server_address, unsigned long server_port)
{
  std::unique_ptr<f_client> local_client_ptr(nullptr);  //  RAII
  try {
    local = new f_client;
  } catch (std::bad_alloc &x) {
    cerr<<"Allocate memory failed."<<endl;
    return -1;
  } catch (f_client::f_client_err &x) {
    cerr<<"Generate client object was failed."<<endl;
    return -1;
  }

  int ret(-1);
  if (!local->setPeerAddressIPv4(server_address))
    goto fdu_client_exit;
  local->setPeerPort(server_port);
  
  std::unique_ptr<typename f_client::fdownload_t> downloadPtr(nullptr);
  //  copy and construct both throw,so have to deal with these exceptions.
  try {
    downloadPtr = new typename f_client::fdownload_t(local->connect(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  } catch (std::bad_alloc &x) {
    cerr<<"Allocate memory failed."<<endl;
    return -1;
  } catch (f_client_err &x) {
    cerr<<"Connect to server was failed."<<endl;
    return -1;
  } catch (typename f_client::fdownload::fdown_err &x) {
    cerr<<"Generate local download object was failed."<<endl;
    return -1;
  }

  downloadPtr->setDownloadPath(download_directory);
  downloadPtr->setFileName(file_path_fully);
  if (!downloadPtr->checkFileIsExisted()) {
    ret = downloadPtr->getFile();
    if (ret != 0) {
      cerr<<"Unknown error occurred when downloading."<<endl;
      cerr<<"Download path : "<<download_directory<<endl;
      cerr<<"Dentry : "<<downloadPtr->getFileName()<<endl;
      cerr<<"Required file : "<<file_path_fully<<endl;
      cerr<<"Error Code : "<<ret<<endl;
      ret = -1;
    } else {
      ret = 0;
    }
  } else {
    cerr<<"The file : "<<downloadPtr->getFileName()<<" is existed."<<endl;
  }

  return ret;
}
