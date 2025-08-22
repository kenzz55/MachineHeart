// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/MHExpertBuff.h"
#include "GameMode/MHBattleGameMode.h"
#include "Character/MHCharacterEnemy.h"
#include "CharacterStat/MHCharacterStatComponent.h"

void UMHExpertBuff::ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM)
{
    if (!Enemy) return;

    if (UMHCharacterStatComponent* S = Enemy->FindComponentByClass<UMHCharacterStatComponent>())
    {
        Enemy->PlayBuff();
        S->AddAttackBuff(AttackBuff, BuffTurns);
    }

    Enemy->FinishTurn();
}
