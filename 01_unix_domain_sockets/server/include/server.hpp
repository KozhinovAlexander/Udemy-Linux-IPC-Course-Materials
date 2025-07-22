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

namespace RTM {

class Server {
public:
private:
};

}  // namespace RTM
