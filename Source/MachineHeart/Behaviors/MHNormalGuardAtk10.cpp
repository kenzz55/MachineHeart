// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/MHNormalGuardAtk10.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MHCharacterBase.h"
#include "Character/MHCharacterEnemy.h"
#include "Character/MHCharacterPlayer.h"

void UMHNormalGuardAtk10::ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM)
{
    if (!Enemy) return;

    if (auto* Player = Cast<AMHCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(Enemy, 0)))
    {
        if (FMath::FRand() <= 0.7f)
        {
            Enemy->PlayAttack(2.f);
            Player->ReceiveBattleDamageFromEnemy(Enemy, /*bUseBonus=*/true, /*Bonus=*/10.f);
            return;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("miss!"));
            Enemy->FinishTurn();
            return;
        }
    }

    Enemy->FinishTurn();
}
