// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MHParryWidget.h"   
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterStat/MHCharacterStatComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Character/MHCharacterEnemy.h"
#include "GameMode/MHBattleGameMode.h"
#include "PaperZDAnimationComponent.h"    
#include "Components/PrimitiveComponent.h"
#include "Character/MHCharacterPlayer.h"
#include "UI/MHSkillWidget.h"
#include "TimerManager.h"
#include "Engine/World.h"

AMHCharacterPlayer::AMHCharacterPlayer()
{
	Inventory = CreateDefaultSubobject<UMHItemInventoryComponent>(TEXT("Inventory"));

	HPWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPWidgetComp"));
	HPWidgetComp->SetupAttachment(RootComponent);
	HPWidgetComp->SetWidgetSpace(EWidgetSpace::World);
	HPWidgetComp->SetDrawAtDesiredSize(true);
	HPWidgetComp->SetRelativeLocation(FVector(0, 0, 200.f));

    HitOverlay = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("HitOverlay"));
    HitOverlay->SetupAttachment(RootComponent);
    HitOverlay->SetLooping(false);
    HitOverlay->SetVisibility(false, true);

    AttackOverlay = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("AttackOverlay"));
    AttackOverlay->SetupAttachment(RootComponent);
    AttackOverlay->SetLooping(false);
    AttackOverlay->SetVisibility(false, true);

    
}

void AMHCharacterPlayer::OnTurnStarted_Implementation()
{
    if (UMHCharacterStatComponent* StatComponent = FindComponentByClass<UMHCharacterStatComponent>())
    {
        StatComponent->OnNewTurn();           // 기존 버프 턴 감소 처리
        StatComponent->IncrementSkillGauge();
        
    }

    
    Super::OnTurnStarted_Implementation();
}

void AMHCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

    UMHGameInstance* GI = Cast<UMHGameInstance>(GetGameInstance());
    if (GI)
    {
        if(auto StatComp = FindComponentByClass<UMHCharacterStatComponent>())
            StatComp->OnSkillGaugeChanged.Broadcast(GI->SavedSkillGauge);


        if (GI->SavedCurrentHp <= 0.f)
        {
           
            SaveStateToGameInstance();
            UE_LOG(LogTemp, Log, TEXT("Initial HP saved: %.1f"), GI->SavedCurrentHp);
        }
        else
        {
            
            LoadStateFromGameInstance();
            UE_LOG(LogTemp, Log, TEXT("HP loaded from GameInstance: %.1f"), GI->SavedCurrentHp);
        }
    }

    if (PlayerHPWidgetClass)
    {
        HPWidgetComp->SetWidgetClass(PlayerHPWidgetClass);
    }

   
    if (UMHEnemyHpWidget* Widget = Cast<UMHEnemyHpWidget>(HPWidgetComp->GetUserWidgetObject()))
    {
        float Percent = Stat->GetCurrentHp() / Stat->GetMaxHp();
        Widget->SetHPPercent(Percent);
        Widget->SetHPText(Stat->GetCurrentHp());
    }

    // HP 변경 바인딩
    if (Stat)
    {
        Stat->OnHpChanged.AddUObject(this, &AMHCharacterPlayer::OnHPChanged);
        Stat->OnHpZero.AddUObject(this, &AMHCharacterPlayer::HandleDeath);
    }

    
    if (AMHBattleGameMode* BattleGM = Cast<AMHBattleGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        
        HPWidgetComp->SetVisibility(true);
    }
    else
    {
        HPWidgetComp->SetVisibility(false);
    }
}

void AMHCharacterPlayer::SaveStateToGameInstance()
{
	UMHGameInstance* GI = Cast<UMHGameInstance>(GetGameInstance());
	if (!GI) return;

	if (auto StatComp = FindComponentByClass<UMHCharacterStatComponent>())
		GI->SavedCurrentHp = StatComp->GetCurrentHp();

	if (auto Inv = FindComponentByClass<UMHItemInventoryComponent>())
		GI->SavedInventory = Inv->Items;
}

void AMHCharacterPlayer::LoadStateFromGameInstance()
{
	UMHGameInstance* GI = Cast<UMHGameInstance>(GetGameInstance());
	if (!GI) return;

	if (auto StatComp = FindComponentByClass<UMHCharacterStatComponent>())
		StatComp->InitCurrentHp(GI->SavedCurrentHp);
}

