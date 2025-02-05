// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Map/MapUtils.h"
#include "MapModeSelectorWidget.generated.h"
class UCustomButtonWidget;
class UMapObject;
//
class UCustomEditableText;

UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UMapModeSelectorWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Sets the reference to the interactive map. */
    UFUNCTION(BlueprintCallable, Category = "MapModeSelectorWidget")
    void SetInteractiveMapReference(class AClickableMap* Map);
protected:
    virtual void NativeOnInitialized() override;
#if WITH_EDITOR
    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Custom Struct Display")
    void CreatePanelSlots();
#endif
    UFUNCTION()
    void SetMapMode(UCustomButtonWidget* ButtonWidget);
protected:
    // UPROPERTY(EditAnywhere)
    // uint8 Rows = 10;
    UPROPERTY(EditAnywhere, Category = "MapModeSelectorWidget")
    uint8 Columns = 5;

    UPROPERTY(EditAnywhere, Category = "MapModeSelectorWidget")
    TSubclassOf<UUserWidget> MapModeSelectButton;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
    UMapObject* MapObject;
    
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Grid Panel")
    class UGridPanel* GridPanel;
    
    /** Reference to the interactive map. */
    UPROPERTY()
    TWeakObjectPtr<class AClickableMap> GameMapReference;

    UPROPERTY()
    TMap<FName, UUserWidget*> MapModeButtonMapping;

};
