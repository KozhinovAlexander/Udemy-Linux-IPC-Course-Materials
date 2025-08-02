/*
 * Copyright (c) 2025 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 * @brief Routig Table Manager (RTM) Server implementation
 */

#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>

#include <chrono>
#include <iostream>
#include <rtm_server.hpp>

using namespace RTM;

#define RTM_SERVER_MAX_CONNECTIONS	20


rtm_server::rtm_server() {
	rtm_table.clear();
}

rtm_server::~rtm_server() {
	close_server();
	stop();
}

int rtm_server::generate_socket_foder_name(std::string& sname) {
	auto ret = 0;

	sname = "/tmp/rtm_server.XXXXXX";
	int fd = mkostemps(const_cast<char*>(sname.c_str()), 0, 0);
	if (fd == -1) {
		perror("mkostemps");
		return EXIT_FAILURE;
	}

	std::cout << "INF: " << "Generated socket folder name: "
		  << sname << std::endl;

	return ret;
}

void rtm_server::add_to_monitored_fd_set(const int skt_fd) {
	monitored_fd_set.insert(skt_fd);
}

void rtm_server::add_to_client_pid_set(const pid_t pid) {
	client_pid_set.insert(pid);
}

void rtm_server::remove_from_monitored_fd_set(const int skt_fd) {
	monitored_fd_set.erase(skt_fd);
}

void rtm_server::remove_from_client_pid_set(const pid_t pid) {
	client_pid_set.erase(pid);
}

/* Inform clients to flush their routing tables and mac lists*/
void rtm_server::flush_clients() {
	for (auto it = client_pid_set.begin(); it != client_pid_set.end();) {
		int pid = *it;
		kill(pid, SIGUSR1);
	}
}

/*Get the numerical max value among all FDs which server is monitoring*/
int rtm_server::get_max_fd() {
	int max = -1;
	for (auto it = monitored_fd_set.begin(); it != monitored_fd_set.end();) {
		if(*it > max) {
			max = *it;
		}
	}
	return max;
}

int rtm_server::close_server() {
	// int synchronized = WAIT;
	int loop, ret = 0;

	/* Notify clients: */
	// sync_msg_t sync_msg;
	// sync_msg.op_code = NONE;
	// for(i = 2; i < MAX_CLIENTS; i++){
	// for (auto it = monitored_fd_set.begin(); it != monitored_fd_set.end();) {
	// 	const int comm_socket_fd = *it;
	// 	// write(comm_socket_fd, &sync_msg, sizeof(sync_msg_t));
	// 	// write(comm_socket_fd, &synchronized, sizeof(int));
	// 	// write(comm_socket_fd, &loop, sizeof(int));
	// }
	rtm_table.clear();

	/* Clean up resources */
	ret = close(connection_socket);
	if (ret) {
		perror("close");
	}

	remove_from_monitored_fd_set(connection_socket);

	ret = unlink(socket_name.c_str());
	if (ret) {
		perror("unlink");
	}

	return ret;
}

void rtm_server::signal_handler(const int signal_num)
{
	if(signal_num == SIGINT) {
		close_server();
		exit(0);
	}
}

int rtm_server::init() {
	auto ret = 0;
	struct sockaddr_un name;

	/* Create socket pair */
	ret = generate_socket_foder_name(socket_name);
	if (ret) {
		return ret;
	}

	add_to_monitored_fd_set(0);

	/* In case the program exited inadvertently on the last run,
	 * remove the socket.
	 */
	unlink(socket_name.c_str());

	/* Create local socket */
	connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (connection_socket == -1) {
		perror("socket");
		return EXIT_FAILURE;
	}

	/* Initialize socket name struct */
	memset(&name, 0, sizeof(struct sockaddr_un));

	/* Specify the socket cridentials */
	name.sun_family = AF_UNIX;
	strncpy(name.sun_path, socket_name.c_str(), sizeof(name.sun_path) - 1);

	/* Bind socket to socket name */
	ret = bind(connection_socket, (const struct sockaddr *)&name,
		   sizeof(name));
	if (ret == -1) {
		perror("bind");
		return EXIT_FAILURE;
	}

	/* Prepare for accepting connections */
	ret = listen(connection_socket, RTM_SERVER_MAX_CONNECTIONS);
	if (ret == -1) {
		perror("listen");
		return EXIT_FAILURE;
	}

	add_to_monitored_fd_set(connection_socket);

	// signal(SIGINT, signal_handler);  //register signal handlers

	std::cout << "INF: " << "Succesfully initialized rtm_server"
		  << std::endl;

	return ret;
}

void rtm_server::table_input_runner(std::atomic<bool>& stop_request,
				    std::mutex& rtm_table_mtx,
				    routing_table& rtm_table) {
	std::cout << "INF: " << "Starting table input thread..." << std::endl;
	auto show_help = []()
	{
		std::cout << "Please select from the following options:" << std::endl
			  << "\t--help shows this help" << std::endl
			  << "\t--create <destination IP> <mask (0-32)> <gateway IP> <OIF>" << std::endl
			  << "\t--update <destination IP> <mask (0-32)> <new gateway IP> <new OIF>" << std::endl
			  << "\t--delete <destination IP> <mask (0-32)>" << std::endl;
		std::cout.flush();
	};

	auto wait_for_input = [](int timeout_ms) -> bool {
		struct pollfd fds[1];
		fds[0].fd = STDIN_FILENO;
		fds[0].events = POLLIN;
		return poll(fds, 1, timeout_ms) > 0;
	};

	using namespace std::literals;
	constexpr auto polling_time_ms = 10;
	while(stop_request.load() == false) {
		if(wait_for_input(polling_time_ms)) {
			std::cout << "rtm_server is waiting for input..." << std::endl;
			std::cout.flush();
			std::string input;
			std::getline(std::cin, input);
			// std::cout << "You entered: [" << input << "]\n";  // could be used for debug
			cud_opcode op = RTM_NONE;
			routing_table_entry entry;
			if(input.starts_with("--help"sv)) {
				show_help();
			} else if(input.starts_with("--create"sv)) {
				op = RTM_CREATE;
			} else if(input.starts_with("--update"sv)) {
				op = RTM_UPDATE;
			} else if(input.starts_with("--delete"sv)) {
				op = RTM_DELETE;
			}

			if (op != RTM_NONE) {
				const std::lock_guard<std::mutex> lock(rtm_table_mtx);
				rtm_table.modify_entry(op, entry);
			}
			std::cout << rtm_table.to_string() << std::endl;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(polling_time_ms));
		}
	}
	std::cout << "INF: " << "Gracefully exit table input thread." << std::endl;
}

int rtm_server::start() {
	int ret = 0;

	ret = init();
	if(ret) {
		return ret;
	}

	std::cout << "INF: " << "Starting rtm_server..." << std::endl;

	table_input_thread_stop_request.store(false);
	table_input_thread = std::make_unique<std::jthread>(table_input_runner,
		std::ref(table_input_thread_stop_request),
		std::ref(rtm_table_mtx), std::ref(rtm_table)
	);

	server_stopped.store(false);
	std::cout << "INF: " << "Succesfull started rtm_server." << std::endl;

	return ret;
}

void rtm_server::stop() {
	if (server_stopped.load() == true) {
		return;
	}
	std::cout << "INF: " << "Stopping rtm_server..." << std::endl;

	if (table_input_thread.get() != nullptr) {
		table_input_thread_stop_request.store(true);
		table_input_thread->join();
		table_input_thread.release();
	}
	std::cout << "INF: " << "Stopped rtm_server." << std::endl;
	server_stopped.store(true);
}
