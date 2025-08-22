// Fill out your copyright notice in the Description page of Project Settings.
#include "Behaviors/EnemyBehaviorBase.h"
#include "EnemyBehaviorBase.h"
#include "Character/MHCharacterEnemy.h"
#include "GameMode/MHBattleGameMode.h"
#include "CharacterStat/MHCharacterStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MHCharacterPlayer.h"


void UEnemyBehaviorBase::ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM)
{
    if (!Enemy || !GM) { if (Enemy) Enemy->FinishTurn(); return; }

    if (AMHCharacterPlayer* Player = Cast<AMHCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(Enemy, 0)))
    {
       
        Player->ReceiveBattleDamageFromEnemy(Enemy, /*bUseBonus=*/false, /*Bonus=*/0.f);
        return; 
    }

    Enemy->FinishTurn();

}
