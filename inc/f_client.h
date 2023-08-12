module;

export module F_CLIENT;
import FOPS;
import NOPS;

import <cstddef>
import <cstdlib>

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

    int getFile(void);
    void setFileName(const char *filename)
    {
      setFileName(std::string{filename});
    }

    //  _dentry is @_directory/@filename
    void setFileName(const std::string &filename)
    {
      std::string::iterator filename_with_slash(filename.begin());
      for (auto it(filename.end()); it >= filename.begin(); --it) {
	if (*it == '/') {
	  filename_with_slash = it;
	  break;
	}
      }
      std::size_t displacement(fileanme_with_slash - filename.begin());
      std::size_t length(filename.end() - filename_with_slash);
      std::string the_file(filename.substr(displacement, length));
      *_dentry = *_directory + the_file;
    }

    void setDownloadPath(const char *download_path)
    {
      setDownloadPath(std::string{download_path});
    }

    void setDownloadPath(const std::string &download_path)
    {
      *_directory = download_path;
    }

    
    fdownload(const fdownload &robj)
      {
	_socket = robj._socket;
	*_filename = *robj._filename;
	*_directory = *robj._directory;
	*_dentry = *robj._dentry;
      }

    fdownload(fdownload &&robj)
      {
	_socket = robj._socket;
	_filename = robj._filename;
	_directory = robj._directory;
	_dentry = robj._dentry;

	robj._socket = -1;
	robj._directory = nullptr;
	robj._dentry = nullptr;
      }

  private:
    int _socket;
    std::string *_filename;
    std::string *_directory;
    std::string *_dentry;
  };

  enum FAMILY {
    IPV4 = 4,
    UNKNOWN = 173
  };

  f_client()
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

  bool setPeerAddressIPv4(const char *netaddr)
  {
    struct sockaddr_in *ipv4_addr = static_cast<struct sockaddr_in *>(_ipv4_addr);
    if (!inet_aton(netaddr, &ipv4_addr->sin_addr))
      return false;
    return true;
  }

  bool setPeerAddressIPv4(const std::string &netaddr)
  {
    const char *cnetaddr = netaddr.c_str();
    return set_ipv4_addr(cnetaddr);
  }

  void setPeerPort(unsigned long vport)
  {
    _port = htonl(vport);
  }

  fdownload connect(void);

  FAMILY getCurrentLinkType(void) const
  {
    return _currentLink;
  }

  f_client(const f_client &robj)
    {
      _currentLink = robj._currentLink;
      _port = robj._port;
      *_ipv4_addr = *robj._ipv4_addr;
      *_ipv4_addr_str = *robj._ipv4_addr_str;
    }

  f_client(f_client &&robj)
    {
      _currentLink = robj._currentLink;
      _port = robj._port;
      _ipv4_addr = robj._ipv4_addr;
      _ipv4_addr_str = robj._ipv4_addr_str;

      robj._currentLink = UNKNOWN;
      robj._port = 0;
      robj._ipv4_addr = nullptr;
      robj._ipv4_addr_str = nullptr;
    }

 private:
  std::string *_ipv4_addr_str;
  struct sockaddr *_ipv4_addr;
  unsigned long _port;

  FAMILY _currentLink;
};
