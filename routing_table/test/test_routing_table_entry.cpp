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

TEST(routing_table_entry, destination_ip2str)
{
	routing_table_entry entry;

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

	const std::string dest_ip_str = routing_table_entry::destination_ip2str(entry);
	EXPECT_EQ(dest_ip_str, "123.234.5.6");
}

TEST(routing_table_entry, serialize)
{
	routing_table_entry entry;
	std::vector<uint8_t> buffer;

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

	const uint32_t total_bytes = entry.size() + 5 * sizeof(uint32_t);

	const auto total_bytes_written = routing_table_entry::serialize(entry, buffer);
	EXPECT_EQ(total_bytes_written, total_bytes);

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

TEST(routing_table_entry, compare_equal)
{
	routing_table_entry entry1;
	routing_table_entry entry2;

	entry1.destination_ip[0] = 17;
	entry1.destination_ip[1] = 91;
	entry1.destination_ip[2] = 123;
	entry1.destination_ip[3] = 231;
	entry1.gateway_ip[0] = 21;
	entry1.gateway_ip[1] = 22;
	entry1.gateway_ip[2] = 33;
	entry1.gateway_ip[3] = 44;
	entry1.destination_mask = 31;
	entry1.oif = "foo_eth0";

	entry2.destination_ip[0] = 17;
	entry2.destination_ip[1] = 91;
	entry2.destination_ip[2] = 123;
	entry2.destination_ip[3] = 231;
	entry2.gateway_ip[0] = 21;
	entry2.gateway_ip[1] = 22;
	entry2.gateway_ip[2] = 33;
	entry2.gateway_ip[3] = 44;
	entry2.destination_mask = 31;
	entry2.oif = "foo_eth0";

	EXPECT_EQ(entry1, entry2);
}

TEST(routing_table_entry, compare_not_equal)
{
	routing_table_entry entry1;
	routing_table_entry entry2;

	entry1.destination_ip[0] = 17;
	entry1.destination_ip[1] = 91;
	entry1.destination_ip[2] = 123;
	entry1.destination_ip[3] = 231;
	entry1.gateway_ip[0] = 21;
	entry1.gateway_ip[1] = 22;
	entry1.gateway_ip[2] = 33;
	entry1.gateway_ip[3] = 44;
	entry1.destination_mask = 31;
	entry1.oif = "foo_eth0";

	entry2.destination_ip[0] = 17;
	entry2.destination_ip[1] = 91;
	entry2.destination_ip[2] = 123;
	entry2.destination_ip[3] = 231;
	entry2.gateway_ip[0] = 21;
	entry2.gateway_ip[1] = 22;
	entry2.gateway_ip[2] = 33;
	entry2.gateway_ip[3] = 44;
	entry2.destination_mask = 31;
	entry2.oif = "foo_eth0_bar";

	EXPECT_NE(entry1, entry2);

	entry1.destination_ip[1] = 32;
	EXPECT_NE(entry1, entry2);
	entry1.destination_ip[1] = 91;

	entry1.gateway_ip[3] = 123;
	EXPECT_NE(entry1, entry2);
	entry1.gateway_ip[3] = 44;

	entry1.destination_mask = 03;
	EXPECT_NE(entry1, entry2);
}

TEST(routing_table_entry, deserialize)
{
	routing_table_entry entry1;
	routing_table_entry entry2;
	std::vector<uint8_t> buffer;

	entry1.destination_ip[0] = 17;
	entry1.destination_ip[1] = 91;
	entry1.destination_ip[2] = 123;
	entry1.destination_ip[3] = 231;
	entry1.gateway_ip[0] = 21;
	entry1.gateway_ip[1] = 22;
	entry1.gateway_ip[2] = 33;
	entry1.gateway_ip[3] = 44;
	entry1.destination_mask = 31;
	entry1.oif = "foo_eth0";

	const auto total_bytes_wr = routing_table_entry::serialize(entry1, buffer);
	const auto total_bytes_rd = routing_table_entry::deserialize(buffer, entry2);
	EXPECT_EQ(total_bytes_wr, total_bytes_rd)
		<< "Total bytes written and read do not match";

	for (size_t i = 0; i < sizeof(entry1.destination_ip); ++i) {
		EXPECT_EQ(entry1.destination_ip[i], entry2.destination_ip[i])
			<< "Destination IP byte " << i << " does not match after deserialization";
	}

	for (size_t i = 0; i < sizeof(entry1.gateway_ip); ++i) {
		EXPECT_EQ(entry1.gateway_ip[i], entry2.gateway_ip[i])
			<< "Gateway IP byte " << i << " does not match after deserialization";
	}

	EXPECT_EQ(entry1.destination_mask, entry2.destination_mask);

	EXPECT_EQ(entry1.oif, entry2.oif)
		<< "Output Interface does not match after deserialization";

	EXPECT_EQ(entry1, entry2);
}
