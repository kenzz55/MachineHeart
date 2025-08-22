// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/MHItemTypes.h"
#include "MHItemInventoryComponent.generated.h"

class AMHCharacterBase;
class UMHItemDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHINEHEART_API UMHItemInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:	
	// Sets default values for this component's properties
	UMHItemInventoryComponent();

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TArray<FInventoryItem> Items;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TMap<EEquipmentSlot, UMHItemDataAsset*> EquippedItems;

    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddItem(UMHItemDataAsset* Item);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool EquipItemByData(UMHItemDataAsset* ItemData);

    /** 특정 슬롯 해제 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UnequipSlot(EEquipmentSlot Slot);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItemByData(UMHItemDataAsset* ItemData);

    UFUNCTION()
    void SyncToGameInstance();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UseItem(UMHItemDataAsset* ItemData, AMHCharacterBase* OptionalTarget = nullptr);

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryUpdated OnInventoryUpdated;
		
};
