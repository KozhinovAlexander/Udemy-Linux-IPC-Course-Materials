/*
 * Copyright (c) 2025 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 * @brief Routig Table Unit-Tests
 */

#include <iostream>
#include <gtest/gtest.h>
#include <rtm_server.hpp>


using namespace RTM;


class rtm_server_client_test : public ::testing::Test {
public:
	rtm_server srv;
protected:
	void SetUp() override
	{
	}
	void TearDown() override
	{
	}
};


TEST_F(rtm_server_client_test, foo)
{
	EXPECT_EQ(true, true);
}
