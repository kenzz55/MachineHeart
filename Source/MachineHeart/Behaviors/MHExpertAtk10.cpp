// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/MHExpertAtk10.h"
#include "Character/MHCharacterEnemy.h"
#include "Character/MHCharacterPlayer.h"
#include "CharacterStat/MHCharacterStatComponent.h"
#include "Kismet/GameplayStatics.h"

void UMHExpertAtk10::ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM)
{
    if (!Enemy) return;

    if (auto* Player = Cast<AMHCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(Enemy, 0)))
    {
        if (FMath::FRand() <= 0.75f)
        {
            Enemy->PlayAttack(2.f);
            Player->ReceiveBattleDamageFromEnemy(Enemy, /*bUseBonus=*/true, /*Bonus=*/10.f);

            if (UMHCharacterStatComponent* S = Player->FindComponentByClass<UMHCharacterStatComponent>())
            {
                S->AddDefenseDebuff(DefenseDown, Turns);
            }

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
