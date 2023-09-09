import "fdu_cs.h";
import <cstring>;
import <iostream>;

enum FDU_MODE { ASSERVER, ASCLIENT, UNKNOWN_MODE };

static void fdu_help(void)
{
  std::cerr<<"Usage : fdu [asserver | asclient] [options]"<<std::endl;
  std::cerr<<"Server options : -a <listen address> -p <listen port> -l <maximum links>"<<std::endl;
  std::cerr<<"Client options : -s <server address> -p <server port> -f <file path> -d <directory>"<<std::endl;
}

static FDU_MODE which_mode_now(const char **argv, unsigned int n)
{
  if (n < 2) {
    fdu_help();
    return UNKNOWN_MODE;
  }
  if (strcmp(argv[1], "asserver") == 0)
    return ASSERVER;
  else if (strcmp(argv[1], "asclient") == 0)
    return ASCLIENT;
  else
    return UNKNOWN_MODE;
}

struct fdu_server_options {
  std::string s_listen_address;
  unsigned int s_listen_port;
  unsigned short s_maximum_links;
  void outputInfo(void)
  {
    std::cerr<<"address : "<<s_listen_address<<std::endl;
    std::cerr<<"port : "<<s_listen_port<<std::endl;
    std::cerr<<"maximum links : "<<s_maximum_links<<std::endl;
  }
};

struct fdu_client_options {
  std::string c_server_address;
  unsigned int c_server_port;
  std::string c_filepath;
  std::string c_directory;
  void outputInfo(void)
  {
    std::cerr<<"address : "<<c_server_address<<std::endl;
    std::cerr<<"port : "<<c_server_port<<std::endl;
    std::cerr<<"file : "<<c_filepath<<std::endl;
    std::cerr<<"directory : "<<c_directory<<std::endl;
  }
};

struct fdu_server_options retriveServerOptions(char **argv, unsigned int n);
struct fdu_client_options retriveClientOptions(char **argv, unsigned int n);

int main(int argc, char *argv[])
{
  FDU_MODE fdu_mode(which_mode_now(argv, argc));
  switch (fdu_mode) {
  case ASSERVER:
    ;
    fdu_server_options options(retriveServerOptions(argv + 2, argc - 2));
    if (fdu_server(options.s_listen_address, options.s_listen_port, options.s_maximum_links) < 0) {
      std::cerr<<"Exception occurred when executing server routine!!"<<std::endl;
      options.outputInfo();
      return -1;
    }
    break;
  case ASCLIENT:
    ;
    fdu_client_options options(retriveClientOptions(argv + 2, argc - 2));
    if (fdu_client(options.c_filepath, options.c_directory,
		   options.c_server_address, options.c_server_port) < 0
	)
      {
	std::cerr<<"Exception occurred when executing client routine!!"<<std::endl;
	options.outputInfo();
	return -1;
      }
    break;
  default:
    std::cerr<<"Unknown mode : "<<argv[1]<<std::endl;
    fdu_help();
    return -1;
  }

  std::cerr<<"fdu server ready exit!!"<<std::endl;
  std::cerr<<"fdu exited!!"<<std::endl;
  return 0;
}

struct fdu_server_options retriveServerOptions(char **argv, unsigned int n)
{

}

struct fdu_client_options retriveClientOptions(char **argv, unsigned int n)
{

}
