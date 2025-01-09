#include "gba/types.h"
#include "constants/flags.h"
#include "event_data.h"
#include "battle_ext.h"
// ============= Part of Type Calculation ==================
#include "battle.h"
#include "battle_ai_script_commands.h"
#include "constants/battle_string_ids.h"
#include "constants/moves.h"
#include "constants/battle_move_effects.h"

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

// =========================================================
//                  Type Calculation:
// =========================================================

// format: attacking type, defending type, damage multiplier
// the multiplier is a (decimal) fixed-point number:
// 20 is ×2.0 TYPE_MUL_SUPER_EFFECTIVE
// 10 is ×1.0 TYPE_MUL_NORMAL
// 05 is ×0.5 TYPE_MUL_NOT_EFFECTIVE
// 00 is ×0.0 TYPE_MUL_NO_EFFECT
const u8 gTypeEffectiveness[NUMBER_OF_MON_TYPES][NUMBER_OF_MON_TYPES]   = {
    [TYPE_NORMAL]       = {
        [TYPE_ROCK]     = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GHOST]    = TYPE_MUL_NO_EFFECT
    },
    [TYPE_FIRE]         = {
        [TYPE_FIRE]     = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_WATER]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GRASS]    = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_ICE]      = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_BUG]      = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_ROCK]     = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_DRAGON]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_SUPER_EFFECTIVE
    },
    [TYPE_WATER]        = {
        [TYPE_FIRE]     = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_WATER]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GRASS]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GROUND]   = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_ROCK]     = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_DRAGON]   = TYPE_MUL_NOT_EFFECTIVE
    },
    [TYPE_ELECTRIC]     = {
        [TYPE_WATER]    = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_ELECTRIC] = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GRASS]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_FLYING]   = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_DRAGON]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GROUND]   = TYPE_MUL_NO_EFFECT
    },
    [TYPE_GRASS]        = {
        [TYPE_WATER]    = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_FIRE]     = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GRASS]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_POISON]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GROUND]   = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_FLYING]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_BUG]      = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_ROCK]     = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_DRAGON]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_NOT_EFFECTIVE
    },
    [TYPE_ICE]          = {
        [TYPE_WATER]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_ICE]      = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GRASS]    = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_GROUND]   = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_FLYING]   = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_DRAGON]   = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_FIRE]     = TYPE_MUL_NOT_EFFECTIVE
    },
    [TYPE_FIGHTING]     = {
        [TYPE_ICE]      = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_POISON]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_FLYING]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_PSYCHIC]  = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_BUG]      = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_ROCK]     = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_DARK]     = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_GHOST]    = TYPE_MUL_NO_EFFECT
    },
    [TYPE_POISON]       = {
        [TYPE_GRASS]    = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_POISON]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GROUND]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_ROCK]     = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GHOST]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_NO_EFFECT
    },
    [TYPE_GROUND]       = {
        [TYPE_FIRE]     = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_ELECTRIC] = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_GRASS]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_POISON]   = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_FLYING]   = TYPE_MUL_NO_EFFECT,
        [TYPE_BUG]      = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_ROCK]     = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_SUPER_EFFECTIVE
    },
    [TYPE_FLYING]       = {
        [TYPE_ELECTRIC] = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GRASS]    = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_FIGHTING] = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_BUG]      = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_ROCK]     = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_NOT_EFFECTIVE
    },
    [TYPE_PSYCHIC]      = {
        [TYPE_FIGHTING] = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_POISON]   = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_PSYCHIC]  = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_DARK]     = TYPE_MUL_NO_EFFECT,
        [TYPE_STEEL]    = TYPE_MUL_NOT_EFFECTIVE
    },
    [TYPE_BUG]          = {
        [TYPE_FIRE]     = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GRASS]    = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_FIGHTING] = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_POISON]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_FLYING]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_PSYCHIC]  = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_GHOST]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_DARK]     = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_NOT_EFFECTIVE
    },
    [TYPE_ROCK]         = {
        [TYPE_FIRE]     = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_ICE]      = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_FIGHTING] = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GROUND]   = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_FLYING]   = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_BUG]      = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_NOT_EFFECTIVE
    },
    [TYPE_GHOST]        = {
        [TYPE_NORMAL]   = TYPE_MUL_NO_EFFECT,
        [TYPE_PSYCHIC]  = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_DARK]     = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_GHOST]    = TYPE_MUL_SUPER_EFFECTIVE
    },
    [TYPE_DRAGON]       = {
        [TYPE_DRAGON]   = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_NOT_EFFECTIVE
    },
    [TYPE_DARK]         = {
        [TYPE_FIGHTING] = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_PSYCHIC]  = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_GHOST]    = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_DARK]     = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_NOT_EFFECTIVE
    },
    [TYPE_STEEL]        = {
        [TYPE_FIRE]     = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_WATER]    = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_ELECTRIC] = TYPE_MUL_NOT_EFFECTIVE,
        [TYPE_ICE]      = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_ROCK]     = TYPE_MUL_SUPER_EFFECTIVE,
        [TYPE_STEEL]    = TYPE_MUL_NOT_EFFECTIVE
    }
};

