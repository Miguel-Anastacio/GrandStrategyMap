#include "ReplaceColorComputeShader.h"
#include "ShadersModule/Public/ReplaceColorComputeShader/ReplaceColorComputeShader.h"
#include "PixelShaderUtils.h"
#include "MeshPassProcessor.inl"
#include "StaticMeshResources.h"
#include "DynamicMeshBuilder.h"
#include "RenderGraphResources.h"
#include "GlobalShader.h"
#include "UnifiedBuffer.h"
#include "CanvasTypes.h"
#include "MeshDrawShaderBindings.h"
#include "RHIGPUReadback.h"
#include "MeshPassUtils.h"
#include "MaterialShader.h"

DECLARE_STATS_GROUP(TEXT("ReplaceColorComputeShader"), STATGROUP_ReplaceColorComputeShader, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("ReplaceColorComputeShader Execute"), STAT_ReplaceColorComputeShader_Execute, STATGROUP_ReplaceColorComputeShader);

// This class carries our parameter declarations and acts as the bridge between cpp and HLSL.
class SHADERSMODULE_API FReplaceColorComputeShader: public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FReplaceColorComputeShader);
	SHADER_USE_PARAMETER_STRUCT(FReplaceColorComputeShader, FGlobalShader);
	
	
	class FReplaceColorComputeShader_Perm_TEST : SHADER_PERMUTATION_INT("TEST", 1);
	using FPermutationDomain = TShaderPermutationDomain<
		FReplaceColorComputeShader_Perm_TEST
	>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
	
		SHADER_PARAMETER(uint32, ColourCount)
		SHADER_PARAMETER(uint32, PixelCount)
		// SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<int>, PixelBuffer)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<int>, PixelBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FColourReplace>, ColorReplacementDataBuffer)

		// SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<int>, Input)
		// SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<int>, Output)
		//

	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		const FPermutationDomain PermutationVector(Parameters.PermutationId);
		
		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		const FPermutationDomain PermutationVector(Parameters.PermutationId);

		/*
		* Here you define constants that can be used statically in the shader code.
		* Example:
		*/
		// OutEnvironment.SetDefine(TEXT("MY_CUSTOM_CONST"), TEXT("1"));

		/*
		* These defines are used in the thread count section of our shader
		*/
		OutEnvironment.SetDefine(TEXT("THREADS_X"), NUM_THREADS_ReplaceColorComputeShader_X);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), NUM_THREADS_ReplaceColorComputeShader_Y);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), NUM_THREADS_ReplaceColorComputeShader_Z);

		// This shader must support typed UAV load and we are testing if it is supported at runtime using RHIIsTypedUAVLoadSupported
		OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);

		// FForwardLightingParameters::ModifyCompilationEnvironment(Parameters.Platform, OutEnvironment);
	}
private:
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType                            ShaderPath                     Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FReplaceColorComputeShader, "/ShadersModuleShaders/ReplaceColorComputeShader/ReplaceColorComputeShader.usf", "ReplaceColorComputeShader", SF_Compute);

void FReplaceColorComputeShaderInterface::DispatchRenderThread(FRHICommandListImmediate& RHICmdList, FReplaceColorComputeShaderDispatchParams Params, TFunction<void(TArray<uint32> OutputVal)> AsyncCallback) {
	FRDGBuilder GraphBuilder(RHICmdList);

	{
		SCOPE_CYCLE_COUNTER(STAT_ReplaceColorComputeShader_Execute);
		DECLARE_GPU_STAT(ReplaceColorComputeShader)
		RDG_EVENT_SCOPE(GraphBuilder, "ReplaceColorComputeShader");
		RDG_GPU_STAT_SCOPE(GraphBuilder, ReplaceColorComputeShader);
		
		typename FReplaceColorComputeShader::FPermutationDomain PermutationVector;
		
		// Add any static permutation options here
		// PermutationVector.Set<FReplaceColorComputeShader::FMyPermutationName>(12345);

		TShaderMapRef<FReplaceColorComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);
		

		bool bIsShaderValid = ComputeShader.IsValid();

		if (bIsShaderValid)
		{
			FRDGBufferRef PixelBuffer = CreateStructuredBuffer(GraphBuilder, TEXT("Pixel Buffer"), sizeof(uint32),
						Params.PixelArray.Num(), Params.PixelArray.GetData(), Params.PixelArray.Num() * sizeof(uint32));

			FRDGBufferRef ColourReplacementDataBuffer = CreateStructuredBuffer(GraphBuilder, TEXT("Colour Replacement Buffer"),
						sizeof(FColorReplace), Params.ReplacementRules.Num(), Params.ReplacementRules.GetData(), Params.ReplacementRules.Num() * sizeof(FColorReplace));
			

			FReplaceColorComputeShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FReplaceColorComputeShader::FParameters>();
			PassParameters->ColourCount = Params.ReplacementRules.Num();
			PassParameters->PixelBuffer = GraphBuilder.CreateUAV(PixelBuffer);
			PassParameters->ColorReplacementDataBuffer = GraphBuilder.CreateSRV(ColourReplacementDataBuffer);
			PassParameters->PixelCount = Params.PixelArray.Num();

			FRHIGPUBufferReadback* GPUBufferReadback = new FRHIGPUBufferReadback(TEXT("ExecuteReplaceColorComputeShaderOutput"));
			auto GroupCount = FComputeShaderUtils::GetGroupCount(FIntVector(1, 1, 1), FComputeShaderUtils::kGolden2DGroupSize);
			GraphBuilder.AddPass(
				RDG_EVENT_NAME("ExecuteReplaceColorComputeShader"),
				PassParameters,
				ERDGPassFlags::AsyncCompute,
				[&PassParameters, ComputeShader, GroupCount](FRHIComputeCommandList& RHICmdList)
			{
				FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *PassParameters, GroupCount);
			});

			AddEnqueueCopyPass(GraphBuilder, GPUBufferReadback, PixelBuffer, 0u);
			
			// Assuming you know the number of elements in the buffer
			int32 NumElements = Params.PixelArray.Num(); // Replace with the actual number of uint32 elements in the buffer

			// Create a CPU-side array to hold the data
			
			auto RunnerFunc = [GPUBufferReadback, AsyncCallback, NumElements](auto&& RunnerFunc) -> void {
				if (GPUBufferReadback->IsReady())
				{
					TArray<uint32> CPUBuffer;
					CPUBuffer.Reserve(NumElements);
					uint32* Buffer = (uint32*)GPUBufferReadback->Lock(NumElements);  // Lock it for 1 element
					if (Buffer)  // Check if the lock was successful
					{
						FMemory::Memcpy(CPUBuffer.GetData(), Buffer, sizeof(uint32) * NumElements);
						GPUBufferReadback->Unlock();  // Unlock the buffer

						AsyncTask(ENamedThreads::GameThread, [AsyncCallback, CPUBuffer]() {
							AsyncCallback(CPUBuffer);  // Call callback with CPU buffer data
						});

						delete GPUBufferReadback;  // Clean up readback buffer
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to lock the GPU readback buffer"));
					}
				}
				else
				{
					AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunc]() {
						RunnerFunc(RunnerFunc);
					});
				}
			};

			AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunc]() {
				RunnerFunc(RunnerFunc);
			});
		}
		else
		{
			#if WITH_EDITOR
				GEngine->AddOnScreenDebugMessage((uint64)42145125184, 6.f, FColor::Red, FString(TEXT("The compute shader has a problem.")));
			#endif
			// We exit here as we don't want to crash the game if the shader is not found or has an error.
		}
	}

	GraphBuilder.Execute();
}