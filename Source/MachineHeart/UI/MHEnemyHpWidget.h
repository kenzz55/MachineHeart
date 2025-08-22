// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MHEnemyHpWidget.generated.h"

class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class MACHINEHEART_API UMHEnemyHpWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "HP")
    void SetHPPercent(float Percent);

    UFUNCTION(BlueprintCallable, Category = "HP")
    void SetHPText(float CurrentHP);

protected:
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HPBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HPText;
};
