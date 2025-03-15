// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "ListViewWidgets.generated.h"

// Wrapper for InstancedStructs so that they can be used as source for ListView
UCLASS()
class UPropGenStructWrapper : public UObject
{
	GENERATED_BODY()
public:    
	void SetStructInstance(FInstancedStruct& InStruct)
	{
		InstancePtr = &InStruct;
		StructInstance = *InstancePtr;
	}
	FInstancedStruct GetStructInstance() const
	{
		// if (StructInstance == nullptr)
		return StructInstance;
	}
protected:
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FInstancedStruct StructInstance;

	FInstancedStruct* InstancePtr;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};


UCLASS(Abstract, BlueprintType)
class DATABASEDWIDGET_API UWPropGenListView : public UUserWidget
{
	GENERATED_BODY()

public:
	// wrapper to init from Instanced Struct exposed to BP
	UFUNCTION(BlueprintCallable,  Category = "Generic Struct Widget")
	virtual void InitFromStructs(const TArray<FInstancedStruct>& Structs);
	
	// wrapper to init from UObject exposed to BP
	UFUNCTION(BlueprintCallable,  Category = "Generic Struct Widget")
	virtual void InitFromObjects(const TArray<UObject*>& Objects);
	
#if WITH_EDITOR
	virtual void CreateListView(TSubclassOf<UUserWidget> WidgetClass);
#endif
protected:
	
#if WITH_EDITOR
	virtual void SetEntryWidgetClass(TSubclassOf<UUserWidget> WidgetClass);
#endif
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = ListViewWidgets)
	class UListView* ListView;

	UPROPERTY(VisibleAnywhere)
	TArray<UObject*> SourceObjects;
	
};
