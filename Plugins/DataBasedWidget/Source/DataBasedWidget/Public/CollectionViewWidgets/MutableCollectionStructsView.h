// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "WidgetCollectionInterface.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "MutableCollectionStructsView.generated.h"

class UTkManagerStructsArray;

// Supports GridView/TreeView/ListView by default
UCLASS(Abstract)
class DATABASEDWIDGET_API UWPropGenMutableCollectionStructsView : public UUserWidget, public IPropGenWidgetCollectionInterface
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	UFUNCTION(BlueprintCallable, Category=MutableCollectionStructsView)
	virtual void SetManager(UTkManagerStructsArray* ManagerStructsArray);

	UFUNCTION()
	virtual void OnStructAdded(const FInstancedStruct& DataStruct);
	UFUNCTION()
	virtual void OnStructRemoved(const FInstancedStruct& DataStruct);
	UFUNCTION()
	virtual void OnInit(const TArray<FInstancedStruct>& Structs);
	UFUNCTION()
	virtual void OnCleared();
	UFUNCTION()
	virtual void OnStructChanged(const FInstancedStruct& Prev, const FInstancedStruct& New);
	
	
	virtual void SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> WidgetClass) override;

	UFUNCTION(BlueprintCallable, Category=MutableCollectionStructsView)
	virtual void ClearManager();

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TWeakObjectPtr<UTkManagerStructsArray> StructManager;

	UPROPERTY()
	TArray<class UPropGenStructWrapper*> StructWrappers;

};
