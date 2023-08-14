module;
#include <unistd.h>
#include <fcntl.h>

export module FOPS;

import <string>;
import <fstream>;

export class fops {
public:
  virtual int open(const char *path, int flags)
  {
    return ::open(path, flags);
  }

  virtual std::fstream *open(const string &path, ios_base::open_mode)
  {
    std::fstream *f(new std::fstream);
    f->open(path, open_mode);
    if (f->is_open()) {
      return f;
    }
    delete f;
    return nullptr;
  }

  virtual std::ssize_t read(int fd, char *buffer, std::size_t n)
  {
    return ::read(fd, buffer, n);
  }

  virtual std::ssize_t read(fstream &f, char *buffer, std::size_t n)
  {
    f.read(buffer, n);
    return f.gcount();
  }

  virtual std::ssize_t write(int fd, const char *buffer, std::size_t n)
  {
    return ::write(fd, buffer, n);
  }

  virtual std::ssize_t write(fstream &f, const char *buffer, std::size_t n)
  {
    f.write(buffer, n);
    return f.gcount();
  }

  virtual off_t lseek(int fd, off_t offset, int whence)
  {
    return ::lseek(fd, offset, whence);
  }

  virtual typename std::fstream::__ostream_type seekp(std::fstream &f, typename std::fstream::off_type offtype, ios_base::seekdir whence)
  {
    return f.seekp(offtype, whence);
  }

  virtual typename std::fstream::__istream_type seekg(std::fstream &f, typename std::fstream::off_type offtype, ios_base::seekdir whence)
  {
    return f.seekg(offtype, whence);
  }

  virtual typename std::fstream::pos_type tellp(std::fstream &f)
  {
    return f.tellp();
  }

  virtual typename std::fstream::pos_type tellg(std::fstream &f)
  {
    return f.tellg();
  }

  virtual void close(int fd)
  {
    ::close(fd);
  }

  virtual void close(std::fstream *f)
  {
    if (!f)
      return;
    f->close();
    delete f;
  }

  virtual std::size_t getFileLength(const char *path)
  {
    struct stat file_stat = {0};
    if (stat(path, &file_stat) < 0)
      return 0;
    return file_stat.st_size;
  }

  virtual std::size_t getFileLength(const std::string &path)
  {
    return getFileLength(path.c_str);
  }
};

module;

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

export module NOPS;

export class nops {
public:
  virtual int socket(int domain, int type, int protocol)
  {
    return ::socket(domain, type, protocol);
  }

  virtual int bind(int socket, struct sockaddr *addr, socklen_t addrlen)
  {
    return ::bind(socket, addr, addrlen);
  }

  virtual int listen(int socket, int backlog)
  {
    return ::listen(socket,backlog);
  }

  virtual int accept(int socket, struct sockaddr *addr, socklen_t *addrlen)
  {
    return ::accept(socket, addr, addrlen);
  }

  virtual int connect(int socket, struct sockaddr *addr, socklen_t addrlen)
  {
    return ::connect(socket, addr, addrlen);
  }

  virtual std::ssize_t send(int socket, const void *buf, std::size_t len, int flags)
  {
    return ::send(socket, buf, len, flags);
  }

  virtual std::ssize_t recv(int socket, void *buf, std::size_t len, int flags)
  {
    return ::recv(socket, buf, len, flags);
  }

  virtual unint32_t htonl(unint32_t value)
  {
    return ::htonl(value);
  }

  virtual unint32_t ntohl(unint32_t value)
  {
    return ::ntohl(value);
  }

  virtual int inet_aton(const char *cp, struct in_addr *in)
  {
    return ::inet_aton(cp, in);
  }

  virtual char *inet_ntoa(struct in_addr in)
  {
    return ::inet_ntoa(in);
  }

};

export module GENERAL_API;
import FOPS;
import NOPS;

class general_api : private fops, private nops { };

export module REQUEST_RESPODING;

enum { FP_LENGTH = 256 };

enum {
  FDU_FILE_READY,
  FDU_FILE_NOEXIST
};

export request_header {
  std::size_t fp_length;
  char file_path[FP_LENGTH];
};

export respoding_header {
  std::size_t file_length;
  int state;  //  FDU_FILE_READY | FDU_FILE_NOEXIST
};

module;
export module GENERIC_IPV4_TCP;
import GENERICAL_API;
export class generic_ipv4_tcp final : private generial_api {
 public:
  
  enum GIP4TCP_ERROR {
    GIP4TCP_ERR_CONSTRUCT = 1
  };
  using gip4tcp_err = GIP4TCP_ERROR;

  generic_ipv4_tcp() noexcept(false)
  {
    _port = 0;
    _ipv4_addr = malloc(sizeof(struct sockaddr));
    _ipv4_addr_str = new std::string;
    if (!_ipv4_addr || !_ipv4_addr_str)
      throw GIP4TCP_ERR_CONSTRUCT;
  }

  ~generic_ipv4_tcp()
  {
    free(_ipv4_addr);
    delete _ipv4_addr_str;
  }

  bool setAddress(const std::string &netaddr)
  {
    struct sockaddr_in *ipv4_addr(static_cast<struct sockaddr_in *>(_ipv4_addr));
    if (!inet_aton(netaddr.c_str(), &ipv4_addr->sin_addr))
      return false;
    *_ipv4_addr_str = netaddr;
    return true;
  }

  bool setAddress(const char *netaddr)
  {
    return setAddress(std::string{netaddr});
  }

  auto getAddressStr(void) const
  {
    return *_ipv4_addr_str;
  }

  auto getAddress(void) const
  {
    return *_ipv4_addr;
  }

  void setPort(unsigned long vport)
  {
    _port = vport;
    static_cast<struct sockaddr_in *>(_ipv4_addr)->sin_port = htonl(_port);
  }

  auto getPort(void) const
  {
    return _port;
  }

  void __do_copy(const decltype(*this) &from)
  {
    *_ipv4_addr = *from._ipv4_addr;
    *_ipv4_addr_str = *from._ipv4_addr_str;
    _port = from._port;
  }

  void __do_move(decltype(*this) &&peer) noexcept
  {
    _ipv4_addr = peer._ipv4_addr;
    _ipv4_addr_str = peer._ipv4_addr_str;
    _port = peer._port;

    peer._ipv4_addr = nullptr;
    peer._ipv4_addr_str = nullptr;
    peer._port = 0;
  }

  generic_ipv4_tcp(const decltype(*this) &robj)
  {
    __do_copy(robj);
  }

  generic_ipv4_tcp(decltype(*this) &&robj)
  {
    __do_move(robj);
  }

  generic_ipv4_tcp &operator=(const decltype(*this) &robj)
  {
    __do_copy(robj);
    return *this;
  }

  generic_ipv4_tcp &operator=(const decltype(*this) &&robj) noexcept
  {
    __do_move(robj);
    return *this;
  }

 private:
  struct sockaddr *_ipv4_addr;
  std::string *_ipv4_addr_str;
  unsigned long _port;  
};
