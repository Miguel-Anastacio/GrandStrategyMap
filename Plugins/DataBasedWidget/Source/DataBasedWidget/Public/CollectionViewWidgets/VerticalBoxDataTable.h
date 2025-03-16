// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "WidgetCollectionInterface.h"
#include "Blueprint/WidgetTree.h"
#include "Types/SlateEnums.h"
#include "Components/VerticalBox.h"
#include "VerticalBoxDataTable.generated.h"

UCLASS(Blueprintable, BlueprintType)
class DATABASEDWIDGET_API UWPropGenVerticalBoxDataTable : public UUserWidget, public IWidgetCollectionInterface
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void InitFromStructs_Implementation(const TArray<FInstancedStruct>& Structs) override;
	virtual void InitFromObjects_Implementation(const TArray<UObject*>& Objects) override;

	virtual UWidget* GetCollectionContainer_Implementation() override
	{
		return Container;
	};
	virtual void SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> MembersWidgetClass) override;

protected:
	virtual void SetDataTable_Implementation(UDataTable* NewDataTable) override;
	
	virtual void SetRootWidget_Implementation(UWidgetTree* Tree) override
	{
		Container = Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName("Container"));;
	};
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = VerticalBoxDataTable)
	class UVerticalBox* Container;

	UPROPERTY(EditAnywhere, Category = VerticalBoxDataTable)
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, Category = VerticalBoxDataTable)
	TSoftObjectPtr<UDataTable> DataTable;

};
