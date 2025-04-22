// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 5, 0)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "BirdEyeController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
struct FInputActionInstance;

// Delegate signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProvinceClickedSignature, int, ProvinceID, FInstancedStruct, data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProvinceHoveredSignature, FColor, Color, int, ProvinceID);

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
    UInputMappingContext *DefaultMappingContext;

    /** Action for moving the camera. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction *CameraMoveAction;

    /** Action for selecting objects. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction *SelectAction;

    /** Action for mouse scrolling (zoom). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction *MouseScrollAction;

    /** Action for opening the country editor UI. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction *OpenCountryEditorAction;
#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction *DisplayLookupTextureAction;
#endif

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
    void CameraMovement(const FInputActionInstance &Instance);

    /** Handles camera zoom events. */
    UFUNCTION(BlueprintCallable, Category = "Input Actions")
    void CameraZoom(const FInputActionInstance &Instance);

    /** Starts movement based on mouse position. */
    UFUNCTION(BlueprintCallable, Category = "Input Actions")
    void StartMovement(const FVector2D &MousePos);

    UFUNCTION(BlueprintCallable, Category = "Input Reactions")
    void HideHUD();

    UFUNCTION(BlueprintCallable, Category = "Input Reactions")
    void ShowProvinceInfo(int Id, const FInstancedStruct &Data);

    UFUNCTION(BlueprintCallable, Category = "Input Reactions")
    void HighlightProvince(const FColor &Color);
#if WITH_EDITOR
    UFUNCTION(BlueprintCallable, Category = "Input Reactions")
    void SetLookupTextureActive();
#endif

    class AClickableMap *PerformLineTraceToFindMap(FHitResult &OutHit, bool &OutResultUnderCursor) const;

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
    UPROPERTY(Transient, BlueprintReadOnly)
    class AClickableMap *Map;

    /** Reference to the map pawn. */
    UPROPERTY(Transient)
    class AMapPawn *MapPawn;
};
