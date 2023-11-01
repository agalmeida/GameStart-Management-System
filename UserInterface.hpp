#ifndef USERINTERFACE
#define USERINTERFACE

#include "Customer.hpp"
#include "Order.hpp"
#include<map>
#include<vector>


class UserInterface
{
	private:
		void FilterGames(std::map<std::string, std::string>& filters);
		void CreateFilterLike(const std::string& attribute, std::map<std::string, std::string>& filters);
		void CreateFilterRange(const std::string& attribute, std::map<std::string, std::string>& filters, std::string low, std::string high = "");
		void SortGames(std::vector<std::map<std::string, std::string>::iterator>& sort_order, std::map<std::string, std::string>& sorts);
		void CreateSort(std::vector<std::map<std::string, std::string>::iterator>& sort_order, std::map<std::string, std::string>& sorts, std::string attribute);
		void DisplayGames(const std::map<std::string, std::string>& filters, const std::vector<std::map<std::string, std::string>::iterator>& sort_order);
	public:
		bool createCustomer(Customer& c);
		bool login(Customer& custA);
		void addToCart(Customer& cust, Order& order);
		void checkout(Customer& cust);
		void displayCart(Customer& cust);
		void orderHistory(Customer& cust);
		void removeFromCart(Customer& cust);
		void BrowseGames();
		void RateGames(Customer& c);
		int menu_select();


	//account functions
	void updateAccount(Customer& custA);
	void view_account(Customer& custA);
};
#endif
