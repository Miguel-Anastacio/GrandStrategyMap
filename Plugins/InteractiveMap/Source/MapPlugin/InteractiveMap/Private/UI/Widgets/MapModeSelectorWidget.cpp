// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/MapModeSelectorWidget.h"
#include "Map/ClickableMap.h"
#include "InteractiveMap.h"
#include "MapObject.h"
#include "Components/GridPanel.h"
#include "UI/Widgets/CustomButtonWidget.h"
#if WITH_EDITOR
#include "Blueprint/WidgetTree.h"
#include "BlueprintLibrary/WidgetEditorFunctionLibrary.h"
#include "WidgetBlueprint.h"
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
				UserWidget->SetButtonText(Widget->GetCategoryName());
			}
		}
	}
	Super::NativePreConstruct();
}

#if WITH_EDITOR
void UMapModeSelectorWidget::CreatePanelSlots() const
{
	CreateMainPanel();

	UGridPanel* AssetGridPanel = Cast<UGridPanel>(UAtkWidgetEditorFunctionLibrary::GetPanelWidget(this, FName("GridPanel")));
	UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(this);
	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(GetClass()->ClassGeneratedBy);
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
	// Clean up GUIDs for widgets that will be removed and current children
	UAtkWidgetEditorFunctionLibrary::ClearChildren(WidgetBP, AssetGridPanel);

	// Create new widgets FIRST
	int RowIndex = 0;
	int ColumnIndex = 0;
	TArray<UWidget*> NewlyCreatedWidgets;
	for(const auto& Property : MapObject->GetVisualProperties())
	{
		if(UUserWidget* NewWidget = MainAssetWidgetTree->ConstructWidget<UUserWidget>(*MapModeSelectButton))
		{
			FName UniqueName = MakeUniqueObjectName(MainAssetWidgetTree, MapModeSelectButton, FName(*Property->Name));
			NewWidget->Rename(*UniqueName.ToString(), MainAssetWidgetTree, REN_DontCreateRedirectors);
			
			NewWidget->SetCategoryName(Property->Name);

			// Add to grid (this makes it part of the widget tree)
			AssetGridPanel->AddChildToGrid(NewWidget, RowIndex, ColumnIndex);
			ColumnIndex++;
			if(ColumnIndex >= Columns)
			{
				ColumnIndex = 0;
				RowIndex++;
			}

			// Store for GUID registration
			NewlyCreatedWidgets.Add(NewWidget);
		}
	}
    
	// Register GUIDs for all newly created widgets
	UAtkWidgetEditorFunctionLibrary::RegisterNewlyCreatedWidgets(NewlyCreatedWidgets, WidgetBP);
	if(AssetGridPanel->GetChildrenCount() == 0)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Widget Type is not of type UUserWidget"));
	}
	
	AssetGridPanel->Modify();
	MainAssetWidgetTree->Modify();
	WidgetBP->Modify();
	UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(this);
}

void UMapModeSelectorWidget::CreateMainPanel() const
{
	UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(this);
	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(GetClass()->ClassGeneratedBy);
    
	if (!MainAssetWidgetTree)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("WidgetTree is null!"));
		return;
	}
    
	if (!WidgetBP)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("WidgetBlueprint is null!"));
		return;
	}
    
	if (!MainAssetWidgetTree->RootWidget)
	{
		// Construct the root widget
		UGridPanel* NewRootWidget = MainAssetWidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass(), FName("GridPanel"));
       
		if (NewRootWidget)
		{
			// Generate and register a GUID for the root widget
			FGuid NewGuid = FGuid::NewGuid();
			WidgetBP->WidgetVariableNameToGuidMap.Add(NewRootWidget->GetFName(), NewGuid);
          
			// Set as root widget
			MainAssetWidgetTree->RootWidget = NewRootWidget;
			// Mark as modified
			UE_LOG(LogInteractiveMap, Log, TEXT("Created MainPanel with GUID: %s"), *NewGuid.ToString());
		}
		else
		{
			UE_LOG(LogInteractiveMap, Error, TEXT("Failed to construct MainPanel widget!"));
		}
	}
}
#endif

void UMapModeSelectorWidget::SetMapMode(UCustomButtonWidget* ButtonWidget)
{
	if(GameMapReference.IsValid())
	{
		GameMapReference->SetMapMode(*ButtonWidget->GetCategoryName());
	}
}
