#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LocalServerConnectButton.generated.h"

class UButton;
class UTextBlock;
class UBorder;

/**
 * Custom Main Menu Button Widget for Direct LAN / Listen Server Connection.
 * Supports Gamepad D-Pad navigation, Slate focus trapping, and SML configuration.
 */
UCLASS(BlueprintType, Blueprintable)
class LOCALSERVERCONNECT_API ULocalServerConnectButton : public UUserWidget
{
    GENERATED_BODY()

public:
    ULocalServerConnectButton(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
    virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;

    /** Execute console command 'open <IP>' */
    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    void ExecuteServerConnect();

    /** Manually set target IP at runtime */
    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    void SetTargetIP(const FString& InIP);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalServerConnect")
    FText ButtonText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalServerConnect")
    FString TargetIP;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "LocalServerConnect")
    TObjectPtr<UButton> btn_Connect;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "LocalServerConnect")
    TObjectPtr<UTextBlock> txt_Label;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "LocalServerConnect")
    TObjectPtr<UBorder> FocusHighlightBorder;

private:
    UFUNCTION()
    void HandleButtonClicked();
};
