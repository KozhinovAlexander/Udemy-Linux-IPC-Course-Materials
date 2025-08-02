/*
 * Copyright (c) 2025 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 * @brief Routig Table Manager (RTM) Server is in a charge of a Level 3 routing table.
 *	Its responsibility is to maintain the L3 routing table and sned
 *	notification of any change in the routing table contents to
 *	connected clients.
 *	State of routing table needs to be synchronised across all connected
 *	clients at at any point of time.
 *
 * - Routig Table Manager (RTM) Server sends CUD (Create, Update, Delete) notifications
 * to all connected client processes.
 * - Routig Table Manager (RTM) Server maintains a Level 3 routing table.
 *    Sample of RTM table entry:
 *    | Destination  | Gateway IP | OIF  |
 *    |--------------|------------|------|
 *    | 122.1.1.1/32 | 10.1.1.1   | eth0 |
 *    | 130.1.1.0/24 | 10.1.1.1   | eth1 |
 *    | 157.0.2.3/24 | 20.1.1.1   | eth2 |
 *
 * - Whenever the user perform any CUD operstion on routing table, RTM server
 * sync that particular peartion to all connected clients.
 * - When a new client connects to RTM server, server sends the entire table state
 * to this newly connected client
 * - At any given point of time the routing table must be identical on RTM server
 * and all connected clients.
 *
 */

#pragma once

#include <sys/un.h>

#include <set>
#include <tuple>
#include <mutex>
#include <string>
#include <thread>
#include <memory>
#include <routing_table.hpp>

namespace RTM {

class rtm_server final {
public:
	rtm_server();
	~rtm_server();

	/**
	 * @brief Starts the server
	 *
	 * @return int - a non-zero error code or zero if ok
	 */
	int start();

	/**
	 * @brief Stops the server
	 *
	 */
	void stop();
private:
	/**
	 * @brief Initialize the server
	 *
	 * @return int - a non-zero error code or zero if ok
	 */
	int init();

	/**
	 * @brief Create a socket pair object
	 *
	 * @param sname - allocated socket name will be written here
	 *
	 * @return int - a non-zero error code or zero if ok
	 */
	int generate_socket_foder_name(std::string& sname);

	/**
	 * @brief Add socket file descriptor to monitored file-descriptor set.
	 *
	 * @param skt_fd - socket file descriptor to be added
	 */
	void add_to_monitored_fd_set(int skt_fd);

	/**
	 * @brief Add a new pid to the client_pid_set array
	 *
	 * @param pid - process id to be added
	 */
	void add_to_client_pid_set(const pid_t pid);

	/**
	 * @brief Remove the FD from monitored_fd_set array
	 *
	 * @param skt_fd - socket file descriptor
	 */
	void remove_from_monitored_fd_set(const int skt_fd);

	/**
	 * @brief Remove the pid from client_pid_set array
	 *
	 * @param pid - process id
	 */
	void remove_from_client_pid_set(const pid_t pid);

	/**
	 * @brief Inform clients to flush their routing tables and mac lists
	 *
	 */
	void flush_clients();

	/**
	 * @brief Get the max fd object which server is monitoring
	 *
	 * @return int - max fd object which server is monitoring
	 */
	int get_max_fd();

	/**
	 * @brief Break out of main infinite loop and inform clients of shutdown
	 *        to exit cleanly.
	 *
	 * @param signal_num - signal to be used
	 */
	void signal_handler(const int signal_num);

	/**
	 * @brief Close server
	 *
	 * @return int - 0 if okay, else error code
	 */
	int close_server();

	/**
	 * @brief Table input runner is used to modify RTM table from CLI
	 *
	 * @param stop_request - the thread will exit gracefully if set true
	 * @param rtm_table_mtx - routing table mutex reference
	 * @param rtm_table - routing table reference
	 */
	static void table_input_runner(std::atomic<bool>& stop_request,
			std::mutex& rtm_table_mtx,
			routing_table& rtm_table);

	routing_table rtm_table;
	std::mutex rtm_table_mtx;  // rtm_table mutex - protects rtm_table

	int connection_socket;
	int data_socket;
	std::string socket_name;

	/* An array of File descriptors which the server process is maintaining
	 * in order to talk with the connected clients. Master skt FD is also
	 * a member of this array
	 */
	std::set<int> monitored_fd_set;
	std::set<pid_t> client_pid_set;  // array of client process id's

	std::atomic<bool> server_stopped;

	std::atomic<bool> table_input_thread_stop_request;
	std::unique_ptr<std::jthread> table_input_thread;
};

}  // namespace RTM
