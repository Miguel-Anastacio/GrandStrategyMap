// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetReactionInterface.h"
#include "MainMenuWidget.generated.h"

/** Custom main menu, adds loading level async to create a more smooth experience when loading the game level through the function AsyncLevelLoad
 * 
 */
UCLASS(Abstract, BlueprintType)
class MANAGERUI_API UMainMenuWidget : public UUserWidget, public IWidgetReactionInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Load")
	void AsyncLevelLoad(const FString& levelDir, const FString& levelName);

protected:
	UFUNCTION()
	void OnPreheatFinished() const;

	virtual void NativeOnInitialized() override;
	void AsyncLevelLoadFinished(const FString& LevelName);

	// Functions bound to button clicks
	UFUNCTION()
	void StartGame();
	UFUNCTION()
	void QuitGame();

protected:
	// Buttons
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Widget")
	TObjectPtr<class UButtonWidget> StartButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Widget")
	TObjectPtr<class UButtonWidget> QuitButton;


	// Name of level that you wish to load when pressing the start Button
	UPROPERTY(EditAnywhere, Category = "Start Level")
	FString StartLevelName;
	// Directory of level that you wish to load when pressing the start Button
	UPROPERTY(EditAnywhere, Category = "Start Level")
	FString LevelDirectory;

	UPROPERTY(EditAnywhere, Category = "Start Level")
	float MinimumLevelLoadTime;

	FString LevelName;

	
};
