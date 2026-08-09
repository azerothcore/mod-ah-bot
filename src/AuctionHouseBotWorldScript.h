/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE
 */

#ifndef AUCTION_HOUSE_BOT_WORLD_SCRIPT_H
#define AUCTION_HOUSE_BOT_WORLD_SCRIPT_H

#include "ScriptMgr.h"

// =============================================================================
// Interaction with the world core mechanisms
// =============================================================================

class AHBot_WorldScript : public WorldScript
{
private:
    void DeleteBots();
    void PopulateBots();

    bool _whisperLoginAttempted = false;
    uint32 _whisperLoginTimer = 0;

public:
    AHBot_WorldScript();

    void OnBeforeConfigLoad(bool reload) override;
    void OnStartup() override;
    void OnUpdate(uint32 diff) override;
};

#endif /* AUCTION_HOUSE_BOT_WORLD_SCRIPT_H */
