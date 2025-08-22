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

    // 모든 마커 초기 색 회색
    for (UImage* M : Markers)
    {
        if (M) M->SetColorAndOpacity(FLinearColor::Gray);
    }

    // 입력 모드 UI Only
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

    // 시작 위치 세팅
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

    // 마커들 보이게 / 회색
    for (UImage* M : Markers)
    {
        if (M)
        {
            M->SetVisibility(ESlateVisibility::HitTestInvisible);
            M->SetColorAndOpacity(FLinearColor::Gray);
        }
    }

    // 타이머 시작
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle, this, &UMHGaugeWidget::UpdateBar, 0.01f, true);

    // 포커스
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

    // 최종 브로드캐스트
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

        // 아직 안 친 마커 중 가장 가까운 것을 찾음
        UImage* Target = FindClosestUnhitMarker(MoveCX, InGeometry);
        if (Target)
        {
            float MarkerCX = GetCenterX_InParent(Target, InGeometry);

            const int32 MoveXi = FMath::RoundToInt(MoveCX);
            const int32 MarkerXi = FMath::RoundToInt(MarkerCX);
            bool bHit = (FMath::Abs(MoveXi - MarkerXi) <= EqualPixelSlack);

            // 색 표시
            Target->SetColorAndOpacity(bHit ? SuccessColor : FailColor);

            // 결과 기록
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
