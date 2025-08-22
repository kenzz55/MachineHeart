// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MHBattleSpawnPoint.generated.h"

UCLASS()
class MACHINEHEART_API AMHBattleSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    TSubclassOf<ACharacter> EnemyClass;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    int32 SpawnIndex = 0;

};
