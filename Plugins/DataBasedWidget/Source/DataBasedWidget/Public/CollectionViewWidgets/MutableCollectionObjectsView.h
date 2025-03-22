// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "WidgetCollectionInterface.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "CollectionViewWidgets.generated.h"

class UTkManagerObjectsArray;
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
class DATABASEDWIDGET_API UWPropGenMutableObjectsCollectionView : public UUserWidget, public IWidgetCollectionInterface
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void SetObjectManager(UTkManagerObjectsArray* ManagerObjectsArray);

	UFUNCTION()
	virtual void OnObjectAdded(UObject* Object);
	UFUNCTION()
	virtual void OnObjectRemoved(UObject* Object);
	UFUNCTION()
	virtual void OnInit(const TArray<UObject*>& Objects);
	UFUNCTION()
	virtual void OnCleared();
	
	virtual void SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> WidgetClass) override;

protected:
	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<UTkManagerObjectsArray> ObjectManager;
	
};
