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

#include "AuctionHouseBotConfig.h"
#include "AuctionHouseMgr.h"
#include "ItemTemplate.h"
#include <numeric>

#include "Log.h"

AHBConfig::AHBConfig(uint32 ahid)
{
    _auctionHouseID = ahid;

    switch (ahid)
    {
    case AUCTIONHOUSE_ALLIANCE:
        _auctionHouseFactionID = 55;
        break;
    case AUCTIONHOUSE_HORDE:
        _auctionHouseFactionID = 29;
        break;
    case AUCTIONHOUSE_NEUTRAL:
        _auctionHouseFactionID = 120;
        break;
    default:
        _auctionHouseFactionID = 120;
        break;
    }
}

uint32 AHBConfig::GetMinItems()
{
    if (!_minItems && _maxItems)
        return _maxItems;
    else if (_maxItems && (_minItems > _maxItems))
        return _maxItems;

    return _minItems;
}

void AHBConfig::SetPercentages(std::array<float, AHB_MAX_QUALITY>& percentages)
{
    float totalPercent = std::accumulate(percentages.begin(), percentages.end(), 0.f);

    if (totalPercent == 0)
    {
        _maxItems = 0;
    }
    else if (std::fabs(totalPercent-100) > 0.1) // float approximate equal
    {
        // re-normalize all percentages
        const float fixMultiplier = 100.f / totalPercent;

        for (auto& it : percentages)
            it *= fixMultiplier;

        LOG_WARN("module.ahbot", "AHConfig: Percentages don't add up to 100 (was {}), they have been auto-normalized.", totalPercent);
    }

    std::copy_n(percentages.begin(), AHB_MAX_QUALITY, _itemsPercent.begin());

    CalculatePercents();
}

uint32 AHBConfig::GetPercentages(uint32 color)
{
    if (color > AHB_ITEM_QUALITY_ARTIFACT)
        return 0;

    return _itemsPercent[color];
}

void AHBConfig::SetMinPrice(uint32 color, uint32 value)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return;

    _minPrice[color] = value;
}

uint32 AHBConfig::GetMinPrice(uint32 color)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return 0;

    auto _GetMinPrice = [this, color](uint32 returnValue)
    {
        uint32 minPrice = _minPrice[color];
        uint32 maxPrice = _maxPrice[color];

        if (!minPrice)
            return returnValue;
        else if (minPrice > maxPrice)
            return maxPrice;

        return minPrice;
    };

    switch (color)
    {
        case ITEM_QUALITY_POOR: return _GetMinPrice(100);
        case ITEM_QUALITY_NORMAL: return _GetMinPrice(150);
        case ITEM_QUALITY_UNCOMMON: return _GetMinPrice(200);
        case ITEM_QUALITY_RARE: return _GetMinPrice(250);
        case ITEM_QUALITY_EPIC: return _GetMinPrice(300);
        case ITEM_QUALITY_LEGENDARY: return _GetMinPrice(400);
        case ITEM_QUALITY_ARTIFACT: return _GetMinPrice(500);
        default:
            return 0;
    }
}

void AHBConfig::SetMaxPrice(uint32 color, uint32 value)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return;

    _maxPrice[color] = value;
}

uint32 AHBConfig::GetMaxPrice(uint32 color)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return 0;

    uint32 maxPrice = _maxPrice[color];

    switch (color)
    {
        case ITEM_QUALITY_POOR: return !maxPrice ? 150 : maxPrice;
        case ITEM_QUALITY_NORMAL: return !maxPrice ? 250 : maxPrice;
        case ITEM_QUALITY_UNCOMMON: return !maxPrice ? 300 : maxPrice;
        case ITEM_QUALITY_RARE: return !maxPrice ? 350 : maxPrice;
        case ITEM_QUALITY_EPIC: return !maxPrice ? 450 : maxPrice;
        case ITEM_QUALITY_LEGENDARY: return !maxPrice ? 550 : maxPrice;
        case ITEM_QUALITY_ARTIFACT: return !maxPrice ? 650 : maxPrice;
        default:
            return 0;
    }
}

