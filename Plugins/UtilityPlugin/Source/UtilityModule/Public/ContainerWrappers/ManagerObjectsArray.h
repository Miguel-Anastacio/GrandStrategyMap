// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ManagerObjectsArray.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectChangedSignature, UObject*, Object);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArraySetSignature, TArray<UObject*>&, Array);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArrayClearedSignature);
UCLASS(BlueprintType, Blueprintable)
class UTkManagerObjectsArray : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ManagerObjectsArray)
	void Add(UObject* Object);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ManagerObjectsArray)
	void Remove(UObject* Object);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ManagerObjectsArray)
	void Clear();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ManagerObjectsArray)
	TArray<UObject*> GetObjects();
	
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray)
	void SetObjects(const TArray<UObject*>& Objects);

	UPROPERTY(BlueprintAssignable, Category=ManagerObjectsArray)
	FOnObjectChangedSignature OnObjectAdded;
	UPROPERTY(BlueprintAssignable, Category=ManagerObjectsArray)
	FOnObjectChangedSignature OnObjectRemoved;
	UPROPERTY(BlueprintAssignable, Category=ManagerObjectsArray)
	FOnArraySetSignature OnArraySet;
	UPROPERTY(BlueprintAssignable, Category=ManagerObjectsArray)
	FOnArrayClearedSignature OnArrayCleared;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn), Category=ManagerObjectsArray)
	TArray<UObject*> Objects;
	
};
