#include <sstream>

#include <routing_table.hpp>
#include <iomanip>


/**
 * @note All serializations used are based on simple casting. This is done for
 * sake of simplicitay of this teaching project. The serialization shall use
 * an architecture inedependent approach in case  of productve applications.
 *
 */

using namespace RTM;

size_t routing_table_entry::size() const
{
	return sizeof(this->destination_ip_u32) +
	       sizeof(this->gateway_ip_u32) +
	       sizeof(this->destination_mask) +
	       this->oif.size();
}

bool routing_table_entry::operator==(const routing_table_entry& other) const
{
	return (std::memcmp(this->destination_ip, other.destination_ip,
			sizeof(this->destination_ip)) == 0 &&
		std::memcmp(this->gateway_ip, other.gateway_ip,
			sizeof(this->destination_ip)) == 0 &&
		this->destination_mask == other.destination_mask &&
		this->oif.compare(other.oif) == 0);
}

std::string routing_table_entry::destination_ip2str(
					const routing_table_entry& entry)
{
	std::string str;
	const auto dest_ip_size = sizeof(entry.destination_ip);
	for (size_t i = 0; i < dest_ip_size; ++i) {
		str += std::to_string(
			static_cast<unsigned char>(
				entry.destination_ip[i]));
		if (i != dest_ip_size - 1) {
			str += ".";
		}
	}
	return str;
}

size_t routing_table_entry::serialize(const routing_table_entry &entry,
				      std::vector<uint8_t>& buffer)
{
	const uint32_t total_bytes = entry.size() + 5 * sizeof(uint32_t);

	size_t offset = 0;
	uint32_t size_tmp = 0;

	buffer.resize(total_bytes, 0);
	buffer.clear();

	std::memcpy(buffer.data() + offset, &total_bytes, sizeof(total_bytes));
	offset += sizeof(total_bytes);

	// Serialize destination IP:
	size_tmp = sizeof(entry.destination_ip_u32);
	std::memcpy(buffer.data() + offset, &size_tmp, sizeof(size_tmp));
	offset += sizeof(size_tmp);
	std::memcpy(buffer.data() + offset, &entry.destination_ip_u32,
			sizeof(entry.destination_ip_u32));
	offset += sizeof(entry.destination_ip_u32);

	// Serialize gateway IP:
	size_tmp = sizeof(entry.gateway_ip_u32);
	std::memcpy(buffer.data() + offset, &size_tmp, sizeof(size_tmp));
	offset += sizeof(size_tmp);
	std::memcpy(buffer.data() + offset, &entry.gateway_ip_u32,
			sizeof(entry.gateway_ip_u32));
	offset += sizeof(entry.gateway_ip_u32);

	// Serialize mask:
	size_tmp = sizeof(entry.destination_mask);
	std::memcpy(buffer.data() + offset, &size_tmp, sizeof(size_tmp));
	offset += sizeof(size_tmp);
	std::memcpy(buffer.data() + offset, &entry.destination_mask,
			sizeof(entry.destination_mask));
	offset += sizeof(entry.destination_mask);

	// Serialize OIF:
	size_tmp = entry.oif.size();
	std::memcpy(buffer.data() + offset, &size_tmp, sizeof(size_tmp));
	offset += sizeof(size_tmp);
	std::memcpy(buffer.data() + offset, entry.oif.c_str(),
			entry.oif.size());
	offset += entry.oif.size();

	assert(offset == total_bytes);

	return total_bytes;
}

size_t routing_table_entry::deserialize(const std::vector<uint8_t>& buffer,
					routing_table_entry &entry)
{
	size_t offset = 0;
	uint32_t size_tmp = 0;
	uint32_t total_size = 0;

	std::memcpy(&total_size, buffer.data() + offset, sizeof(total_size));
	offset += sizeof(total_size);

	std::memcpy(&size_tmp, buffer.data() + offset, sizeof(size_tmp));
	offset += sizeof(size_tmp);

	std::memcpy(entry.destination_ip, buffer.data() + offset,
			size_tmp);
	offset += sizeof(destination_ip);

	std::memcpy(&size_tmp, buffer.data() + offset, sizeof(size_tmp));
	offset += sizeof(size_tmp);

	std::memcpy(entry.gateway_ip, buffer.data() + offset,
			size_tmp);
	offset += sizeof(gateway_ip);

	std::memcpy(&size_tmp, buffer.data() + offset, sizeof(size_tmp));
	offset += sizeof(size_tmp);

	std::memcpy(&entry.destination_mask, buffer.data() + offset,
			size_tmp);
	offset += sizeof(destination_mask);

	std::memcpy(&size_tmp, buffer.data() + offset, sizeof(size_tmp));
	offset += sizeof(size_tmp);

	entry.oif.resize(size_tmp);
	std::memcpy(&entry.oif[0], buffer.data() + offset, size_tmp);
	offset += size_tmp;

	assert(total_size == offset);

	return total_size;
}

