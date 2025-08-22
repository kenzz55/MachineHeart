// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyBehaviorBase.generated.h"

class AMHCharacterEnemy;
class AMHBattleGameMode;


UCLASS(Abstract, BlueprintType)
class MACHINEHEART_API UEnemyBehaviorBase : public UDataAsset
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy|Behavior")
    void ExecuteBehavior(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM);
    virtual void ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM);

};
