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
#include <cstring>
#include <string>

namespace RTM {

/**
 * @brief Routing Table Management Operation Codes
 */
enum {
	RTM_CREATE = 0,
	RTM_UPDATE,
	RTM_DELETE,
} cud_opcode_t;


/**
 * @brief Routing Table Entry Structure
 *
 */
struct routing_table_entry
{
	uint8_t destination_ip[4];   // IPv4 address in dotted decimal format
	uint8_t gateway_ip[4];       // IPv4 address in dotted decimal format
	uint8_t destination_mask;    // CIDR notation (e.g., 24 for /24)
	std::string oif;             // Output Interface (e.g., "eth0", "eth1", etc.)

	/**
	 * @brief Get the size of the routing table entry
	 *
	 * @return size_t - the size in bytes of the routing table entry
	 */
	size_t size() const;

	/**
	 * @brief Serialize the routing table entry into a buffer
	 *
	 * erialization format:
	 * 	<total_bytes>
	 * 	<dest_ip_bytes><dest_ip>
	 * 	<gateway_ip_bytes><gateway_ip>
	 * 	<mask_bytes><mask>
	 * 	<oif_bytes><oif>
	 * @note: bytes sizes are given as 32 bit unsigned integers
	 *
	 * @param entry - the routing table entry to serialize
	 * @param buffer - the buffer to write the serialized data into
	 */
	static void serialize(const routing_table_entry &entry,
				uint8_t* buffer);

	/**
	 * @brief Deserialize a buffer into a routing table entry
	 *
	 * @param entry - the routing table entry to populate
	 * @param buffer - the buffer containing the serialized data
	 */
	static void deserialize(routing_table_entry &entry,
				const uint8_t* buffer);
};


class routing_table {
public:
	routing_table();
	~routing_table();

	int insert();
	int update();
	// int delete();
private:
};
}  // namespace RTM