u8 AttacksThisTurn(u8 battlerId, u16 move) // Note: returns 1 if it's a charging turn, otherwise 2
{
    // first argument is unused
    if (gBattleMoves[move].effect == EFFECT_SOLAR_BEAM
        && (gBattleWeather & B_WEATHER_SUN))
        return 2;

    if (gBattleMoves[move].effect == EFFECT_SKULL_BASH
     || gBattleMoves[move].effect == EFFECT_RAZOR_WIND
     || gBattleMoves[move].effect == EFFECT_SKY_ATTACK
     || gBattleMoves[move].effect == EFFECT_SOLAR_BEAM
     || gBattleMoves[move].effect == EFFECT_SEMI_INVULNERABLE
     || gBattleMoves[move].effect == EFFECT_BIDE)
    {
        if ((gHitMarker & HITMARKER_CHARGING))
            return 1;
    }
    return 2;
}

void BattleExtension_CheckImmuneAbilities(void)
{
    u8 flags        = 0;
    s32 i           = 0;
    u8 effectCount  = 3;
    u8 moveType;

    if (gCurrentMove == MOVE_STRUGGLE || !gBattleMoves[gCurrentMove].power)
        return;

    GET_MOVE_TYPE(gCurrentMove, moveType);

    if (gBattleMons[gBattlerTarget].ability == ABILITY_LEVITATE &&
        moveType == TYPE_GROUND)
    {
        gLastUsedAbility = ABILITY_LEVITATE;
        gBattleCommunication[MISS_TYPE] = B_MSG_GROUND_MISS;
        RecordAbilityBattle(gBattlerTarget, ABILITY_LEVITATE);
        return;
    }

    GetTypeEffectivenessByType(&effectCount, moveType, gBattleMons[gBattlerTarget].type1);

    if (gBattleMons[gBattlerTarget].type2 != gBattleMons[gBattlerTarget].type1)
        GetTypeEffectivenessByType(&effectCount, moveType, gBattleMons[gBattlerTarget].type2);

    while (TRUE)
    {
        if (effectCount == 0)
        {
            gMoveResultFlags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
            gProtectStructs[gBattlerAttacker].targetNotAffected = 1;
            break;
        }
        if (effectCount == 3)
            break;

        if (effectCount < 3)
        {
            // Not very effective has a flagbit value of 1, apparently.
            flags |= 2;
            effectCount++;
        }
        else
        {
            // Super effective has a flagbit value of 1, apparently.
            flags |= 1;
            effectCount--;
        }
    }

    if (gBattleMons[gBattlerTarget].ability == ABILITY_WONDER_GUARD && 
        AttacksThisTurn(gBattlerAttacker, gCurrentMove) == 2)
    {
        if ((!(flags & 1)) && gBattleMoves[gCurrentMove].power)
        {
            gLastUsedAbility = ABILITY_WONDER_GUARD;
            gBattleCommunication[MISS_TYPE] = B_MSG_AVOIDED_DMG;
            RecordAbilityBattle(gBattlerTarget, ABILITY_WONDER_GUARD);
        }
    }
}

u8 GetTypeEffectivenessByType(u8 *effectCount, u8 atkType, u8 defType)
{
    u8 effectiveness    = TYPE_MUL_NORMAL;
    effectiveness       = GET_TYPE_EFFECTIVENESS(atkType, defType);

    switch (effectiveness) {
        case TYPE_MUL_NO_EFFECT:
            (*effectCount) = 0;
            break;
        case TYPE_MUL_NOT_EFFECTIVE:
            if ((*effectCount) > 0)
                (*effectCount)--;
            break;
        case TYPE_MUL_SUPER_EFFECTIVE:
            if ((*effectCount) > 0)
                (*effectCount)++;
            break;
    }
    return *effectCount;
}

