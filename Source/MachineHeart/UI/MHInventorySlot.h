// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MHInventorySlot.generated.h"

/**
 * 
 */

class UImage;
class UTextBlock;
class UMHItemDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, UMHItemDataAsset*, ItemData);


UCLASS()
class MACHINEHEART_API UMHInventorySlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetupSlot(UMHItemDataAsset* InItemData, int32 Quantity);

    UPROPERTY(BlueprintAssignable, Category = "Slot")
    FOnSlotClicked OnSlotClicked;


protected:
    virtual void NativeConstruct() override;

    UMHItemDataAsset* SlotItemData = nullptr;

    UPROPERTY(meta = (BindWidget))
    UImage* IconImage;

    UPROPERTY(meta = (BindWidget)) UTextBlock* QuantityText;

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
