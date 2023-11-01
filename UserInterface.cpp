#include "UserInterface.hpp"
#include "Database.hpp"
#include "Order.hpp"
#include<iostream>
#include<string>
#include<regex>
#include <iomanip>


/*Prompts the user for information and creates a new customer object.
*Returns a customer object containing name, password, address, email, 
and finanical/credit card fields. Customer ID is not set.*/
bool UserInterface::createCustomer(Customer& c) {
	std::string input;
	bool success = false;
	int attempts = 3;
	do//prompt for name and add to customer
	{
		std::cout << "Please enter your first and last name: ";
		std::getline(std::cin, input);
		success = c.setName(input);
		if (!success) {
			attempts--;
			std::cerr << "\nERROR: Invalid Name. "
				<< "\nFirst and Last name may only contain letters and must be seperated by a space and have a max of 50 characters."
				<< "\nAttempts remaining " << attempts << std::endl;
			system("pause");
		}
		
	} while (!success && attempts > 0);

	if (attempts == 0) return success;

	attempts = 3;

	do//prompt for pass and add to customer
	{
		std::cout << "Please enter a password: ";
		std::getline(std::cin, input);
		success = c.setPassword(input);
		if (!success) {
			attempts--;
			std::cerr << "\nERROR: Invalid password. "
				<< "\nPassword must contain between 8 and 15 characters."
				<< "\nAttempts remaining " << attempts << std::endl;
			system("pause");
		}

	} while (!success && attempts > 0);

	if (attempts == 0) return success;

	attempts = 3;

	do//prompt for address and add to customer
	{
		std::cout << "Please enter your address: ";
		std::getline(std::cin, input);
		success = c.setAddress(input);
		if (!success) {
			attempts--;
			std::cerr << "\nERROR: Invalid address. "
				<< "\nAddress must contain between 1 and 200 characters."
				<< "\nAttempts remaining " << attempts << std::endl;
			system("pause");
		}

	} while (!success && attempts > 0);

	if (attempts == 0) return success;

	attempts = 3;
	
	do//prompt for email and add to customer
	{
		std::cout << "Please enter your email: ";
		std::getline(std::cin, input);
		success = c.setEmail(input);
		if (!success) {
			attempts--;
			std::cerr << "\nERROR: Invalid email. "
				<< "\nEmail must be of form username@domainname.extension"
				<< "\nAttempts remaining " << attempts << std::endl;
			system("pause");
		}

	} while (!success && attempts > 0);

	if (attempts == 0) return success;

	attempts = 3;
	
	do//prompt for credit card number and add to customer
	{
		std::cout << "Please enter your credit card number: ";
		std::getline(std::cin, input);
		success = c.setFinancials(input);
		if (!success) {
			attempts--;
			std::cerr << "\nERROR: Invalid credit card number. "
				<< "\nCredit card number must be 16 digits."
				<< "\nAttempts remaining " << attempts << std::endl;
			system("pause");
		}

	} while (!success && attempts > 0);
	
  return success;
}

bool UserInterface::login(Customer& custA) {
	bool loop = false;
	int inputId;
	std::string inputPassword;
	std::stringstream ss;
	pqxx::result r;
	int counter = 2;
	do {

		std::cout << "\nEnter customerID: ";
		inputId = menu_select();

		if (inputId <= -1  ) {
			std::cout << "\nCustomer ID must be a whole number\n";
			continue;
		}
		std::cout << "Enter password: ";
		std::getline(std::cin, inputPassword);
		custA.setID(inputId);
		custA.setPassword(inputPassword);
		std::string id = std::to_string(inputId);
		Database DB(id, inputPassword);
		if (DB.isOpen()==true) {
			ss << "SELECT name, address, email, credit_card_num FROM customer_info;";
			r = DB.transaction(ss.str());
			pqxx::result::const_iterator it = r.begin();

			custA.setName(it[0].as<std::string>());
			custA.setAddress(it[1].as<std::string>());
			custA.setEmail(it[2].as<std::string>());
			custA.setFinancials(it[3].as<std::string>());
			loop = true;
		}
		else
		{

			std::cout << "\ninvalid login info";
			std::cout << "\nattempts left: " << counter <<"\n";
			counter--;
		}
		if (counter == -1) {
			system("pause");
			return false;
		}

	} while (loop != true);
	return true;
}

