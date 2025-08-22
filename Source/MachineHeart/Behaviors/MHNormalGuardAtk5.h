// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Behaviors/EnemyBehaviorBase.h"
#include "MHNormalGuardAtk5.generated.h"

/**
 * 
 */
UCLASS()
class MACHINEHEART_API UMHNormalGuardAtk5 : public UEnemyBehaviorBase
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM) override;
};
