// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/MHItemDataAsset.h"
#include "Blueprint/UserWidget.h"
#include "MHBattleUIWidget.generated.h"

/**
 * 
 */
class UDragDropOperation;
class AMHCharacterEnemy;
class UMHItemDataAsset;

UCLASS()
class MACHINEHEART_API UMHBattleUIWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	EEquipmentSlot PendingSlot = EEquipmentSlot::None;
protected:

	virtual bool NativeOnDragOver(const FGeometry& InGeometry,const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InEvent, UDragDropOperation* InOp) override;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UMHGaugeWidget* GaugeWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UCanvasPanel* CombatPanel;

	UFUNCTION()
	void HandleGaugeResult(bool bSuccess);

private:
	
	UPROPERTY()
	AMHCharacterEnemy* PendingTarget = nullptr;

	UPROPERTY() const UMHItemDataAsset* PendingItem = nullptr;

	float PendingDamage = 0.f;
};
