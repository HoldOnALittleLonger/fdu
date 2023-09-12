//module;

#ifndef _MISC_H_
#define _MISC_H_

#include <unistd.h>
#include <fcntl.h>

//export module FOPS;

//import <string>;
//import <fstream>;
//import <exception>;

#include <string>
#include <fstream>
#include <exception>

#include <utility>


extern "C" {
  int open(const char *, int, ...);
  ssize_t read(int, void *, size_t);
  ssize_t write(int, const void *, size_t);
  off_t lseek(int, off_t, int);
  int close(int);
  int stat(const char *, struct stat *);
  int fcntl(int, int, ...);
  int dup(int);
}


//export class fops {
class fops {
public:
  virtual int open(const char *path, int flags)
  {
    return ::open(path, flags);
  }

  virtual std::fstream *open(const std::string &path, std::ios_base::openmode mode)
  {
    std::fstream *f(new std::fstream);
    f->open(path, mode);
    if (f->is_open()) {
      return f;
    }
    delete f;
    return nullptr;
  }

  //  __basic_file is protected member in fstream
  //  but fstream::rdbuf() could returns the object.
  virtual int retriveFdForFstream(std::fstream &f)
  {
    auto retrive = [](std::filebuf &fb) -> int {
      class helper : public std::filebuf {
      public:
	int handle(void) { return _M_file.fd(); }
      };

      return dynamic_cast<helper&>(fb).handle();
    };

    int fd(-1);
    try {
      fd = retrive(*f.rdbuf());
    } catch(std::bad_cast &x) {
      fd = -1;
    }
    return fd;
  }

  virtual int dup(int oldfd)
  {
    return ::dup(oldfd);
  }

  virtual ssize_t read(int fd, char *buffer, std::size_t n)
  {
    return ::read(fd, buffer, n);
  }

  virtual ssize_t read(std::fstream &f, char *buffer, std::size_t n)
  {
    f.read(buffer, n);
    return f.gcount();
  }

  virtual ssize_t write(int fd, const char *buffer, std::size_t n)
  {
    return ::write(fd, buffer, n);
  }

  virtual ssize_t write(std::fstream &f, const char *buffer, std::size_t n)
  {
    f.write(buffer, n);
    return f.gcount();
  }

  virtual off_t lseek(int fd, off_t offset, int whence)
  {
    return ::lseek(fd, offset, whence);
  }

  virtual typename std::fstream::__ostream_type &seekp(std::fstream &f, typename std::fstream::off_type offtype, std::ios_base::seekdir whence)
  {
    return f.seekp(offtype, whence);
  }

  virtual typename std::fstream::__istream_type &seekg(std::fstream &f, typename std::fstream::off_type offtype, std::ios_base::seekdir whence)
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
    return getFileLength(path.c_str());
  }

  void getRDFileLock(int fd)
  {
    getFileLock(fd, F_RDLCK);
  }

  void getWRFileLock(int fd)
  {
    getFileLock(fd, F_WRLCK);
  }

  //  locker always lock all 
  void releaseFileLock(int fd)
  {
    struct flock lock = {
      .l_type = F_UNLCK,
      .l_whence = SEEK_SET,
      .l_start = 0,
      .l_len = 0
    };
    fcntl(fd, F_SETLKW, &lock);
    return;
  }

private:
  //  locker always lock all
  void getFileLock(int fd, short type)
  {
    struct flock lock = {
      .l_type = type,
      .l_whence = SEEK_SET,
      .l_start = 0,
      .l_len = 0
    };
    fcntl(fd, F_SETLKW, &lock);
    return;
  }
};

//module;

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//export module NOPS;

extern "C" {
  int socket(int, int, int);
  int bind(int, const struct sockaddr *, socklen_t);
  int listen(int, int);
  int accept(int, struct sockaddr *, socklen_t *);
  int connect(int, const sockaddr *, socklen_t);
  ssize_t send(int, const void *, size_t, int);
  ssize_t recv(int, void *, size_t, int);
  uint32_t htonl(uint32_t);
  int inet_aton(const char *, struct in_addr *);
  char *inet_ntoa(struct in_addr);
}

