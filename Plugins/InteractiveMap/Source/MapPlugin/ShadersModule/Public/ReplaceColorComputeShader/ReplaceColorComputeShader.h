#pragma once

#include "CoreMinimal.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialRenderProxy.h"

#include "ReplaceColorComputeShader.generated.h"

struct FColorReplace
{
	FIntVector3 ColorToReplace;
	FIntVector3 NewColor;

	FColorReplace(const FIntVector3& OldColor, const FIntVector3& Color) : ColorToReplace(OldColor), NewColor(Color) {};
	FColorReplace(const FColor& OldColor, const FColor& Color) :
	ColorToReplace(FIntVector3(OldColor.R, OldColor.G, OldColor.B)), NewColor(FIntVector3(Color.R, Color.G, Color.B)) {};
};
struct SHADERSMODULE_API FReplaceColorComputeShaderDispatchParams
{
	TArray<uint32> PixelArray;
	TArray<FColorReplace> ReplacementRules;

	FReplaceColorComputeShaderDispatchParams(const TArray<uint8>& Pixels, const TArray<FColorReplace>& ReplacementRules)
		: ReplacementRules(ReplacementRules)
	{
		PixelArray = UAtkTextureUtilsFunctionLibrary::PackUint8ToUint32(Pixels);
		
	};
};

// This is a public interface that we define so outside code can invoke our compute shader.
class SHADERSMODULE_API FReplaceColorComputeShaderInterface {
public:
	// Executes this shader on the render thread
	static void DispatchRenderThread(
		FRHICommandListImmediate& RHICmdList,
		FReplaceColorComputeShaderDispatchParams Params,
		TFunction<void(TArray<uint32> OutputVal)> AsyncCallback
	);
	
	// Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
	static void DispatchGameThread(
		FReplaceColorComputeShaderDispatchParams& Params,
		const TFunction<void(TArray<uint32> OutputVal)>& AsyncCallback
	);
	// Dispatches this shader. Can be called from any thread
	static void Dispatch(
		FReplaceColorComputeShaderDispatchParams Params,
		const TFunction<void(TArray<uint32> OutputVal)>& AsyncCallback
	);
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReplaceColorComputeShaderLibrary_AsyncExecutionCompleted, const int, Value);

UCLASS() // Change the _API to match your project
class SHADERSMODULE_API UReplaceColorComputeShaderLibrary_AsyncExecution : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	// Execute the actual load
	virtual void Activate() override {
		// Create a dispatch parameters struct and fill it the input array with our args
		// FReplaceColorComputeShaderDispatchParams Params(1, 1, 1);
		// Params.Input[0] = Arg1;
		// Params.Input[1] = Arg2;

		// Dispatch the compute shader and wait until it completes
		// FReplaceColorComputeShaderInterface::Dispatch(Params, [this](int OutputVal) {
		// 	this->Completed.Broadcast(OutputVal);
		// });
	}
	
	
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", Category = "ComputeShader", WorldContext = "WorldContextObject"))
	static UReplaceColorComputeShaderLibrary_AsyncExecution* ExecuteBaseComputeShader(UObject* WorldContextObject, int Arg1, int Arg2) {
		UReplaceColorComputeShaderLibrary_AsyncExecution* Action = NewObject<UReplaceColorComputeShaderLibrary_AsyncExecution>();
		Action->Arg1 = Arg1;
		Action->Arg2 = Arg2;
		Action->RegisterWithGameInstance(WorldContextObject);

		return Action;
	}

	UPROPERTY(BlueprintAssignable)
	FOnReplaceColorComputeShaderLibrary_AsyncExecutionCompleted Completed;

	
	int Arg1;
	int Arg2;
	
};