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

namespace RTM {
	class routing_table {
	public:
		routing_table(/* args */);
		~routing_table();
	private:
	};
}  // namespace RTM
