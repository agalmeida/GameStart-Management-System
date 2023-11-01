#ifndef DATABASE
#define DATABASE

#include<string>
#include "Customer.hpp"
#include <pqxx/pqxx>

class Database
{
private:
	std::string connection_string;//information required to open a new connection to the database
	pqxx::connection* con = nullptr;//connection to the database
	pqxx::result* res = nullptr;//the result of the previous transaction with the database
public:
	Database(std::string user = "administrator", std::string pass = "1234");
	~Database();
	pqxx::result transaction(std::string query);
	void displayResult();
	void displayResult(const pqxx::result& r);
	void RegisterCustomer();
	/*inline wrapper function for pqxx::connection::quote
	*pass it user input of any type and it will
	*return the input as a quoted and escaped string*/
	template<typename T> std::string quote(const T &input) { return con->quote(input); }
	bool isOpen();
	void errorQuit();
};

#endif // !DATABASE

