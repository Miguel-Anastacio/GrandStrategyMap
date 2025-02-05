// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/MapModeSelectorWidget.h"
#include "Map/ClickableMap.h"
#include "InteractiveMap.h"
#include "MapObject.h"
#if WITH_EDITOR
#include "WidgetBlueprint.h"
#endif
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"

#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "UI/Widgets/CustomButtonWidget.h"
#if WITH_EDITOR
#include "Kismet2/BlueprintEditorUtils.h"
#endif

void UMapModeSelectorWidget::SetInteractiveMapReference(class AClickableMap* Map)
{
	GameMapReference = Map;
}

void UMapModeSelectorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	for(const auto& Widget : GridPanel->GetAllChildren())
	{
		if(UCustomButtonWidget* UserWidget = Cast<UCustomButtonWidget>(Widget))
		{
			UserWidget->OnClickedDelegate.AddDynamic(this, &UMapModeSelectorWidget::SetMapMode);
			UserWidget->Text = FText::FromString(Widget->GetName());
		}
	}
		
}

void UMapModeSelectorWidget::CreatePanelSlots()
{
#if WITH_EDITOR
	if (!GridPanel)
		return;

	const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(GetClass());
	const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
	UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());

	// We *cannot* use the BindWidget-marked GridPanel, instead we need to get the widget in the asset's widget tree.
	// However thanks to the BindWidget, we can be relatively sure that FindWidget will be successful.
	UGridPanel* AssetGridPanel = Cast<UGridPanel>(MainAsset->WidgetTree->FindWidget("GridPanel"));
	if(!AssetGridPanel)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Missing Main Panel"));
		return;
	}
	if(!MapObject)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Please Set a MapObject"));
		return;
	}
	
	AssetGridPanel->ClearChildren();
	TArray<FVisualPropertyType*> PropTypes;
	if(UDataManagerFunctionLibrary::ReadDataTableToArray(MapObject->VisualPropertyTypesDT, PropTypes))
	{
		int RowIndex = 0;
		int ColumnIndex = 0;
		for(const auto& Type : PropTypes)
		{
			if(UUserWidget* NewWidget = MainAsset->WidgetTree->ConstructWidget<UUserWidget>(*MapModeSelectButton))
			{
				NewWidget->Rename(*(Type->Type.ToString()));
				AssetGridPanel->AddChildToGrid(NewWidget, RowIndex, ColumnIndex);
				ColumnIndex++;
				if(ColumnIndex > Columns)
				{
					ColumnIndex = 0;
					RowIndex++;
				}
			}
			
		}
	}

	AssetGridPanel->Modify();
	MainAsset->Modify();
	if(AssetGridPanel->GetChildrenCount() == 0)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Widget Type is not of type UUserWidget"));
	}
	
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainAsset);
#endif
	
}

void UMapModeSelectorWidget::SetMapMode(UCustomButtonWidget* ButtonWidget)
{
	if(GameMapReference.IsValid())
	{
		GameMapReference->SetMapMode(ButtonWidget->GetFName());
	}
}
