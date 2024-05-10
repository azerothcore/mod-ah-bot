/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AUCTION_HOUSE_BOT_CONFIG_H
#define AUCTION_HOUSE_BOT_CONFIG_H

#include "Define.h"
#include "Duration.h"
#include "SharedDefines.h"
#include <array>

enum AHItemQualities
{
    AHB_ITEM_QUALITY_POOR = 7,      // GREY
    AHB_ITEM_QUALITY_NORMAL,        // WHITE
    AHB_ITEM_QUALITY_UNCOMMON,      // GREEN
    AHB_ITEM_QUALITY_RARE,          // BLUE
    AHB_ITEM_QUALITY_EPIC,          // PURPLE
    AHB_ITEM_QUALITY_LEGENDARY,     // ORANGE
    AHB_ITEM_QUALITY_ARTIFACT,      // LIGHT YELLOW
};

constexpr uint32 AHB_MAX_DEFAULT_QUALITY = 6;
constexpr uint32 AHB_DEFAULT_QUALITY_SIZE = AHB_MAX_DEFAULT_QUALITY + 1;
constexpr uint32 AHB_MAX_QUALITY = AHB_ITEM_QUALITY_ARTIFACT + 1;

class AHBConfig
{
public:
    AHBConfig() = default;
    ~AHBConfig() = default;

    AHBConfig(uint32 ahid);

    inline uint32 GetAuctionHouseID()
    {
        return _auctionHouseID;
    }

    inline uint32 GetAuctionHouseFactionID()
    {
        return _auctionHouseFactionID;
    }

    inline void SetMinItems(uint32 value)
    {
        _minItems = value;
    }

    uint32 GetMinItems();

    inline void SetMaxItems(uint32 value)
    {
        _maxItems = value;
        // CalculatePercents() needs to be called, but only if
        // SetPercentages() has been called at least once already.
    }

    inline uint32 GetMaxItems()
    {
        return _maxItems;
    }

    void SetPercentages(std::array<float, AHB_MAX_QUALITY>& percentages);

    uint32 GetPercentages(uint32 color);

    void SetMinPrice(uint32 color, uint32 value);

    uint32 GetMinPrice(uint32 color);

    void SetMaxPrice(uint32 color, uint32 value);

    uint32 GetMaxPrice(uint32 color);

    void SetMinBidPrice(uint32 color, uint32 value);

    uint32 GetMinBidPrice(uint32 color);

    void SetMaxBidPrice(uint32 color, uint32 value);

    uint32 GetMaxBidPrice(uint32 color);

    void SetMaxStack(uint32 color, uint32 value);

    uint32 GetMaxStack(uint32 color);

    void SetBuyerPrice(uint32 color, uint32 value);

    uint32 GetBuyerPrice(uint32 color);

    inline void SetBiddingInterval(Minutes value)
    {
        _buyerBiddingInterval = value;
    }

    inline Minutes GetBiddingInterval()
    {
        return _buyerBiddingInterval;
    }

    void CalculatePercents();

    uint32 GetPercents(uint32 color);

    inline std::array<uint32, AHB_MAX_QUALITY> const* GetPercents()
    {
        return &_itemsPercentages;
    }

    void DecreaseItemCounts(uint32 Class, uint32 Quality);

    void DecreaseItemCounts(uint32 color);

    void IncreaseItemCounts(uint32 Class, uint32 Quality);

    void IncreaseItemCounts(uint32 color);

    void ResetItemCounts();

    uint32 TotalItemCounts();

    uint32 GetItemCounts(uint32 color);

    inline std::array<uint32, AHB_MAX_QUALITY> const* GetItemCounts()
    {
        return &_itemsCount;
    }

    inline void SetBidsPerInterval(uint32 value)
    {
        _buyerBidsPerInterval = value;
    }

    inline uint32 GetBidsPerInterval()
    {
        return _buyerBidsPerInterval;
    }

private:
    uint32 _auctionHouseID{ 0 };
    uint32 _auctionHouseFactionID{ 0 };

    uint32 _minItems{ 0 };
    uint32 _maxItems{ 0 };

    Minutes _buyerBiddingInterval{ 0min };
    uint32 _buyerBidsPerInterval{ 0 };

    std::array<float, AHB_MAX_QUALITY> _itemsPercent{};
    std::array<uint32, AHB_MAX_QUALITY> _itemsPercentages{};
    std::array<uint32, AHB_DEFAULT_QUALITY_SIZE> _buyerPrice{};
    std::array<uint32, AHB_DEFAULT_QUALITY_SIZE> _maxStack{};
    std::array<uint32, AHB_DEFAULT_QUALITY_SIZE> _minBidPrice{};
    std::array<uint32, AHB_DEFAULT_QUALITY_SIZE> _maxBidPrice{};
    std::array<uint32, AHB_DEFAULT_QUALITY_SIZE> _minPrice{};
    std::array<uint32, AHB_DEFAULT_QUALITY_SIZE> _maxPrice{};
    std::array<uint32, AHB_MAX_QUALITY> _itemsCount{};
};

#endif // AUCTION_HOUSE_BOT_CONFIG_H
