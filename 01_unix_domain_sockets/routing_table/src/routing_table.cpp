
#include <routing_table.hpp>

using namespace RTM;

void routing_table_entry_t::serialize(uint8_t* buffer) const {
	// Copy destination_ip
	std::memcpy(buffer, destination_ip, 4);
	buffer += 4;
	// Copy gateway_ip
	std::memcpy(buffer, gateway_ip, 4);
	buffer += 4;
	// Copy oif
	std::memcpy(buffer, oif, 16);
	buffer += 16;
	// Copy destination_mask
	*buffer = destination_mask;
};
