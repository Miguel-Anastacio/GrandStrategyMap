#pragma once

#include "CoreMinimal.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialRenderProxy.h"

#include "ReplaceColorComputeShader.generated.h"

struct FColorReplace
{
	FIntVector3 ColorToReplace;
	FIntVector3 NewColor;
};
struct SHADERSMODULE_API FReplaceColorComputeShaderDispatchParams
{
	TArray<uint32> PixelArray;
	TArray<FColorReplace> ReplacementRules;

	FReplaceColorComputeShaderDispatchParams(const TArray<uint8>& Pixels, const TArray<FColorReplace>& ReplacementRules)
		: ReplacementRules(ReplacementRules)
	{

		// Ensure the input array has at least 4 bytes to process
		int32 NumElements = Pixels.Num();
		int32 NumPackedElements = NumElements / 4; // Number of uint32 values we can pack
		int32 Remainder = NumElements % 4; // Remaining bytes that don't form a full uint32
		PixelArray.Empty(NumPackedElements); // Clear the array before filling it
		if (Remainder != 0)
		{
			return;
		}
		
		PixelArray.Reserve(NumPackedElements);
		for (int32 i = 0; i < NumPackedElements; ++i)
		{
			// Grab the next 4 bytes from the uint8 array
			uint32 PackedValue = 0;

			// Combine 4 uint8s into a single uint32
			PackedValue |= Pixels[i * 4 + 0] << 0;   // First byte at the lowest byte position
			PackedValue |= Pixels[i * 4 + 1] << 8;   // Second byte shifted by 8 bits
			PackedValue |= Pixels[i * 4 + 2] << 16;  // Third byte shifted by 16 bits
			PackedValue |= Pixels[i * 4 + 3] << 24;  // Fourth byte shifted by 24 bits
			// Add the packed value to the uint32 array
			PixelArray.Emplace(PackedValue);
		}
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

	// FRDGBufferRef FReplaceColorComputeShaderInterface::CreateStructuredBuffer(
	// FRDGBuilder& GraphBuilder,
	// const TCHAR* DebugName,
	// const TArray<uint8>& InputData)
	// {
	// 	const uint32 BufferSize = InputData.Num() * sizeof(uint8);
	// 	FRDGBufferRef Buffer = GraphBuilder.CreateBuffer(
	// 		FRDGBufferDesc::CreateStructuredDesc(sizeof(uint8), InputData.Num()),
	// 		DebugName
	// 	);
	//
	// 	uint8* BufferData = static_cast<uint8*>(GraphBuilder.allo(BufferSize));
	// 	FMemory::Memcpy(BufferData, InputData.GetData(), BufferSize);
	//
	// 	AddBufferUploadPass(GraphBuilder, Buffer, BufferData, BufferSize);
	//
	// 	return Buffer;
	// }

	// Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
	static void DispatchGameThread(
		FReplaceColorComputeShaderDispatchParams& Params,
		const TFunction<void(TArray<uint32> OutputVal)>& AsyncCallback
	)
	{
		ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
		[Params, AsyncCallback](FRHICommandListImmediate& RHICmdList)
		{
			DispatchRenderThread(RHICmdList, Params, AsyncCallback);
		});
	}

	// Dispatches this shader. Can be called from any thread
	static void Dispatch(
		FReplaceColorComputeShaderDispatchParams Params,
		const TFunction<void(TArray<uint32> OutputVal)>& AsyncCallback
	)
	{
		if (IsInRenderingThread()) {
			DispatchRenderThread(GetImmediateCommandList_ForRenderCommand(), Params, AsyncCallback);
		}else{
			DispatchGameThread(Params, AsyncCallback);
		}
	}
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