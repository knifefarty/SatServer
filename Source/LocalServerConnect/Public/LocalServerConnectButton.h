#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LocalServerConnectButton.generated.h"

class UButton;
class UTextBlock;

/**
 * Custom Main Menu Button Widget for Direct LAN / Listen Server Connection.
 * Integrates directly with Satisfactory's FrontEnd Button styling and Slate animations.
 */
UCLASS(BlueprintType, Blueprintable)
class LOCALSERVERCONNECT_API ULocalServerConnectButton : public UUserWidget
{
    GENERATED_BODY()

public:
    ULocalServerConnectButton(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;

    /** Execute console command 'open <IP>' */
    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    void ExecuteServerConnect();

    /** Manually set target IP at runtime */
    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    void SetTargetIP(const FString& InIP);

    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    void HandleButtonClicked();

    /** Static helper to connect from any context */
    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    static void ConnectToConfiguredServer(UObject* WorldContextObject);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalServerConnect")
    FText ButtonText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalServerConnect")
    FString TargetIP;

    UPROPERTY(BlueprintReadOnly, Category = "LocalServerConnect")
    TObjectPtr<UUserWidget> ChildFrontEndButton;
};
