#include <unistd.h>

#include <concepts>

/*  handle_options - a function template used to tell compiler how to generate a
 *                   options resolver used to resolve program options input
 *                   by user.
 *  @argc:           total number of arguments.
 *  @argv:           options.
 *  @fmt:            format string used by getopt().
 *  @the_list:       a pointer points to an object is type of opt_arg_list,
 *                   @callback will processes primary works on it.
 *  @callback:       callback function used to interpret @the_list.
 *  Exceptions:
 *                   UNKNOWN_OPTION
 *                   LACK_NECESSARY_OPTION
 */
template<typename _TypeTag_CompileTime>
void handle_options(int argc, char **argv, const char *fmt,
                    opt_arg_list *the_list, list_op_callback callback) noexcept(false)
{
  static_assert(std::same_as<_TypeTag_CompileTime, server_options_tag> || 
                std::same_as<_TypeTag_CompileTime, client_options_tag>);

  using lvalue_traits = option_type_mapping<get_traits_by_Tag<_TypeTag_CompileTime *>>;
  using rvalue_traits = option_type_mapping<get_traits_by_Tag<_TypeTag_CompileTime>>;
  loc_ret_type v(0);
  char opt = '?';

  while ((opt = getopt(argc, argv, fmt)) != -1)
    {
      if (opt != '?') {
        v = callback(the_list, opt);
        if (!v)
          throw LACK_NECESSARY_OPTION;
      }

      switch (opt) {
      case 'a':
        *static_cast<typename lvalue_traits::a_type>(v) = typename rvalue_traits::a_type{optarg};
        break;
      case 'p':
        *static_cast<typename lvalue_traits::p_type>(v) = typename rvalue_traits::p_type(strtoul(optarg, nullptr, 10u));
        break;
      case 'l':
        *static_cast<typename lvalue_traits::l_type>(v) = typename rvalue_traits::l_type(strtoul(optarg, nullptr, 10u));
        break;
      case 's':
        *static_cast<typename lvalue_traits::s_type>(v) = typename rvalue_traits::s_type{optarg};
        break;
      case 'f':
        *static_cast<typename lvalue_traits::f_type>(v) = typename rvalue_traits::f_type{optarg};
        break;
      case 'd':
        *static_cast<typename lvalue_traits::d_type>(v) = typename rvalue_traits::d_type{optarg};
        break;
      default:
        throw UNKNOWN_OPTION;
      } 
    }
}



