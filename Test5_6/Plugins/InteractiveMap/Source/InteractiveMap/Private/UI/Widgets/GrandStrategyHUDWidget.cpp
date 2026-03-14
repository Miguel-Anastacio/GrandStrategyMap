// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/GrandStrategyHUDWidget.h"

#include "InteractiveMap.h"
#include "Components/CanvasPanel.h"
#include "Components/NamedSlot.h"
#include "UI/Widgets/CustomButtonWidget.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"
#include "UI/Widgets/WidgetDataInterface.h"
#if WITH_EDITOR
#include "Blueprint/WidgetTree.h"
#include "BlueprintLibrary/WidgetEditorFunctionLibrary.h"
#include "WidgetBlueprint.h"
#endif

void UGrandStrategyHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// Create map mode selector
	UUserWidget* ModeSelectorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), MapModeSelectorWidgetClass);
	MapModeSelectorSlot->SetContent(ModeSelectorWidget);
}

void UGrandStrategyHUDWidget::SetTileSelectedWidgetData(const FInstancedStruct& Data) const
{
	// Already created widget and put it on slot then just update
	if (SelectedSlot->HasAnyChildren())
	{
		UWidget* Content = SelectedSlot->GetContent();
		if (Content->Implements<UWidgetDataInterface>())
		{
			IWidgetDataInterface::Execute_SetData(Content, Data); // BP-safe call
		}
		return;
	}
	
	if (!TileDisplayWidgetClass)
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Tile Selected Widget class not set"));
		return;
	}
	
	UUserWidget* TileSelectedWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), TileDisplayWidgetClass);
	if (TileSelectedWidget->Implements<UWidgetDataInterface>())
	{
		IWidgetDataInterface::Execute_SetData(TileSelectedWidget, Data); // BP-safe call
	}
	
	SelectedSlot->SetContent(TileSelectedWidget);
}

void UGrandStrategyHUDWidget::SetTileSelectedVisibility(const ESlateVisibility Visible) const
{
	SelectedSlot->SetVisibility(Visible);
}

#if WITH_EDITOR
void UGrandStrategyHUDWidget::SetTileDisplayWidgetClass(const TSubclassOf<UUserWidget> Class)
{
	TileDisplayWidgetClass = Class;
}

void UGrandStrategyHUDWidget::SeMapModeSelectorWidgetClass(const TSubclassOf<UUserWidget> Class)
{
	MapModeSelectorWidgetClass = Class;
}
void UGrandStrategyHUDWidget::CreateHudElements()
{
	const FName MainPanelName = FName("MainPanel");
	UAtkWidgetEditorFunctionLibrary::CreateRootWidget<UCanvasPanel>(this, MainPanelName);
	
	UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(UAtkWidgetEditorFunctionLibrary::GetPanelWidget(this, MainPanelName));
	UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(this);
	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(this->GetClass()->ClassGeneratedBy);
	
	// Clean up GUIDs for widgets that will be removed and current children
	UAtkWidgetEditorFunctionLibrary::ClearChildren(WidgetBP, CanvasPanel);

	// Create new widgets FIRST
	TArray<UWidget*> NewlyCreatedWidgets;
	if(UNamedSlot* NamedSlot = CreateNamedSlot(FString("SelectedSlot")))
	{
		CanvasPanel->AddChildToCanvas(NamedSlot);
		NewlyCreatedWidgets.Add(NamedSlot);
	}
	if(UNamedSlot* NamedSlot = CreateNamedSlot(FString("MapModeSelectorSlot")))
	{
		CanvasPanel->AddChildToCanvas(NamedSlot);
		NewlyCreatedWidgets.Add(NamedSlot);
	}
    
	// Register GUIDs for all newly created widgets
	UAtkWidgetEditorFunctionLibrary::RegisterNewlyCreatedWidgets(NewlyCreatedWidgets, WidgetBP);
	
	CanvasPanel->Modify();
	MainAssetWidgetTree->Modify();
	WidgetBP->Modify();
	UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(this);
	
}

UNamedSlot* UGrandStrategyHUDWidget::CreateNamedSlot(const FString& SlotName) const
{
	UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(this);
	UNamedSlot* NamedSlot = MainAssetWidgetTree->ConstructWidget<UNamedSlot>();
	if (NamedSlot)
	{
		NamedSlot->Rename(*SlotName, MainAssetWidgetTree, REN_DontCreateRedirectors);
	}
	return NamedSlot;
}
#endif

