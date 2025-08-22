// Fill out your copyright notice in the Description page of Project Settings.


#include "MHCharacterEnemy.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/MHBattleGameMode.h"
#include "TimerManager.h"
#include "Sound/SoundBase.h"
#include "UI/MHEnemyHPWidget.h"

AMHCharacterEnemy::AMHCharacterEnemy()
{
    HPWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPWidgetComp"));
    HPWidgetComp->SetupAttachment(RootComponent);
    HPWidgetComp->SetWidgetSpace(EWidgetSpace::World);
    HPWidgetComp->SetDrawAtDesiredSize(true);
    HPWidgetComp->SetRelativeLocation(FVector(0, 0, 200.f));

    InfoWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidgetComp"));
    InfoWidgetComp->SetupAttachment(RootComponent);
    InfoWidgetComp->SetWidgetSpace(EWidgetSpace::World);
    InfoWidgetComp->SetDrawSize(FVector2D(200, 200));
    InfoWidgetComp->SetVisibility(false);

    FlipbookComp = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipbookComp"));
    FlipbookComp->SetupAttachment(RootComponent);
    FlipbookComp->SetLooping(true);
    FlipbookComp->SetPlayRate(1.f);
}

void AMHCharacterEnemy::PlayIdle(float PlayRate)
{
    GetWorldTimerManager().ClearTimer(RevertToIdleTimer);
    if (FlipbookComp && Flip_Idle)
    {
        FlipbookComp->SetFlipbook(Flip_Idle);
        FlipbookComp->SetLooping(true);
        FlipbookComp->SetPlayRate(PlayRate);
        FlipbookComp->PlayFromStart();
    }
}

void AMHCharacterEnemy::PlayAttack(float PlayRate)
{
    if (Flip_Attack) PlayOnceThenIdle(Flip_Attack, PlayRate);
    UE_LOG(LogTemp, Warning, TEXT("Flip_Attack Duration = %.2f"), Flip_Attack->GetTotalDuration());
}

void AMHCharacterEnemy::PlayDeath(float PlayRate)
{
    GetWorldTimerManager().ClearTimer(RevertToIdleTimer);
    if (FlipbookComp && Flip_Death)
    {
        FlipbookComp->SetFlipbook(Flip_Death);
        FlipbookComp->SetLooping(false);
        FlipbookComp->SetPlayRate(PlayRate);
        FlipbookComp->PlayFromStart();
    }
}

void AMHCharacterEnemy::PlayHeal(float PlayRate)
{
    if (Flip_Heal) PlayOnceThenIdle(Flip_Heal, PlayRate);
}

void AMHCharacterEnemy::PlayDebuff(float PlayRate)
{
    if (Flip_Debuff) PlayOnceThenIdle(Flip_Debuff, PlayRate);
}

void AMHCharacterEnemy::PlayBuff(float PlayRate)
{
    if (Flip_Buff) PlayOnceThenIdle(Flip_Buff, PlayRate);
}

void AMHCharacterEnemy::PlayHurt(float PlayRate)
{
    if (Flip_Hurt) PlayOnceThenIdle(Flip_Hurt, PlayRate);
}

void AMHCharacterEnemy::BeginPlay()
{
    Super::BeginPlay();

    PlayIdle();

    if (EnemyHPWidgetClass)
    {
        HPWidgetComp->SetWidgetClass(EnemyHPWidgetClass);
    }

    if (UMHEnemyHpWidget* Widget = Cast<UMHEnemyHpWidget>(HPWidgetComp->GetUserWidgetObject()))
    {
        float Percent = Stat->GetCurrentHp() / Stat->GetMaxHp();
        Widget->SetHPPercent(Percent);
        Widget->SetHPText(Stat->GetCurrentHp());
    }

    if (UMHCharacterStatComponent* MyStat = FindComponentByClass<UMHCharacterStatComponent>())
    {
        MyStat->OnHpChanged.AddUObject(this, &AMHCharacterEnemy::OnHPChanged);
        MyStat->OnHpZero.AddUObject(this, &AMHCharacterEnemy::HandleDeath);
        MyStat->OnDamaged.AddUObject(this, &AMHCharacterEnemy::HandleDamaged);
    }

}

void AMHCharacterEnemy::HandleDeath()
{
    if (auto* GM = Cast<AMHBattleGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        GM->OnCharacterDeath(this);
    }

    if (DeathSFX) 
    {
        UGameplayStatics::PlaySound2D(this, DeathSFX);
    }

    SetLifeSpan(1.0f);

}

void AMHCharacterEnemy::OnHPChanged(float CurrentHp)
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
    }
}

void AMHCharacterEnemy::OnTurnStarted_Implementation()
{
    Super::OnTurnStarted_Implementation();

    if (auto* EStat = FindComponentByClass<UMHCharacterStatComponent>())
    {
        EStat->OnNewTurn();
    }

    // 짧은 딜레이 후 행동 위임
    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle,this,&AMHCharacterEnemy::ExecuteEnemyAction,0.5f,false);
}

void AMHCharacterEnemy::ExecuteEnemyAction()
{
    AMHBattleGameMode* GM = Cast<AMHBattleGameMode>(UGameplayStatics::GetGameMode(this));

    if (BehaviorAsset && GM)
    {
        // DataAsset에 모든 로직 위임
        BehaviorAsset->ExecuteBehavior(this, GM);
    }
    else
    {
        
        FinishTurn();
    }
}

void AMHCharacterEnemy::PlayOnceThenIdle(UPaperFlipbook* FB, float PlayRate)
{
    if (!FlipbookComp || !FB) return;

    GetWorldTimerManager().ClearTimer(RevertToIdleTimer);

    FlipbookComp->SetFlipbook(FB);
    FlipbookComp->SetLooping(false);
    FlipbookComp->SetPlayRate(PlayRate);
    FlipbookComp->PlayFromStart();

    // 길이(초)만큼 지난 후 Idle 복귀
    const float Duration = FB->GetTotalDuration() / FMath::Max(PlayRate, KINDA_SMALL_NUMBER);

    FTimerDelegate RevertDelegate;
    RevertDelegate.BindUObject(this, &AMHCharacterEnemy::PlayIdle, /*PlayRate=*/1.f);

    GetWorld()->GetTimerManager().SetTimer(
        RevertToIdleTimer,
        RevertDelegate,
        Duration,
        /*bLoop=*/false
    );
}

void AMHCharacterEnemy::HandleDamaged(float Damage, AActor* DamagedActor)
{
    UE_LOG(LogTemp, Warning, TEXT("[Enemy HandleDamaged] This=%s  DamagedActor=%s  Damage=%.1f"),
        *GetName(), DamagedActor ? *DamagedActor->GetName() : TEXT("nullptr"), Damage);

    
    if (DamagedActor != this || Damage <= 0.f) return;

    if (Flip_Hurt)
    {
        PlayOnceThenIdle(Flip_Hurt);
    }
}