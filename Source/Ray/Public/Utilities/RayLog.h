#pragma once

#include "CoreMinimal.h"


RAY_API DECLARE_LOG_CATEGORY_EXTERN(A_RAYGAMELOG, Log, All); //추가

#define LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

#define LOG_S(Verbosity) UE_LOG(A_RAYGAMELOG, Verbosity, TEXT("%s"), *LOG_CALLINFO)

#define LOG(Verbosity, Format, ...) UE_LOG(A_RAYGAMELOG, Verbosity, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define RAY_LOG(Verbosity, Format, ...) UE_LOG(A_RAYGAMELOG, Verbosity, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define RAY_LOG_WARNING(Format, ...) UE_LOG(A_RAYGAMELOG, Warning, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define RAY_LOG_ERROR(Format, ...) UE_LOG(A_RAYGAMELOG, Error, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define	RAY_LOG_SCREEN(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(Format, ##__VA_ARGS__))
#define	RAY_LOG_SCREEN_T(Time, Format, ...) GEngine->AddOnScreenDebugMessage(-1, Time, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

#define RAY_LOG_SCREEN_ERROR(Format, ...) \
{ \
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(Format, ##__VA_ARGS__)); \
RAY_LOG_ERROR(Format, ##__VA_ARGS__); \
}


#define RAY_CHECK(Expr, ...) {if(!(Expr)) {LOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}
#define RAY_IF_CHECK(Expr, ErrorMsg, ...) {if(!(Expr)) {RAY_LOG_SCREEN_ERROR(ErrorMsg); return __VA_ARGS__;}}
