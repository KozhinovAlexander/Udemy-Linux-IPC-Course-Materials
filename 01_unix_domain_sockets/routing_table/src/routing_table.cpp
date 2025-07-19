
#include <routing_table.hpp>

using namespace RTM;

size_t routing_table_entry::size() const
{
	return sizeof(this->destination_ip) +
	       sizeof(this->gateway_ip) +
	       sizeof(this->destination_mask) +
	       this->oif.size();
};

void routing_table_entry::serialize(const routing_table_entry &entry,
				    uint8_t* buffer)
{
	// serialization format:
	// nite: bytes sizes are given as 32 bit unsigned integers
	// <total_bytes>
	// <dest_ip_bytes><dest_ip>
	// <gateway_ip_bytes><gateway_ip>
	// <mask_bytes><mask>
	// <oif_bytes><oif>

	const uint32_t total_bytes = entry.size() + 4 * sizeof(uint32_t);

	size_t offset = 0;
	uint32_t size_tmp = 0;

	std::memset(buffer, 0, total_bytes);

	std::memcpy(buffer+offset, &total_bytes, sizeof(total_bytes));
	offset += sizeof(total_bytes);

	size_tmp = sizeof(entry.destination_ip);
	std::memcpy(buffer+offset, &size_tmp, sizeof(size_tmp));
	offset += sizeof(size_tmp);

	std::memcpy(buffer+offset, entry.destination_ip,
		    sizeof(entry.destination_ip));
	offset += sizeof(entry.destination_ip);

	size_tmp = sizeof(entry.gateway_ip);
	std::memcpy(buffer+offset, &size_tmp, sizeof(size_tmp));
	offset += sizeof(size_tmp);

	std::memcpy(buffer+offset, entry.gateway_ip,
		    sizeof(entry.gateway_ip));
	offset += sizeof(entry.gateway_ip);

	std::memcpy(buffer+offset, &entry.destination_mask,
		    sizeof(entry.destination_mask));
	offset += sizeof(entry.destination_mask);

	std::memcpy(buffer+offset, entry.oif.c_str(), entry.oif.size());
};