void UserInterface::addToCart(Customer& cust, Order& orderA) {
	
	//1. Cart table has customerid, versionid and quantity
	Database DB;
	DB.errorQuit();
	orderA.setCustomerId(cust.getID());
	int versionId;
	int qty;
	std::stringstream ss;
	pqxx::result r;
	std::stringstream query;
	//2. We will get versionid and quantity from user input and add into the order object 

	std::cout << "\nEnter versionID: ";
	versionId = menu_select();

	if (versionId <= -1) {
		std::cout << "\nInvalid input (must be a positive integer)\nReturning to menu.\n";
		return;
	}

	query << "SELECT versionid FROM gameversion WHERE versionid = " << DB.quote(versionId) << ";";

	r = DB.transaction(query.str());

	if (r.empty()) {
		std::cout << "\nThat game ID is not in database.\n";
		std::cout << "Returning to menu.\n";
		return;
	}
	orderA.setVersionId(versionId);

	std::cout << "\nEnter quantity (Maximum of 10 copies per order): ";
	qty = menu_select();
	

	if (qty <= 0 || qty >= 11) {
		std::cout << "\nInvalid input (must be a positive integer less than or equal to 10)\nReturning to menu.\n";
		return;
	}

	query.str("");
	query << "SELECT qty FROM cart WHERE customerid = " << cust.getID() << "AND versionid = "<< DB.quote(orderA.getVersionId()) << ";";
	r = DB.transaction(query.str());
	int table_qty = 0;

	if (!r.empty()) {
		table_qty = r[0][0].as<int>();
	}

	if (table_qty == 10) {
		std::cout << "You already have the maximum of 10 copies of this game in your cart!\nReturning to menu.\n";
		return;
	}

	int new_qty = table_qty + qty;

	if (new_qty > 10) {
		qty = 10 - table_qty;
		std::cout << "Updating quantity to " << qty << ". Only a maximum of 10 copies are allowed!\n";
	}

	orderA.setQty(qty);
	
	// put this in an if statement that checks if the result is already on the table and update accordingly
	// ON CONFLICT pkey DO UPDATE SET atrib EQUALS atrib + atrib.EXCLUDED statement might be what you need  

	ss << "INSERT INTO cart VALUES("
		<< DB.quote(orderA.getCustomerId()) << ","
		<< DB.quote(orderA.getVersionId()) << ","
		<< DB.quote(orderA.getQty()) << ") ON CONFLICT (versionid,customerid) DO UPDATE SET qty = cart.qty + EXCLUDED.qty;";

	DB.transaction(ss.str()); //insert new order into the cart table

	query.str("");

	query << "SELECT game_title FROM gameversion WHERE versionId = " << DB.quote(orderA.getVersionId()) <<";";

	r = DB.transaction(query.str());

	std::cout << r[0][0].c_str() <<" added to cart successfully!" << std::endl;
	
}

