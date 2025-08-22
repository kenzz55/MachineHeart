// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MHGaugeWidget.h"
#include "TimerManager.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UMHGaugeWidget::NativeConstruct()
{
    Super::NativeConstruct();

    Elapsed = 0.f;
    bAnyFail = false;
    HitMarkers.Empty();
    Markers.Empty();

   
    if (MarkerImage0) Markers.Add(MarkerImage0);
    if (MarkerImage1) Markers.Add(MarkerImage1);
    if (MarkerImage2) Markers.Add(MarkerImage2);



    if (MovingBarImage)
    {
        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MovingBarImage->Slot))
        {
            InitMovingPos = CanvasSlot->GetPosition();
        }
    }

    // ��� ��Ŀ �ʱ� �� ȸ��
    for (UImage* M : Markers)
    {
        if (M) M->SetColorAndOpacity(FLinearColor::Gray);
    }

    // �Է� ��� UI Only
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        FInputModeUIOnly Mode;
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(Mode);
    }
}

void UMHGaugeWidget::StartGauge()
{
    Elapsed = 0.f;
    bAnyFail = false;
    HitMarkers.Empty();

    // ���� ��ġ ����
    if (MovingBarImage)
    {
        MovingBarImage->SetVisibility(ESlateVisibility::HitTestInvisible);

        if (bUseDesignerStartPos)
        {
            if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MovingBarImage->Slot))
            {
                CanvasSlot->SetPosition(InitMovingPos);
            }
        }
        else
        {
            MovingBarImage->SetRenderTranslation(InitMovingPos);
        }
    }

    // ��Ŀ�� ���̰� / ȸ��
    for (UImage* M : Markers)
    {
        if (M)
        {
            M->SetVisibility(ESlateVisibility::HitTestInvisible);
            M->SetColorAndOpacity(FLinearColor::Gray);
        }
    }

    // Ÿ�̸� ����
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle, this, &UMHGaugeWidget::UpdateBar, 0.01f, true);

    // ��Ŀ��
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(TakeWidget());
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(Mode);
    }
    
}

void UMHGaugeWidget::FinishGauge()
{
    bool bSuccess = (!bAnyFail && HitMarkers.Num() == Markers.Num());

    // ���� ��ε�ĳ��Ʈ
    OnGaugeResult.Broadcast(bSuccess);

 
    SetVisibility(ESlateVisibility::Hidden);
}

void UMHGaugeWidget::UpdateBar()
{
    Elapsed += 0.01f;
    float DX = Speed * Elapsed;

    if (MovingBarImage)
    {
        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MovingBarImage->Slot))
        {
            CanvasSlot->SetPosition(InitMovingPos + FVector2D(DX, 0.f));
        }
        else
        {
            MovingBarImage->SetRenderTranslation(InitMovingPos + FVector2D(DX, 0.f));
        }
    }

    if (MoveRange > 0.f && DX >= MoveRange)
    {
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
        FinishGauge();
    }
}

float UMHGaugeWidget::GetCenterX_InParent(UWidget* Child, const FGeometry& ParentGeo) const
{
    const FGeometry& ChildGeo = Child->GetCachedGeometry();
    const FVector2D AbsPos = ChildGeo.GetAbsolutePosition(); 
    const FVector2D Size = ChildGeo.GetLocalSize();        

   
    const FVector2D LocalPos = ParentGeo.AbsoluteToLocal(AbsPos);
    return LocalPos.X + Size.X * 0.5f;
}

UImage* UMHGaugeWidget::FindClosestUnhitMarker(float MovingCenterX, const FGeometry& ParentGeo) const
{
    UImage* BestMarker = nullptr;
    float BestDist = FLT_MAX;

    for (UImage* M : Markers)
    {
        if (!M || HitMarkers.Contains(M)) continue;

        float MCX = GetCenterX_InParent(M, ParentGeo);
        float Dist = FMath::Abs(MovingCenterX - MCX);
        if (Dist < BestDist)
        {
            BestDist = Dist;
            BestMarker = M;
        }
    }
    return BestMarker;
}

FReply UMHGaugeWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::SpaceBar)
    {
        if (HitSFX)
        {
            UGameplayStatics::PlaySound2D(this, HitSFX);
        }

 
        float MoveCX = GetCenterX_InParent(MovingBarImage, InGeometry);

        // ���� �� ģ ��Ŀ �� ���� ����� ���� ã��
        UImage* Target = FindClosestUnhitMarker(MoveCX, InGeometry);
        if (Target)
        {
            float MarkerCX = GetCenterX_InParent(Target, InGeometry);

            const int32 MoveXi = FMath::RoundToInt(MoveCX);
            const int32 MarkerXi = FMath::RoundToInt(MarkerCX);
            bool bHit = (FMath::Abs(MoveXi - MarkerXi) <= EqualPixelSlack);

            // �� ǥ��
            Target->SetColorAndOpacity(bHit ? SuccessColor : FailColor);

            // ��� ���
            if (bHit) HitMarkers.Add(Target);
            else      bAnyFail = true;
        }
        else
        {
          
            bAnyFail = true;
        }

        return FReply::Handled();
    }
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