void AMHCharacterPlayer::UseAllEnemySkill()
{

    UMHGameInstance* GI = Cast<UMHGameInstance>(GetWorld()->GetGameInstance());
    if (!GI) return;
        if (GI->SavedSkillGauge < 6)
            return;
   
    UWorld* World = GetWorld();
    if (!World) return;

    // 모든 적 찾기
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(World, AMHCharacterEnemy::StaticClass(), Found);

    for (AActor* A : Found)
    {
        AMHCharacterEnemy* Enemy = Cast<AMHCharacterEnemy>(A);
        if (!Enemy) continue;

        if (UMHCharacterStatComponent* EnemyStat = Enemy->FindComponentByClass<UMHCharacterStatComponent>())
        {
           
            EnemyStat->ApplyDamage(20.f);
            // 방어력 5 디버프, 1턴 유지
            EnemyStat->AddDefenseDebuff(5.f, 1);
        }
    }

    // 스킬 사용 뒤 플레이어 턴 종료
    if (auto* GM = Cast<AMHBattleGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        GM->EndTurn();
    }
    GI->SavedSkillGauge = 0;
    if (UMHCharacterStatComponent* StatComp = FindComponentByClass<UMHCharacterStatComponent>())
    {
        StatComp->OnSkillGaugeChanged.Broadcast(0);
    }

    TArray<UUserWidget*> FoundWidgets;
    // GetAllWidgetsOfClass(UObject* WorldContextObject, TArray<T*>& OutWidgets, TSubclassOf<UUserWidget> WidgetClass, bool bTopLevelOnly)
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
        GetWorld(),
        FoundWidgets,
        UMHSkillWidget::StaticClass(),
        /*bTopLevelOnly=*/ true
    );

    for (UUserWidget* W : FoundWidgets)
    {
        if (UMHSkillWidget* SKW = Cast<UMHSkillWidget>(W))
        {
            SKW->SetVisibility(ESlateVisibility::Hidden);
            break; 
        }
    }

}

void AMHCharacterPlayer::ReceiveBattleDamageFromEnemy(AMHCharacterEnemy* InAttacker, bool bUseBonus, float BonusValue)
{
    if (!InAttacker)
    {
        
        if (Stat) Stat->ApplyDamage(1.f);
        return;
    }

   
    if (ParryWidget && ParryWidget->IsInViewport())
    {
        if (bUseBonus) InAttacker->DealDamageToWithBonus(this, BonusValue);
        else           InAttacker->DealDamageTo(this);
        InAttacker->FinishTurn();
        return;
    }

    PendingAttacker = InAttacker;
    bPendingUseBonus = bUseBonus;
    PendingBonusValue = BonusValue;

    
    if (!ParryWidget)
    {
        if (!ParryWidgetClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("ParryWidgetClass is not set on Player"));
            
            if (bUseBonus) InAttacker->DealDamageToWithBonus(this, BonusValue);
            else           InAttacker->DealDamageTo(this);
            InAttacker->FinishTurn();
            return;
        }

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            ParryWidget = CreateWidget<UMHParryWidget>(PC, ParryWidgetClass);
        }
    }

    if (!ParryWidget) 
    {
        if (bUseBonus) InAttacker->DealDamageToWithBonus(this, BonusValue);
        else           InAttacker->DealDamageTo(this);
        InAttacker->FinishTurn();
        return;
    }

    
    if (!ParryWidget->IsInViewport())
        ParryWidget->AddToViewport(1000);

   
    if (ParryHandle.IsValid())
    {
        ParryWidget->OnParryResult.Remove(ParryHandle);
        ParryHandle.Reset();
    }
    ParryHandle = ParryWidget->OnParryResult.AddUObject(this, &AMHCharacterPlayer::HandleParryResult);

    
    ParryWidget->SetVisibility(ESlateVisibility::Visible);
    ParryWidget->StartParry();
}