void UserInterface::checkout(Customer& cust) {
	// calculate total price 
	Database DB;
	DB.errorQuit();
	std::stringstream ss;
	ss << "SELECT price, qty FROM cart NATURAL JOIN gameversion WHERE customerid = "
		<< cust.getID() << ";";
	pqxx::result r = DB.transaction(ss.str());
	if (r.empty()) {
		std::cout << "\nThere are currently no items in your cart.\n";
		return;
	}

	float total_price = 0.0;

	if (!r.empty()) {
		for (auto it = r.begin(); it != r.end(); ++it) {
			total_price += (it[0].as<float>() * it[1].as<float>());
		}
	}
	float tax = total_price * 0.1;
	// display price
	std::cout << std::fixed << std::setprecision(2) << "\nSUBTOTAL: " << total_price <<"\nTAXES AND FEES: " << tax << "\nTOTAL: " << total_price + tax << std::endl;
	// ask if they want to continue
	std::cout << "Complete purchase?\n";
	std::cout << "1. Yes\n";
	std::cout << "2. No\n";

	int user_cont = menu_select();

	if (user_cont == 2) {
		std::cout << "\nReturning to menu.\n";
		return;
	}
	if (user_cont != 1) {
		std::cout << "\nError: invalid selection (selection must be an integer between 1 and 2).\n";
		return;
	}

 	// remove all of the orders from cart and add to order and orderdetails
	ss.str("");
	ss << "INSERT INTO orders VALUES((SELECT COUNT(*) FROM orders),"
		<< cust.getID() << ", NOW());";

	std::stringstream query;
	query << "SELECT versionid, price, qty FROM cart NATURAL JOIN gameversion WHERE customerid = " << cust.getID() << ";";
	r = DB.transaction(query.str());
	if (!r.empty())
		for (auto it = r.begin(); it != r.end(); ++it) {
			ss << "INSERT INTO orderdetails VALUES((SELECT COUNT(*) FROM orders) - 1," << it[0].c_str() << "," << it[1].c_str() << "," << it[2].c_str() << ");";
		}
	ss << "DELETE FROM cart WHERE customerid = " << cust.getID() << ";" << "SELECT * FROM orders WHERE order_num = (SELECT COUNT(*) FROM orders)-1 AND customerid = " << cust.getID()
		<< ";";

	r = DB.transaction(ss.str());
	auto result = r.begin();

	// tell customer approx ship date (something like "Thank you for your purchase! Your order should be shipping in approximately 2 days!
	std::cout << "Thank you for your purchase! Your order number is " << r[0][0].c_str() << " and your estimated ship date is " << r[0][3].c_str()<<"!\n";

}

void UserInterface::displayCart(Customer& cust) {
	// create connection to DB
	Database DB;
	DB.errorQuit();
	pqxx::result r;

	// select title, price and qty from both gameversion and cart
	std::stringstream ss;
	ss << "SELECT game_title, versionid, price, qty FROM cart NATURAL JOIN gameversion WHERE customerid = "
		<< cust.getID() << ";";
	r = DB.transaction(ss.str());

	//display the result
	DB.displayResult(r);
} 

void UserInterface::orderHistory(Customer& cust) {
	
	// Establish DB connection
	Database DB;
	DB.errorQuit();
	// Display order history from order table
	std::stringstream ss;
	pqxx::result r;

	// Select order_num, game_title, qty, price, order_date, ship_date FROM order NATURAL JOIN orderDetails NATURAL JOIN gameversion WHERE customerId = cust.getID();
	ss << "SELECT order_num, game_title, qty, price, order_date, ship_date FROM orders NATURAL JOIN orderdetails NATURAL JOIN gameversion WHERE customerid = " << cust.getID()<< ";";
	r = DB.transaction(ss.str());

	DB.displayResult(r);
}

void UserInterface::removeFromCart(Customer& cust) {
	// Check if the item is in the cart
	std::cout << "\nCart Display\n";
	displayCart(cust);
	Database DB;
	DB.errorQuit();
	std::stringstream ss;
	pqxx::result r;

	ss << "SELECT versionid FROM cart WHERE customerid = "
		<< cust.getID() << ";";
	r = DB.transaction(ss.str());
	if (r.empty()) {
		std::cout << "\nThere are currently no items in your cart.\n";
		return;
	}

	int item;
	std::cout << "\nPlease input versionID for removal: ";
	item = menu_select();

	if (item <= -1) {
		std::cout << "\nInvalid input (must be a positive integer)\nReturning to menu.\n";
		return;
	}

	ss.str("");
	ss << "SELECT versionid FROM cart WHERE customerid = " << cust.getID() << " AND versionid = " << DB.quote(item) << ";";

	r = DB.transaction(ss.str());

	if (r.empty()) {
		std::cout << "Item is not in your cart.\n";
		return;
	}
	// If the value is in the table, remove it, else exit
	// DELETE 

	ss.str("");
	ss << "DELETE FROM cart WHERE versionid = " << DB.quote(item) << ";";
	DB.transaction(ss.str());
	std::cout << "Item removed from cart successfully!\n";

}

