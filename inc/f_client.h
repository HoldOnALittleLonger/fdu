module;

export module F_CLIENT;
import GENERAL_API;
import GENERIC_IPV4_TCP;

import <cstddef>;
import <cstdlib>;
import <cstring>;

export class f_client final : private general_api {
 public:

  enum F_CLIENT_ERROR {
    F_CLIENT_ERR_CONSTRUCT = 1,
    F_CLIENT_ERR_SOCKET,
    F_CLIENT_ERR_CONNECT
  };
  using f_client_err = F_CLIENT_ERROR;

  class fdownload final : private general_api {
  public:
    
    enum FDOWNLOAD_ERROR {
      FDOWNLOAD_ERR_CONSTRUCT = 1
      FDOWNLOAD_ERR_MEMORY,
      FDOWNLOAD_ERR_COPY,
      FDOWNLOAD_ERR_RESPOND,
      FDOWNLOAD_ERR_FILE
    };
    using fdown_err = FDOWNLOAD_ERROR;

    explicit fdownload(int sockfd) noexcept(false) : _socket(sockfd)
    {
      _filename = new std::string;
      _directory = new std::string;
      _dentry = new std::string;
      _db_size = 4096;
      _download_buffer = new char[_db_size];
      if (!_filename || !_directory || !_dentry || !_download_buffer)
	throw FDOWNLOAD_ERR_CONSTRUCT;
    }
    ~fdownload()
      {
	//  destructor does not release socket,
	//  client have to release it manually.
	delete[] _download_buffer;
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

    //  maybe client needs this function to examine that
    //  if the file is been existed.
    std::string getFileName(void) const
    {
      return *_dentry;
    }

    void setDownloadPath(const char *download_path)
    {
      setDownloadPath(std::string{download_path});
    }

    void setDownloadPath(const std::string &download_path)
    {
      *_directory = download_path;
    }

    void setDownloadBufferSize(std::size_t n) noexcept(false)
    {
      if (n == _db_size)  //  if n == _db_size,do nothing.
	return;

      auto n(_db_size);
      char *temp = new char[n];
      if (!temp)
	throw FDOWNLOAD_ERR_MEMORY;
      if (_download_buffer)
	delete[] _download_buffer;
      _db_size = n;
      _download_buffer = temp;
    }
    
    fdownload(const fdownload &robj) noexcept(false)
      {
	_socket = robj._socket;
	*_filename = *robj._filename;
	*_directory = *robj._directory;
	*_dentry = *robj._dentry;
	try {
	  setDownloadBufferSize(robj._db_size);
	} catch (fdownload_err &x) {
	  throw FDOWNLOAD_ERR_COPY;
	}
	memcpy(_download_buffer, robj._download_buffer, _db_size);
      }

    fdownload(fdownload &&robj)
      {
	_socket = robj._socket;
	_filename = robj._filename;
	_directory = robj._directory;
	_dentry = robj._dentry;
	_db_size = robj._db_size;
	_download_buffer = robj._download_buffer;

	robj._socket = -1;
	robj._filename = nullptr;
	robj._directory = nullptr;
	robj._dentry = nullptr;
	robj._db_size = 0;
	robj._download_buffer = nullptr;
      }

    bool operator bool()
    {
      return _socket >= 0;
    }

    void releaseLink(void)
    {
      shutdown(_socket, SHUT_RDWR);
      _socket = -1;
    }

  private:
    int _socket;
    std::unique_ptr<std::string> _filename;
    std::unique_ptr<std::string> _directory;
    std::unique_ptr<std::string> _dentry;
    std::size_t _db_size;
    char *_download_buffer;
  };

  using fdownload_t = fdownload;

  enum LinkType {
    IPV4 = 4,
    UNKNOWN = 173
  };

  f_client() noexcept(false)
    {
      try {
	_gip4tcp = new generic_ipv4_tcp;
      } catch (...) {
	throw F_CLIENT_ERR_CONSTRUCT;
      }
      _linktype = UNKNOWN;
    }
  ~f_client() {}

  bool setPeerAddressIPv4(const char *netaddr)
  {
    return _gip4tcp->setAddress(netaddr);
  }

  bool setPeerAddressIPv4(const std::string &netaddr)
  {
    return _gip4tcp->setAddress(netaddr);
  }

  void setPeerPort(unsigned long vport)
  {
    _gip4tcp->setPort(vport);
    _linktype = IPV4;
  }

  auto getPeerAddressIPv4Str(void) const
  {
    return _gip4tcp->getAddressStr();
  }

  auto getPeerAddressIPv4(void) const
  {
    return _gip4tcp->getAddress();
  }

  auto getPeerPort(void) const
  {
    return _gip4tcp->getPort();
  }

  fdownload_t connect(int domain, int type, int protocol) noexcept(false);

  LinkType getCurrentLinkType(void) const
  {
    return _linktype;
  }

  //  copy constructor
  f_client(const f_client &robj)
    {
      *_gip4tcp = *robj._gip4tcp;
      _linktype = robj._linktype;
    }

  //  move constructor
  explicit f_client(f_client &&robj)
    {
      _gip4tcp = robj._gip4tcp;
      _linktype = robj._linktype;
      robj._linktype = UNKNOWN;
    }

 private:
  std::unique_ptr<generic_ipv4_tcp> _gip4tcp;
  LinkType _linktype;
};
