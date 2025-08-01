/*
 * Copyright (c) 2025 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 * @brief Routig Table Unit-Tests
 */

#include <ranges>
#include <iostream>
#include <gtest/gtest.h>
#include <routing_table.hpp>


using namespace RTM;


class routing_table_test : public ::testing::Test {
public:
	routing_table rt;
protected:
	void SetUp() override
	{
		rt.clear();
	}
	void TearDown() override
	{
		// Code to clean up after tests, if needed
	}
};


TEST_F(routing_table_test, create_entry)
{
	routing_table_entry entry;

	entry.destination_ip[0] = 17;
	entry.destination_ip[1] = 91;
	entry.destination_ip[2] = 123;
	entry.destination_ip[3] = 231;
	entry.gateway_ip[0] = 21;
	entry.gateway_ip[1] = 22;
	entry.gateway_ip[2] = 33;
	entry.gateway_ip[3] = 44;
	entry.destination_mask = 31;
	entry.oif = "ens31";

	const auto num_entries = 1000'000;
	for (size_t i = 0; i < num_entries; ++i) {
		entry.destination_ip[0] = static_cast<uint8_t>(i >> 3*8);
		entry.destination_ip[1] = static_cast<uint8_t>(i >> 2*8);
		entry.destination_ip[2] = static_cast<uint8_t>(i >> 1*8);
		entry.destination_ip[3] = static_cast<uint8_t>(i >> 0*8);
		const auto& entry_key = entry.destination_ip_u32;

		rt.create_entry(entry);

		const auto new_entry = rt.at(entry_key);
		EXPECT_EQ(rt.size(), i+1);

		for (size_t i = 0; i < 4; ++i) {
			EXPECT_EQ(new_entry.destination_ip[i], entry.destination_ip[i]);
			EXPECT_EQ(new_entry.gateway_ip[i], entry.gateway_ip[i]);
		}

		EXPECT_EQ(new_entry.destination_mask, entry.destination_mask);
		EXPECT_EQ(new_entry.oif, entry.oif);
	}
}

TEST_F(routing_table_test, delete_entry)
{
	routing_table_entry entry;

	entry.destination_ip[0] = 17;
	entry.destination_ip[1] = 91;
	entry.destination_ip[2] = 123;
	entry.destination_ip[3] = 231;
	entry.gateway_ip[0] = 21;
	entry.gateway_ip[1] = 22;
	entry.gateway_ip[2] = 33;
	entry.gateway_ip[3] = 44;
	entry.destination_mask = 31;
	entry.oif = "ens31";

	EXPECT_EQ(rt.size(), 0);

	const auto num_entries = 1000'000;
	for (size_t i = 0; i < num_entries; ++i) {
		entry.destination_ip[0] = static_cast<uint8_t>(i >> 3*8);
		entry.destination_ip[1] = static_cast<uint8_t>(i >> 2*8);
		entry.destination_ip[2] = static_cast<uint8_t>(i >> 1*8);
		entry.destination_ip[3] = static_cast<uint8_t>(i >> 0*8);
		rt.create_entry(entry);
	}
	EXPECT_EQ(rt.size(), num_entries);

	for (size_t i = 0; i < num_entries; ++i) {
		entry.destination_ip[0] = static_cast<uint8_t>(i >> 3*8);
		entry.destination_ip[1] = static_cast<uint8_t>(i >> 2*8);
		entry.destination_ip[2] = static_cast<uint8_t>(i >> 1*8);
		entry.destination_ip[3] = static_cast<uint8_t>(i >> 0*8);
		rt.delete_entry(entry);
		EXPECT_EQ(rt.size(), num_entries - i - 1);
	}
	EXPECT_EQ(rt.size(), 0);
}

TEST_F(routing_table_test, comparson_operator)
{
	routing_table rt_other;
	routing_table_entry entry;

	// Same tables:
	const auto num_entries = 1'000;
	for (size_t i = 0; i < num_entries; ++i) {
		entry.destination_ip[0] = static_cast<uint8_t>(i >> 3*8);
		entry.destination_ip[1] = static_cast<uint8_t>(i >> 2*8);
		entry.destination_ip[2] = static_cast<uint8_t>(i >> 1*8);
		entry.destination_ip[3] = static_cast<uint8_t>(i >> 0*8);
		entry.gateway_ip[0] = static_cast<uint8_t>(i >> 0*8);
		entry.gateway_ip[1] = static_cast<uint8_t>(i >> 1*8);
		entry.gateway_ip[2] = static_cast<uint8_t>(i >> 2*8);
		entry.gateway_ip[3] = static_cast<uint8_t>(i >> 3*8);
		entry.destination_mask = i;
		entry.oif = "ens_" + std::to_string(static_cast<uint32_t>(i));

		rt.create_entry(entry);
		rt_other.create_entry(entry);
	}
	EXPECT_EQ(rt, rt_other);

	// Tables differ by size:
	const auto& entry_key = entry.destination_ip_u32;
	rt_other.delete_entry(entry);
	EXPECT_NE(rt, rt_other);

	// Tables have same size again and same content:
	rt.delete_entry(entry);
	EXPECT_EQ(rt, rt_other);

	// Tables have same size but different entries
	for (size_t i = 0; i < num_entries; ++i) {
		entry.destination_ip[0] = static_cast<uint8_t>(i + 101 >> 3*8);
		entry.destination_ip[1] = static_cast<uint8_t>(i + 101 >> 2*8);
		entry.destination_ip[2] = static_cast<uint8_t>(i + 101 >> 1*8);
		entry.destination_ip[3] = static_cast<uint8_t>(i + 101 >> 0*8);
		entry.gateway_ip[0] = static_cast<uint8_t>(i + 99 >> 0*8);
		entry.gateway_ip[1] = static_cast<uint8_t>(i + 99 >> 1*8);
		entry.gateway_ip[2] = static_cast<uint8_t>(i + 99 >> 2*8);
		entry.gateway_ip[3] = static_cast<uint8_t>(i + 99 >> 3*8);
		entry.destination_mask = i;
		entry.oif = "foo_ens_" + std::to_string(static_cast<uint32_t>(i));

		rt_other.create_entry(entry);
	}
	EXPECT_NE(rt, rt_other);
}

