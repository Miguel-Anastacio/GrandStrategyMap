// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "BlueprintLibrary/MiscFunctionLibrary.h"
#include <string_view>
#include "UtilityModule.h"
#include "Misc/ScopedSlowTask.h"
#include "Misc/FeedbackContext.h"

static float CurrentProgress = 0.0f;
void UAtkMiscFunctionLibrary::ExecuteSlowTaskWithProgressBar(TFunction<void(TFunction<void(float, std::string_view)>)> Task)
{
	CurrentProgress = 0.0f;
	constexpr float AmountOfWork = 100.0f;
	FScopedSlowTask SlowTask = FScopedSlowTask(AmountOfWork, NSLOCTEXT("SlowTask", "SlowTask", "Processing..."), true);
	SlowTask.MakeDialog();
	auto ProgressCallback = [&](float Progress, std::string_view message)
	{
		// CurrentProgress += Progress;
		// // Update the progress bar
		// SlowTask.EnterProgressFrame(
		// 	CurrentProgress,
		// 	FText::Format(
		// 		NSLOCTEXT("SlowTask", "Processing", "({1})"),
		// 		FText::AsNumber(FMath::RoundToInt(CurrentProgress)), // First parameter: Progress as a percentage
		// 		FText::FromString(FString(message.data()))    // Second parameter: Convert std::string_view
		// 	)
		// );
	};

	// execute slow task and pass progress callback
	if(Task)
	{
        Task(ProgressCallback);
	}
}

int32 UAtkMiscFunctionLibrary::HexToDecimal(const FString& Hex)
{
	static const TMap<TCHAR, int32> HexMap
	{
		TPair<TCHAR, int32>('0', 0),
		TPair<TCHAR, int32>('1', 1),
		TPair<TCHAR, int32>('2', 2),
		TPair<TCHAR, int32>('3', 3),
		TPair<TCHAR, int32>('4', 4),
		TPair<TCHAR, int32>('5', 5),
		TPair<TCHAR, int32>('6', 6),
		TPair<TCHAR, int32>('7', 7),
		TPair<TCHAR, int32>('8', 8),
		TPair<TCHAR, int32>('9', 9),
		TPair<TCHAR, int32>('A', 10),
		TPair<TCHAR, int32>('B', 11),
		TPair<TCHAR, int32>('C', 12),
		TPair<TCHAR, int32>('D', 13),
		TPair<TCHAR, int32>('E', 14),
		TPair<TCHAR, int32>('F', 15),
   };
	const FString Temp = Hex.Reverse().ToUpper();
	int32 Total = 0;
	for (int i = 0; i < Temp.Len(); i++)
	{
		const int32* Value = HexMap.Find(Temp[i]);
		if (Value)
		{
			Total += FMath::Pow(16.f, i) * (*Value);
		}
		else
		{
			UE_LOG(LogUtilityModule, Warning, TEXT("Invalid Hex value"));
		}
	}
	return Total;
}

FColor UAtkMiscFunctionLibrary::ConvertHexStringToRGB(const FString& Color)
{
	if (Color.StartsWith(FString("#")))
	{
		const FString noPrefix = Color.RightChop(1);
		const FString r = noPrefix.LeftChop(6);
		const FString g = noPrefix.Mid(2, 2);
		const FString b = noPrefix.Mid(4, 2);
		const FString alpha = noPrefix.RightChop(6);

		FColor ColorValue;
		ColorValue.R = HexToDecimal(r);
		ColorValue.G = HexToDecimal(g);
		ColorValue.B = HexToDecimal(b);
		ColorValue.A = HexToDecimal(alpha);

		return ColorValue;
	}

	UE_LOG(LogUtilityModule, Error, TEXT("Color is not hexadecimal format"));
	return FColor();
}

