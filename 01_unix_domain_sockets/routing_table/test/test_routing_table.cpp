/*
 * Copyright (c) 2025 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 * @brief Routig Table Unit-Tests
 */

#include <gtest/gtest.h>

#include <routing_table.hpp>


using namespace RTM;


TEST(routing_table_entry, serialize) {
	routing_table_entry entry;
	uint8_t buffer[24] = {0};

	EXPECT_EQ(alignof(entry), 8);

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

	routing_table_entry::serialize(entry, buffer);

	EXPECT_EQ(buffer[0], 123);
	EXPECT_EQ(buffer[1], 234);
	EXPECT_EQ(buffer[2], 5);
	EXPECT_EQ(buffer[3], 6);
	EXPECT_EQ(buffer[4], 10);
	EXPECT_EQ(buffer[5], 11);
	EXPECT_EQ(buffer[6], 12);
	EXPECT_EQ(buffer[7], 13);
	EXPECT_EQ(buffer[8], 24);
	EXPECT_EQ(buffer[9], 'e');
	EXPECT_EQ(buffer[10], 't');
	EXPECT_EQ(buffer[11], 'h');
	EXPECT_EQ(buffer[12], '0');

	for (size_t i = 13; i < sizeof(buffer); ++i) {
		EXPECT_EQ(buffer[i], 0)
			<< "The rest of the buffer shall be zeroed after the serialization";
	}
}

