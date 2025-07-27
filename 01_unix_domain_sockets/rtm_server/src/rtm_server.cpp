/*
 * Copyright (c) 2025 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 * @brief Routig Table Manager (RTM) Server implementation
 */

#include <unistd.h>
#include <sys/socket.h>

#include <iostream>
#include <rtm_server.hpp>

using namespace RTM;


rtm_server::rtm_server() {
	memset(&this->name, 0, sizeof(this->name));
	rtm_table.clear();
}

rtm_server::~rtm_server() {
	/* Remove socket pair */
	int ret = rmdir(socket_name.c_str());
	if (ret) {
		perror("rmdir");
	}
}

int rtm_server::create_socket_pair(std::string& sname) {
	auto ret = 0;

	sname = "/tmp/rtm_server.XXXXXX";
	char *dir = mkdtemp(const_cast<char *>(sname.c_str()));
	if (!dir) {
		perror("mkdtemp");
		return EXIT_FAILURE;
	}

	std::cout << "LOG: " << "Succesfully created socket pair: "
		  << sname << std::endl;

	return ret;
}

int rtm_server::init() {
	int ret = 0;

	/* Create socket pair */
	ret = create_socket_pair(socket_name);
	if (ret) {
		return ret;
	}

	/* Create local socket. */
	this->connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (this->connection_socket == -1) {
		perror("socket");
		return EXIT_FAILURE;
	}

	/* Bind socket to socket name. */
	name.sun_family = AF_UNIX;
	strncpy(name.sun_path, socket_name.c_str(), sizeof(name.sun_path) - 1);

	ret = bind(connection_socket, (const struct sockaddr *) &name,
		   sizeof(name));
	if (ret == -1) {
		perror("bind");
		return EXIT_FAILURE;
	}

	return ret;
}

void rtm_server::start() {
}
