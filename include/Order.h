#pragma once

#include <list>
#include <exception>
#include <format>

#include "OrderType.h"
#include "Side.h"
#include "Usings.h"
#include "Constants.h"


class Order
{
    public:
        Order(OrderType orderType, OrderId orderid, Side side, Price price, Quantity quantity)
            : orderType_{ orderType }
            , orderId_{ orderid }
            , side_{ side }
            , price_{ price }
            , InitialQuantity_{ quantity }
            , remainingQuantity_{ quantity }
        { }

        // New market order constructor for sweeping the market. I.e. you want to fill for as much as possible
        // Agressing agaisnt all asks, therefore we do not care about price
        Order(OrderId orderId, Side side, Quantity quantity)
            : Order(OrderType::Market, orderId, side, Constants::InvalidPrice, quantity)
        { }

        OrderId GetOrderId() const { return orderId_; }
        Side GetSide() const { return side_; }
        Price GetPrice() const { return price_; }
        OrderType GetOrderType() const { return orderType_; }
        Quantity GetInitialQuantity() const { return InitialQuantity_; }
        Quantity GetRemainingQuantity() const { return remainingQuantity_; }
        Quantity GetFilledQuantity() const { return GetInitialQuantity() - GetRemainingQuantity(); }
        bool IsFilled() const {return GetRemainingQuantity() == 0;}
        void Fill(Quantity quantity)
        {
            if (quantity > GetRemainingQuantity())
                throw std::logic_error(std::format("Order ({}) cannot be filled for more than its ramaining quantity." , GetOrderId()));

            remainingQuantity_ -= quantity;
        }
        void ToGoodTillCancel(Price price)
        {
            if (GetOrderType() != OrderType::Market)
                throw std::logic_error(std::format("Order ({}) cannot have its price adjusted, only market orders can." , GetOrderId()));
            
            if (!std::isfinite(price))
                throw std::logic_error(std::format("Order ({}) must be a tradeable price", GetOrderId()));
        }

    private:
        OrderType orderType_;
        OrderId orderId_;
        Side side_;
        Price price_;
        Quantity InitialQuantity_;
        Quantity remainingQuantity_;    
};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;