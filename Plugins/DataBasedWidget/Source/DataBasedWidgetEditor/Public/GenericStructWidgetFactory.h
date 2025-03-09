#pragma once

#include "AssetDefinitionDefault.h"
#include "ClassIconFinder.h"
#include "GenericStructWidget.h"
#include "Factories/Factory.h"
#include "UObject/NameTypes.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "GenericStructWidgetFactory.generated.h"

UCLASS(MinimalAPI, hidecategories=Object)
class UGenericStructWidgetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class UAssetDefinition_GenericStructWidget : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	
	virtual FText GetAssetDisplayName() const override { return FText::FromString("GenericWidget"); }
	virtual FLinearColor GetAssetColor() const override { return FLinearColor(FColor(48, 165, 171)); }
	virtual TSoftClassPtr<UObject> GetAssetClass() const override { return UGenericStructWidget::StaticClass(); }

	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override
	{
		static const FAssetCategoryPath Categories[] = {FText::FromString("UI")};
		
		return Categories;
	}

	virtual const FSlateBrush* GetThumbnailBrush(const FAssetData& InAssetData, const FName InClassName) const override
	{
		return FClassIconFinder::FindThumbnailForClass(InAssetData.GetClass());
	}

	// virtual EAssetCommandResult OpenAssets(const FAssetOpenArgs& OpenArgs) const override;
};