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
	UClass* ClassType;
	
	// Widget used by default
	UPROPERTY(EditAnywhere, Category=StructWidgetMap)
	TSubclassOf<UUserWidget> DefaultWidgetType;
	
	UPROPERTY(Category = StructWidgetMap, EditAnywhere)
	TMap<FName, TSubclassOf<UUserWidget>> PropertyWidgetMap;

	
#if WITH_EDITOR
	UFUNCTION(CallInEditor, BlueprintCallable, Category=StructWidgetMap)
	void Reset();

	void CreateFromClass(UClass* Class, const TSubclassOf<UUserWidget>& DefaultWidgetType);

	bool IsValid() const;
	const UClass* GetDataClass() const;
#endif
protected:
#if WITH_EDITOR
	// UObject interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface
#endif

	void FillPropertyWidgetMap(const UClass* Class);
};
