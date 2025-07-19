/*
 * Copyright (c) 2025 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 * @brief Routig Table Implements follwoing table:
 *
 *  | Destination  | Gateway IP | OIF  |
 *  |--------------|------------|------|
 *  | 122.1.1.1/32 | 10.1.1.1   | eth0 |
 *  | 130.1.1.0/24 | 10.1.1.1   | eth1 |
 *  | 157.0.2.3/24 | 20.1.1.1   | eth2 |
 *
 */

#pragma once

#include <cstdint>

namespace RTM {

/**
 * @brief Routing Table Management Operation Codes
 */
enum
{
	RTM_CREATE = 0,
	RTM_UPDATE,
	RTM_DELETE,
} cud_opcode_t;


/**
 * @brief Routing Table Entry Structure
 *
 */
struct routing_table_entry_t
{
	uint8_t destination_mask;  // CIDR notation (e.g., 24 for /24)
	uint8_t destination_ip[4]; // IPv4 address in dotted decimal format
	uint8_t gateway_ip[4];     // IPv4 address in dotted decimal format
	char oif[16];              // Output Interface (e.g., "eth0", "eth1", etc.)
};


class routing_table {
public:
	routing_table();
	~routing_table();

	int insert();
	int update();
	int delete();
private:
};
}  // namespace RTM