void BattleExtension_TypeCalc(u16 *move, u8 *attacker, u8 *target)
{
    s32 i               = 0;
    u8 moveType;
    u8 effectCount      = 3;
    u8 effectiveness;

    // Fetch move type.
    GET_MOVE_TYPE(*move, moveType);

    if ((*move) == MOVE_RETURN || (*move) == MOVE_FRUSTRATION)
    {
        moveType    = gBattleMons[*attacker].type1;
    }
    
    // check stab
    if (IS_BATTLER_OF_TYPE(*attacker, moveType))
    {
        gBattleMoveDamage = (gBattleMoveDamage * 15) / 10;
    }

    if ((gBattleMons[*target].ability == ABILITY_LEVITATE) && 
        (moveType == TYPE_GROUND))
    {
        gLastUsedAbility = gBattleMons[*target].ability;
        gMoveResultFlags |= (MOVE_RESULT_MISSED | MOVE_RESULT_DOESNT_AFFECT_FOE);
        gLastLandedMoves[*target] = 0;
        gLastHitByType[*target] = 0;
        gBattleCommunication[MISS_TYPE] = B_MSG_GROUND_MISS;
        RecordAbilityBattle(*target, gLastUsedAbility);
        goto finalize_flags;
    }

    GetTypeEffectivenessByType(&effectCount, moveType, gBattleMons[*target].type1);
    if (gBattleMons[*target].type2 == gBattleMons[*target].type1)
        goto finalize_flags;

    GetTypeEffectivenessByType(&effectCount, moveType, gBattleMons[*target].type2);

    // Check for Foresight.
    if ((gBattleMons[*target].status2 & STATUS2_FORESIGHT) && 
        (effectCount < 1))
    {
        if (((moveType == TYPE_NORMAL) || (moveType == TYPE_FIGHTING)) &&
            (((gBattleMons[*target].type1 == TYPE_GHOST)) ||
            ((gBattleMons[*target].type2 == TYPE_GHOST))))
            effectCount = 3;
    }
    
    // Types determined. Write flags.
finalize_flags:
    while (TRUE)
    {
        // Move has no effect
        if (effectCount == 0)
        {
            gMoveResultFlags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
            gMoveResultFlags &= ~MOVE_RESULT_NOT_VERY_EFFECTIVE;
            gMoveResultFlags &= ~MOVE_RESULT_SUPER_EFFECTIVE;
            break;
        }
        if (effectCount == 3)
            break;

        if (effectCount < 3)
        {
            if (!(gMoveResultFlags & MOVE_RESULT_NOT_VERY_EFFECTIVE))
                gMoveResultFlags |= MOVE_RESULT_NOT_VERY_EFFECTIVE;
            effectCount++;
        }
        else
        {
            if (!(gMoveResultFlags & MOVE_RESULT_SUPER_EFFECTIVE))
                gMoveResultFlags |= MOVE_RESULT_SUPER_EFFECTIVE;
            effectCount--;
        }
    }

    if (gBattleMons[*target].ability == ABILITY_WONDER_GUARD && 
        AttacksThisTurn(*attacker, gCurrentMove) == 2 && 
        (!(gMoveResultFlags & MOVE_RESULT_SUPER_EFFECTIVE))
        && gBattleMoves[gCurrentMove].power)
    {
        gLastUsedAbility = ABILITY_WONDER_GUARD;
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gLastLandedMoves[*target] = 0;
        gLastHitByType[*target] = 0;
        gBattleCommunication[MISS_TYPE] = B_MSG_AVOIDED_DMG;
        RecordAbilityBattle(*target, gLastUsedAbility);
    }
    if (gMoveResultFlags & MOVE_RESULT_DOESNT_AFFECT_FOE)
        gProtectStructs[*attacker].targetNotAffected = 1;
}

// ===============================
//      Friendship damage
//      calculation
// ===============================

static void BattleExtension_CalcFriendshipDmg2(u16 *move, u16 *power, u8 friendship)
{
    // Avoid memory assignment in this region.
    if (power == NULL)
        return;

    if (gBattleMoves[(*move)].effect == EFFECT_RETURN)
        (*power) = gBattleMoves[(*move)].power * (friendship) / 25;
    else if (gBattleMoves[(*move)].effect == EFFECT_FRUSTRATION)// EFFECT_FRUSTRATION
        (*power) = gBattleMoves[(*move)].power * (255 - friendship) / 25;
}

void BattleExtension_CalcFriendshipDmg(u16 *move, u16 *power, u8 *attacker)
{
    BattleExtension_CalcFriendshipDmg2(move, power, gBattleMons[(*attacker)].friendship);
}

void BattleExtension_CalcFriendshipDmgMon(u16 *move, u16 *power, struct Pokemon *mon)
{
    u8 friendship   = GetMonData(mon, MON_DATA_FRIENDSHIP, NULL);
    BattleExtension_CalcFriendshipDmg2(move, power, friendship);
}