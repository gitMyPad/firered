#include "gba/types.h"
#include "constants/flags.h"
#include "event_data.h"
#include "battle_ext.h"

u8 GetBadgeCount(void)
{
    u16 i;
    u8  count = 0;
    for (i = 1; i <= NUM_BADGES; i++) {
        if (FlagGet(FLAG_BADGE01_GET + i - 1))
            count++;
    }
    return count;
}

u16 GetBadgeExpMultiplier(u8 badgeCount)
{
    return 100 + 25*badgeCount;
}