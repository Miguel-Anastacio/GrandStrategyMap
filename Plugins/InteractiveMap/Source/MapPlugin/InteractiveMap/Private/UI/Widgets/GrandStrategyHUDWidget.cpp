// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/GrandStrategyHUDWidget.h"
#include "UI/Widgets/CustomButtonWidget.h"
#include "Map/ClickableMap.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"
#include "UI/Widgets/MapModeSelectorWidget.h"

void UGrandStrategyHUDWidget::SetGameMapReference()
{
	if (AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), AClickableMap::StaticClass()))
	{
		GameMap = Cast<AClickableMap>(Actor);
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
