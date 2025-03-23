// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TemplatedArrayWrapper.h"
#include "ManagerObjectsArray.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArrayChangedSignature, UObject*, Object);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArraySetSignature, const TArray<UObject*>&, Array);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArrayClearedSignature);
UCLASS(BlueprintType, Blueprintable,DisplayName=ManagerObjectsArray)
class UTILITYMODULE_API UTkManagerObjectsArray : public UObject
{
	GENERATED_BODY()

public:
	UTkManagerObjectsArray(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitProperties() override;

	// BP exposed methods to call TArrayWrapper methods
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=Add)
	void Add_BP(UObject* Object);
	
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=AddMultiple)
	void AddMultiple_BP(const TArray<UObject*>& Multiple);
	
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=Remove)
	void Remove_BP(UObject* Object);
	
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=Clear)
	void Clear_BP();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ManagerObjectsArray, DisplayName=GetArray)
	TArray<UObject*> GetArray_BP() const;
	
	UFUNCTION(BlueprintCallable, Category=ManagerObjectsArray, DisplayName=SetArray)
	void SetArray_BP(const TArray<UObject*>& NewObjects);

	UPROPERTY(BlueprintAssignable, Category=ManagerObjectsArray)
	FOnArrayChangedSignature OnObjectAdded;
	
	UPROPERTY(BlueprintAssignable, Category=ManagerObjectsArray)
	FOnArrayChangedSignature OnObjectRemoved;

	UPROPERTY(BlueprintAssignable, Category=ManagerObjectsArray)
	FOnArraySetSignature OnArraySet;

	UPROPERTY(BlueprintAssignable, Category=ManagerObjectsArray)
	FOnArrayClearedSignature OnArrayCleared;
	
protected:
	TArrayWrapper<UObject*, FOnArrayChangedSignature, FOnArraySetSignature, FOnArrayClearedSignature> ArrayWrapper;
};
