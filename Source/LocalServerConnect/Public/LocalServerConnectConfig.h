#pragma once

#include "CoreMinimal.h"
#include "Configuration/ModConfiguration.h"
#include "LocalServerConnectConfig.generated.h"

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

    /** Helper to save and persist the Server IP to disk */
    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    static void SaveConfiguredServerIP(const FString& NewIP);

    static FString GetConfigFilePath();
};
