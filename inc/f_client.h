//module;

//export module F_CLIENT;
//import GENERAL_API;
//import GENERIC_IPV4_TCP;

//import <cstddef>;
//import <cstdlib>;
//import <cstring>;
//import <memory>;

#ifndef _F_CLIENT_H_
#define _F_CLIENT_H_

#include "misc.h"
#include <cstddef>

//export 
class f_client final : private general_api {
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
      FDOWNLOAD_ERR_FILE,
      FDOWNLOAD_ERR_OPENFILE
    };
    using fdown_err = FDOWNLOAD_ERROR;

    explicit fdownload(int sockfd) noexcept(false) : _socket(sockfd)
    {
      _filename = new std::string;
      _directory = new std::string;
      _dentry = new std::string;
      _db_size = FDU_DEFAULT_BUFFER_SIZE;
      _download_buffer = nullptr;
      if (!_filename || !_directory || !_dentry)
	throw FDOWNLOAD_ERR_CONSTRUCT;
    }
    ~fdownload()
      {
 	releaseLink();
	delete[] _download_buffer;
      }

    unsigned short getFile(void);
    void setFileName(const char *filename)
    {
      setFileName(std::string{filename});
    }

    //  _dentry is @_directory/@filename
    void setFileName(const std::string &filename)
    {
      auto filename_with_slash(filename.begin());
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
      *_filename = filename;
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

    void setDownloadBufferSize(std::size_t n)
    {
      if (!n)
	n = FDU_DEFAULT_BUFFER_SIZE;  //  does not allow zero byte
      _db_size = n;
    }

    bool checkFileIsExisted(void)
    {
      return getFileLength(*dentry) != 0;
    }
    
    fdownload(const fdownload &robj) noexcept(false)
      {
	__doCopy(robj);
      }

    fdownload &operator=(const fdownload &robj) noexcept(false);
      {
	if (this == &robj)
	  return *this;
	__doCopy(robj);
	return *this;
      }

    fdownload(fdownload &&robj)
      {
	__doMove(robj);
      }

    fdownload &operator=(fdownload &&robj)
      {
	__doMove(robj);
	return *this;
      }

    bool operator bool()
    {
      return _socket >= 0;
    }

    void releaseLink(void)
    {
      if(_socket < 0)  //  close a fd which is less than 0 is nonsence.
	return;
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

    void __doCopy(const fdownload &robj) noexcept(false)
    {
      setDownloadBufferSize(robj._db_size);
      if (robj._download_buffer) {
	try {
	  __allocateDownloadBuffer();
	} catch (std::bad_alloc &x) {
	  throw FDOWNLOAD_ERR_COPY;
	}
	memcpy(_download_buffer, robj._download_buffer, _db_size);
      }

      if (_socket >= 0 && robj._socket != _socket)
	releaseLink();
      _socket = dup(robj._socket);    //  call dup to copy socket might couse copy semantic error.
                                      //  but there is no way to ensure that @robj will not be destroyed after
                                      //  copy completed.
      *_filename = *robj._filename;
      *_directory = *robj._directory;
      *_dentry = *robj._dentry;
    }

    void __doMove(fdownload &&robj) noexcept
    {
      if (_socket >= 0 && _socket != robj._socket)
	releaseLink();
      _socket = dup(robj._socket);

      _filename = robj._filename;
      _directory = robj._directory;
      _dentry = robj._dentry;
      _db_size = robj._db_size;
      if (_download_buffer)
	delete[] _download_buffer;
      _download_buffer = robj._download_buffer;

      robj.releaseLink();
      robj._filename = nullptr;
      robj._directory = nullptr;
      robj._dentry = nullptr;
      robj._db_size = 0;
      robj._download_buffer = nullptr;
    }

    void __allocateDownloadBuffer(void) noexcept(false)
    {
      char *tempPtr = new char[_db_size];

      //  rcu
      if (!tempPtr)
	throw std::bad_alloc();
      if (_download_buffer)
	delete[] _download_buffer;
      _download_buffer = tempPtr;
    }

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

#endif
