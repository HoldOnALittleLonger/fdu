#include <unistd.h>

#include <cstdarg>
#include <concepts>

template<typename _TypeTag_CompileTime>
void handle_options(int argc, char **argv, const char *fmt, ...) noexcept(false)
{
  static_assert(std::same_as<_TypeTag_CompileTime, server_options_tag> || 
                std::same_as<_TypeTag_CompileTime, client_options_tag>);

  va_list ap;
  va_start(ap, fmt);
  char opt('?');

  using _Type = _TypeTag_CompileTime::tag_owner;
  using lvalue_traits = option_type_mapping<_Type*>;
  using rvalue_traits = option_type_mapping<_Type>;

  while ((opt = getopt(argc, argv, fmt)) != -1)
    {
      switch (opt) {
      case 'a':
        *(va_arg(ap, typename lvalue_traits::a_type)) = typename rvalue_traits::a_type{optarg};
        break;
      case 'p':
        *(va_arg(ap, typename lvalue_traits::p_type)) = typename rvalue_traits::p_type(strtoul(optarg, nullptr, 10u));
        break;
      case 'l':
        *(va_arg(ap, typename lvalue_traits::l_type)) = typename rvalue_traits::l_type(strtoul(optarg, nullptr, 10u));
        break;
      case 's':
        *(va_arg(ap, typename lvalue_traits::s_type)) = typename rvalue_traits::s_type{optarg};
        break;
      case 'f':
        *(va_arg(ap, typename lvalue_traits::f_type)) = typename rvalue_traits::f_type{optarg};
        break;
      case 'd':
        *(va_arg(ap, typename lvalue_traits::d_type)) = typename rvalue_traits::d_type{optarg};
        break;
      default:
        throw UNKNOWN_OPTION;
      } 
    }

  va_end(ap);
}



