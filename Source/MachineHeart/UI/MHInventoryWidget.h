// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Items/MHItemDataAsset.h"
#include "MHInventoryWidget.generated.h"

/**
 * 
 */
class UWidget;
class UImage;
class UButton;
class UUniformGridPanel;
class UMHInventorySlot;
class UProgressBar;

UCLASS()
class MACHINEHEART_API UMHInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;


    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* InventoryGrid;

 
    UPROPERTY(EditAnywhere, Category = "Inventory")
    TSubclassOf<UMHInventorySlot> SlotWidgetClass;

   
    UPROPERTY(EditAnywhere, Category = "Inventory")
    int32 MaxRows = 4;

    UPROPERTY(EditAnywhere, Category = "Inventory")
    int32 NumColumns = 6;

    int32 PendingEquipSlotIndex;
    EEquipmentSlot PendingUnequipSlotType;

    UMHItemDataAsset* LastClickedData = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* AttackPowerText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* DefenseText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* HealthText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TurnSpeedText;

    UPROPERTY(meta = (BindWidget)) UWidget* ItemPanel;

    UPROPERTY(meta = (BindWidget)) UWidget* EquipmentPanel;

    UPROPERTY(meta = (BindWidget)) UWidget* EquippedPanel;
    UPROPERTY(meta = (BindWidget)) UButton* EquipButton;

    UPROPERTY(meta = (BindWidget)) UButton* UseButton;

    UPROPERTY(meta = (BindWidget)) UButton* UnequipButton;

    UPROPERTY(meta = (BindWidget)) UButton* DiscardButton;

    UPROPERTY(meta = (BindWidget)) UTextBlock* ItemNameText;

    UPROPERTY(meta = (BindWidget)) UTextBlock* StatsText;

    UPROPERTY(meta = (BindWidget)) UTextBlock* EffectText;

    UPROPERTY(meta = (BindWidget)) UTextBlock* Equipped_ItemNameText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Equipped_StatsText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Equipped_EffectText;

    UPROPERTY(meta = (BindWidget)) UTextBlock* Item_ItemNameText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Item_StatsText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Item_EffectText;

    UPROPERTY(meta = (BindWidget)) UImage* SwordSlotImage;
    UPROPERTY(meta = (BindWidget)) UImage* GunSlotImage;
    UPROPERTY(meta = (BindWidget)) UImage* EnergySlotImage;
    UPROPERTY(meta = (BindWidget)) UImage* ArmorSlotImage;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* SkillGaugeBar;

    UFUNCTION()
    void HandleSkillGaugeChanged(int32 NewGauge);

    UFUNCTION()
    void ShowInventoryItemMenu(UMHItemDataAsset * ClickedItem);

    UFUNCTION()
    void HandleUseButtonClicked();

private:
    UFUNCTION()
    void RefreshInventory();

    UFUNCTION()
    void RefreshStatsDisplay();

    UFUNCTION()
    void ShowItemMenu(UMHItemDataAsset* ClickedItem);

    UFUNCTION()
    void HandleEquipButtonClicked();

    UFUNCTION()
    void HandleUnequipButtonClicked();

    UFUNCTION()
    void HandleDiscardButtonClicked();

    UFUNCTION()
    void HandleHpChanged(float NewHp);

    bool IsBattleContext() const;

    UFUNCTION() FEventReply OnSwordSlotMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
    UFUNCTION() FEventReply OnGunSlotMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
    UFUNCTION() FEventReply OnEnergySlotMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
    UFUNCTION() FEventReply OnArmorSlotMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

    //UFUNCTION()
    //void HandleUnequipButtonClicked();
};
