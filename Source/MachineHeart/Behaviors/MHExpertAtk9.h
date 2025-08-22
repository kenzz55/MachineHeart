// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Behaviors/EnemyBehaviorBase.h"
#include "MHExpertAtk9.generated.h"

/**
 * 
 */
UCLASS()
class MACHINEHEART_API UMHExpertAtk9 : public UEnemyBehaviorBase
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM) override;
};
