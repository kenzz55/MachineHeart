// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/MHHelperGuardAtkBuff.h"
#include "GameMode/MHBattleGameMode.h"
#include "Character/MHCharacterEnemy.h"
#include "CharacterStat/MHCharacterStatComponent.h"

void UMHHelperGuardAtkBuff::ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM)
{
    if (!Enemy || !GM) { if (Enemy) Enemy->FinishTurn(); return; }

    Enemy->PlayBuff(); 

   
    TArray<AMHCharacterEnemy*> Alive;
    GM->GetAliveEnemies(Alive);       
    for (AMHCharacterEnemy* E : Alive)
    {
        if (!E || E == Enemy) continue; 
        if (UMHCharacterStatComponent* S = E->FindComponentByClass<UMHCharacterStatComponent>())
        {
            S->ApplyDamage(DamageToOthers); 
            S->AddAttackBuff(AttackBuff, BuffTurns);
        }
    }

    Enemy->FinishTurn();
}
