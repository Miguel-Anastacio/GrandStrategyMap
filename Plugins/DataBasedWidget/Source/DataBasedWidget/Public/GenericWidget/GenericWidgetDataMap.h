// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenericWidgetDataMap.generated.h"

UCLASS(BlueprintType, DisplayName=WidgetMapDataAsset)
class DATABASEDWIDGET_API UPropGenWidgetMapDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
#if WITH_EDITOR
	UFUNCTION(CallInEditor, BlueprintCallable, Category=StructWidgetMap)
	void Reset();

	void CreateFromData(UStruct* BaseStruct, const TSubclassOf<UUserWidget>& DefaultWidgetType);
	bool IsValid() const;
	const UStruct* GetDataClass() const;
	const UScriptStruct* GetClassAsScriptStruct() const;
	const UClass* GetClassAsUClass() const;
#endif
	// Widget used by default
	UPROPERTY(EditAnywhere, Category=StructWidgetMap, meta=(MustImplement="/Script/DataBasedWidget.GenericUserWidgetInterface"))
	TSubclassOf<UUserWidget> DefaultWidgetType;
	
	UPROPERTY(Category = StructWidgetMap, EditAnywhere, meta=(AllowedClasses="UserWidget", MustImplement="/Script/DataBasedWidget.GenericUserWidgetInterface",
																GetKeyOptions="GetPropertyNameOptions"))
	TMap<FName, TSubclassOf<UUserWidget>> PropertyWidgetMap;

protected:
#if WITH_EDITOR
	// UObject interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface
	
	UFUNCTION()
	TArray<FString> GetPropertyNameOptions() const;
#endif
	void FillPropertyWidgetMap(const UStruct* BaseClass);
	void SetClass();
	
	UPROPERTY(Category=StructWidgetMap, EditAnywhere, meta=(InlineEditConditionToggle))
    bool bUObjectBased = true;

	UPROPERTY(Category=StructWidgetMap, EditAnywhere, meta=(EditCondition="bUObjectBased"))
	UClass* Class = nullptr;
	UPROPERTY(Category=StructWidgetMap, EditAnywhere, meta=(EditCondition="!bUObjectBased"))
	UScriptStruct* Struct = nullptr;

	UPROPERTY()
	UStruct* ClassType = nullptr;
	
};
