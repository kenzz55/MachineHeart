// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MHBattleManager.h"
#include "Character/MHCharacterBase.h"
#include "CharacterStat/MHCharacterStatComponent.h"
#include "MHBattleGameMode.generated.h"

/**
 * 
 */

//USTRUCT()
//struct FTurnEntry
//{
//    GENERATED_BODY()
//    UPROPERTY() AMHCharacterBase* Character = nullptr;
//    UPROPERTY() float Initiative = 0.f;
//};
class AMHCharacterEnemy;
class AMHCharacterPlayer;
class UUserWidget;
class UAudioComponent;
class USoundBase;

UCLASS()
class MACHINEHEART_API AMHBattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    AMHBattleGameMode();

    virtual void BeginPlay() override;

    virtual void StartPlay() override;

    UFUNCTION(BlueprintCallable)
    void EndBattle(bool bPlayerWon);

    UFUNCTION()
    void OnCharacterDeath(AMHCharacterBase* DeadCharacter);

    // 턴 시작·종료
    void StartTurn(AMHCharacterBase* Char);
    void EndTurn();

    UFUNCTION(BlueprintCallable, Category = "Battle")
    void GetAliveEnemies(TArray<AMHCharacterEnemy*>& OutEnemies) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
    UAudioComponent* BGMComp;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    USoundBase* BattleMusic;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    USoundBase* DefeatMusic;

    void TryStartBattle();



protected:
    void SpawnEnemiesFromPoints();


    UPROPERTY(EditAnywhere, Category = "Battle")
    FName WorldMapLevelName;

    UPROPERTY()
    AMHCharacterBase* PlayerActor;
    //TArray<AMHCharacterBase*> PlayerActors;
    UPROPERTY() 
    TArray<AMHCharacterEnemy*> EnemyActors;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> DefeatWidgetClass;

    UPROPERTY()
    UUserWidget* DefeatWidgetInstance = nullptr;

private:
    UPROPERTY()
    UMHBattleManager* BattleManager;

  
   
private:
    // 전체 참가자 & 턴 순서
    /*UPROPERTY()
    TArray<AMHCharacterBase*> Participants;*/

    UPROPERTY()
    TArray<AMHCharacterBase*> TurnOrder;

    bool bFirstTurnDelayDone = false;

    // **현재 턴**
    int32 CurrentTurnIndex = 0;
    
    void StartNewRound();
};
