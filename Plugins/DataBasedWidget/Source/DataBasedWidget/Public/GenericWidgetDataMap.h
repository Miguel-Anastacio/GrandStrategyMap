// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenericWidgetDataMap.generated.h"

UCLASS(BlueprintType)
class DATABASEDWIDGET_API UWidgetMapDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(Category=StructWidgetMap, EditAnywhere)
	UScriptStruct* StructType;
	// Widget used by default
	UPROPERTY(EditAnywhere, Category=StructWidgetMap)
	TSubclassOf<UUserWidget> DefaultWidgetType;
	
	// The sprite to use for an interior region fill
	UPROPERTY(Category = StructWidgetMap, EditAnywhere)
	TMap<FName, TSubclassOf<UUserWidget>> PropertyWidgetMap;
protected:
#if WITH_EDITOR
	// UObject interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface
#endif
#if WITH_EDITOR
	UFUNCTION(CallInEditor, BlueprintCallable, Category=StructWidgetMap)
	void Reset();
#endif
	
};
