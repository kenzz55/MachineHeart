#pragma once

#include "CoreMinimal.h"
#include "MHItemTypes.generated.h"

class UMHItemDataAsset;
USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UMHItemDataAsset* ItemData = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool bEquipped = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Quantity = 1;
};

UENUM(BlueprintType)
enum class EItemUseContext : uint8
{
	BattleOnly,
	FieldOnly,
	Anywhere
};

UENUM(BlueprintType)
enum class EItemUseTarget : uint8
{
	Self,
	EnemySingle,   
	AllEnemies
};