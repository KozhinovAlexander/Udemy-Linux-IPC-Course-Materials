#include <routing_table.hpp>

using namespace RTM;

size_t routing_table_entry::size() const
{
	return sizeof(this->destination_ip) +
	       sizeof(this->gateway_ip) +
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

void routing_table::create_entry(const routing_table_entry &entry)
{
	const auto key = entry.destination_ip2str(entry);
	this->table[key] = entry;
}

void routing_table::update_entry(const routing_table_entry &entry)
{
}

void routing_table::delete_entry(const routing_table_entry &entry)
{
	const auto key = entry.destination_ip2str(entry);
	this->table.erase(key);
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
		const auto gateway_ip_str = \
			std::to_string(entry.gateway_ip[0]) + "." +
			std::to_string(entry.gateway_ip[1]) + "." +
			std::to_string(entry.gateway_ip[2]) + "." +
			std::to_string(entry.gateway_ip[3]);
		str += key + delim +
			routing_table_entry::destination_ip2str(entry) +
			"/" + std::to_string(entry.destination_mask) + delim +
			gateway_ip_str + delim + entry.oif + "\n";
	}
	return str;
}
