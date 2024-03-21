// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LayerManagerHUD.h"
#include "CoreGameHUD.generated.h"

/**
 * Example HUD that inherits from the LayerManager HUD
 */
UCLASS()
class MANAGERUI_API ACoreGameHUD : public ALayerManagerHUD
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	// EXAMPLE ONLY
	// This is an example event for like an inventory widget
	UFUNCTION(BlueprintNativeEvent, Category = "UI Event")
	void ToggleInventory();
	void ToggleInventory_Implementation();

	// You can define the pause behaviour either in blueprint or c++
	UFUNCTION(BlueprintNativeEvent,Category = "UI Event")
	void TogglePause();
	void TogglePause_Implementation();

protected:
	// This are classes of widgets that can be spawned in c++
	UPROPERTY(EditAnywhere, Category = "Menus")
	TSubclassOf<class UUserWidget> MainMenuClass;
	UPROPERTY(EditAnywhere, Category = "Menus")
	TSubclassOf<class UUserWidget> OptionsMenuClass;
	UPROPERTY(EditAnywhere, Category = "Game")
	TSubclassOf<class UUserWidget> HUDClass;
	UPROPERTY(EditAnywhere, Category = "Game")
	TSubclassOf<class UUserWidget> InventoryClass;
	UPROPERTY(EditAnywhere, Category = "Modals")
	TSubclassOf<class UUserWidget> ConfirmModalClass;

	// References to specific widgets, only have to define in c++ and set if you wish to 
	// use them in code
	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	TObjectPtr<class UUserWidget> HUDWidget;
	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	TObjectPtr<class UUserWidget> InventoryWidget;
	//UPROPERTY(BlueprintReadWrite)
	//TObjectPtr<class UUserWidget> PauseWidget;

	

};
