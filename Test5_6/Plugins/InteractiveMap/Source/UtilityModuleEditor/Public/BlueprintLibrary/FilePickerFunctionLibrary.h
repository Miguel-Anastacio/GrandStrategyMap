// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FilePickerFunctionLibrary.generated.h"

UCLASS()
class UTILITYMODULEEDITOR_API UAtkFilePickerFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
	
public:
	/*
	 * Opens a file dialog for the specified data. Leave FileTypes empty to be able to select any files.
	 * Filetypes must be in the format of: <File type Description>|*.<actual extension>
	 * You can combine multiple extensions by placing ";" between them
	 * For example: Text Files|*.txt|Excel files|*.csv|Image Files|*.png;*.jpg;*.bmp will display 3 lines for 3 different type of files.
	 */
	UFUNCTION(BlueprintCallable, Category = "FilePicker")
	static void OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames);

	UFUNCTION(BlueprintCallable, Category = "FilePicker")
	static void OpenFileDialogJson(const FString& DefaultPath, TArray<FString>& OutFileNames);

	UFUNCTION(BlueprintCallable, Category = "FilePicker")
	static void OpenDirectoryDialog(const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName);
};

