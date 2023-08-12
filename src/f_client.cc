import F_CLIENT;
import REQUEST_RESPONDING;

typename f_client::fdownload f_client::connect(void)
{

}

int f_client::fdownload::getFile(void)
{
  request_head rh = {0};
  strncpy(rh.file_path, _filename->c_str(), _filename->length());
  std::fstream *local_file = open(_dentry->c_str(), ios_base::out | ios_base::trun | ios_base::binary);
  if (!local_file) 
    return F_CLIENT_ERR_OPENFILE;

  
}


