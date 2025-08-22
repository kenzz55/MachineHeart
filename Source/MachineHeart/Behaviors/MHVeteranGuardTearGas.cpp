// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/MHVeteranGuardTearGas.h"
#include "Character/MHCharacterEnemy.h"
#include "CharacterStat/MHCharacterStatComponent.h"
#include "GameMode/MHBattleGameMode.h"

void UMHVeteranGuardTearGas::ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM)
{
    if (!Enemy) return;

    //StatComponent �� 20% Ȯ�� ���� ���� 2��(��ų ��� �� ����) ����
    if (auto* EStat = Enemy->FindComponentByClass<UMHCharacterStatComponent>())
    {
        EStat->AddIgnoreNextAttack(0.2f, 2);
        UE_LOG(LogTemp, Warning, TEXT("Tear Gas! ."));
    }

  
    Enemy->FinishTurn();
}
