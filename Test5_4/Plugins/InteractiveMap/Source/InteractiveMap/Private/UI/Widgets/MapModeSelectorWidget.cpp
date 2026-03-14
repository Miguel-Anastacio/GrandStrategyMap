// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/MapModeSelectorWidget.h"
#include "Map/ClickableMap.h"
#include "InteractiveMap.h"
#include "MapObject.h"
#include "Components/GridPanel.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/CustomButtonWidget.h"
#if WITH_EDITOR
#include "Blueprint/WidgetTree.h"
#include "BlueprintLibrary/WidgetEditorFunctionLibrary.h"
#include "WidgetBlueprint.h"
#endif

void UMapModeSelectorWidget::SetInteractiveMapReference()
{
	if (AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), AClickableMap::StaticClass()))
	{
		if(AClickableMap* GameMap = Cast<AClickableMap>(Actor))
		{
			GameMapReference = GameMap;
		}
	}
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
	
	SetInteractiveMapReference();
	Super::NativeOnInitialized();
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
		if(UCustomButtonWidget* NewWidget = MainAssetWidgetTree->ConstructWidget<UCustomButtonWidget>(*MapModeSelectButton))
		{
			FName UniqueName = MakeUniqueObjectName(MainAssetWidgetTree, MapModeSelectButton, FName(*Property->Name));
			NewWidget->Rename(*UniqueName.ToString(), MainAssetWidgetTree, REN_DontCreateRedirectors);
			
			NewWidget->SetButtonText(Property->Name);

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
		UE_LOG(LogInteractiveMap, Error, TEXT("Widget Type is not of type UCustomButtonWidget"));
	}
	
	AssetGridPanel->Modify();
	MainAssetWidgetTree->Modify();
	WidgetBP->Modify();
	UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(this);
}

void UMapModeSelectorWidget::CreateMainPanel() const
{
	if (!UAtkWidgetEditorFunctionLibrary::CreateRootWidget<UGridPanel>(this, FName("GridPanel")))
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Creating GridPanel for map mode selector failed!"));
	}
}
#endif

void UMapModeSelectorWidget::SetMapMode(const UCustomButtonWidget* ButtonWidget)
{
	if(GameMapReference.IsValid())
	{
		GameMapReference->SetMapMode(*ButtonWidget->ButtonText);
	}
}
