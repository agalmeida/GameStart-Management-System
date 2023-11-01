#include "Database.hpp"
#include "Customer.hpp"
#include "UserInterface.hpp"
#include<iostream>
#include<sstream>
#include<iomanip>
#include<format>
#include"Customer.hpp"
#include"UserInterface.hpp"

/*Database constructor opens a connection to database,
*or exits the program if unable to establish the connection.
*Takes two optional parameters:
*parameter user a string containing the user to be used for login.
*parameter pass a string containing the password to be used for login.
*If no parameters are supplied connection will be opened as user administrator*/
Database::Database(std::string user, std::string pass) {
	connection_string = std::format("dbname = gamestart user = {} password = {} host = localhost port = 5432", user, pass);
	try
	{
		con = new pqxx::connection(connection_string);
	}
	catch (const std::exception& e)
	{
		//do nothing
		//error will handled through checking isOpen or terminating via errorQuit()
	}
}

/*Database destructor deletes the connection 
*and the result of the last query if any exists.*/
Database::~Database() {
	delete con;
	delete res;
}

/*Creates a transaction and executes the given query.
*Returns a copy of the result.
*parameter query is a string formatted as a PostgreSQL query.*/
pqxx::result Database::transaction(std::string query) {
	if (con == nullptr) exit(EXIT_FAILURE);
	try
	{
		pqxx::work transaction(*con);
		if (res != nullptr) {
			delete res;//deallocates the previous result
			res = nullptr;//don't leave res dangling in case the transaction fails to return a new result
		}
		res = new pqxx::result(transaction.exec(query));
		transaction.commit();
		return *res;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
}

/*Displays the result of previous database transaction*/
void Database::displayResult() {
	if(res != nullptr)
		displayResult(*res);
}

/*Displays the given result
*parameter r is the result of a database transaction*/
void Database::displayResult(const pqxx::result& r) {
	std::ostringstream title;
	std::vector<size_t> padding;
	padding.resize(r.columns());

	if (r.size() == 0) {//no results to display
		std::cout << "\nNo Results\n";
		return;
	}

	//set padding amount for each column to the size of the longest value in the column
	for (auto it = r.begin(); it != r.end(); it++)
	{
		for (size_t col = 0; col < r.columns(); col++) {
			if (it[col].size() > padding[col])
				padding[col] = it[col].size();
		}
			
	}

	//create table header
	title << std::endl;
	for (size_t col = 0; col < r.columns(); col++) {
		std::string cname = r.column_name(col);
		if (cname.size() > padding[col])//adjust padding if column name is longer than the current padding
			padding[col] = cname.size();
		title << std::format("{:^{}}|", cname, padding[col] + 2);
	}
	title << std::endl;

	size_t len = title.str().size();
	title << std::format("{:-<{}}", "-", len) << std::endl;//create bottom border for table header

	size_t count = 1;
	std::ostringstream output;
	//scan through the result row wise and format and insert the values in each column into the output stream
	for (auto it = r.begin(); it != r.end(); it++, count++)
	{
		for (int col = 0; col < r.columns(); col++)
			output << std::format("{:<{}}|", it[col].c_str(), padding[col] + 2);
		output << std::endl;
		if (count % 10 == 0) {//print results in groups of 10
			std::cout << title.str() << output.str();
			std::ostringstream empty;
			output.swap(empty);//clear the output stream
			//pause to allow user to read result and/or stop prematurely
			std::cout << "Page " << count / 10 << " press enter to continue or q to quit...";
			std::string next;
			std::getline(std::cin, next);
			std::cout << std::endl;
			if (next == "q") break;
		}
		
	}
	if(!output.str().empty())//print any remaining results
		std::cout << title.str() << output.str() << std::endl;
}

/*Registers a new customer with the database.
*Calls UserInterface::createCustomer to 
*prompt the customer for the necessary information
*before inserting customer into database.
*A role is also be created for the customer via
the stored function create_customer upon insertion.*/
void Database::RegisterCustomer() {
	UserInterface UI;
	Customer c;
	if (!UI.createCustomer(c)) {
		std::cerr << "\nRegistration Failed\n";
		return;
	}
	//create customer insertion query from provided customer information
	//use of quote function to quote and escape customer values should prevent SQL injection
	std::stringstream ss;
	ss << "INSERT INTO customer VALUES("
		<< "(SELECT COUNT (*) FROM customer),"//use table count as next customerid
		<< con->quote(c.getName()) << ","
		<< con->quote(c.getPassword()) << ",E" //E allows for C style string escapes in address
		<< con->quote(c.getAddress()) << ","
		<< con->quote(c.getEmail()) << ","
		<< con->quote(c.getFinancials()) << ");";

	//secondary query to return customer information from database for display
	ss << " SELECT * FROM customer WHERE  customerid = (SELECT COUNT (*) FROM customer) - 1"
		<< " AND name = " << con->quote(c.getName()) << " AND password = " << con->quote(c.getPassword()) 
		<< " AND address = " << con->quote(c.getAddress()) << " AND email = " << con->quote(c.getEmail())
		<< " AND credit_card_num = " << con->quote(c.getFinancials()) << ";";

	this->transaction(ss.str());//insert new customer into the customers table
	//diplay confirmation
	std::cout << "\nRegistration Complete!\nPlease save this information for your records.\n"
		<< "!!!YOU WILL NEED YOUR CUSTOMER ID AND PASSWORD TO LOGIN!!!\n";
	this->displayResult();
}

/*Wrapper function for pqxx::connection::is_open
*returns true if connection is open/succeeded
*returns false if connection is closed/failed*/
bool Database::isOpen() {
	if (con == nullptr) return false;
	try
	{
		return con->is_open();//may throw exception
	}
	catch (const std::exception& e)
	{
		return false;

	}	
}

/*Terminates application if database connection closed/invalid.*/
void Database::errorQuit() {
	if (!this->isOpen()) {
		std::cerr << "\nUnable to establish connection to GameStart!\nTerminating Application!\n";
		system("pause");
		exit(EXIT_FAILURE);
	}
}