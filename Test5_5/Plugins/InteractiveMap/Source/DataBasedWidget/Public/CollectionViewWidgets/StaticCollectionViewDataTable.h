// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "WidgetCollectionInterface.h"
#include "Blueprint/WidgetTree.h"
#include "Types/SlateEnums.h"
#include "Components/VerticalBox.h"
#include "StaticCollectionViewDataTable.generated.h"

UCLASS(Blueprintable, BlueprintType, DisplayName=StaticCollectionViewDataTable)
class DATABASEDWIDGET_API UWPropGenStaticCollectionViewDataTable : public UUserWidget, public IPropGenWidgetCollectionInterface, public IPropGenWidgetDataTableInterface
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual UWidget* GetCollectionContainer_Implementation() override
	{
		return Container;
	};
	
	virtual void SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> MembersWidgetClass) override;
	virtual void SetDataTable_Implementation(UDataTable* NewDataTable) override;

protected:
	
#if WITH_EDITOR
	UFUNCTION(Blueprintable, CallInEditor, Category = VerticalBoxDataTable)
	void RefreshContainer() const;
	
	virtual void SetRootWidget(UWidgetTree* Tree) override
	{
		Container = Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName("Container"));;
	};
#endif
	
	virtual void Init();
	
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = VerticalBoxDataTable)
	class UVerticalBox* Container;

	UPROPERTY(EditAnywhere, Category = VerticalBoxDataTable)
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, Category = VerticalBoxDataTable)
	TSoftObjectPtr<UDataTable> DataTable;

};
