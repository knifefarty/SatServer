#include "LocalServerConnectButton.h"
#include "LocalServerConnectConfig.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ULocalServerConnectButton::ULocalServerConnectButton(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ButtonText = FText::FromString(TEXT("Connect to Local Host"));
    TargetIP = TEXT("192.168.1.89");
    SetIsFocusable(true);
}

void ULocalServerConnectButton::NativeConstruct()
{
    Super::NativeConstruct();

    FString ConfiguredIP = ULocalServerConnectConfig::GetConfiguredServerIP();
    if (!ConfiguredIP.IsEmpty())
    {
        TargetIP = ConfiguredIP;
    }

    // If not using a UMG asset, dynamically construct button and text in WidgetTree
    if (!btn_Connect && WidgetTree)
    {
        btn_Connect = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Dynamic_btn_Connect"));
        txt_Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Dynamic_txt_Label"));

        if (txt_Label)
        {
            txt_Label->SetText(ButtonText);
            FSlateFontInfo FontInfo = FCoreStyle::GetDefaultFontStyle("Bold", 22);
            txt_Label->SetFont(FontInfo);
            txt_Label->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            txt_Label->SetJustification(ETextJustify::Left);
        }

        if (btn_Connect)
        {
            if (txt_Label)
            {
                btn_Connect->AddChild(txt_Label);
            }
            WidgetTree->RootWidget = btn_Connect;
        }
    }

    if (btn_Connect)
    {
        btn_Connect->IsFocusable = true;
        btn_Connect->OnClicked.RemoveAll(this);
        btn_Connect->OnClicked.AddDynamic(this, &ULocalServerConnectButton::HandleButtonClicked);
    }

    if (txt_Label)
    {
        txt_Label->SetText(ButtonText);
    }

    if (FocusHighlightBorder)
    {
        FocusHighlightBorder->SetVisibility(ESlateVisibility::Hidden);
    }
}

void ULocalServerConnectButton::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
    Super::NativeOnAddedToFocusPath(InFocusEvent);

    if (FocusHighlightBorder)
    {
        FocusHighlightBorder->SetVisibility(ESlateVisibility::Visible);
    }
}

void ULocalServerConnectButton::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
    Super::NativeOnRemovedFromFocusPath(InFocusEvent);

    if (FocusHighlightBorder)
    {
        FocusHighlightBorder->SetVisibility(ESlateVisibility::Hidden);
    }
}

void ULocalServerConnectButton::HandleButtonClicked()
{
    ExecuteServerConnect();
}

void ULocalServerConnectButton::ExecuteServerConnect()
{
    ConnectToConfiguredServer(this);
}

void ULocalServerConnectButton::ConnectToConfiguredServer(UObject* WorldContextObject)
{
    FString EffectiveIP = ULocalServerConnectConfig::GetConfiguredServerIP();
    if (EffectiveIP.TrimStartAndEnd().IsEmpty())
    {
        EffectiveIP = TEXT("192.168.1.89");
    }

    FString Command = FString::Printf(TEXT("open %s"), *EffectiveIP);

    UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
    APlayerController* PC = World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr;

    if (PC && World)
    {
        UKismetSystemLibrary::ExecuteConsoleCommand(World, Command, PC);
    }
    else if (GEngine)
    {
        GEngine->Exec(World, *Command);
    }
}

void ULocalServerConnectButton::SetTargetIP(const FString& InIP)
{
    TargetIP = InIP;
}
