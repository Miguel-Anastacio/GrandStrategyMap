// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/MapModeSelectorWidget.h"
#include "Map/ClickableMap.h"
#include "InteractiveMap.h"
#include "MapObject.h"
#include "Blueprint/WidgetTree.h"
#include "BlueprintLibrary/WidgetEditorFunctionLibrary.h"
#include "Components/GridPanel.h"
#include "UI/Widgets/CustomButtonWidget.h"
#if WITH_EDITOR
#include "WidgetBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#endif

void UMapModeSelectorWidget::SetInteractiveMapReference(class AClickableMap* Map)
{
	GameMapReference = Map;
}

void UMapModeSelectorWidget::NativeOnInitialized()
{
	for(const auto& Widget : GridPanel->GetAllChildren())
	{
		if(UCustomButtonWidget* UserWidget = Cast<UCustomButtonWidget>(Widget))
		{
			UserWidget->OnClickedDelegate.AddDynamic(this, &UMapModeSelectorWidget::SetMapMode);
		}
	}
	Super::NativeOnInitialized();
}

void UMapModeSelectorWidget::NativePreConstruct()
{
	// Set text of Widgets
	if(GridPanel)
	{
		for(const auto& Widget : GridPanel->GetAllChildren())
		{
			if(UCustomButtonWidget* UserWidget = Cast<UCustomButtonWidget>(Widget))
			{
				UserWidget->SetButtonText(FText::FromString(Widget->GetCategoryName()));
			}
		}
	}
	Super::NativePreConstruct();
}

#if WITH_EDITOR
void UMapModeSelectorWidget::CreatePanelSlots() const
{
	CreateMainPanel();

	// We *cannot* use the BindWidget-marked GridPanel, instead we need to get the widget in the asset's widget tree.
	// However thanks to the BindWidget, we can be relatively sure that FindWidget will be successful.
	UGridPanel* AssetGridPanel = Cast<UGridPanel>(UAtkWidgetEditorFunctionLibrary::GetPanelWidget(this, FName("GridPanel")));
	UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(this);
	if(!AssetGridPanel)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Missing Grid Panel"));
		return;
	}
	if(!MapObject)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Please Set a MapObject"));
		return;
	}
	if(!MainAssetWidgetTree)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Widget Tree is Null"));
		return;
	}
	
	AssetGridPanel->ClearChildren();
	AssetGridPanel->Modify();
	UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(this);
	int RowIndex = 0;
	int ColumnIndex = 0;
	for(const auto& Property : MapObject->GetVisualProperties())
	{
		if(UUserWidget* NewWidget = MainAssetWidgetTree->ConstructWidget<UUserWidget>(*MapModeSelectButton))
		{
			NewWidget->SetCategoryName(Property->Name);
			AssetGridPanel->AddChildToGrid(NewWidget, RowIndex, ColumnIndex);
			ColumnIndex++;
			if(ColumnIndex >= Columns)
			{
				ColumnIndex = 0;
				RowIndex++;
			}
		}
	}
	
	if(AssetGridPanel->GetChildrenCount() == 0)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Widget Type is not of type UUserWidget"));
	}
	
	AssetGridPanel->Modify();
	UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(this);
}

void UMapModeSelectorWidget::CreateMainPanel() const
{
	UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(this);
	if (!MainAssetWidgetTree)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("WidgetTree is null!"));
		return;
	}
	
	if (!MainAssetWidgetTree->RootWidget)
	{
		MainAssetWidgetTree->RootWidget = MainAssetWidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass(), FName("GridPanel"));
		MainAssetWidgetTree->Modify();
		UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(this);
	}
}
#endif

void UMapModeSelectorWidget::SetMapMode(UCustomButtonWidget* ButtonWidget)
{
	if(GameMapReference.IsValid())
	{
		GameMapReference->SetMapMode(ButtonWidget->GetFName());
	}
}
