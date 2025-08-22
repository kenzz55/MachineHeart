// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MHCharacterBase.h"
#include "UI/MHEnemyHPWidget.h"         
#include "TimerManager.h"
#include "Behaviors/EnemyBehaviorBase.h"
#include "Components/WidgetComponent.h"
#include "PaperFlipbookComponent.h"    
#include "PaperFlipbook.h"
#include "MHCharacterEnemy.generated.h"

class UMHItemDataAsset;
class USoundBase;
/**
 * 
 */
UCLASS()
class MACHINEHEART_API AMHCharacterEnemy : public AMHCharacterBase
{
	GENERATED_BODY()

public: 
	AMHCharacterEnemy();

    UPROPERTY(EditAnywhere, Category = "Anim|Flipbook")
    UPaperFlipbook* Flip_Idle = nullptr;

    UPROPERTY(EditAnywhere, Category = "Anim|Flipbook")
    UPaperFlipbook* Flip_Hurt = nullptr;

    UPROPERTY(EditAnywhere, Category = "Anim|Flipbook")
    UPaperFlipbook* Flip_Attack = nullptr;

    UPROPERTY(EditAnywhere, Category = "Anim|Flipbook")
    UPaperFlipbook* Flip_Death = nullptr;

    UPROPERTY(EditAnywhere, Category = "Anim|Flipbook")
    UPaperFlipbook* Flip_Heal = nullptr;

    UPROPERTY(EditAnywhere, Category = "Anim|Flipbook")
    UPaperFlipbook* Flip_Debuff = nullptr;

    UPROPERTY(EditAnywhere, Category = "Anim|Flipbook")
    UPaperFlipbook* Flip_Buff = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Anim|Flipbook")
    void PlayIdle(float PlayRate = 1.f);

    UFUNCTION(BlueprintCallable, Category = "Anim|Flipbook")
    void PlayHurt(float PlayRate = 1.f);

    UFUNCTION(BlueprintCallable, Category = "Anim|Flipbook")
    void PlayAttack(float PlayRate = 1.f);

    UFUNCTION(BlueprintCallable, Category = "Anim|Flipbook")
    void PlayDeath(float PlayRate = 1.f);

    UFUNCTION(BlueprintCallable, Category = "Anim|Flipbook")
    void PlayHeal(float PlayRate = 1.f);

    UFUNCTION(BlueprintCallable, Category = "Anim|Flipbook")
    void PlayDebuff(float PlayRate = 1.f);

    UFUNCTION(BlueprintCallable, Category = "Anim|Flipbook")
    void PlayBuff(float PlayRate = 1.f);

    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    USoundBase* DeathSFX;

    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    USoundBase* AttackedSFX;

protected:
    virtual void BeginPlay() override;

    // 스탯 컴포넌트의 OnHpZero 이벤트에 바인딩할 데스 핸들러
    UFUNCTION(BlueprintCallable)
    void HandleDeath();

    void OnHPChanged(float CurrentHp);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Behavior")
    UEnemyBehaviorBase* BehaviorAsset;

   
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* HPWidgetComp;

    
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> EnemyHPWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* InfoWidgetComp;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> InfoWidgetClass;

protected:
    virtual void OnTurnStarted_Implementation() override;

private:
    
    void ExecuteEnemyAction();

    UPROPERTY(VisibleAnywhere, Category = "Anim|Flipbook")
    UPaperFlipbookComponent* FlipbookComp;

    
    void PlayOnceThenIdle(UPaperFlipbook* FB, float PlayRate = 1.f);

    
    UFUNCTION()
    void HandleDamaged(float Damage, AActor* DamagedActor);

    FTimerHandle RevertToIdleTimer;
};
