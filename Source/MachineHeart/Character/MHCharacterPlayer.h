// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MHCharacterBase.h"
#include "Components/WidgetComponent.h" 
#include "Inventory/MHItemInventoryComponent.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "GameInstance/MHGameInstance.h"
#include "UI/MHEnemyHPWidget.h" 
#include "MHCharacterPlayer.generated.h"

/**
 * 
 */

class UMHParryWidget;
class AMHCharacterEnemy;
class USoundBase;

UCLASS()
class MACHINEHEART_API AMHCharacterPlayer : public AMHCharacterBase
{
	GENERATED_BODY()

public:
	AMHCharacterPlayer();

	virtual void OnTurnStarted_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void SaveStateToGameInstance();
	
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void LoadStateFromGameInstance();

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void UseAllEnemySkill();

	UFUNCTION(BlueprintCallable, Category = "Battle|Parry")
	void ReceiveBattleDamageFromEnemy(AMHCharacterEnemy* InAttacker, bool bUseBonus = false, float BonusValue = 0.f);

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void PlayAttackOverlayOnce(EEquipmentSlot Slot);

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundBase* AttackedSFX;


protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMHItemInventoryComponent> Inventory;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PlayerHPWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HPWidgetComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hit")
	UPaperFlipbookComponent* HitOverlay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit")
	UPaperFlipbook* HitFB;

	void PlayHitOverlayOnce();
	FTimerHandle HitHideHandle;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	UPaperFlipbookComponent* AttackOverlay;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TMap<EEquipmentSlot, UPaperFlipbook*> AttackFlipbooks;

	


	FTimerHandle AttackHideHandle;

	UPROPERTY(EditAnywhere, Category = "Hit")
	bool bUseHitCooldown = false;

	bool bHitCoolingDown = false;
	FTimerHandle HitCooldownHandle;

	UPROPERTY(EditAnywhere, Category = "Hit", meta = (EditCondition = "bUseHitCooldown"))
	float HitCooldownSeconds = 0.05f; 

	UFUNCTION()
	void HandleDeath();

	void OnHPChanged(float CurrentHp);

private:

	UPROPERTY(EditDefaultsOnly, Category = "UI|Parry")
	TSubclassOf<UMHParryWidget> ParryWidgetClass;

	
	UPROPERTY() UMHParryWidget* ParryWidget = nullptr;

	
	UPROPERTY(EditAnywhere, Category = "Battle|Parry")
	int32 ParryCounterDamage = 5;

	
	TWeakObjectPtr<AMHCharacterEnemy> PendingAttacker = nullptr;
	bool  bPendingUseBonus = false;
	float PendingBonusValue = 0.f;

	// 패링 결과 콜백
	UFUNCTION() void HandleParryResult(bool bSuccess);

	
	FDelegateHandle ParryHandle;

};
