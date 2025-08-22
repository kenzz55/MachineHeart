// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "MHGaugeWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGaugeResult, bool, bSuccess);

class USoundBase;

UCLASS()
class MACHINEHEART_API UMHGaugeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    /** 성공/실패 결과 델리게이트 */
    UPROPERTY(BlueprintAssignable, Category = "Gauge")
    FOnGaugeResult OnGaugeResult;

    UFUNCTION(BlueprintCallable, Category = "Gauge")
    void StartGauge();
    void FinishGauge();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gauge|SFX")
    USoundBase* HitSFX;

protected:
    virtual void NativeConstruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    void UpdateBar();

  
    float GetCenterX_InParent(UWidget* Child, const FGeometry& ParentGeo) const;


    UPROPERTY(meta = (BindWidget))
    UImage* MovingBarImage;

    UPROPERTY(meta = (BindWidgetOptional))
    UImage* MarkerImage0;
    UPROPERTY(meta = (BindWidgetOptional))
    UImage* MarkerImage1;
    UPROPERTY(meta = (BindWidgetOptional))
    UImage* MarkerImage2;

 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gauge")
    float Speed = 400.f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gauge")
    float MoveRange = 600.f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gauge")
    bool bUseDesignerStartPos = true;

  
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gauge")
    int32 EqualPixelSlack = 0;

  
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gauge")
    FLinearColor SuccessColor = FLinearColor::Green;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gauge")
    FLinearColor FailColor = FLinearColor::Red;

private:
    FTimerHandle TimerHandle;
    float Elapsed = 0.f;

 
    FVector2D InitMovingPos = FVector2D::ZeroVector;

    TArray<UImage*> Markers;         
    TSet<UImage*>    HitMarkers;     
    bool bAnyFail = false;

 
    UImage* FindClosestUnhitMarker(float MovingCenterX, const FGeometry& ParentGeo) const;

};
