
#include <string>

#ifndef CUSTOMER
#define CUSTOMER

class Customer {
private:
	int id;
	std::string name;
	std::string password;
	std::string address;
	std::string email;
	std::string financials;//credit card number
	static constexpr int limits[] = { 50, 15, 200, 50, 16 };//length limits for name, password, address, email, financials

public:
	Customer();//default constructor
	Customer(const Customer& customer);//copy constructor
	~Customer();//destructor
	Customer operator=(const Customer& customer);//assignment

	//getters
	int getID() const;
	std::string getFinancials() const;
	std::string getName() const;
	std::string getPassword() const;
	std::string getAddress() const;
	std::string getEmail() const;
	//setters
	void setID(const int& id);
	bool setFinancials(const std::string& financials);
	bool setName(const std::string& name);
	bool setPassword(const std::string& password);
	bool setAddress(const std::string& address);
	bool setEmail(const std::string& email);

};

#endif // !CUSTOMER


