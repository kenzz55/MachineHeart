// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/MHHelperGuardDFDown.h"
#include "Character/MHCharacterEnemy.h"
#include "Character/MHCharacterPlayer.h"
#include "CharacterStat/MHCharacterStatComponent.h"
#include "Kismet/GameplayStatics.h"

void UMHHelperGuardDFDown::ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM)
{
    if (!Enemy || !GM) { if (Enemy) Enemy->FinishTurn(); return; }

    Enemy->PlayDebuff(); 

    if (AMHCharacterPlayer* Player = Cast<AMHCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(Enemy, 0)))
    {
        if (UMHCharacterStatComponent* S = Player->FindComponentByClass<UMHCharacterStatComponent>())
        {
            S->AddDefenseDebuff(DefenseDown, Turns); 
        }
    }

    Enemy->FinishTurn(); 
}
