// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Behaviors/EnemyBehaviorBase.h"
#include "MHVeteranGuardTearGas.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MACHINEHEART_API UMHVeteranGuardTearGas : public UEnemyBehaviorBase
{
	GENERATED_BODY()

public:
	virtual void ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM) override;
};
