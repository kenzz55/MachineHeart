// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/MHItemDataAsset.h" 
#include "Blueprint/UserWidget.h"
#include "MHSkillSlotWidget.generated.h"

/**
 * 
 */


class UImage;
class UDragDropOperation;

UCLASS()
class MACHINEHEART_API UMHSkillSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
  
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float SkillDamage = 25.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    EEquipmentSlot DesiredEquipSlot = EEquipmentSlot::None;

    UPROPERTY(BlueprintReadOnly, Category = "Skill")
    const UMHItemDataAsset* CurrentItemData = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Skill")
    void RefreshFromEquipped();

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* SlotImage;

    virtual void NativeConstruct() override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

private:
    void HandleEquipmentChanged(EEquipmentSlot ChangedSlot);
    FDelegateHandle EquipChangedHandle;
};
