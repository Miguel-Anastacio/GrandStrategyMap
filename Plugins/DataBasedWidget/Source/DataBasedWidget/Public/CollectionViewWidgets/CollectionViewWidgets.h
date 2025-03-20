// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "WidgetCollectionInterface.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "CollectionViewWidgets.generated.h"

// Wrapper for InstancedStructs so that they can be used as source for ListView
UCLASS()
class UPropGenStructWrapper : public UObject
{
	GENERATED_BODY()
public:    
	void SetStructInstance(const FInstancedStruct& InStruct)
	{
		StructInstance = InStruct;
	}
	FInstancedStruct GetStructInstance() const
	{
		return StructInstance;
	}
protected:
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FInstancedStruct StructInstance;
};

UCLASS(Abstract)
class DATABASEDWIDGET_API UWPropGenCollectionView : public UUserWidget, public IWidgetCollectionInterface
{
	GENERATED_BODY()

public:
	virtual void AddObject_Implementation(UObject* Object) override;
	virtual void RemoveObject_Implementation(UObject* Object) override;
	// wrapper to init from Instanced Struct exposed to BP
	virtual void InitFromStructs_Implementation(const TArray<FInstancedStruct>& Structs) override;
	
	// init from UObject exposed to BP
	virtual void InitFromObjects_Implementation(const TArray<UObject*>& Objects) override;
	
	virtual void SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> WidgetClass) override;

protected:
	UPROPERTY(VisibleInstanceOnly)
	TArray<UObject*> SourceObjects;
	
};