void UserInterface::updateAccount(Customer& custA) {
	std::string input;
	std::string id = std::to_string(custA.getID());
	Database DB(id, custA.getPassword());
	DB.errorQuit();
	pqxx::result r;
	int option = -1;
	bool loop = false;
	bool second_attempt;
	do {
		std::cout << "\nSelect what you would like to update or enter 0 to exit:\n";
		std::cout << "1. Name\n";
		std::cout << "2. Password\n";
		std::cout << "3. Email\n";
		std::cout << "4. Card Number\n";
		std::cout << "5. Address\n";
		option = menu_select();
		second_attempt = false;
		switch (option)
		{
		case 1: { //update name
			std::stringstream ss;
			do//prompt for new name
			{
				if (second_attempt == true) {
					std::cout << "\nERROR: Invalid Name. " << 
						"\nFirst and Last name may only contain letters and must be seperated by a space and have a max of 50 characters.\n" ;
					system("pause");

				}
				std::cout << "Enter your new name: ";
				std::getline(std::cin, input);
				second_attempt = true;
				

			} while (!custA.setName(input));
			ss << "UPDATE customer_info SET name = " << DB.quote(custA.getName())  << ";";
			r = DB.transaction(ss.str());
			break;
		}
		case 2: {
			std::stringstream ss;
			do//prompt for new password 
			{
				if (second_attempt == true) {
					std::cout << "\nERROR: Invalid password. "
						<< "\nPassword must contain between 8 and 15 characters.\n";
					system("pause");
				}
				std::cout << "\nEnter your new password: ";
				std::getline(std::cin, input);
				second_attempt = true;

			} while (!custA.setPassword(input));
			ss << "UPDATE customer_info SET password = " << DB.quote(custA.getPassword())  << ";";
			r = DB.transaction(ss.str());
			break;
		}
		case 3: {
			std::stringstream ss;
			do//prompt for new email 
			{
				if (second_attempt == true) {
					std::cout << "\nERROR: Invalid email. "
						<< "\nEmail must be of form username@domainname.extension\n";
					system("pause");
				}
				std::cout << "\nEnter your new email: ";
				std::getline(std::cin, input);
				second_attempt = true;

			} while (!custA.setEmail(input));
			ss << "UPDATE customer_info SET email = " << DB.quote(custA.getEmail()) <<  ";";
			r = DB.transaction(ss.str());
			break;
		}
		case 4: {
			std::stringstream ss;
			do//prompt for new credit card number 
			{
				if (second_attempt == true) {
					std::cout << "\nERROR: Invalid credit card number."
						<< "\nCredit card number must be 16 digits.\n";
					system("pause");
				}
				std::cout << "\nEnter your new credit card number: ";
				std::getline(std::cin, input);
				second_attempt = true;


			} while (!custA.setFinancials(input));
			ss << "UPDATE customer_info SET credit_card_num = " << custA.getFinancials()  << ";";
			r = DB.transaction(ss.str());
			break;
		}
		case 5: {
			std::stringstream ss;
			do//prompt for new address 
			{
				if (second_attempt == true) {
					std::cout << "\nERROR: Invalid address. "
						<< "\nAddress must contain between 1 and 200 characters.\n";
					system("pause");
				}
				std::cout << "\nEnter your new address: ";
				std::getline(std::cin, input);
				second_attempt = true;


			} while (!custA.setAddress(input));
			ss << "UPDATE customer_info SET address = " << DB.quote(custA.getAddress()) << ";";
			r = DB.transaction(ss.str());
			break;
		}
		case 0: { //exit
			loop = true;
			break;
		}
		default:
			std::cout << "\nPlease enter a valid input\n";
			break;
		}
	} while (loop != true);
}

