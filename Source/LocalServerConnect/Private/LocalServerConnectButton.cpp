#include "LocalServerConnectButton.h"
#include "LocalServerConnectConfig.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/UnrealType.h"

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

    if (!WidgetTree)
    {
        return;
    }

    // Try to instantiate Satisfactory's authentic Widget_FrontEnd_Button
    UClass* FEButtonClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/FactoryGame/Interface/UI/Menu/Widget_FrontEnd_Button.Widget_FrontEnd_Button_C"));
    if (FEButtonClass)
    {
        ChildFrontEndButton = WidgetTree->ConstructWidget<UUserWidget>(FEButtonClass, TEXT("FE_LocalConnectBtn"));
        if (ChildFrontEndButton)
        {
            WidgetTree->RootWidget = ChildFrontEndButton;

            // Set button text via SetTitle function
            UFunction* SetTitleFunc = ChildFrontEndButton->FindFunction(FName(TEXT("SetTitle")));
            if (SetTitleFunc)
            {
                struct FSetTitleParams
                {
                    FText Title;
                };
                FSetTitleParams Params;
                Params.Title = ButtonText;
                ChildFrontEndButton->ProcessEvent(SetTitleFunc, &Params);
            }

            // Set IsBigButton to true for main menu primary button styling
            UFunction* SetBigFunc = ChildFrontEndButton->FindFunction(FName(TEXT("SetIsBigButton")));
            if (SetBigFunc)
            {
                struct FSetBigParams
                {
                    bool bIsBig;
                };
                FSetBigParams Params;
                Params.bIsBig = true;
                ChildFrontEndButton->ProcessEvent(SetBigFunc, &Params);
            }

            // Hook click event via Unreal multicast delegate property reflection
            if (FMulticastDelegateProperty* DelegateProp = CastField<FMulticastDelegateProperty>(ChildFrontEndButton->GetClass()->FindPropertyByName(FName(TEXT("OnClicked")))))
            {
                FScriptDelegate Delegate;
                Delegate.BindUFunction(this, FName(TEXT("HandleButtonClicked")));
                DelegateProp->AddDelegate(Delegate, ChildFrontEndButton);
            }
            return;
        }
    }

    // Fallback: Construct standard UButton + UTextBlock
    UButton* FallbackBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("FallbackBtn"));
    UTextBlock* FallbackTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FallbackTxt"));

    if (FallbackTxt)
    {
        FallbackTxt->SetText(ButtonText);
        FSlateFontInfo FontInfo = FCoreStyle::GetDefaultFontStyle("Bold", 20);
        FallbackTxt->SetFont(FontInfo);
        FallbackTxt->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    }

    if (FallbackBtn)
    {
        if (FallbackTxt)
        {
            FallbackBtn->AddChild(FallbackTxt);
        }
        FallbackBtn->OnClicked.AddDynamic(this, &ULocalServerConnectButton::HandleButtonClicked);
        WidgetTree->RootWidget = FallbackBtn;
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
