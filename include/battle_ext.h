#ifndef GUARD_CONSTANTS_BATTLE_EXT_H
#define GUARD_CONSTANTS_BATTLE_EXT_H

#include "battle_main.h"

#define NORMAL_DMG_COUNTER 5

#define NONCRIT_DMG 10
#define CRIT_DMG 15

#define IS_CRIT (gCritMultiplier > NONCRIT_DMG)

u8 GetBadgeCount(void);
// ================= Type Calc  ===================
u8 GetTypeEffectivenessByType(u8 *effectCount, u8 atkType, u8 defType);
u8 AttacksThisTurn(u8 battlerId, u16 move); // Note: returns 1 if it's a charging turn, otherwise 2.
void BattleExtension_CheckImmuneAbilities(void);
void BattleExtension_TypeCalcParam5(u16 *move, u8 *attacker, u8 *target, u8 *battleFlag, bool8 updateBattleState);
void BattleExtension_TypeCalcWithDefType(u16 *move, u8 *attacker, u8 targDef1, u8 targDef2, u8 *battleFlag, bool8 updateBattleState);
void BattleExtension_TypeCalc(u16 *move, u8 *attacker, u8 *target);
void BattleExtension_CalcFriendshipDmg(u16 *move, u16 *power, u8 *attacker);
void BattleExtension_CalcFriendshipDmgMon(u16 *move, u16 *power, struct Pokemon *mon);

#define GET_BADGE_EXP_MULT(badgeCount) (100 + (badgeCount)*25)
#define GET_TYPE_EFFECTIVENESS(type1, type2) (gTypeEffectiveness[(type1)][(type2)])

#endif