/*The browse games menu.
*Allows user to filter, sort, and display games.*/
void UserInterface::BrowseGames() {
	std::map<std::string, std::string> filters;//holds all applied filters
	std::map<std::string, std::string> sorts;//holds all applied sorts
	std::vector<std::map<std::string, std::string>::iterator> sort_order;//the order the sorts should be applied in
	//std::string input;
	const int exit = 4;
	int selection = -1;
	do
	{
		//prompt customer for selection
		std::cout << "\nBrowse Games:\n";
		std::cout << "1. Filter\n2. Sort\n3. Display\n4. Exit\n";

		selection = menu_select();

		switch (selection)
		{
		case 1://filter
			FilterGames(filters);
			break;
		case 2://sort
			SortGames(sort_order, sorts);
			break;
		case 3://display
			DisplayGames(filters, sort_order);
			break;
		case exit://return to customer menu
			break;
		default://invalid selection
			std::cerr << "\nERROR: Input must integer in range 1 to " << exit << std::endl;
			system("pause");
			break;
		}

	} while (selection != exit);
	return;
}

/*Sets for filters for diplaying games.
*parameter filters is a map of filters composed as pairs of strings,
*where the key is the attribute and the value is the condition*/
void UserInterface::FilterGames(std::map<std::string, std::string>& filters) {
	int selection = -1;
	const int exit = 10;
	bool quit = false;
	do
	{
		//prompt customer for selection
		std::cout << "\nApply Filters:\n";
		std::cout << "1. Title\n2. Franchise\n"
			<< "3. Price\n4. Genre\n" 
			<< "5. System\n6. Age Rating\n"
			<< "7. Customer Rating\n8. Show Applied Filters\n"
			<< "9. Clear All Filters\n10. Exit\n\n";
		selection = menu_select();
		switch (selection)
		{
		case 1://title
			std::cout << "\nEnter Title: ";
			CreateFilterLike("game_title", filters);
			break;
		case 2://franchise
			std::cout << "\nEnter Franchise: ";
			CreateFilterLike("franchise", filters);
			break;
		case 3: {//price
			std::cout << "\nPlease enter a minimum and maximum price.\n"
				<< "Leave minimum blank for no lower limit\n"
				<< "Leave maximum blank for no upper limit.\n\n";
			std::cout << "Minimum price: ";
			std::string low, high;
			std::smatch sm;
			std::regex price_expr("([[:digit:]]{1,3}(\\.[[:digit:]]{2})?)?");//blank or 1 to 3 digits before optional decimal place and 2 digits
			std::getline(std::cin, low);
			std::cout << "Maximum price: ";
			std::getline(std::cin, high);
			if (std::regex_match(low, sm, price_expr) && std::regex_match(high, sm, price_expr))
				CreateFilterRange("price", filters, low, high);
			else {
				std::cerr << "\nERROR: Unexpected Value.\n"
					<< "Prices must be expressed as whole numbers or decimal values expressed to the hundreths place.\n"
					<< "Ex. 10 or 10.00\n";
				system("pause");
			}
			break;
		}
		case 4://genre
			std::cout << "\nEnter Genre: ";
			CreateFilterLike("genre", filters);
			break;
		case 5://system
			std::cout << "\nEnter System: ";
			CreateFilterLike("system_title", filters);
			break;
		case 6://age rating(ESRB)
			std::cout << "\nEnter Age Rating (ESRB): ";
			CreateFilterLike("age_rating", filters);
			break;
		case 7: {//customer rating
			std::cout << "\nPlease enter a minimum rating.\n\n";
			std::cout << "Minimum Rating: ";
			std::string rating;
			std::smatch sm;
			std::regex rate_expr("[1-9]|10");//match 1 to 10
			std::getline(std::cin, rating);
			if (std::regex_match(rating, sm, rate_expr))
				CreateFilterRange("rating", filters, rating);
			else {
				std::cerr << "\nERROR: Unexpected Value.\n"
					<< "Rating must be expressed as a whole number between 1 and 10.\n";
				system("pause");
			}
			break;
		}
		case 8: {//show all applied filters
			if (filters.empty()) {
				std::cout << "\nApplied Filters: None\n";
				break;
			}
			std::string val;
			std::smatch sm;
			std::regex fvals_expr("(>=.+)|(<=.+)|(%.+%)");//matches filter values of the form range or like
			std::cout << "\nApplied Filers:\n";
			for (auto i : filters) {
				std::regex_search(i.second, sm, fvals_expr);//extract the filter value from the condition
				val = sm.str();
				if (val[0] == '%')//remove % signs from like filter values
					val = val.substr(1, val.length() - 2);
				std::cout << i.first << " " << val << std::endl;//print filter name and value
			}
			break;
		}
		case 9://clear all applied filters
			filters.clear();
			break;
		case exit://exit
			quit = true;
			break;
		default://invalid
			std::cerr << "\nERROR: Input must integer in range 1 to " << exit << std::endl;
			system("pause");
			break;
		}
	} while (!quit);
}

