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
    u8 effectCount  = NORMAL_DMG_COUNTER;
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
        if (effectCount == NORMAL_DMG_COUNTER)
            break;

        if (effectCount < NORMAL_DMG_COUNTER)
        {
            // Not very effective has a flagbit value of 2, apparently.
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

static u8 GetTypeEffectByTypeRaw(u8 *effectCount, u8 atkType, u8 defType)
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

static void BattleExtension_TypeCalcHelperWithKnownType(u16 *move, u8 *moveType,
                                           u8 *attackerType1, u8 *attackerType2,
                                           u8 *defenderType1, u8 *defenderType2,
                                           u8 attacker)
{
    *attackerType1  = gBattleMons[attacker].type1;
    *attackerType2  = gBattleMons[attacker].type2;

    if (gBattleStruct->dynamicMoveType)
        *moveType   = gBattleStruct->dynamicMoveType & DYNAMIC_TYPE_MASK;
    else if ((*move) == MOVE_RETURN || (*move) == MOVE_FRUSTRATION)
        *moveType   = *attackerType1;
    else
        *moveType = gBattleMoves[*move].type;
}

static void BattleExtension_TypeCalcHelper(u16 *move, u8 *moveType,
                                           u8 *attackerType1, u8 *attackerType2,
                                           u8 *defenderType1, u8 *defenderType2,
                                           u8 attacker, u8 target)
{
    *defenderType1  = gBattleMons[target].type1;
    *defenderType2  = gBattleMons[target].type2;
    BattleExtension_TypeCalcHelperWithKnownType(move, moveType,
                                                attackerType1, attackerType2,
                                                defenderType1, defenderType2,
                                                attacker);

    // Iron Shell will adopt the STEEL type whenever
    // the defender takes a super effective hit.
    if (gBattleMons[target].ability == ABILITY_IRON_SHELL)
    {
        if ((GET_TYPE_EFFECTIVENESS(*moveType, *defenderType1) == TYPE_MUL_SUPER_EFFECTIVE) &&
            (*defenderType1 != TYPE_STEEL))
            *defenderType1  = TYPE_STEEL;

        if ((*defenderType2 != *defenderType1) &&
            (GET_TYPE_EFFECTIVENESS(*moveType, *defenderType2) == TYPE_MUL_SUPER_EFFECTIVE) &&
            (*defenderType2 != TYPE_STEEL))
            *defenderType2  = TYPE_STEEL;
    }

    // Foresight overrides the ghost type immunity from
    // Normal and Fighting type attacks.
    // Current implementation is to set the target mon's
    // effective type from Ghost to ???, which should
    // take neutral damage across all sources.
    if ((gBattleMons[target].status2 & STATUS2_FORESIGHT) &&
        (IS_BATTLER_OF_TYPE2(*defenderType1, *defenderType2, TYPE_GHOST)) &&
        ((*moveType == TYPE_FIGHTING) ||
         (*moveType == TYPE_NORMAL)))
    {
        if (*defenderType1 == TYPE_GHOST)
            *defenderType1  = TYPE_MYSTERY;
        if (*defenderType2 == TYPE_GHOST)
            *defenderType2  = TYPE_MYSTERY;
    }
}

u8 GetTypeEffectivenessByType(u8 *effectCount, u8 atkType, u8 defType)
{
    return GetTypeEffectByTypeRaw(effectCount, atkType, defType);
}

void BattleExtension_TypeCalcParam5(u16 *move, u8 *attacker, u8 *target, u8 *battleFlag,
                                    bool8 updateBattleState)
{
    s32 i               = 0;
    u8 effectCount      = NORMAL_DMG_COUNTER;
    u8 moveType;
    u8 attackerType1, attackerType2;
    u8 defenderType1, defenderType2;
    u8 effectiveness;

    // Fetch move type, as well as attacker and
    // target types.
    BattleExtension_TypeCalcHelper(move, &moveType, &attackerType1,
                                   &attackerType2, &defenderType1,
                                   &defenderType2, *attacker,
                                   *target);

    // check stab
    if (IS_BATTLER_OF_TYPE2(attackerType1, attackerType2, moveType))
        gBattleMoveDamage = (gBattleMoveDamage * 15) / 10;

    if ((gBattleMons[*target].ability == ABILITY_LEVITATE) && 
        (moveType == TYPE_GROUND))
    {
        if (updateBattleState)
            gLastUsedAbility                = gBattleMons[*target].ability;
        (*battleFlag)                      |= (MOVE_RESULT_MISSED | MOVE_RESULT_DOESNT_AFFECT_FOE);

        if (updateBattleState)
        {
            gLastLandedMoves[*target]       = 0;
            gLastHitByType[*target]         = 0;
            gBattleCommunication[MISS_TYPE] = B_MSG_GROUND_MISS;
            RecordAbilityBattle(*target, gLastUsedAbility);
        }
        goto finalize_flags;
    }

    GetTypeEffectivenessByType(&effectCount, moveType, defenderType1);
    if (!(IS_BATTLER_MULTITYPE(defenderType1, defenderType2)))
        goto check_soft_shell;

    GetTypeEffectivenessByType(&effectCount, moveType, defenderType2);

check_soft_shell:
    // Check for Soft Shell (and block it if Mold Breaker is ever implemented).
    if (gBattleMons[*target].ability == ABILITY_SOFT_SHELL)
    {
        switch (moveType)
        {
            case TYPE_FIGHTING:
            case TYPE_GROUND:
                if (effectCount > 0)
                    effectCount--;
                break;
        }
    }
    
    // Check if the move is a status move before writing the flags.
    if (((gBattleMoves[*move].targetFlags & MOVETARGET_MASK) == 
          MOVETARGET_FLAG_STATUS) && 
        ((effectCount != NORMAL_DMG_COUNTER) && 
         (effectCount != 0)))
    {
        effectCount = NORMAL_DMG_COUNTER;
    }
        
    // Types determined. Write flags.
finalize_flags:
    while (TRUE)
    {
        // Move has no effect
        if (effectCount == 0)
        {
            (*battleFlag) |= MOVE_RESULT_DOESNT_AFFECT_FOE;
            (*battleFlag) &= ~MOVE_RESULT_NOT_VERY_EFFECTIVE;
            (*battleFlag) &= ~MOVE_RESULT_SUPER_EFFECTIVE;
            break;
        }
        if (effectCount == NORMAL_DMG_COUNTER)
            break;

        if (effectCount < NORMAL_DMG_COUNTER)
        {
            if (!((*battleFlag) & MOVE_RESULT_NOT_VERY_EFFECTIVE))
                (*battleFlag) |= MOVE_RESULT_NOT_VERY_EFFECTIVE;
        }
        else
        {
            if (!((*battleFlag) & MOVE_RESULT_SUPER_EFFECTIVE))
                (*battleFlag) |= MOVE_RESULT_SUPER_EFFECTIVE;
        }
        effectCount = NORMAL_DMG_COUNTER;
    }

    if (gBattleMons[*target].ability == ABILITY_WONDER_GUARD && 
        AttacksThisTurn(*attacker, gCurrentMove) == 2 && 
        (!((*battleFlag) & MOVE_RESULT_SUPER_EFFECTIVE))
        && gBattleMoves[gCurrentMove].power)
    {
        if (updateBattleState)
            gLastUsedAbility            = ABILITY_WONDER_GUARD;

        (*battleFlag)                  |= MOVE_RESULT_MISSED;

        if (updateBattleState)
        {
            gLastLandedMoves[*target]   = 0;
            gLastHitByType[*target]     = 0;
            gBattleCommunication[MISS_TYPE] = B_MSG_AVOIDED_DMG;
            RecordAbilityBattle(*target, gLastUsedAbility);
        }
    }
    if ((updateBattleState) &&
        (*battleFlag) & MOVE_RESULT_DOESNT_AFFECT_FOE)
        gProtectStructs[*attacker].targetNotAffected = 1;
}

void BattleExtension_TypeCalcWithDefType(u16 *move, u8 *attacker, u8 targDef1, u8 targDef2, u8 *battleFlag, bool8 updateBattleState)
{
    u8 moveType;
    u8 attackerType1, attackerType2;

    BattleExtension_TypeCalcHelperWithKnownType(move, &moveType, &attackerType1,
                                                &attackerType2, &targDef1,
                                                &targDef2, *attacker);
}

void BattleExtension_TypeCalc(u16 *move, u8 *attacker, u8 *target)
{
    BattleExtension_TypeCalcParam5(move, attacker, target, &gMoveResultFlags, TRUE);
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