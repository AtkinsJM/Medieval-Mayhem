// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InputComponent.h"
#include "MainCharacterInputComponent.generated.h"

/**
 * 
 */

UCLASS()
class MEDIEVALMAYHEM_API UMainCharacterInputComponent : public UInputComponent
{
	GENERATED_UCLASS_BODY()

public:
	template<class T>
	FInputActionBinding& BindActionWithParam(const FName ActionName, const EInputEvent KeyEvent, UObject* Object, const FName FunctionName, T Param)
	{
		FInputActionBinding ActionBinding(ActionName, KeyEvent);

		FInputActionHandlerSignature ActionHandler;
		ActionHandler.BindUFunction(Object, FunctionName, Param);
		ActionBinding.ActionDelegate = ActionHandler;
		return AddActionBinding(ActionBinding);
	}
};