/*Creates a filter of the form attribute ILIKE '%user input%'
*parameter attribute is a table attribute
*parameter filters is a map of filters composed as pairs of strings,
*where the key is the attribute and the value is the condition*/
void UserInterface::CreateFilterLike(const std::string& attribute, std::map<std::string, std::string>& filters) {
	Database DB("browser", "1234");
	DB.errorQuit();
	std::string input;
	std::getline(std::cin, input);
	if (!input.empty()) {
		input = "%" + input + "%";
		input = DB.quote(input);
		input = " ILIKE " + input;
		filters[attribute] = input;
	}
	else
		filters.erase(attribute);
}

/*Creates a range based filter of 1 of 3 forms: 
*(1) attribute >= value_low AND attribute <= value_high
*(2) attribute >= value_low
*(3) attribute <= value_high
*parameter attribute is a table attribute
*parameter low is the lower bound of the range
*optional parameter high is the upper bound of the range,
*if omitted fitler will be of form 2
*if both parameters low and high are empty strings the filter will be cleared
*parameter filters is a map of filters composed as pairs of strings,
*where the key is the attribute and the value is the condition*/
void UserInterface::CreateFilterRange(const std::string& attribute, std::map<std::string, std::string>& filters, std::string low, std::string high) {
	Database DB("browser", "1234");
	DB.errorQuit();
	std::string combined;
	if (low.empty() && high.empty()) {//clear the filter
		filters.erase(attribute);
	}
	else if (high.empty()) {//attribute >= value_low
		low = " >= " + DB.quote(low);
		filters[attribute] = low;
	}
	else if (low.empty()) {//attribute <= value_high
		high = " <= " + DB.quote(high);
		filters[attribute] = high;
	}
	else//attribute >= value_low AND attribute <= value_high
	{
		low = " >= " + DB.quote(low);
		high = " <= " + DB.quote(high);
		combined = low + " AND " + attribute + high;
		filters[attribute] = combined;
	}
}

