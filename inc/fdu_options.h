#pragma once

#include <cstddef>
#include <string>
#include <iostream>

struct fdu_server_options;
struct fdu_client_options;

struct server_options_tag {
  typedef fdu_server_options tag_owner;
};
struct client_options_tag {
  typedef fdu_client_options tag_owner;
};

struct general_option_types {
  using context_type = std::string;
  using unsigned_type = unsigned int;
  using signed_type = int;
  using floating_type = float;
};

template<class _Tp>
struct option_type_traits {
  typedef typename _Tp::option_category option_category;
  typedef typename _Tp::context_type context_type;
  typedef typename _Tp::unsigned_type unsigned_type;
  typedef typename _Tp::signed_type signed_type;
  typedef typename _Tp::floating_type floating_type;
};

template<class _Tp>
struct option_type_traits<_Tp *> {
  typedef typename _Tp::option_category option_category_pointer;
  typedef typename _Tp::context_type* context_type;
  typedef typename _Tp::unsigned_type* unsigned_type;
  typedef typename _Tp::signed_type* signed_type;
  typedef typename _Tp::floating_type* floating_type;
};

template<class _Tp>
struct option_type_traits<const _Tp *> {
  typedef const typename _Tp::option_category option_category_const_pointer;
  typedef const typename _Tp::context_type* context_type;
  typedef const typename _Tp::unsigned_type* unsigned_type;
  typedef const typename _Tp::signed_type* signed_type;
  typedef const typename _Tp::floating_type* floating_type;
};

struct fdu_server_options {
  using option_category = server_options_tag;
  using context_type = general_option_types::context_type;
  using unsigned_type = general_option_types::unsigned_type;
  using signed_type = general_option_types::signed_type;
  using floating_type = general_option_types::floating_type;

  context_type s_listen_address_;
  unsigned_type s_listen_port_;
  unsigned_type s_maximum_links_;

  friend
  std::ostream &operator<<(std::ostream &printer, fdu_server_options &server)
  {
    printer<<"address : "<<server.s_listen_address_<<std::endl;
    printer<<"port : "<<server.s_listen_port_<<std::endl;
    printer<<"maximum links : "<<server.s_maximum_links_<<std::endl;
    return printer;
  }
};

struct fdu_client_options {
  using option_category = client_options_tag;
  using context_type = general_option_types::context_type;
  using unsigned_type = general_option_types::unsigned_type;
  using signed_type = general_option_types::signed_type;
  using floating_type = general_option_types::floating_type;

  context_type c_server_address_;
  unsigned_type c_server_port_;
  context_type c_filepath_;
  context_type c_directory_;
        
  friend
  std::ostream &operator<<(std::ostream &printer, fdu_client_options &client)
  {
    printer<<"address : "<<client.c_server_address_<<std::endl;
    printer<<"port : "<<client.c_server_port_<<std::endl;
    printer<<"file : "<<client.c_filepath_<<std::endl;
    printer<<"directory : "<<client.c_directory_<<std::endl;
    return printer;
  }
};

template<typename _Tp>
struct option_type_mapping {
  using option_traits = option_type_traits<_Tp>;
  using a_type = option_traits::context_type;
  using p_type = option_traits::unsigned_type;
  using l_type = option_traits::unsigned_type;
  using s_type = option_traits::context_type;
  using f_type = option_traits::context_type;
  using d_type = option_traits::context_type;
};

enum OPTION_EXCEPTIONS {
  UNKNOWN_OPTION,
  LACK_NECESSARY_OPTION
};

template<typename _Tag>
struct get_traits_by_Tag : public option_type_traits<typename _Tag::tag_owner> {};

template<typename _Tag>
struct get_traits_by_Tag<_Tag *> : public option_type_traits<typename _Tag::tag_owner *> {};

template<typename _Tag>
struct get_traits_by_Tag<const _Tag *> : public option_type_traits<const typename _Tag::tag_owner *> {};

template<typename _TypeTag_CompileTime>
void handle_options(int argc, char **argv, const char *fmt, ...);

#include "../src/fdu_options.cc"