void AHBConfig::SetMinBidPrice(uint32 color, uint32 value)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return;

    _minBidPrice[color] = value;
}

uint32 AHBConfig::GetMinBidPrice(uint32 color)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return 0;

    uint32 minBidPrice = _minBidPrice[color];
    return minBidPrice > 100 ? 100 : minBidPrice;
}

void AHBConfig::SetMaxBidPrice(uint32 color, uint32 value)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return;

    _maxBidPrice[color] = value;
}

uint32 AHBConfig::GetMaxBidPrice(uint32 color)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return 0;

    uint32 maxBidPrice = _maxBidPrice[color];
    return maxBidPrice > 100 ? 100 : maxBidPrice;
}

void AHBConfig::SetMaxStack(uint32 color, uint32 value)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return;

    _maxStack[color] = value;
}

uint32 AHBConfig::GetMaxStack(uint32 color)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return 0;

    return _maxStack[color];
}

void AHBConfig::SetBuyerPrice(uint32 color, uint32 value)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return;

    _buyerPrice[color] = value;
}

uint32 AHBConfig::GetBuyerPrice(uint32 color)
{
    if (color >= AHB_DEFAULT_QUALITY_SIZE)
        return 0;

    return _buyerPrice[color];
}

void AHBConfig::CalculatePercents()
{
    for (size_t i = 0; i < AHB_MAX_QUALITY; i++)
    {
        double itemPercent = _itemsPercent[i];
        _itemsPercentages[i] = uint32(std::ceil(itemPercent / 100 * _maxItems));
    }

    uint32 totalPercent = std::accumulate(_itemsPercentages.begin(), _itemsPercentages.end(), 0);
    int32 diff = (_maxItems - totalPercent);

    // If we don't match the expected maxItems, fill it up using Normal/Uncommon items
    if (diff < 0)
    {
        if (_itemsPercentages[AHB_ITEM_QUALITY_NORMAL] - diff > 0)
            _itemsPercentages[AHB_ITEM_QUALITY_NORMAL] -= diff;
        else if (_itemsPercentages[AHB_ITEM_QUALITY_UNCOMMON] - diff > 0)
            _itemsPercentages[AHB_ITEM_QUALITY_UNCOMMON] -= diff;
    }
    else if (diff > 0)
        _itemsPercentages[AHB_ITEM_QUALITY_NORMAL] += diff;
}

uint32 AHBConfig::GetPercents(uint32 color)
{
    if (color >= AHB_MAX_QUALITY)
        return 0;

    return _itemsPercentages[color];
}

void AHBConfig::DecreaseItemCounts(uint32 Class, uint32 Quality)
{
    switch (Class)
    {
    case ITEM_CLASS_TRADE_GOODS:
        DecreaseItemCounts(Quality);
        break;
    default:
        DecreaseItemCounts(Quality + 7);
        break;
    }
}

void AHBConfig::DecreaseItemCounts(uint32 color)
{
    if (color >= AHB_MAX_QUALITY)
        return;

    _itemsCount[color]--;
}

void AHBConfig::IncreaseItemCounts(uint32 Class, uint32 Quality)
{
    switch (Class)
    {
    case ITEM_CLASS_TRADE_GOODS:
        IncreaseItemCounts(Quality);
        break;
    default:
        IncreaseItemCounts(Quality + AHB_MAX_DEFAULT_QUALITY + 1);
        break;
    }
}

void AHBConfig::IncreaseItemCounts(uint32 color)
{
    if (color >= AHB_MAX_QUALITY)
        return;

    _itemsCount[color]++;
}

void AHBConfig::ResetItemCounts()
{
    _itemsCount.fill(0);
}

uint32 AHBConfig::TotalItemCounts()
{
    return std::accumulate(_itemsCount.begin(), _itemsCount.end(), 0);
}

uint32 AHBConfig::GetItemCounts(uint32 color)
{
    if (color >= AHB_MAX_QUALITY)
        return 0;

    return _itemsCount[color];
}