void AMHCharacterPlayer::PlayHitOverlayOnce()
{
    if (!HitFB || !HitOverlay) return;

   
    UPrimitiveComponent* BaseRender = nullptr;
    if (UPaperZDAnimationComponent* AnimComp = FindComponentByClass<UPaperZDAnimationComponent>())
    {
        BaseRender = Cast<UPrimitiveComponent>(AnimComp->GetRenderComponent());
    }

   
    if (BaseRender)
    {
      
        BaseRender->SetVisibility(false, /*bPropagateToChildren=*/true);
    }

    

    // 오버레이 보이기 + 재생
    GetWorldTimerManager().ClearTimer(HitHideHandle);
    HitOverlay->SetFlipbook(HitFB);
    HitOverlay->SetVisibility(true, true);
    HitOverlay->PlayFromStart();

    const float Len = HitOverlay->GetFlipbook()
        ? HitOverlay->GetFlipbook()->GetTotalDuration()
        : 0.25f;

    
    GetWorldTimerManager().SetTimer(
        HitHideHandle,
        FTimerDelegate::CreateWeakLambda(this, [this, BaseRender]()
            {
                HitOverlay->SetVisibility(false, true);
                if (BaseRender)
                {
                    BaseRender->SetVisibility(true, /*bPropagateToChildren=*/true);
                }
            }),
        Len,
        false
    );
}

void AMHCharacterPlayer::PlayAttackOverlayOnce(EEquipmentSlot Slot)
{
    UPaperFlipbook** FBPtr = AttackFlipbooks.Find(Slot);
    if (!FBPtr || !*FBPtr || !AttackOverlay) return;

    // PaperZD 본체 렌더 컴포넌트
    UPrimitiveComponent* BaseRender = nullptr;
    if (UPaperZDAnimationComponent* AnimComp = FindComponentByClass<UPaperZDAnimationComponent>())
    {
        BaseRender = Cast<UPrimitiveComponent>(AnimComp->GetRenderComponent());
    }

    // 본체 숨기기
    if (BaseRender)
    {
        BaseRender->SetVisibility(false, /*bPropagateToChildren=*/true);
       
    }

    // 오버레이 재생
    GetWorldTimerManager().ClearTimer(AttackHideHandle);
    AttackOverlay->SetFlipbook(*FBPtr);
    AttackOverlay->SetVisibility(true, true);
    AttackOverlay->PlayFromStart();

    const float Len = AttackOverlay->GetFlipbook()
        ? AttackOverlay->GetFlipbook()->GetTotalDuration()
        : 0.35f;

    // 끝나면 오버레이 끄고 본체 켜기
    GetWorldTimerManager().SetTimer(
        AttackHideHandle,
        FTimerDelegate::CreateWeakLambda(this, [this, BaseRender]()
            {
                AttackOverlay->SetVisibility(false, true);
                if (BaseRender)
                {
                    BaseRender->SetVisibility(true, /*bPropagateToChildren=*/true);
                }
            }),
        Len,
        false
    );
}

void AMHCharacterPlayer::HandleDeath()
{
    if (auto* GM = Cast<AMHBattleGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        GM->EndBattle(false);
    }
    //Destroy();
}

void AMHCharacterPlayer::OnHPChanged(float CurrentHp)
{
    if (UMHEnemyHpWidget* Widget = Cast<UMHEnemyHpWidget>(HPWidgetComp->GetUserWidgetObject()))
    {

        if (AttackedSFX)
        {
            UGameplayStatics::PlaySound2D(this, AttackedSFX);
        }

        float Percent = CurrentHp / Stat->GetMaxHp();
        Widget->SetHPPercent(Percent);
        Widget->SetHPText(CurrentHp);

        PlayHitOverlayOnce();
    }
}

void AMHCharacterPlayer::HandleParryResult(bool bSuccess)
{
    if (ParryWidget && ParryHandle.IsValid())
    {
        ParryWidget->OnParryResult.Remove(ParryHandle);
        ParryHandle.Reset();
    }

    AMHCharacterEnemy* Attacker = PendingAttacker.Get();

    if (bSuccess)
    {
        // 피해 0 → 아무 것도 하지 않음
        // 카운터 고정 2
        if (Attacker && Attacker->Stat)
        {
            Attacker->Stat->ApplyDamage(static_cast<float>(ParryCounterDamage));
        }
    }
    else
    {
        // 패링 실패 → 원래 의도대로 피해 적용
        if (Attacker)
        {
            if (bPendingUseBonus) Attacker->DealDamageToWithBonus(this, PendingBonusValue);
            else                  Attacker->DealDamageTo(this);
        }
    }

    
    if (Attacker)
    {
        Attacker->FinishTurn();
    }

    // 클린업
    PendingAttacker = nullptr;
    bPendingUseBonus = false;
    PendingBonusValue = 0.f;
}
