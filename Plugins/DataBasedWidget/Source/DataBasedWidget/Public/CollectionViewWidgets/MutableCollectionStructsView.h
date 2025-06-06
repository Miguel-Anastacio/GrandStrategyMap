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
	UFUNCTION(BlueprintCallable, Category = MutableCollectionStructsView)
	virtual void SetManager(UTkManagerStructsArray *ManagerStructsArray);

	UFUNCTION(BlueprintCallable, Category = MutableCollectionStructsView)
	virtual void ClearManager();

	UFUNCTION()
	virtual void OnStructAdded(const FInstancedStruct &DataStruct);
	UFUNCTION()
	virtual void OnStructRemoved(const FInstancedStruct &DataStruct);
	UFUNCTION()
	virtual void OnInit(const TArray<FInstancedStruct> &Structs);
	UFUNCTION()
	virtual void OnCleared();
	UFUNCTION()
	virtual void OnStructChanged(const FInstancedStruct &Prev, const FInstancedStruct &New);

	virtual void SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> WidgetClass) override;
	;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Data)
	TWeakObjectPtr<UTkManagerStructsArray> StructManager;

	UPROPERTY()
	TArray<class UPropGenStructWrapper *> StructWrappers;
};
