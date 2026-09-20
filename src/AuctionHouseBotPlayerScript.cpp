/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "WorldSession.h"
#include "Config.h"
#include "Log.h"
#include "ObjectMgr.h"

#include "AuctionHouseBot.h"
#include "AuctionHouseBotCommon.h"
#include "AHBotWhisperOrderParse.h"

class AHBot_PlayerScript : public PlayerScript
{
public:
    AHBot_PlayerScript() : PlayerScript("AHBot_PlayerScript", {
        PLAYERHOOK_CAN_PLAYER_USE_PRIVATE_CHAT
    }) { }

    bool OnPlayerCanUseChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg, Player* receiver) override
    {
        if (!gWhisperOrders || type != CHAT_MSG_WHISPER || !player || !receiver)
            return true;

        if (!gWhisperOrdersReceiver || receiver->GetGUID().GetCounter() != gWhisperOrdersReceiver)
            return true;

        if (!gWhisperOrdersAccount || player->GetSession()->GetAccountId() != gWhisperOrdersAccount)
        {
            LOG_INFO("module", "AHBot: whisper to clerk ignored (account {} not WhisperOrdersAccount {})",
                player->GetSession()->GetAccountId(), gWhisperOrdersAccount);
            return true; // silent ignore
        }

        uint32_t itemId = 0;
        uint32_t quantity = 0;
        if (!AHBotParseWhisperOrder(msg, itemId, quantity))
        {
            LOG_INFO("module", "AHBot: whisper to clerk not an order (need item-link + quantity). msg='{}'", msg);
            return true; // not an order; leave whisper alone
        }

        if (gBots.empty())
        {
            ChatHandler handler(player->GetSession());
            handler.PSendSysMessage("上架失败：拍卖机器人未就绪");
            return false;
        }

        LOG_INFO("module", "AHBot: whisper order from {} item {} qty {}", player->GetName(), itemId, quantity);

        // Listing is always performed by the AH seller bot(s), never by the clerk.
        AuctionHouseBot::OrderResult result = (*gBots.begin())->SellOrderedItem(itemId, quantity);

        if (ItemLocale const* il = sObjectMgr->GetItemLocale(itemId))
            ObjectMgr::GetLocaleString(il->Name, player->GetSession()->GetSessionDbLocaleIndex(), result.itemName);

        ChatHandler handler(player->GetSession());
        if (result.success)
        {
            if (result.listedStacks > 1)
            {
                handler.PSendSysMessage("已上架：{} x{}（共{}笔），起拍合计 {}，一口价合计 {}",
                    result.itemName, result.listedQuantity, result.listedStacks,
                    AHBotFormatCopper(result.totalBid), AHBotFormatCopper(result.totalBuyout));
            }
            else
            {
                handler.PSendSysMessage("已上架：{} x{}，起拍合计 {}，一口价合计 {}",
                    result.itemName, result.listedQuantity,
                    AHBotFormatCopper(result.totalBid), AHBotFormatCopper(result.totalBuyout));
            }
            LOG_INFO("module", "AHBot: whisper order OK listed {} x{}", itemId, result.listedQuantity);
        }
        else
        {
            if (result.listedQuantity > 0)
            {
                handler.PSendSysMessage("部分上架：{} x{}，起拍合计 {}，一口价合计 {}。{}",
                    result.itemName, result.listedQuantity,
                    AHBotFormatCopper(result.totalBid), AHBotFormatCopper(result.totalBuyout),
                    result.error);
            }
            else
                handler.PSendSysMessage("上架失败：{}", result.error);
            LOG_INFO("module", "AHBot: whisper order FAIL item {} err='{}'", itemId, result.error);
        }

        // Order handled; do not deliver whisper to clerk bot AI
        return false;
    }
};

void AddAHBotPlayerScripts()
{
    new AHBot_PlayerScript();
}
