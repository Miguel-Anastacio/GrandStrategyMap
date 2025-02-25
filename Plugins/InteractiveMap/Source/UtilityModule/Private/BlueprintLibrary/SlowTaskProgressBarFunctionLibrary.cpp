// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "BlueprintLibrary/SlowTaskProgressBarFunctionLibrary.h"

#include <string_view>

#include "Misc/ScopedSlowTask.h"
#include "Misc/FeedbackContext.h"
static float CurrentProgress = 0.0f;
void USlowTaskProgressBarFunctionLibrary::ExecuteSlowTaskWithProgressBar(TFunction<void(TFunction<void(float, std::string_view)>)> Task)
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
