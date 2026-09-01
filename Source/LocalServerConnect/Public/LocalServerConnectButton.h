#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "LocalServerConnectButton.generated.h"

class UButton;
class UTextBlock;
class UEditableTextBox;
class UVerticalBox;
class UHorizontalBox;

/**
 * Custom Main Menu Button Widget for Direct LAN / Listen Server Connection.
 * Features inline IP editing and persistence right from the Main Menu.
 */
UCLASS(BlueprintType, Blueprintable)
class LOCALSERVERCONNECT_API ULocalServerConnectButton : public UUserWidget
{
    GENERATED_BODY()

public:
    ULocalServerConnectButton(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;

    /** Execute connection to the currently configured IP */
    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    void ExecuteServerConnect();

    /** Manually set target IP at runtime and persist to config */
    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    void SetTargetIP(const FString& InIP);

    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    void HandleButtonClicked();

    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    void HandleSaveClicked();

    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    void HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

    /** Static helper to connect from any context */
    UFUNCTION(BlueprintCallable, Category = "LocalServerConnect")
    static void ConnectToConfiguredServer(UObject* WorldContextObject);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalServerConnect")
    FText ButtonText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalServerConnect")
    FString TargetIP;

    UPROPERTY(BlueprintReadOnly, Category = "LocalServerConnect")
    TObjectPtr<UUserWidget> ChildFrontEndButton;

    UPROPERTY(BlueprintReadOnly, Category = "LocalServerConnect")
    TObjectPtr<UEditableTextBox> IPEditBox;

    UPROPERTY(BlueprintReadOnly, Category = "LocalServerConnect")
    TObjectPtr<UButton> SaveButton;

    UPROPERTY(BlueprintReadOnly, Category = "LocalServerConnect")
    TObjectPtr<UTextBlock> SaveButtonText;
};
