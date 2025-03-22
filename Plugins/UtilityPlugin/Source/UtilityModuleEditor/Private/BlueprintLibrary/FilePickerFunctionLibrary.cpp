// Copyright 2024 An@stacioDev All rights reserved.
#if WITH_EDITOR
#include "BlueprintLibrary/FilePickerFunctionLibrary.h"

#include "HAL/PlatformFileManager.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"

void UAtkFilePickerFunctionLibrary::OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames)
{
	if (GEditor)
	{
		TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindBestParentWindowForDialogs(nullptr);
		// Get the native window handle
		const void* ParentWindowHandle = nullptr;
		if (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid())
		{
			ParentWindowHandle = ParentWindow->GetNativeWindow()->GetOSWindowHandle();
		}

		// Use the DesktopPlatformModule to open the file dialog
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform)
		{
			uint32 SelectionFlag = 0; // Single file selection
			DesktopPlatform->OpenFileDialog(
				ParentWindowHandle,
				DialogTitle,
				DefaultPath,
				TEXT(""),  // Default file
				FileTypes,
				SelectionFlag,
				OutFileNames
			);
		}
	}
}

void UAtkFilePickerFunctionLibrary::OpenFileDialogJson(const FString& DefaultPath, TArray<FString>& OutFileNames)
{
	OpenFileDialog("Select Data File", DefaultPath, FString("JSon Files|*.json"), OutFileNames);
}

void UAtkFilePickerFunctionLibrary::OpenDirectoryDialog(const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName)
{
	if (GEditor)
	{
		TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindBestParentWindowForDialogs(nullptr);
		// Get the native window handle
		const void* ParentWindowHandle = nullptr;
		if (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid())
		{
			ParentWindowHandle = ParentWindow->GetNativeWindow()->GetOSWindowHandle();
		}

		// Use the DesktopPlatformModule to open the file dialog
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform)
		{
			uint32 SelectionFlag = 0; // Single file selection
			DesktopPlatform->OpenDirectoryDialog(
				ParentWindowHandle,
				DialogTitle,
				DefaultPath,
				OutFolderName
			);
		}
	}
}
#endif
