#ifndef _FDU_CS_H_
#define _FDU_CS_H_

#include <thread>
#include <memory>
#include <functional>
#include <string>
#include <cstddef>
#include <iostream>

extern int fdu_client(const std::string &file_path_fully, const std::string &download_directory,
		      const std::string &server_address, unsigned long server_port);

extern int fdu_server(const std::string &listen_address, unsigned long port, unsigned int maximum_links);

#endif
