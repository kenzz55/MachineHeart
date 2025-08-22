// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/MHVeteranGuardAtk7.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MHCharacterBase.h"
#include "Character/MHCharacterEnemy.h"
#include "Character/MHCharacterPlayer.h"

void UMHVeteranGuardAtk7::ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM)
{
    if (!Enemy) return;

    if (auto* Player = Cast<AMHCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(Enemy, 0)))
    {
        Enemy->PlayAttack(2.f);
        Player->ReceiveBattleDamageFromEnemy(Enemy, /*bUseBonus=*/true, /*Bonus=*/7.f);
        return;
    }

    Enemy->FinishTurn();
}
