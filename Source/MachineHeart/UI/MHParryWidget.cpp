// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MHParryWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

void UMHParryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    bEnded = false;
    Elapsed = 0.f;

    if (MovingBar) MovingBar->SetColorAndOpacity(ReadyColor);
    if (Marker)
    {
        Marker->SetVisibility(ESlateVisibility::HitTestInvisible);
        Marker->SetColorAndOpacity(ReadyColor);
    }
}

void UMHParryWidget::NativeDestruct()
{
    CancelParry();
    Super::NativeDestruct();
}



void UMHParryWidget::StartParry()
{
  
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PC, this);
        PC->bShowMouseCursor = false;
    }

    bEnded = false;
    Elapsed = 0.f;

    bKeyPressed = false;
    bPressedSuccess = false;

    if (Marker)
    {
        Marker->SetColorAndOpacity(ReadyColor);     
    }


    if (!TimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMHParryWidget::TickParry, 0.016f, true);
    }
}

void UMHParryWidget::CancelParry()
{
    FinishParry(false);
}

void UMHParryWidget::TickParry()
{
    if (bEnded || !MovingBar) return;

    Elapsed += 0.016f;

    const float PosX = FMath::Clamp(Elapsed * Speed, 0.f, MoveRange);
    FWidgetTransform T = MovingBar->RenderTransform;
    T.Translation.X = PosX;
    MovingBar->SetRenderTransform(T);


    if (PosX >= MoveRange)
    {
        const bool bSuccessFinal = (bKeyPressed && bPressedSuccess);
        FinishParry(bSuccessFinal);
    }
}

FReply UMHParryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (bEnded) return FReply::Unhandled();

    if (InKeyEvent.GetKey() == EKeys::SpaceBar)
    {
        if (HitSFX)
        {
            UGameplayStatics::PlaySound2D(this, HitSFX);
        }

       
        if (bKeyPressed) return FReply::Handled();

        if (!MovingBar || !Marker) return FReply::Handled();

        const float BarCenterX = GetCenterX_InParent(MovingBar, InGeometry);
        const float MarkerCenterX = GetCenterX_InParent(Marker, InGeometry);
        const float Delta = FMath::Abs(BarCenterX - MarkerCenterX);

        bKeyPressed = true;
        bPressedSuccess = (Delta <= (float)EqualPixelSlack);

    
        Marker->SetColorAndOpacity(bPressedSuccess ? SuccessColor : FailColor);

        return FReply::Handled();
    }
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UMHParryWidget::FinishParry(bool bSuccess)
{
    if (bEnded) return;
    bEnded = true;

    GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        UWidgetBlueprintLibrary::SetInputMode_GameOnly(PC);
        PC->bShowMouseCursor = true;
    }

    OnParryResult.Broadcast(bSuccess);

    RemoveFromParent();
}

float UMHParryWidget::GetCenterX_InParent(UWidget* Child, const FGeometry& ParentGeo) const
{
    if (!Child) return 0.f;
    const FGeometry& ChildGeo = Child->GetCachedGeometry();
    const FVector2D  LocalPos = ParentGeo.AbsoluteToLocal(ChildGeo.GetAbsolutePosition());
    return LocalPos.X + (ChildGeo.GetLocalSize().X * 0.5f);
}