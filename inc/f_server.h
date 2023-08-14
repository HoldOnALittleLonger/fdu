module;

export module F_SERVER;
import GENERAL_API;

export class f_server final : private general_api {
 public:

  enum LinkType {
    IPV4,
    UNKNOWN
  };

  f_server(unsigned short max_links = 3) : _maximum_links(max_links)
    {
      _ipv4_addr = malloc(sizeof(struct sockaddr));
      _ipv4_addr_str = new std::string;
      if (!_ipv4_addr || !_ipv4_addr_str)
	throw F_SERVER_ERR_CONSTRUCT;
      _type = UNKNOWN;
      _port = 0;
    }

  ~f_server()
    {
      free(_ipv4_addr);
      delete _ipv4_addr_str;
    }

  bool setListenAddressIPv4(const string &netaddr)
  {
    struct sockaddr_in *ipv4_addr = static_cast<struct sockaddr_in *>(_ipv4_addr);
    if (!inet_aton(netaddr, &ipv4_addr->sin_addr))
      return false;
    _type = IPV4;
    return true;
  }
  bool setListenAddressIPv4(const char *netaddr)
  {
    return setListenAddressIPv4(std::string{netaddr});
  }

  void setListenPort(unsigned short vport)
  {
    _port = vport;
    static_cast<struct sockaddr_in *>(_ipv4_addr)->sin_port = htonl(_port);
  }

  void setMaximumLinks(unsigned short n)
  {
    _maximum_links = n;
  }

  fupload accept(void);

 private:
  unsigned short _maximum_links;
  struct sockaddr *_ipv4_addr;
  std::string *_ipv4_addr_str;
  unsigned long _port;
  LinkType _type;
};
