#ifndef AH_BOT_WHISPER_ORDER_PARSE_H
#define AH_BOT_WHISPER_ORDER_PARSE_H

#include <cstdint>
#include <cctype>
#include <string>

inline bool AHBotParseWhisperOrder(std::string const& msg, uint32_t& outItemId, uint32_t& outQuantity)
{
    std::size_t const itemTag = msg.find("|Hitem:");
    if (itemTag == std::string::npos)
        return false;

    std::size_t pos = itemTag + 7;
    if (pos >= msg.size() || !std::isdigit(static_cast<unsigned char>(msg[pos])))
        return false;

    uint32_t itemId = 0;
    while (pos < msg.size() && std::isdigit(static_cast<unsigned char>(msg[pos])))
    {
        itemId = itemId * 10u + static_cast<uint32_t>(msg[pos] - '0');
        ++pos;
    }
    if (itemId == 0)
        return false;

    // Link form: |Hitem:...|h[Name]|h
    std::size_t firstH = msg.find("|h", pos);
    if (firstH == std::string::npos)
        return false;
    std::size_t secondH = msg.find("|h", firstH + 2);
    if (secondH == std::string::npos)
        return false;

    pos = secondH + 2;
    // Client links often end with |r before the quantity.
    if (pos + 1 < msg.size() && msg[pos] == '|' && msg[pos + 1] == 'r')
        pos += 2;

    while (pos < msg.size() && std::isspace(static_cast<unsigned char>(msg[pos])))
        ++pos;

    if (pos >= msg.size() || !std::isdigit(static_cast<unsigned char>(msg[pos])))
        return false;

    uint32_t quantity = 0;
    while (pos < msg.size() && std::isdigit(static_cast<unsigned char>(msg[pos])))
    {
        quantity = quantity * 10u + static_cast<uint32_t>(msg[pos] - '0');
        ++pos;
    }
    if (quantity == 0)
        return false;

    while (pos < msg.size() && std::isspace(static_cast<unsigned char>(msg[pos])))
        ++pos;
    if (pos != msg.size())
        return false;

    outItemId = itemId;
    outQuantity = quantity;
    return true;
}

inline std::string AHBotFormatCopper(uint64_t copper)
{
    uint64_t g = copper / 10000ull;
    uint64_t s = (copper % 10000ull) / 100ull;
    uint64_t c = copper % 100ull;
    return std::to_string(g) + "g " + std::to_string(s) + "s " + std::to_string(c) + "c";
}

#endif
