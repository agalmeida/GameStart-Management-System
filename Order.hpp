#include <string>

#ifndef ORDER
#define ORDER

class Order {
private:
	int num;
	std::string orderDate;
	std::string shipDate;
	float price;
	int qty;
	int customerId;
	int versionId;
public:
	Order();
	~Order();
	Order(const Order& order);
	Order operator=(const Order& order);

	//Getters
	int getOrderNum() const;
	std::string getOrderDate() const;
	std::string getShipDate() const;
	float getPrice() const;
	int getQty() const;
	int getCustomerId() const;
	int getVersionId() const;

	//Setters
	void setOrderNum(const int& num);
	void setOrderDate(const std::string& orderDate);
	void setShipDate(const std::string& shipDate);
	void setPrice(const float& price);
	void setQty(const int& qty);
	void setCustomerId(const int& customerId);
	void setVersionId(const int& versionId);


};

#endif // !ORDER


