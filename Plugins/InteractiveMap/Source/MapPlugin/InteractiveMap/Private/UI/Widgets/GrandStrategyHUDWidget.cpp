// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/GrandStrategyHUDWidget.h"

#include "InteractiveMap.h"
#include "Components/NamedSlot.h"
#include "UI/Widgets/CustomButtonWidget.h"
#include "Map/ClickableMap.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"
#include "UI/Widgets/MapModeSelectorWidget.h"
#include "UI/Widgets/WidgetDataInterface.h"

void UGrandStrategyHUDWidget::SetGameMapReference()
{
	if (AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), AClickableMap::StaticClass()))
	{
		AClickableMap* GameMap = Cast<AClickableMap>(Actor);
		if(UMapModeSelectorWidget && GameMap)
		{
			UMapModeSelectorWidget->SetInteractiveMapReference(GameMap);
		}
	}
}

void UGrandStrategyHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetGameMapReference();
}

void UGrandStrategyHUDWidget::SetTileSelectedWidgetData(const FInstancedStruct& Data)
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

void UGrandStrategyHUDWidget::SetTileSelectedVisibility(const ESlateVisibility Visible)
{
	SelectedSlot->SetVisibility(Visible);
}
