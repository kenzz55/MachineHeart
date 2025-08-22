// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "MHParryWidget.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnParryResult, bool /*bSuccess*/);

class USoundBase;

UCLASS()
class MACHINEHEART_API UMHParryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:


    UFUNCTION(BlueprintCallable, Category = "Parry")
    void StartParry();


    UFUNCTION(BlueprintCallable, Category = "Parry")
    void CancelParry();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gauge|SFX")
    USoundBase* HitSFX;

    FOnParryResult OnParryResult;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry") int32 EqualPixelSlack = 4; 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry") float Speed = 500.f;       
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry") float MoveRange = 600.f;  

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
    void TickParry();
    void FinishParry(bool bSuccess);
    float GetCenterX_InParent(UWidget* Child, const FGeometry& ParentGeo) const;

private:

    UPROPERTY(meta = (BindWidget)) UImage* MovingBar = nullptr;

  
    UPROPERTY(meta = (BindWidget)) UImage* Marker = nullptr;


    UPROPERTY(EditAnywhere, Category = "Parry|Visual") FLinearColor ReadyColor = FLinearColor::White;
    UPROPERTY(EditAnywhere, Category = "Parry|Visual") FLinearColor SuccessColor = FLinearColor::Green;
    UPROPERTY(EditAnywhere, Category = "Parry|Visual") FLinearColor FailColor = FLinearColor::Red;

  
    FTimerHandle TimerHandle;
    float Elapsed = 0.f;
    bool  bEnded = false; 
    bool  bKeyPressed = false; 
    bool  bPressedSuccess = false; 
};