//export class nops {
class nops {
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

  virtual int connect(int socket, const struct sockaddr *addr, socklen_t addrlen)
  {
    return ::connect(socket, addr, addrlen);
  }

  virtual ssize_t send(int socket, const void *buf, std::size_t len, int flags)
  {
    return ::send(socket, buf, len, flags);
  }

  virtual ssize_t recv(int socket, void *buf, std::size_t len, int flags)
  {
    return ::recv(socket, buf, len, flags);
  }

  virtual uint32_t htonl(uint32_t value)
  {
    return ::htonl(value);
  }

  virtual uint32_t ntohl(uint32_t value)
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

//module;

#include <cstring>

//export module GENERAL_API;
//import FOPS;
//import NOPS;

//export
enum { FDU_DEFAULT_BUFFER_SIZE = 4096u };

//  is-a
//export
class general_api : public fops, public nops { };

//export module REQUEST_RESPODING;

//export
enum { FP_LENGTH = 256 };

//export
enum {
  FDU_FILE_READY,
  FDU_FILE_NOEXIST
};

//export
struct request_header {
  std::size_t fp_length;
  char file_path[FP_LENGTH];
};

//export
struct responding_header {
  std::size_t file_length;
  int state;  //  FDU_FILE_READY | FDU_FILE_NOEXIST
};

//module;

#include <cstdlib>

//export module GENERIC_IPV4_TCP;
//import GENERICAL_API;
//import <memory>;  //  RAII

#include <memory>

extern "C" {
  void *malloc(size_t);
}

//export class generic_ipv4_tcp final : private general_api {
class generic_ipv4_tcp final : private general_api {
 public:
  
  enum GIP4TCP_ERROR {
    GIP4TCP_ERR_CONSTRUCT = 1
  };
  using gip4tcp_err = GIP4TCP_ERROR;

  generic_ipv4_tcp() noexcept(false)
  {
    _port = 0;
    _ipv4_addr_str = std::move(decltype(_ipv4_addr_str){new std::string});
    _ipv4_addr = static_cast<decltype(_ipv4_addr)>(malloc(sizeof(struct sockaddr)));
    if (!_ipv4_addr)
      throw GIP4TCP_ERR_CONSTRUCT;
  }

  ~generic_ipv4_tcp()
  {
    free(_ipv4_addr);
  }

  bool setAddress(const std::string &netaddr)
  {
    struct sockaddr_in *ipv4_addr((struct sockaddr_in *)_ipv4_addr);
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
    ((struct sockaddr_in *)_ipv4_addr)->sin_port = htonl(_port);
  }

  auto getPort(void) const
  {
    return _port;
  }

  generic_ipv4_tcp(const generic_ipv4_tcp &robj)
  {
    __do_copy(robj);
  }

  generic_ipv4_tcp(generic_ipv4_tcp &&robj)
  {
    __do_move(std::forward<generic_ipv4_tcp &&>(robj));
  }

  generic_ipv4_tcp &operator=(const generic_ipv4_tcp &robj)
  {
    __do_copy(robj);
    return *this;
  }

  generic_ipv4_tcp &operator=(generic_ipv4_tcp &&robj) noexcept
  {
    __do_move(std::forward<generic_ipv4_tcp &&>(robj));
    return *this;
  }

 private:
  struct sockaddr *_ipv4_addr;
  std::unique_ptr<std::string> _ipv4_addr_str;
  unsigned long _port;  

  void __do_copy(const generic_ipv4_tcp &from)
  {
    *_ipv4_addr = *from._ipv4_addr;
    *_ipv4_addr_str = *from._ipv4_addr_str;
    _port = from._port;
  }

  void __do_move(generic_ipv4_tcp &&from) noexcept
  {
    _ipv4_addr = from._ipv4_addr;
    _ipv4_addr_str = std::forward<decltype(_ipv4_addr_str) &&>(from._ipv4_addr_str);
    _port = from._port;

    from._ipv4_addr = nullptr;
    from._ipv4_addr_str = nullptr;
    from._port = 0;
  }
};

#endif
