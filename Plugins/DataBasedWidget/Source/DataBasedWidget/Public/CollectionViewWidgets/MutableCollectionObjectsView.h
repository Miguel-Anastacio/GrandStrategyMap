// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 5, 0)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif

#include "WidgetCollectionInterface.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "MutableCollectionObjectsView.generated.h"

class UTkManagerObjectsArray;
// Wrapper for InstancedStructs so that they can be used as source for ListView
UCLASS(DisplayName = InstancedStructWrapper)
class UPropGenStructWrapper : public UObject
{
	GENERATED_BODY()
public:
	void SetStructInstance(const FInstancedStruct &InStruct)
	{
		StructInstance = InStruct;
	}
	FInstancedStruct GetStructInstance() const
	{
		return StructInstance;
	}

protected:
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = Data)
	FInstancedStruct StructInstance;
};

UCLASS(Abstract)
class DATABASEDWIDGET_API UWPropGenMutableCollectionObjectsView : public UUserWidget, public IPropGenWidgetCollectionInterface
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	UFUNCTION(BlueprintCallable, Category = MutableCollectionStructsView)
	virtual void SetObjectManager(UTkManagerObjectsArray *ManagerObjectsArray);
	UFUNCTION(BlueprintCallable, Category = MutableCollectionStructsView)
	virtual void ClearManager();

	UFUNCTION()
	virtual void OnObjectAdded(UObject *Object);
	UFUNCTION()
	virtual void OnObjectRemoved(UObject *Object);
	UFUNCTION()
	virtual void OnInit(const TArray<UObject *> &Objects);
	UFUNCTION()
	virtual void OnCleared();
	UFUNCTION()
	virtual void OnObjectChanged(const UObject *Object);

	virtual void SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> WidgetClass) override;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Data)
	TWeakObjectPtr<UTkManagerObjectsArray> ObjectManager;
};
