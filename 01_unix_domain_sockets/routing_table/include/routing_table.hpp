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
#include <string>
#include <array>
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
	 * @param buffer - the std::array buffer to write the serialized data into
	 * @return size_t - the number of bytes written to the buffer
	 */
	template <std::size_t N>
	static size_t serialize(const routing_table_entry &entry,
				std::array<uint8_t, N>& buffer)
	{
		const uint32_t total_bytes = entry.size() + 5 * sizeof(uint32_t);

		size_t offset = 0;
		uint32_t size_tmp = 0;

		if (total_bytes > buffer.size()) {
			throw std::runtime_error(
				"Buffer too small for routing table entry");
		}

		buffer.fill(0);

		std::memcpy(buffer.data() + offset, &total_bytes, sizeof(total_bytes));
		offset += sizeof(total_bytes);

		size_tmp = sizeof(entry.destination_ip);
		std::memcpy(buffer.data() + offset, &size_tmp, sizeof(size_tmp));
		offset += sizeof(size_tmp);

		std::memcpy(buffer.data() + offset, entry.destination_ip,
			sizeof(entry.destination_ip));
		offset += sizeof(entry.destination_ip);

		size_tmp = sizeof(entry.gateway_ip);
		std::memcpy(buffer.data() + offset, &size_tmp, sizeof(size_tmp));
		offset += sizeof(size_tmp);

		std::memcpy(buffer.data() + offset, entry.gateway_ip,
			sizeof(entry.gateway_ip));
		offset += sizeof(entry.gateway_ip);

		size_tmp = sizeof(entry.destination_mask);
		std::memcpy(buffer.data() + offset, &size_tmp, sizeof(size_tmp));
		offset += sizeof(size_tmp);

		std::memcpy(buffer.data() + offset, &entry.destination_mask,
			sizeof(entry.destination_mask));
		offset += sizeof(entry.destination_mask);

		size_tmp = entry.oif.size();
		std::memcpy(buffer.data() + offset, &size_tmp, sizeof(size_tmp));
		offset += sizeof(size_tmp);

		std::memcpy(buffer.data() + offset, entry.oif.c_str(), entry.oif.size());

		return total_bytes;
	}

	/**
	 * @brief Deserialize a std::array buffer into a routing table entry
	 *
	 * @param buffer - the std::array buffer containing the serialized data
	 * @param entry - the routing table entry to populate
	 * @return size_t - the number of bytes read from the buffer
	 */
	template <std::size_t N>
	static size_t deserialize(const std::array<uint8_t, N>& buffer,
				routing_table_entry &entry)
	{
		size_t offset = 0;
		uint32_t size_tmp = 0;
		uint32_t total_size = 0;

		std::memcpy(&total_size, buffer.data() + offset, sizeof(total_size));
		offset += sizeof(total_size);

		std::memcpy(&size_tmp, buffer.data() + offset, sizeof(size_tmp));
		offset += sizeof(size_tmp);

		std::memcpy(entry.destination_ip, buffer.data() + offset,
			    size_tmp);
		offset += sizeof(destination_ip);

		std::memcpy(&size_tmp, buffer.data() + offset, sizeof(size_tmp));
		offset += sizeof(size_tmp);

		std::memcpy(entry.gateway_ip, buffer.data() + offset,
			    size_tmp);
		offset += sizeof(gateway_ip);

		std::memcpy(&size_tmp, buffer.data() + offset, sizeof(size_tmp));
		offset += sizeof(size_tmp);

		std::memcpy(&entry.destination_mask, buffer.data() + offset,
			    size_tmp);
		offset += sizeof(destination_mask);

		std::memcpy(&size_tmp, buffer.data() + offset, sizeof(size_tmp));
		offset += sizeof(size_tmp);

		entry.oif.resize(size_tmp);
		std::memcpy(&entry.oif[0], buffer.data() + offset, size_tmp);

		return total_size;
	}
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
	const routing_table_entry& at(const std::string& key) const
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
	template <std::size_t N>
	static size_t serialize(routing_table &table, std::array<uint8_t, N>& buffer)
	{
		// Total size consists of:
		// - 4 bytes for total size
		// - 4 bytes for number of entries
		// - size of each serialized entry (will be calculated later)
		uint32_t total_size = 4 + 4;
		size_t offset = 4;  // buffer offset in bytes - start with number of entries

		const uint32_t num_entries = static_cast<uint32_t>(table.size());
		std::memcpy(buffer.data() + offset, &num_entries, num_entries);
		offset += sizeof(num_entries);

		size_t bytes_written = 0;
		std::array<uint8_t, 1024> entry_buffer;  // do not expect a huge buffers so far
		for (const auto& [key, entry] : table.table) {
			// @note: it is not necessary to serialize the keys, since they
			// are already included in each entry
			bytes_written = routing_table_entry::serialize(entry, entry_buffer);
			std::memcpy(buffer.data() + offset, entry_buffer.data(),
				    bytes_written);
			total_size += bytes_written;
			offset += bytes_written;
		}
		std::memcpy(buffer.data(), &total_size, sizeof(total_size));

		return static_cast<size_t>(total_size);
	}

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
	template <std::size_t N>
	static size_t deserialize(const std::array<uint8_t, N>& buffer,
				  routing_table &table)
	{
		uint32_t total_size = 0;
		uint32_t num_entries = 0;
		size_t offset = 0;

		std::memcpy(&total_size, buffer.data() + offset, sizeof(total_size));
		offset += sizeof(total_size);

		std::memcpy(&num_entries, buffer.data() + offset, sizeof(num_entries));
		offset += sizeof(num_entries);

		routing_table_entry entry;
		size_t entry_cnt = 0;
		uint32_t entry_size = 0;
		std::array<uint8_t, 1024> entry_buffer = {0};
		while(entry_cnt < num_entries) {
			entry_buffer.fill(0);  // clear the entry buffer
			std::memcpy(&entry_size, buffer.data() + offset, sizeof(entry_size));
			std::memcpy(entry_buffer.data(), buffer.data() + offset, entry_size);

			const auto bytes_read =
				routing_table_entry::deserialize(entry_buffer, entry);
			table.create_entry(entry);

			offset += bytes_read;
			entry_cnt++;
		}

		return static_cast<size_t>(offset);
	}

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
	std::map<std::string, routing_table_entry> table;  // store routing table entries
};
}  // namespace RTM
