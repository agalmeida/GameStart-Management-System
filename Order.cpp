#include "Order.hpp"

Order::Order() {}

Order::Order(const Order& order) {
	this->num = order.num;
	this->orderDate = order.orderDate;
	this->shipDate = order.shipDate;
	this->price = order.price;
	this->qty = order.qty;
	this->customerId = order.customerId;
	this->versionId = order.versionId;
}

Order::~Order() {};

Order Order::operator=(const Order& order) {
	this->num = order.num;
	this->orderDate = order.orderDate;
	this->shipDate = order.shipDate;
	this->price = order.price;
	this->qty = order.qty;
	this->customerId = order.customerId;
	this->versionId = order.versionId;
}

int Order::getOrderNum() const {
	return this->num;
}

std::string Order::getOrderDate() const {
	return this->orderDate;
}

std::string Order::getShipDate() const {
	return this->shipDate;
}

float Order::getPrice() const {
	return this->price;
}

int Order::getQty() const {
	return this->qty;
}

int Order::getCustomerId() const {
	return this->customerId;
}

int Order::getVersionId() const{
	return this->versionId;
}

void Order::setOrderNum(const int& num) {
	this->num = num;
}

void Order::setOrderDate(const std::string& orderDate) {
	this->orderDate = orderDate;
}

void Order::setShipDate(const std::string& shipDate) {
	this->shipDate = shipDate;
}

void Order::setPrice(const float& price) {
	this->price = price;
}

void Order::setQty(const int& qty) {
	this->qty = qty;
}

void Order::setCustomerId(const int& customerId) {
	this->customerId = customerId;
}

void Order::setVersionId(const int& versionId) {
	this->versionId = versionId;
}