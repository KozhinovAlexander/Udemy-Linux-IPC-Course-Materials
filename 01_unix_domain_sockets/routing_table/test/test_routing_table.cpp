/*
 * Copyright (c) 2025 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 * @brief Routig Table Unit-Tests
 */

#include <gtest/gtest.h>

#include <routing_table.hpp>


using namespace RTM;


TEST(routing_table_entry, size)
{
	routing_table_entry entry;
	EXPECT_EQ(entry.size(), 9);

	entry.oif = "eth0";
	EXPECT_EQ(entry.size(), 13);
}

TEST(routing_table_entry, serialize)
{
	routing_table_entry entry;
	uint8_t buffer[128] = {0};

	entry.destination_ip[0] = 123;
	entry.destination_ip[1] = 234;
	entry.destination_ip[2] = 5;
	entry.destination_ip[3] = 6;
	entry.gateway_ip[0] = 10;
	entry.gateway_ip[1] = 11;
	entry.gateway_ip[2] = 12;
	entry.gateway_ip[3] = 13;
	entry.destination_mask = 24;
	entry.oif = "eth0";

	const uint32_t total_bytes = entry.size() + 4 * sizeof(uint32_t);

	routing_table_entry::serialize(entry, buffer);

	size_t offset = 0;
	EXPECT_EQ(buffer[0], total_bytes);
	offset = sizeof(total_bytes);

	EXPECT_EQ(buffer[offset], sizeof(entry.destination_ip));
	offset += sizeof(uint32_t);

	for (size_t i = 0; i < sizeof(entry.destination_ip); ++i) {
		EXPECT_EQ(buffer[offset + i], entry.destination_ip[i])
			<< "Destination IP byte " << i << " does not match";
	}
	offset += sizeof(entry.destination_ip);

	EXPECT_EQ(buffer[offset], sizeof(entry.gateway_ip));
	offset += sizeof(uint32_t);

	for (size_t i = 0; i < sizeof(entry.gateway_ip); ++i) {
		EXPECT_EQ(buffer[offset + i], entry.gateway_ip[i])
			<< "Gsteway IP byte " << i << " does not match";
	}
	offset += sizeof(entry.gateway_ip);

	EXPECT_EQ(buffer[offset], sizeof(entry.destination_mask));
	offset += sizeof(uint32_t);

	EXPECT_EQ(buffer[offset], entry.destination_mask);
	offset += sizeof(entry.destination_mask);

	EXPECT_EQ(buffer[offset], entry.oif.size());
	offset += sizeof(uint32_t);

	EXPECT_EQ(buffer[offset+0], 'e');
	EXPECT_EQ(buffer[offset+1], 't');
	EXPECT_EQ(buffer[offset+2], 'h');
	EXPECT_EQ(buffer[offset+3], '0');
	offset += entry.oif.size();

	for (size_t i = offset; i < sizeof(buffer); ++i) {
		EXPECT_EQ(buffer[i], 0)
			<< "The rest of the buffer shall be zeroed after the serialization";
	}
}

TEST(routing_table_entry, serialize)
{
	routing_table_entry entry;
	uint8_t buffer[128] = {0};

	routing_table_entry::deserialize(entry, buffer);
}
