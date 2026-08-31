#pragma once

#include "CoreMinimal.h"
#include "Configuration/ModConfiguration.h"
#include "LocalServerConnectConfig.generated.h"

/**
 * Configuration structure for Local Server Connect
 */
USTRUCT(BlueprintType)
struct FLocalServerConnectConfigStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server Connection")
    FString ServerIP = TEXT("192.168.1.89");
};

/**
 * Mod Configuration Class exposed to SML and in-game Mod Settings
 */
UCLASS(BlueprintType)
class LOCALSERVERCONNECT_API ULocalServerConnectConfig : public UModConfiguration
{
    GENERATED_BODY()

public:
    ULocalServerConnectConfig();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Server Connection")
    FString ServerIP = TEXT("192.168.1.89");

    /** Helper to retrieve the configured Server IP directly */
    UFUNCTION(BlueprintPure, Category = "LocalServerConnect")
    static FString GetConfiguredServerIP();
};
