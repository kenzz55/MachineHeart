// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MHSkillSlotWidget.h"
#include "GameInstance/MHGameInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"

void UMHSkillSlotWidget::RefreshFromEquipped()
{
    CurrentItemData = nullptr;

    if (UWorld* World = GetWorld())
    {
        if (UMHGameInstance* GI = World->GetGameInstance<UMHGameInstance>())
        {
            CurrentItemData = GI->GetEquippedItem(DesiredEquipSlot);
        }
    }

    // 아이콘 세팅
    if (SlotImage)
    {
        if (CurrentItemData && CurrentItemData->ItemIcon) 
        {
            SlotImage->SetBrushFromTexture(CurrentItemData->ItemIcon, /*bMatchSize=*/false);
        }
        else
        {
            
            SlotImage->SetBrushFromTexture(nullptr);
        }
    }

    const bool bHasItem = (CurrentItemData != nullptr);

    
    SetVisibility(bHasItem ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

    
    SetIsEnabled(bHasItem);
}

void UMHSkillSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
    RefreshFromEquipped();

    if (UMHGameInstance* GI = GetWorld()->GetGameInstance<UMHGameInstance>())
    {
        EquipChangedHandle = GI->OnEquipmentChanged.AddUObject(
            this, &UMHSkillSlotWidget::HandleEquipmentChanged);
    }
}

FReply UMHSkillSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton)
            .NativeReply;
    }
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UMHSkillSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    UDragDropOperation* Op = NewObject<UDragDropOperation>(this);
    Op->Payload = this;
    Op->Pivot = EDragPivot::CenterCenter;

    if (SlotImage)
    {
        UImage* DragImg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
        const FSlateBrush BrushCopy = SlotImage->GetBrush();
        DragImg->SetBrush(BrushCopy);
        Op->DefaultDragVisual = DragImg;
    }
    OutOperation = Op;
}

void UMHSkillSlotWidget::HandleEquipmentChanged(EEquipmentSlot ChangedSlot)
{
    if (ChangedSlot == DesiredEquipSlot)
    {
        RefreshFromEquipped();
    }
}
