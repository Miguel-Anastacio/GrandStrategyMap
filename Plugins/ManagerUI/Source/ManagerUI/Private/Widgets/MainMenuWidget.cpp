// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenuWidget.h"
#include "Components/Button.h"
#include "Widgets/ButtonWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/UObjectGlobals.h"
#include "LayerManagerHUD.h"
#include "TimerManager.h"
#include "Engine/EngineTypes.h"
void UMainMenuWidget::AsyncLevelLoad(const FString& levelDir, const FString& levelName)
{
	LoadPackageAsync(levelDir + levelName,
		FLoadPackageAsyncDelegate::CreateLambda([=, this](const FName& packageName, UPackage* loadedPackage, EAsyncLoadingResult::Type result)
			{
				if (result == EAsyncLoadingResult::Succeeded)
				{
					AsyncLevelLoadFinished(levelName);
				}
			}
		),
		0,
		PKG_ContainsMap);

	if (StartButton)
		StartButton->SetVisibility(ESlateVisibility::Collapsed);
	if (QuitButton)
		QuitButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UMainMenuWidget::OnPreheatFinished() const
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
	ALayerManagerHUD* hud = Cast<ALayerManagerHUD>(GetOwningPlayer()->GetHUD());
	if (hud)
	{
		hud->PopFromCurrentLayer();
	}
}

void UMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IsValid(StartButton))
	{
		UButton* button = StartButton->GetButton();
		if (IsValid(button))
			button->OnClicked.AddDynamic(this, &UMainMenuWidget::StartGame);

		//StartButton->UIInputActionDelegate.AddDynamic(this, &UMainMenuWidget::StartGame);
	}

	if (IsValid(QuitButton))
	{
		UButton* button = QuitButton->GetButton();
		if (IsValid(button))
			button->OnClicked.AddDynamic(this, &UMainMenuWidget::QuitGame);

		//QuitButton->UIInputActionDelegate.AddDynamic(this, &UMainMenuWidget::QuitGame);
	}

}

void UMainMenuWidget::AsyncLevelLoadFinished(const FString& levelName)
{
	FTimerHandle timer;
	LevelName = levelName;
	GetWorld()->GetTimerManager().SetTimer(timer, this, &UMainMenuWidget::OnPreheatFinished, MinimumLevelLoadTime, false);
}


void UMainMenuWidget::StartGame()
{
	AsyncLevelLoad(LevelDirectory, StartLevelName);
}

void UMainMenuWidget::QuitGame()  
{
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
}
