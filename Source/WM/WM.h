#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(WMLOG, Log, All)

#define APPINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

#define PRINT_LOG(format, ...) UE_LOG(LogTemp, Warning, TEXT("%s %s"), *APPINFO, *FString::Printf(format, ##__VA_ARGS__))