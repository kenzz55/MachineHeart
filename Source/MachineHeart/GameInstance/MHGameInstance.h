// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Items/MHItemDataAsset.h"
#include "Items/MHItemTypes.h"
#include "MHGameInstance.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipmentChangedNative, EEquipmentSlot);

UCLASS()
class MACHINEHEART_API UMHGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// 런타임에 복사해둘 스탯·인벤토리 데이터
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Persist")
	float SavedCurrentHp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Persist")
	TArray<FInventoryItem> SavedInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Persist")
	TMap<EEquipmentSlot, UMHItemDataAsset*> SavedEquippedItems;

	UPROPERTY(BlueprintReadWrite)
	FVector PendingReturnLocation = FVector::ZeroVector;

	
	UPROPERTY(BlueprintReadWrite)
	TSet<FName> ClearedBattleZones;

	UPROPERTY(BlueprintReadWrite, Category = "Persist")
	int32 SavedSkillGauge = 0;

	UFUNCTION(BlueprintPure, Category = "Persist|Equip")
	const UMHItemDataAsset* GetEquippedItem(EEquipmentSlot Slot) const;


	UFUNCTION(BlueprintCallable)
	void RegisterClearedBattleZone(FName ZoneID)
	{
		ClearedBattleZones.Add(ZoneID);
	}

	FOnEquipmentChangedNative OnEquipmentChanged;

	UFUNCTION(BlueprintCallable, Category = "Persist")
	void ResetPersistData();   

};