/*Sets options for sorting games in ascending or 
*descending order for various attributes.
*parameter sorts is a map where,
*the key is the attribute to sort by,
*and the value indicates the type of sort: ascending or descending
*parameter sort_order is a vector that stores the sorts, 
*so they may be applied in the order they are entered by the user*/
void UserInterface::SortGames(std::vector<std::map<std::string, std::string>::iterator>& sort_order, std::map<std::string, std::string>& sorts) {
	int selection = -1;
	const int exit = 11;
	bool quit = false;
	do
	{
		//prompt customer for selection
		std::cout << "\nSort Games By:\n";
		std::cout << "1. Title\n2. Franchise\n"
			<< "3. Price\n4. Release Date\n"
			<< "5. Genre\n6. System\n"
			<< "7. Age Rating\n8. Customer Rating\n"
			<< "9. Show Applied Sorts\n"
			<< "10. Clear All Sorts\n11. Exit\n\n";
		selection = menu_select();

		switch (selection)
		{
		case 1://title
			CreateSort(sort_order, sorts, "game_title");
			break;
		case 2://franchise
			CreateSort(sort_order, sorts, "franchise");
			break;
		case 3://price
			CreateSort(sort_order, sorts, "price");
			break;
		case 4://release date
			CreateSort(sort_order, sorts, "release_date");
			break;
		case 5://genre
			CreateSort(sort_order, sorts, "genre");
			break;
		case 6://system
			CreateSort(sort_order, sorts, "system");
			break;
		case 7://age rating
			CreateSort(sort_order, sorts, "age_rating");
			break;
		case 8://user rating
			CreateSort(sort_order, sorts, "rating");
			break;
		case 9://show applied sorts
			if (!sort_order.empty()) {
				std::cout << "\nOrder by:\n";
				for (auto i : sort_order) {
					std::cout << i->first << i->second << std::endl;
				}
			}
			else
				std::cout << "\nNo sorts are currently applied.\n";
			break;
		case 10://clear all sorts
			sort_order.clear();
			sorts.clear();
			break;
		case exit:
			quit = true;
			break;
		default://invalid input
			std::cerr << "\nERROR: Input must integer in range 1 to " << exit << std::endl;
			system("pause");
			break;
		}
	} while (!quit);

}

/*Creates a sort string of the form attribute ASC/DESC
*parameter sorts is a map where,
*the key is the attribute to sort by,
*and the value indicates the type of sort: ascending or descending
*parameter sort_order is a vector that stores the sorts,
*so they may be applied in the order they are entered by the user
*parameter attribute is a string containing the name of the table attribute to sort by*/
void UserInterface::CreateSort(std::vector<std::map<std::string, std::string>::iterator>& sort_order, std::map<std::string, std::string>& sorts, std::string attribute) {
	int selection = -1;
	std::cout << "1. Ascending\n2. Descending\n";
	selection = menu_select();
	if (selection == 1) {//ascending
		auto it = sorts.find(attribute);//find the attribute in map
		sorts[attribute] = " ASC";//set new sort type
		if (it == sorts.end())//add attributes not previously in map to sort order
			sort_order.push_back(sorts.find(attribute));
	}
	else if (selection == 2) {//descending
		auto it = sorts.find(attribute);
		sorts[attribute] = " DESC";
		if (it == sorts.end())
			sort_order.push_back(sorts.find(attribute));
	}
	else {//invalid input
		std::cerr << "\nERROR: Input must integer in range 1 to 2\n";
		system("pause");
	}
}

/*Queries the database for a list of games to display 
*based on the filters and sorting options 
*set by FilterGames() and SortGames().
*parameter filters is a map of filters composed as pairs of strings,
*where the key is the attribute and the value is the condition
*parameter sort_order is a vector that stores the sorts in the order specified by the user*/
void UserInterface::DisplayGames(const std::map<std::string, std::string>& filters, const std::vector<std::map<std::string, std::string>::iterator>& sort_order) {
	std::string query = "SELECT * FROM browse_games";//base query on browse_games view (displays all games with their avg ratings)
	if (!filters.empty()) {//apply filter conditions
		auto i = filters.begin();
		query += " WHERE " + i->first + i->second;
		for (i++;i != filters.end(); i++)
			query += " AND " + i->first + i->second;
	}
	if (!sort_order.empty()) {//apply order conditions
		size_t i = 0;
		query += " ORDER BY " + sort_order[i]->first + sort_order[i]->second + " NULLS LAST";
		for (i++; i < sort_order.size(); i++)
		{
			query += ", " + sort_order[i]->first + sort_order[i]->second + " NULLS LAST";
		}
	}
	query += ";";
	Database DB("browser", "1234");
	DB.errorQuit();
	DB.transaction(query);
	DB.displayResult();
}


