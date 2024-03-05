#include <unistd.h>

//import "fdu_cs.h";
//import <cstring>;
//import <iostream>;
//import <cstdlib>;
//import <exception>;

#include "fdu_cs.h"

#include <cstring>
#include <cstdlib>
#include <exception>

using opt_type = char;
using opt_out_type = void *;

/*  typedef opt_arg_list - vector as the container of <option-argument> key-value
 *                         pair.
 *                         the pair represents the relationship between options and
 *                         arguments.
 */
using opt_arg_list = std::vector<std::pair<opt_type, opt_out_type>>;

#include "fdu_options.h"

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

/*  handle_options_callback - callback function,just a search routine running with
 *                            O(N).
 */
static loc_ret_type handle_options_callback(const opt_arg_list *the_list, char c)
{
  loc_ret_type v(0);
  for (auto i : *the_list) {
    if (i.first == c) {
      v = i.second;
      break;
    }
  }
  return v;
}

void handle_server_options(fdu_server_options &s, int argc, char **argv, const char *fmt)
{
  s.s_listen_address_ = decltype(s.s_listen_address_){"0.0.0.0"};
  s.s_listen_port_ = 58892u;
  s.s_maximum_links_ = 3;

  opt_arg_list the_list;
  the_list.push_back(std::make_pair<opt_type, opt_out_type>('a',
    static_cast<opt_out_type>(&s.s_listen_address_)));
  the_list.push_back(std::make_pair<opt_type, opt_out_type>('p',
    static_cast<opt_out_type>(&s.s_listen_port_)));
  the_list.push_back(std::make_pair<opt_type, opt_out_type>('l',
    static_cast<opt_out_type>(&s.s_maximum_links_)));

  try {
    handle_options<server_options_tag>(argc, argv,
                                       fmt,
                                       &the_list,
                                       handle_options_callback);
  } catch (OPTION_EXCEPTIONS &e) {
    general_option_exception_handler_abort(e);
  }
}

void handle_client_options(fdu_client_options &c, int argc, char **argv, const char *fmt)
{
  #define INVALID_FILE_PATH  "/dev/zero"
  #define DEFAULT_DIRECTORY  "."

  c.c_server_address_ = decltype(c.c_server_address_){"0.0.0.0"};
  c.c_server_port_ = 58892u;
  c.c_filepath_ = decltype(c.c_filepath_){INVALID_FILE_PATH};
  c.c_directory_ = decltype(c.c_directory_){DEFAULT_DIRECTORY};
  
  opt_arg_list the_list;
  the_list.push_back(std::make_pair<opt_type, opt_out_type>('s',
    static_cast<opt_out_type>(&c.c_server_address_)));
  the_list.push_back(std::make_pair<opt_type, opt_out_type>('p',
    static_cast<opt_out_type>(&c.c_server_port_)));
  the_list.push_back(std::make_pair<opt_type, opt_out_type>('f',
    static_cast<opt_out_type>(&c.c_filepath_)));
  the_list.push_back(std::make_pair<opt_type, opt_out_type>('d',
    static_cast<opt_out_type>(&c.c_directory_)));

  try {
    handle_options<client_options_tag>(argc, argv,
                                       fmt,
                                       &the_list,
                                       handle_options_callback);
  } catch (OPTION_EXCEPTIONS &e) {
    general_option_exception_handler_abort(e);
  }

  if (c.c_filepath_ == INVALID_FILE_PATH)
    general_option_exception_handler_abort(LACK_NECESSARY_OPTION);

  #undef DEFAULT_DIRECTORY
  #undef INVALID_FILE_PATH
}

