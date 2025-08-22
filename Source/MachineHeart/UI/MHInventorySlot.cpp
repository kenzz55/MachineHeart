// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MHInventorySlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/MHItemDataAsset.h"

void UMHInventorySlot::SetupSlot(UMHItemDataAsset* InItemData, int32 Quantity)
{
    SlotItemData = InItemData;
    if (!IconImage || !QuantityText)
        return;

    if (InItemData && InItemData->ItemIcon)
    {
      
        IconImage->SetBrushFromTexture(InItemData->ItemIcon, false);
        IconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

        // ���� ǥ��
        if (Quantity > 1)
        {
            QuantityText->SetText(FText::AsNumber(Quantity));
            QuantityText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            QuantityText->SetText(FText::GetEmpty());
            QuantityText->SetVisibility(ESlateVisibility::Collapsed);
        }

        // ���� ��ü�� ���̰�
        SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        // ������ ������ ����
        SetVisibility(ESlateVisibility::Hidden);
    }

}

void UMHInventorySlot::NativeConstruct()
{
    Super::NativeConstruct();

    SetVisibility(ESlateVisibility::Hidden);
}

FReply UMHInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && SlotItemData)
    {
        OnSlotClicked.Broadcast(SlotItemData);
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}


