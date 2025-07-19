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
typedef enum
{
	RTM_CREATE = 0,
	RTM_UPDATE,
	RTM_DELETE,
} cud_opcode_t;


struct routing_table_entry
{
	uint8_t mask;
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
