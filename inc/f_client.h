module;

export module F_CLIENT;
import MISC;

import <cstddef>

export class f_client final : private fops, private nops {
 public:

  class fdownload final : private fops, private nops {
  public:
    explicit fdownload(int sockfd) : _socket(sockfd)
    {
      _directory = new std::string;
      _dentry = new std::string;
    }
    ~fdownload()
      {
	shutdown(sockfd, SHUT_RDWR);
	delete _directory;
	delete _dentry;
      }

    int getFile(const char *path);
    int getFile(const std::string &path);

  private:
    int _socket;
    std::string *_directory;
    std::string *_dentry;
  };

  enum FAMILY {
    IPV4 = 4,
    UNKNOWN = 173
  };

  explicit f_client()
  {
    _ipv4_addr_str = new std::string;
    _ipv4_addr = malloc(sizeof(struct sockaddr));
    port = 0;
    _currentLink = UNKNOWN;
  }
  ~f_client()
    {
      delete _ipv4_addr_str;
      free(_ipv4_addr);
    }

  bool setIpv4Addr(const char *netaddr)
  {
    struct sockaddr_in *ipv4_addr = static_cast<struct sockaddr_in *>(_ipv4_addr);
    if (!inet_aton(netaddr, &ipv4_addr->sin_addr))
      return false;
    return true;
  }

  bool setIpv4Addr(const std::string &netaddr)
  {
    const char *cnetaddr = netaddr.c_str();
    return set_ipv4_addr(cnetaddr);
  }

  void setIpv4Port(unsigned long vport)
  {
    port = htonl(vport);
  }

  fdownload connect(void);

 private:
  std::string *_ipv4_addr_str;
  struct sockaddr *_ipv4_addr;
  unsigned long port;

  FAMILY _currentLink;
};