void routing_table::create_entry(const routing_table_entry &entry)
{
	this->table[entry.destination_ip_u32] = entry;
}

void routing_table::update_entry(const routing_table_entry &entry)
{
}

void routing_table::delete_entry(const routing_table_entry &entry)
{
	this->table.erase(entry.destination_ip_u32);
}

size_t routing_table::serialize(routing_table &table, std::vector<uint8_t> &buffer)
{
	// Total size consists of:
	// - 4 bytes for total size
	// - 4 bytes for number of entries
	// - size of each serialized entry (will be calculated later)
	uint32_t total_size = 4 + 4;
	size_t offset = 4;  // buffer offset in bytes - start with number of entries

	const uint32_t num_entries = static_cast<uint32_t>(table.size());
	std::memcpy(buffer.data() + offset, &num_entries, sizeof(num_entries));
	offset += sizeof(num_entries);

	std::vector<uint8_t> entry_buffer;
	for (const auto& [key, entry] : table.table) {
		// @note: it is not necessary to serialize the keys, since they
		// are already included in each entry
		const auto bytes_written = routing_table_entry::serialize(entry, entry_buffer);
		std::memcpy(buffer.data() + offset, entry_buffer.data(),
				bytes_written);
		total_size += bytes_written;
		offset += bytes_written;
	}
	std::memcpy(buffer.data(), &total_size, sizeof(total_size));

	return static_cast<size_t>(total_size);
}

size_t routing_table::deserialize(const std::vector<uint8_t>& buffer,
				  routing_table &table)
{
	uint32_t total_size = 0;
	uint32_t num_entries = 0;
	size_t offset = 0;

	std::memcpy(&total_size, buffer.data() + offset, sizeof(total_size));
	offset += sizeof(total_size);

	std::memcpy(&num_entries, buffer.data() + offset, sizeof(num_entries));
	offset += sizeof(num_entries);

	routing_table_entry entry;
	uint32_t entry_cnt = 0;
	uint32_t entry_size = 0;
	std::vector<uint8_t> entry_buffer;
	while(entry_cnt < num_entries) {
		entry_buffer.clear();
		std::memcpy(&entry_size, buffer.data() + offset, sizeof(entry_size));
		entry_buffer.resize(entry_size, 0);
		std::memcpy(entry_buffer.data(), buffer.data() + offset, entry_size);

		const auto bytes_read =
			routing_table_entry::deserialize(entry_buffer, entry);
		table.create_entry(entry);

		offset += bytes_read;
		entry_cnt++;
	}

	return static_cast<size_t>(offset);
}

bool routing_table::operator==(const routing_table& other) const
{
	if (this->size() != other.size()) {
		return false;
	}

	for(size_t i = 0; i < this->size(); ++i) {
		const auto& entry = this->table.begin()->second;
		const auto& other_entry = other.table.begin()->second;
		if (entry != other_entry) {
			return false;
		}
	}

	return true;
}

std::string routing_table::to_string() const
{
	// This function is still under development and may not be complete.
	std::string delim = "\t | ";
	std::string str = "Key" + delim + "Destination IP/Mask" + delim +
			  "Gateway IP"+ delim + "OIF\n";
	for (const auto& [key, entry] : this->table) {
		std::stringstream ss1;
		ss1 << std::hex << std::setw(8) << std::setfill('0')
		    << entry.destination_ip_u32;
		const auto destination_ip_str =
			std::to_string(entry.destination_ip[0]) + "." +
			std::to_string(entry.destination_ip[1]) + "." +
			std::to_string(entry.destination_ip[2]) + "." +
			std::to_string(entry.destination_ip[3]) +
			" (" + "0x" + ss1.str() + ")";
		std::stringstream ss2;
		ss2 << std::hex << std::setw(8) << std::setfill('0')
		    << entry.gateway_ip_u32;
		const auto gateway_ip_str = \
			std::to_string(entry.gateway_ip[0]) + "." +
			std::to_string(entry.gateway_ip[1]) + "." +
			std::to_string(entry.gateway_ip[2]) + "." +
			std::to_string(entry.gateway_ip[3]) +
			" (" + "0x" + ss2.str() + ")";

		const auto& key_str = entry.destination_ip2str(entry);
		str += key_str + delim + destination_ip_str + "/" +
			std::to_string(entry.destination_mask) + delim +
			gateway_ip_str + delim + entry.oif + "\n";
	}
	return str;
}
