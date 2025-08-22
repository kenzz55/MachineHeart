// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/MHGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/MHGameInstance.h"

void AMHGameMode::StartPlay()
{
    Super::StartPlay();

    // PendingReturnLocation�� ���� ������ �÷��̾� ���� �� ��ġ �ű��
    UMHGameInstance* GI = Cast<UMHGameInstance>(GetGameInstance());
    if (GI && !GI->PendingReturnLocation.IsZero())
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            if (APawn* Pawn = PC->GetPawn())
            {
                Pawn->SetActorLocation(GI->PendingReturnLocation);
                GI->PendingReturnLocation = FVector::ZeroVector;
            }
        }
    }
}
