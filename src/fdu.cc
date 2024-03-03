#include <unistd.h>

//import "fdu_cs.h";
//import <cstring>;
//import <iostream>;
//import <cstdlib>;
//import <exception>;

#include "fdu_cs.h"
#include "fdu_options.h"
#include <cstring>
#include <cstdlib>
#include <exception>

enum FDU_MODE { ASSERVER, ASCLIENT, UNKNOWN_MODE };

static void fdu_help(void)
{
  std::cerr<<"Usage : fdu [asserver | asclient] [options]"<<std::endl;
  std::cerr<<"Server options : -a <listen address> -p <listen port> -l <maximum links>"<<std::endl;
  std::cerr<<"Client options : -s <server address> -p <server port> -f <file path> -d <directory>"<<std::endl;
}

static FDU_MODE which_mode_now(char **argv, unsigned int n)
{
  if (strcmp(argv[1], "asserver") == 0)
    return ASSERVER;
  else if (strcmp(argv[1], "asclient") == 0)
    return ASCLIENT;
  else
    return UNKNOWN_MODE;
}

void handle_server_options(fdu_server_options &s, int argc, char **argv, const char *fmt);
void handle_client_options(fdu_client_options &c, int argc, char **argv, const char *fmt);

static inline
void general_option_exception_handler_abort(const OPTION_EXCEPTIONS &x)
{
  switch (x) {
  case UNKNOWN_OPTION:
    std::cerr<<"Unknown option had been given."<<std::endl;
    break;
  case LACK_NECESSARY_OPTION:
    std::cerr<<"Lack necessary option and argument."<<std::endl;
    break;
  }
  fdu_help();
  std::abort();
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr<<"Argument is too few!"<<std::endl;
    fdu_help();
    return -1;
  }

  FDU_MODE fdu_mode(which_mode_now(argv, argc));

#ifdef __DEBUG
  for (auto i(0); i < argc; ++i)
    std::cerr<<argv[i]<<" ";
  std::cerr<<std::endl;
#endif

  switch (fdu_mode) {
  case ASSERVER:
    {
    fdu_server_options s_options;
    handle_server_options(s_options, argc - 1, argv + 1, "a:p:l:");

    if (fdu_server(s_options.s_listen_address_,
                   s_options.s_listen_port_, s_options.s_maximum_links_) < 0) {
      std::cerr<<"Exception occurred when executing server routine!!"<<std::endl;
      std::cout<<s_options;
      return -1;
    }
    }
    break;
  case ASCLIENT:
    {
    fdu_client_options c_options;
    handle_client_options(c_options, argc - 1, argv + 1, "s:p:f:d:");
    
    if (fdu_client(c_options.c_filepath_, c_options.c_directory_,
                   c_options.c_server_address_, c_options.c_server_port_) < 0) {
      std::cerr<<"Exception occurred when executing client routine!!"<<std::endl;
      std::cout<<c_options;
      return -1;
    }
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

void handle_server_options(fdu_server_options &s, int argc, char **argv, const char *fmt)
{
  /*  server is allowed to be setup with the default values.
   *  if prepare to assign an argument,then have to assign all.
   *  the total number of arguments is 3,and insist appear order.
   *  otherwise,program will bug.
   */
  if (argc > 1 && argc < 6)
    general_option_exception_handler_abort(LACK_NECESSARY_OPTION);

  s.s_listen_address_ = decltype(s.s_listen_address_){"0.0.0.0"};
  s.s_listen_port_ = 58892u;
  s.s_maximum_links_ = 3;

  try {
    handle_options<server_options_tag>(argc, argv, fmt,
                                       &s.s_listen_address_,
                                       &s.s_listen_port_,
                                       &s.s_maximum_links_);
  } catch (OPTION_EXCEPTIONS &e) {
    general_option_exception_handler_abort(e);
  }
}

void handle_client_options(fdu_client_options &c, int argc, char **argv, const char *fmt)
{
  /*  client must be given all informations about the server,the file
   *  is going to be retrived,and the directory where to save it.
   *  total number of these arguments is 4,insist the appear order.
   *  otherwise,program will bug.
   */
  if (argc < 8)
    general_option_exception_handler_abort(LACK_NECESSARY_OPTION);

  #define INVALID_FILE_PATH  "unexisted_entity"
  #define DEFAULT_DIRECTORY  "."

  c.c_server_address_ = decltype(c.c_server_address_){"0.0.0.0"};
  c.c_server_port_ = 58892u;
  c.c_filepath_ = decltype(c.c_filepath_){INVALID_FILE_PATH};
  c.c_directory_ = decltype(c.c_directory_){DEFAULT_DIRECTORY};

  try {
    handle_options<client_options_tag>(argc, argv, fmt,
                                       &c.c_server_address_,
                                       &c.c_server_port_,
                                       &c.c_filepath_,
                                       &c.c_directory_);
  } catch (OPTION_EXCEPTIONS &e) {
    general_option_exception_handler_abort(e);
  }

  #undef DEFAULT_DIRECTORY
  #undef INVALID_FILE_PATH
}

