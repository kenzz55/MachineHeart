// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Behaviors/EnemyBehaviorBase.h"
#include "MHEnemyBehaviorWeightedSelector.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FBehaviorWeightEntry
{
    GENERATED_BODY()

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    UEnemyBehaviorBase* BehaviorAsset = nullptr;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior", meta = (ClampMin = "0.0"))
    float Weight = 1.0f;
};


UCLASS(BlueprintType)
class MACHINEHEART_API UMHEnemyBehaviorWeightedSelector : public UEnemyBehaviorBase
{
	GENERATED_BODY()

public:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    TArray<FBehaviorWeightEntry> Entries;

    virtual void ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM) override;
	
};
