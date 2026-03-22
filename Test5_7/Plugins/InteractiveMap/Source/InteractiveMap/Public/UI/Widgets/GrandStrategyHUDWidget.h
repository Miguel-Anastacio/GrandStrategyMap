// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GrandStrategyHUDWidget.generated.h"

/**
 * This is the base HUD widget for the Grand Strategy map plugin.
 * It provides functionality to interact with the game map and save data.
 */
class UCustomButtonWidget;
class UTileSelectedWidget;
UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UGrandStrategyHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetTileSelectedWidgetData(const FInstancedStruct& Data) const;
	
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetTileSelectedVisibility(const ESlateVisibility Visible) const;
	
#if WITH_EDITOR
	void CreateHudElements();
	void SetTileDisplayWidgetClass(const TSubclassOf<UUserWidget> Class);
	void SeMapModeSelectorWidgetClass(const TSubclassOf<UUserWidget> Class);
#endif	
	
protected:
	/** Native initialization override. */
	virtual void NativeOnInitialized() override;
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Widgets")
	UNamedSlot* SelectedSlot;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Widgets")
	UNamedSlot* MapModeSelectorSlot;
	
	UPROPERTY(EditAnywhere, Category = "Widgets", meta = (MustImplement = "/Script/InteractiveMap.WidgetDataInterface"))
	TSubclassOf<UUserWidget> TileDisplayWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UUserWidget> MapModeSelectorWidgetClass;
	
	
	/** Hold ref to widget to handle map mode selection */
	UPROPERTY()
	TWeakObjectPtr<class UMapModeSelectorWidget> MapModeSelectorWidgetRef;
	
private:
#if WITH_EDITOR
	UNamedSlot* CreateNamedSlot(const FString& SlotName) const;
#endif	
	
	
};
