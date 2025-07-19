
#include <routing_table.hpp>

using namespace RTM;

void routing_table_entry::serialize(const routing_table_entry &entry,
				    uint8_t* buffer)
{
	size_t offset = 0;

	std::memcpy(buffer+offset, entry.destination_ip,
		    sizeof(entry.destination_ip));
	offset += sizeof(entry.destination_ip);

	std::memcpy(buffer+offset, entry.gateway_ip,
		    sizeof(entry.destination_ip));
	offset += sizeof(entry.destination_ip);

	std::memcpy(buffer+offset, &entry.destination_mask,
		    sizeof(entry.destination_mask));
	offset += sizeof(entry.destination_mask);

	std::memcpy(buffer+offset, entry.oif.c_str(), entry.oif.size());
};
