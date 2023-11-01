 #include "Customer.hpp"
#include<regex>

Customer::Customer() {
	id = -1;
}
Customer::Customer(const Customer& customer) {
	this->id = customer.id;
	this->name = customer.name;
	this->password = customer.password;
	this->address = customer.address;
	this->email = customer.email;
}
Customer::~Customer() {}
Customer Customer::operator=(const Customer& customer) {
	this->id = customer.id;
	this->name = customer.name;
	this->password = customer.password;
	this->address = customer.address;
	this->email = customer.email;
}
int Customer::getID() const {
	return this->id;
}
std::string Customer::getFinancials() const {
	return this->financials;
}
std::string Customer::getName() const {
	return this->name;
}
std::string Customer::getPassword() const {
	return this->password;
}
std::string Customer::getAddress() const {
	return this->address;
}
std::string Customer::getEmail() const {
	return this->email;
}
void Customer::setID(const int& id) {
	this->id = id;
}
bool Customer::setName(const std::string& name) {
	std::smatch sm;
	std::regex name_expr("[[:alpha:]]+\\s[[:alpha:]]+");//2 sets of 1 or more alpha characters with whitespace between them
	bool result = std::regex_match(name, sm, name_expr) && name.size() <= limits[0];
	if (result)
		this->name = name;
	return result;
}
bool Customer::setPassword(const std::string& password) {
	std::smatch sm;
	std::regex pass_expr(".{8,15}");//pass length 8 to 15 with no character restriction
	bool result = std::regex_match(password, sm, pass_expr);
	if (result)
		this->password = password;
	return result;
}
bool Customer::setAddress(const std::string& address) {//address not practical to regex, must be between 1 and 200 characters
	bool result = !address.empty() && address.size() <= limits[2];
	if (result)
		this->address = address;
	return result;
}
bool Customer::setEmail(const std::string& email) {
	std::smatch sm;
	std::regex email_expr(".+@.+\\..+");//very basic email regex, email must contain @ and .
	bool result = std::regex_match(email, sm, email_expr) && email.size() <= limits[3];
	if (result)
		this->email = email;
	return result;
}
bool Customer::setFinancials(const std::string& financials) {
	std::smatch sm;
	std::regex cc_expr("[[:digit:]]{16}");//credit card number must be 16 digits
	bool result = std::regex_match(financials, sm, cc_expr);
	if (result)
		this->financials = financials;
	return result;
}