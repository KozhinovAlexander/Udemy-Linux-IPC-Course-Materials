/*
 * Copyright (c) 2025 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 * @brief Routig Table Implements follwoing table:
 *
 *  | Destination (Key) | Destination  | Gateway IP | OIF  |
 *  |-------------------|--------------|------------|------|
 *  | 122.1.1.1/32      | 122.1.1.1/32 | 10.1.1.1   | eth0 |
 *  | 130.1.1.0/24      | 130.1.1.0/24 | 10.1.1.1   | eth1 |
 *  | 157.0.2.3/24      | 157.0.2.3/24 | 20.1.1.1   | eth2 |
 *
 */

#pragma once

#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <string>
#include <vector>
#include <span>
#include <map>

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
	union {
		// IPv4 address in dotted decimal format
		uint8_t destination_ip[4];
		uint32_t destination_ip_u32;
	};
	union {
		// IPv4 address in dotted decimal format
		uint8_t gateway_ip[4];
		uint32_t gateway_ip_u32;
	};
	uint8_t destination_mask;    // CIDR notation (e.g., 24 for /24)
	std::string oif;             // Output Interface (e.g., "eth0", "eth1", etc.)

	/**
	 * @brief Get the size of the routing table entry
	 *
	 * @return size_t - the size in bytes of the routing table entry
	 */
	size_t size() const;

	/**
	 * @brief Comparison operator for routing table entries
	 *
	 * @param other - the routing table entry to compare with
	 * @return true if the entries are equal, false otherwise
	 */
	bool operator==(const routing_table_entry& other) const;

	/**
	 * @brief Inequality operator for routing table entries
	 *
	 * @param other - the routing table entry to compare with
	 * @return true - if the entries are not equal, false otherwise
	 * @note This operator is the negation of the equality operator.
	 */
	bool operator!=(const routing_table_entry& other) const
	{
		return !(this->operator==(other));
	}

	/**
	 * @brief Convert the destination IP of the routing table entry to a string
	 *
	 * @param entry - the routing table entry to convert
	 * @return std::string - the string representation of the destination IP
	 */
	static std::string destination_ip2str(const routing_table_entry& entry);

	/**
	 * @brief Serialize the routing table entry into a std::array buffer
	 *
	 * Serialization format:
	 * 	<total_bytes>
	 * 	<dest_ip_bytes><dest_ip>
	 * 	<gateway_ip_bytes><gateway_ip>
	 * 	<mask_bytes><mask>
	 * 	<oif_bytes><oif>
	 * @note: bytes sizes are given as 32 bit unsigned integers
	 *
	 * @param entry - the routing table entry to serialize
	 * @param buffer - the buffer to write the serialized data into
	 * @return size_t - the number of bytes written to the buffer
	 */
	static size_t serialize(const routing_table_entry &entry,
				std::vector<uint8_t>& buffer);

	/**
	 * @brief Deserialize a std::array buffer into a routing table entry
	 *
	 * @param buffer - the buffer containing the serialized data
	 * @param entry - the routing table entry to populate
	 * @return size_t - the number of bytes read from the buffer
	 */
	static size_t deserialize(const std::vector<uint8_t>& buffer,
				  routing_table_entry &entry);
};


/**
 * @brief Routing Table Class
 *
 */
class routing_table {
public:
	routing_table() {};
	~routing_table() {};

	/**
	 * @brief Create a entry object
	 *
	 * @param entry - the routing table entry to create
	 */
	void create_entry(const routing_table_entry &entry);

	/**
	 * @brief Update a routing table entry
	 *
	 * @param entry - the routing table entry to update.
	 * @note The destination IP is used as the key for update.
	 */
	void update_entry(const routing_table_entry &entry);

	/**
	 * @brief Delete a routing table entry
	 *
	 * @param entry - the routing table entry to delete
	 * @note The destination IP is used as the key for deletion.
	 * Other membders of the entry are ignored.
	 * @note This function could be potentially slow but acceptable for
	 * this project.
	 */
	void delete_entry(const routing_table_entry &entry);

	/**
	 * @brief Get a routing table entry by key
	 *
	 * @param key 	- the key of the entry to retrieve
	 * @return const routing_table_entry& - the reference to routing table entry
	 */
	const routing_table_entry& at(const uint32_t& key) const
	{
		return this->table.at(key);
	}

	/**
	 * @brief Clear the routing table
	 *
	 */
	void clear()
	{
		this->table.clear();
	}

	/**
	 * @brief Get the number of entries in the routing table
	 *
	 * @return size_t - the number of entries in the routing table
	 */
	size_t size() const
	{
		return this->table.size();
	}

	/**
	 * @brief Check if the routing table is empty
	 *
	 * @return true if the routing table is empty, false otherwise
	 */
	bool empty() const
	{
		return this->table.empty();
	}

	/**
	 * @brief Serialize the routing table into a buffer
	 *
	 * @param table - the routing table to serialize
	 * @param buffer - the buffer to write the serialized data into
	 * @return size_t - the number of bytes written to the buffer
	 *
	 * @note The serialization format is:
	 * 	<total_size_bytes><num_entries>
	 * 	<serialized_entry_1>...<serialized_entry_n>
	 * @note: each entry has its own serialization size at the beginnig
	 * @note: all sizes are given as 32 bit unsigned integers
	 */
	static size_t serialize(routing_table &table, std::vector<uint8_t> &buffer);

	/**
	 * @brief Deserialize a routing table from a buffer
	 *
	 * @param table - the routing table to serialize
	 * @param buffer - the buffer to write the serialized data into
	 * @return size_t - the number of bytes written to the buffer
	 *
	 * @note The serialization format is:
	 * 	<total_size_bytes><num_entries>
	 * 	<serialized_entry_1>...<serialized_entry_n>
	 * @note: each entry has its own serialization size at the beginnig
	 * @note: all sizes are given as 32 bit unsigned integers
	 */
	static size_t deserialize(const std::vector<uint8_t>& buffer,
				  routing_table &table);

	/**
	 * @brief Comparison operator for routing table entries
	 *
	 * @param other - the routing table to compare with
	 * @return true if the entries are equal, false otherwise
	 */
	bool operator==(const routing_table& other) const;

	/**
	 * @brief Inequality operator for routing table
	 *
	 * @param other - the routing table to compare with
	 * @return true - if the tables are not equal, false otherwise
	 * @note This operator is the negation of the equality operator.
	 */
	bool operator!=(const routing_table& other) const
	{
		return !(this->operator==(other));
	}

	/**
	 * @brief Convert the routing table to a string representation
	 *
	 * @return std::string& - the string representation of the routing table
	 */
	std::string to_string() const;
private:
	std::map<uint32_t, routing_table_entry> table;  // store routing table entries
};
}  // namespace RTM
