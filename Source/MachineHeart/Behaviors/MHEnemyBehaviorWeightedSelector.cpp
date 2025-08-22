// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/MHEnemyBehaviorWeightedSelector.h"
#include "Kismet/KismetMathLibrary.h"

void UMHEnemyBehaviorWeightedSelector::ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM)
{
    float TotalWeight = 0.f;
    for (auto& E : Entries)
        if (E.BehaviorAsset && E.Weight > 0.f)
            TotalWeight += E.Weight;

    if (TotalWeight <= 0.f)
    {
        // 기본 공격 폴백
        Super::ExecuteBehavior_Implementation(Enemy, GM);
        return;
    }

    float Roll = FMath::FRandRange(0.f, TotalWeight);
    float Accum = 0.f;
    for (auto& E : Entries)
    {
        if (!E.BehaviorAsset || E.Weight <= 0.f)
            continue;
        Accum += E.Weight;
        if (Roll <= Accum)
        {
            E.BehaviorAsset->ExecuteBehavior(Enemy, GM);
            return;
        }
    }

    
    Entries[0].BehaviorAsset->ExecuteBehavior(Enemy, GM);
}
