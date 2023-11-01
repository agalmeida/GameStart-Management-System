#include <string>
#include <sstream>
#include <iostream>
#include <pqxx/pqxx>
#include "Customer.hpp"
#include "UserInterface.hpp"
#include "Database.hpp"
#include "Order.hpp"


int main()
{
	std::string input;//user input
	int option = -1;//main menu and customer option
	bool loggedin = false;
	UserInterface UI;
	Customer custA;
	Order orderA;

	do
	{
		if (!loggedin) {//main menu
			//display menu title
			std::cout << "\nWelcome to GameStart!\n" << std::endl;
			//display menu options
			std::cout << "1. Register New Customer" << std::endl;
			std::cout << "2. Customer Login" << std::endl;
			std::cout << "3. Browse Games" << std::endl;
			std::cout << "4. Exit" << std::endl;
			option = UI.menu_select();

			switch (option)
			{
			case 1: {//register new customer
				Database DB("new_customer", "1234");
				DB.errorQuit();//terminate on bad connection
				DB.RegisterCustomer();
				system("pause");
				system("CLS");
				break;
			}
			case 2: {//customer login
				UserInterface UI;
				loggedin = UI.login(custA);
				system("CLS");
				break;
			}
			case 3: {//browse games
				UI.BrowseGames();
				break;
			}
			case 4://exit
				break;
			default://invalid selection display error
				std::cerr << "\nERROR: Input must integer in range 1 to 4\n" << std::endl;
				system("pause");
				break;
			}
		}
		else {//customer menu

				std::cout << "\nGameStart Customer Menu\n";
				std::cout << "1. Browse Games\n";
				std::cout << "2. Rate Product\n";
				std::cout << "3. Edit Cart\n";
				std::cout << "4. Checkout\n";
				std::cout << "5. Order History\n";
				std::cout << "6. View Account Details\n";
				std::cout << "7. Update Account Details\n";
				std::cout << "8. Logout\n";
				option = UI.menu_select();

				switch (option)
				{
				case 1: {//browse games
					UI.BrowseGames();
					break;
				}
				case 2: {//rate games
					UI.RateGames(custA);
					break;
				}
				case 3: {//Edit cart
					std::cout << "\nCart Display\n";
					UI.displayCart(custA);
					std::cout << "\nOptions\n";
					std::cout << "1. Add item to cart\n";
					std::cout << "2. Remove item from cart\n";
					std::cout << "3. Exit\n";
					int cart = UI.menu_select();
					if(cart == 1){ UI.addToCart(custA, orderA); }
					else if (cart == 2) { UI.removeFromCart(custA); }
					else if (cart == 3) { std::cout << "\nReturning to menu.\n"; break; }
					else { std::cout << "\nInvalid input (must be a positive integer between 1 and 3)\nReturning to menu.\n"; break; }
						
					break;
				}
				case 4: {//checkout

					std::cout << "Cart Display\n";
					UI.displayCart(custA);
					std::cout << "Would you like to checkout?\n";
					std::cout << "1. Yes\n";
					std::cout << "2. No\n";
					int i = UI.menu_select();

					if (i == 1) {
						UI.checkout(custA);
						system("pause");
						system("CLS");
					}
					
					else {
						break;
					}

					break;
				}
				case 5: {//order history
					UserInterface UI;
					std::cout << "Your order history:\n";
					UI.orderHistory(custA);
					system("pause");
					system("CLS");
					break;
				}
				case 6: {//view account
					UI.view_account(custA);
					system("pause");
					system("CLS");
					break;
				}
				case 7: {//update account
					UI.updateAccount(custA);
					system("pause");
					system("CLS");
					break;
				}
				case 8: {
					loggedin = false;
					break;
				}
				default://invalid selection display error
					std::cerr << "\nERROR: Input must integer in range 1 to 8\n" << std::endl;
					system("pause");
					break;
				}
		}
	} while (option != 4 || loggedin);//continue unless user selects exit from main menu


	return 0;
}
