// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "Map/MapUtils.h"
#include "BirdEyeController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
struct FInputActionInstance;


// Delegate signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProvinceClickedSignature, FName, ProvinceID, FProvinceData, data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProvinceHoveredSignature, FColor, Color);

/**     
 * Base Controller class for interactive maps, handles input for mouse clicks, camera movement, menus and camera zoom
 */
UCLASS()
class INTERACTIVEMAP_API ABirdEyeController : public APlayerController
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    ABirdEyeController();

    /** Mapping context for the controller. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    /** Action for moving the camera. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* CameraMoveAction;

    /** Action for selecting objects. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* SelectAction;

    /** Action for mouse scrolling (zoom). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MouseScrollAction;

    /** Action for opening the country editor UI. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* OpenCountryEditorAction;

    UPROPERTY(BlueprintAssignable)
    FOnProvinceClickedSignature ProvinceClickedDelegate;

    UPROPERTY(BlueprintAssignable)
    FOnProvinceHoveredSignature ProvinceHoveredDelegate;

    UPROPERTY(BlueprintAssignable)
    FOnMapClickedSignature MapClickedDelegate;

protected:
    /** Sets up the input component. */
    virtual void SetupInputComponent() override;

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    /** Handles mouse click events. */
    UFUNCTION(BlueprintCallable, Category = "Input Actions")
    void MouseClick();

    /** Handles camera movement events. */
    UFUNCTION(BlueprintCallable, Category = "Input Actions")
    void CameraMovement(const FInputActionInstance& instance);

    /** Handles camera zoom events. */
    UFUNCTION(BlueprintCallable, Category = "Input Actions")
    void CameraZoom(const FInputActionInstance& instance);

    /** Starts movement based on mouse position. */
    UFUNCTION(BlueprintCallable, Category = "Input Actions")
    void StartMovement(const FVector2D& mousePos);

    /** Toggles the country editor UI. */
    UFUNCTION(BlueprintCallable, Category = "Input Actions")
    void ToggleCountryEditor();

    UFUNCTION(BlueprintCallable, Category = "Input Reactions")
    void HideHUD();

    UFUNCTION(BlueprintCallable, Category = "Input Reactions")
    void ShowProvinceInfo(FName id, FProvinceData data);

    UFUNCTION(BlueprintCallable, Category = "Input Reactions")
    void HighlightProvince(FColor color);

protected:
    /** Offset for mouse click on Z-axis. */
    UPROPERTY(EditAnywhere, Category = "Mouse Click")
    float zOffset = 300.0f;
    /** Define channel where the mouse traces are performed 
    *   For the ClickableMap to work make sure that at least one of the map meshes blocks this channel
    */
    UPROPERTY(EditAnywhere, Category = "Mouse Click")
    TEnumAsByte<ECollisionChannel> MouseTraceChannel = ECC_Visibility;

    /** Flag indicating if a province is selected. */
    bool bProvinceSelected = false;

    /** Flag indicating camera movement. */
    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    bool bMoveCamera;

    /** Input for camera movement. */
    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    FVector2D MoveInput;

    /** Center of the viewport. */
    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    FVector2D ViewportCenter;

    /** Reference to the interactive map. */
    UPROPERTY(Transient)
    class AClickableMap* Map;

    /** Reference to the map pawn. */
    UPROPERTY(Transient)
    class AMapPawn* MapPawn;
};