/*Allows users to rate games.
*parameter c is a reference to the customer/account whose id will be used to rate a game.
*The user will be prompted for a version id and user rating.
*If the inputs are in valid ranges the user's rating for that game version will be either created or updated as appropriate.
*Confirmation of the new rating is displayed to the user on success.*/
void UserInterface::RateGames(Customer& c) {
	std::string input;
	std::stringstream query;
	int verID = -1;
	int rating = -1;
	Database DB(std::to_string(c.getID()), c.getPassword());//connect to DB as customer
	DB.errorQuit();

	std::cout << "\nGames are rated on a scale of 1 to 10.\nPlease enter the version id of the game you would like to rate.\n\n"
		<< "Version ID: ";
	verID = menu_select();//get versionid from user
	pqxx::result r = DB.transaction("SELECT COUNT(*) FROM browse_games;");//get the number of distinct game versions
	int verMax = r[0][0].as<int>();
	if (verID >= 0 && verID < verMax) {//versionid is in valid range
		//confirm user selected correct game to rate
		query << "SELECT versionid, game_title, system_title from browse_games where versionid = " << verID;
		r = DB.transaction(query.str());
		std::cout << "Would you like to rate " << r[0][1].c_str() << " on " << r[0][2].c_str() << "?(y/n): ";
		std::getline(std::cin, input);
		switch (input[0])
		{
		case 'y'://yes rate the game
			std::cout << "Enter Rating: ";
			rating = menu_select();
			if (rating >= 1 && rating <= 10) {//rating is valid
				//insert or update the customer's rating of the game
				query.str("");//clear previous query
				query << "INSERT INTO rate VALUES (" << c.getID() << ", " << verID << ", " << rating 
					<< ") ON CONFLICT (customerid, versionid) DO UPDATE SET user_rating = EXCLUDED.user_rating;";
				DB.transaction(query.str());
				//display confimration of the new rating
				query.str("");
				query << "SELECT customerid, versionid, game_title, system_title, user_rating FROM gameversion NATURAL JOIN rate "
					<< "WHERE customerid = " << c.getID() << " AND versionid = " << verID << ";";
				DB.transaction(query.str());
				DB.displayResult();
				std::cout << "\nRating Updated Successfully\n";
			}
			else
				std::cerr << "\nERROR: Invalid Rating. Rating must be an integer between 1 and 10.\n";
			break;
		case 'n'://no return to customer menu
			return;
		default://invalid input
			std::cerr << "\nERROR: Input must be y for yes or n for no\n";
			system("pause");
			break;
		}
	}
	else {
		std::cerr << "\nERROR: " << verID << " is not a valid game version ID.\n"
			<< "Valid version IDs are integers from 0 to " << verMax - 1 << ".\n";
	}
		

}

/*collects user input for a menu
*returns the selected integer option
*returns -1 if input is invalid*/
int UserInterface::menu_select() {
	int option = -1;
	std::string input;
	std::getline(std::cin, input);
	try
	{
		option = std::stoi(input);
	}
	catch (const std::exception& e)
	{
		//std::cerr << std::endl << e.what() << std::endl;
		option = -1;
	}
	return option;
}



void UserInterface::view_account(Customer& custA) {
	std::string id = std::to_string(custA.getID());
	Database DB(id, custA.getPassword());
	DB.errorQuit();
	std::stringstream ss;
	ss << "SELECT * FROM customer_info;";
	pqxx::result r = DB.transaction(ss.str());
	DB.displayResult();


}
