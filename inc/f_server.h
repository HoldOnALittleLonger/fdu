module;

export module F_SERVER;
import GENERAL_API;
import GENERIC_IPV4_TCP;

export class f_server final : private general_api {
 public:

  class fupload final : private general_api {
  public:
    explicit fupload(int sockfd) : _communicateSocket(sockfd)
    {
      _ub_size = 4096;
      _upload_buffer = new char[_ub_size];
      if (!_upload_buffer)
	throw FUPLOAD_ERR_CONSTRUCT;
    }
    ~fupload()
      {
	shutdown(_communicateSocket, SHUT_RDWR);
	delete[] _upload_buffer;
      }

    fupload(const fupload &robj) noexcept(false)
      {
	__doCopy(robj);
      }

    fupload&operator=(const fupload &robj) noexcept(false)
      {
	__doCopy(robj);
	return *this;
      }

    fupload(fupload &&robj) noexcept(false)
      {
	__doMove(robj);
      }

    fupload&operator=(fupload &&robj) noexcept(false)
      {
	__doMove(robj);
	return *this;
      }

    void setUploadBufferSize(std::size_t n) noexcept(false)
    {
      if (n == _ub_size)
	return;
      //  rcu
      char *temp = new char[n];
      if (!temp)
	throw FUPLOAD_ERR_MEMORY;
      if (_upload_buffer)
	delete[] _upload_buffer;
      _upload_buffer = temp;
      _ub_size = n;
    }

    request_header readRequest(void);
    responding_header checkFile(const request_header &);
    void sendFile(const responding_header &);

  private:
    int _communicateSocket;
    std::size_t _ub_size;
    char *_upload_buffer;

    void __doCopy(const fupload &robj) noexcept(false)
    {
      if (this == &robj)
	return;

      try {
	setUploadBufferSize(robj._ub_size);
      } catch(...) {
	throw FUPLOAD_ERR_COPY;
      }
      memcpy(_upload_buffer, robj._upload_Buffer, robj._ub_size);
      if (_communicateSocket >= 0)
	close(_communicateSocket);
      _communicateSocket = robj._communicateSocket;
    }

    void __doMove(fupload &&robj)
    {
      if (&robj == this)
	return;
      if (_communicateSocket >= 0)
	close(_communicateSocket);
      if (_upload_buffer)
	delete[] _upload_buffer;
      _communicateSocket = robj._communicateSocket;
      _ub_size = robj._ub_size;
      _upload_buffer = robj._upload_buffer;

      robj._communicateSocket = -1;
      robj._ub_size = 0;
      robj._upload_buffer = nullptr;
    }
  };
  using fupload_t = fupload;


  enum F_SERVER_ERROR {
    F_SERVER_ERR_CONSTRUCT = 1
  };
  using f_server_err = F_SERVER_ERROR;

  enum LinkType {
    IPV4,
    UNKNOWN
  };

  f_server(unsigned short max_links = 3) : _maximum_links(max_links), _type(UNKNOWN)
    {
      try {
	_gip4tcp = new generic_ipv4_tcp;
      } catch(...) {
	throw F_SERVER_ERR_CONSTRUCT;
      }
      _listenSocket = -1;
    }

  ~f_server()
    {
      (void)close(_listenSocket);
    }

  bool setListenAddressIPv4(const string &netaddr)
  {
    return _gip4tcp->setAddress(netaddr);
  }
  bool setListenAddressIPv4(const char *netaddr)
  {
    return _gip4tcp->setAddress(netaddr);
  }

  void setListenPort(unsigned short vport)
  {
    _gip4tcp->setPort(vport);
    _type = IPV4;
  }

  void setMaximumLinks(unsigned short n)
  {
    _maximum_links = n;
  }
  void getMaximumLinks(void) const
  {
    return _maximum_links;
  }

  fupload_t accept(void);

  auto getCurrentAddress(void) const
  {
    return _gip4tcp->getAddress();
  }

  auto getCurrentAddressStr(void) const
  {
    return _gip4tcp->getAddressStr();
  }

  LinkType getCurrentLinkType(void) const
  {
    return _type;
  }

  auto getCurrentPort(void) const
  {
    return _gip4tcp->getPort();
  }

  void reset(void)
  {
    (void)close(_listenSocket);
    _type = UNKNOWN;
  }

 private:
  int _listenSocket;
  unsigned short _maximum_links;
  std::unique_ptr<generic_ipv4_tcp> _gip4tcp;
  LinkType _type;
};