TEST_F(routing_table_test, serialize_deserialize)
{
	constexpr auto num_entries = 1'000'000;
	constexpr auto bytes_per_entry = 38;
	std::vector<uint8_t> buffer(num_entries * bytes_per_entry, 0);
	routing_table rt_deserialized;
	routing_table_entry entry;

	entry.destination_ip[0] = 17;
	entry.destination_ip[1] = 91;
	entry.destination_ip[2] = 123;
	entry.destination_ip[3] = 231;
	entry.gateway_ip[0] = 21;
	entry.gateway_ip[1] = 22;
	entry.gateway_ip[2] = 33;
	entry.gateway_ip[3] = 44;
	entry.destination_mask = 31;
	entry.oif = "foo_ens31";

	const auto entry_ser_bytes = routing_table_entry::serialize(entry, buffer);
	EXPECT_EQ(entry_ser_bytes, bytes_per_entry);
	buffer.clear();

	EXPECT_EQ(rt.size(), 0);

	for (size_t i = 0; i < num_entries; ++i) {
		entry.destination_ip[0] = static_cast<uint8_t>(i >> 3*8);
		entry.destination_ip[1] = static_cast<uint8_t>(i >> 2*8);
		entry.destination_ip[2] = static_cast<uint8_t>(i >> 1*8);
		entry.destination_ip[3] = static_cast<uint8_t>(i >> 0*8);

		entry.gateway_ip[0] = 1 + static_cast<uint8_t>(i >> 3*8);
		entry.gateway_ip[1] = 2 + static_cast<uint8_t>(i >> 2*8);
		entry.gateway_ip[2] = 3 + static_cast<uint8_t>(i >> 1*8);
		entry.gateway_ip[3] = 4 + static_cast<uint8_t>(i >> 0*8);
		rt.create_entry(entry);
	}
	EXPECT_EQ(rt.size(), num_entries);

	// Test serialization method:
	const auto table_ser_bytes = routing_table::serialize(rt, buffer);
	EXPECT_EQ(table_ser_bytes, 4 + 4 + num_entries * entry_ser_bytes);

	// Test deserialization method:
	const auto table_deser_bytes = routing_table::deserialize(buffer, rt_deserialized);
	EXPECT_EQ(table_ser_bytes, table_deser_bytes);
	EXPECT_EQ(rt == rt_deserialized, true);
	EXPECT_EQ(rt, rt_deserialized);
}

TEST_F(routing_table_test, to_string) {
	constexpr auto num_entries = 100'000;
	constexpr auto bytes_per_entry = 38;
	std::vector<uint8_t> buffer(num_entries * bytes_per_entry, 0);
	routing_table rt_deserialized;
	routing_table_entry entry;

	entry.destination_ip[0] = 17;
	entry.destination_ip[1] = 91;
	entry.destination_ip[2] = 123;
	entry.destination_ip[3] = 231;
	entry.gateway_ip[0] = 21;
	entry.gateway_ip[1] = 22;
	entry.gateway_ip[2] = 33;
	entry.gateway_ip[3] = 44;
	entry.destination_mask = 31;

	constexpr auto oif_default_name = "foo_ens31";
	entry.oif = oif_default_name;

	const auto entry_ser_bytes = routing_table_entry::serialize(entry, buffer);
	EXPECT_EQ(entry_ser_bytes, bytes_per_entry);
	buffer.clear();

	EXPECT_EQ(rt.size(), 0);

	for (size_t i = 0; i < num_entries; ++i) {
		entry.destination_ip[0] = static_cast<uint8_t>(i >> 3*8);
		entry.destination_ip[1] = static_cast<uint8_t>(i >> 2*8);
		entry.destination_ip[2] = static_cast<uint8_t>(i >> 1*8);
		entry.destination_ip[3] = static_cast<uint8_t>(i >> 0*8);

		entry.gateway_ip[0] = 1 + static_cast<uint8_t>(i >> 3*8);
		entry.gateway_ip[1] = 2 + static_cast<uint8_t>(i >> 2*8);
		entry.gateway_ip[2] = 3 + static_cast<uint8_t>(i >> 1*8);
		entry.gateway_ip[3] = 4 + static_cast<uint8_t>(i >> 0*8);

		const auto num_dots = i % 7;
		entry.oif = oif_default_name;
		for(auto j = 0; j < num_dots; j++) {
			entry.oif += '.';
		}
		rt.create_entry(entry);
	}
	EXPECT_EQ(rt.size(), num_entries);

	const auto rt_str = rt.to_string(true);

	// Split table as string by new line and compute length of each line:
	std::vector<size_t> rt_str_lines_lengths;
	for (const auto&& part : std::views::split(rt_str, '\n')) {
		const auto line_len = part.end() - part.begin();
		if(line_len) {
			rt_str_lines_lengths.push_back(line_len);
		}
	}

	// All line lengths shall be same:
	const auto expected_len = rt_str_lines_lengths.at(0);
	for(const auto& len : rt_str_lines_lengths) {
		EXPECT_EQ(len, expected_len);
	}

	// Use command below to show the table:
	// std::cout << rt_str << std::endl;
}
