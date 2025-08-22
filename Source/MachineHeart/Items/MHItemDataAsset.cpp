// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/MHItemDataAsset.h"
#include "Character/MHCharacterBase.h"
#include "Character/MHCharacterEnemy.h"
#include "CharacterStat/MHCharacterStatComponent.h"
#include "GameMode/MHBattleGameMode.h"
#include "Kismet/GameplayStatics.h"

bool UMHItemDataAsset::ApplyUse(AMHCharacterBase* User, AMHCharacterBase* Target, bool bIsBattle, AMHBattleGameMode* GM) const
{
    if (!bIsConsumable) return false;

    // 컨텍스트 가드
    if (UseContext == EItemUseContext::BattleOnly && !bIsBattle) return false;
    if (UseContext == EItemUseContext::FieldOnly && bIsBattle) return false;

    //Self 전용 효과 (회복/자버프/피격확률 감소)
    if (UseTarget == EItemUseTarget::Self)
    {
        if (!User || !User->Stat) return false;
        UMHCharacterStatComponent* ST = User->Stat;

        if (HealAmount > 0.f)                              ST->Heal(HealAmount);
        else
        {
            ST->ApplyDamage(-HealAmount);
        }
        if (AttackBuff != 0.f && AttackBuffTurns > 0)      ST->AddAttackBuff(AttackBuff, AttackBuffTurns);
        if (TurnSpeedBuff != 0.f && TurnSpeedBuffTurns > 0)ST->AddTurnSpeedBuff(TurnSpeedBuff, TurnSpeedBuffTurns);
        if (IncomingHitChanceScale < 1.0f && IncomingHitChanceTurns > 0)
            ST->AddIncomingDamageChanceScale(IncomingHitChanceScale, IncomingHitChanceTurns);
        return true;
    }

    // 모든 적 대상 효과
    if (UseTarget == EItemUseTarget::AllEnemies)
    {
        if (!bIsBattle || !GM) return false;

        TArray<AMHCharacterEnemy*> Enemies;
        GM->GetAliveEnemies(Enemies);

        bool bApplied = false;
        for (AMHCharacterEnemy* E : Enemies)
        {
            if (!E || !E->Stat) continue;

            if (AttackBuff != 0.f && AttackBuffTurns > 0)
            {
                E->Stat->AddAttackBuff(AttackBuff, AttackBuffTurns);
                bApplied = true;
            }

            
            if (TurnSpeedBuff != 0.f && TurnSpeedBuffTurns > 0)
            {
                E->Stat->AddTurnSpeedBuff(TurnSpeedBuff, TurnSpeedBuffTurns);
                bApplied = true;
            }

            if (EnemyDefenseDebuff != 0.f && EnemyDefenseDebuffTurns > 0)
            {
                E->Stat->AddDefenseDebuff(EnemyDefenseDebuff, EnemyDefenseDebuffTurns);
                bApplied = true;
            }
     
        }
        return bApplied;
    }


    return false;
}
