#ifndef GUARD_CONSTANTS_BATTLE_EXT_H
#define GUARD_CONSTANTS_BATTLE_EXT_H

u8 GetBadgeCount(void);

#define GET_BADGE_EXP_MULT(badgeCount) (100 + (badgeCount)*25)
// u16 GetBadgeExpMultiplier(u8 badgeCount);

#endif