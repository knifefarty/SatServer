#include "LocalServerConnectButton.h"
#include "LocalServerConnectConfig.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "UObject/UnrealType.h"

ULocalServerConnectButton::ULocalServerConnectButton(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ButtonText = FText::FromString(TEXT("Connect to Local Host"));
    TargetIP = TEXT("192.168.1.89");
    SetIsFocusable(true);
}

TSharedRef<SWidget> ULocalServerConnectButton::RebuildWidget()
{
    if (!WidgetTree)
    {
        WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
    }

    FString ConfiguredIP = ULocalServerConnectConfig::GetConfiguredServerIP();
    if (!ConfiguredIP.IsEmpty())
    {
        TargetIP = ConfiguredIP;
    }

    if (WidgetTree && !WidgetTree->RootWidget)
    {
        UVerticalBox* RootVBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootVBox"));
        WidgetTree->RootWidget = RootVBox;

        // 1. Construct authentic FrontEnd button
        UClass* FEButtonClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/FactoryGame/Interface/UI/Menu/Widget_FrontEnd_Button.Widget_FrontEnd_Button_C"));
        if (FEButtonClass)
        {
            ChildFrontEndButton = WidgetTree->ConstructWidget<UUserWidget>(FEButtonClass, TEXT("FE_LocalConnectBtn"));
            if (ChildFrontEndButton)
            {
                UFunction* SetTitleFunc = ChildFrontEndButton->FindFunction(FName(TEXT("SetTitle")));
                if (SetTitleFunc)
                {
                    struct FSetTitleParams { FText Title; };
                    FSetTitleParams Params;
                    Params.Title = ButtonText;
                    ChildFrontEndButton->ProcessEvent(SetTitleFunc, &Params);
                }

                UFunction* SetBigFunc = ChildFrontEndButton->FindFunction(FName(TEXT("SetIsBigButton")));
                if (SetBigFunc)
                {
                    struct FSetBigParams { bool bIsBig; };
                    FSetBigParams Params;
                    Params.bIsBig = true;
                    ChildFrontEndButton->ProcessEvent(SetBigFunc, &Params);
                }

                UVerticalBoxSlot* BtnSlot = RootVBox->AddChildToVerticalBox(ChildFrontEndButton);
                if (BtnSlot)
                {
                    BtnSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 2.0f));
                }
            }
        }

        // Fallback main button if FrontEnd button not found
        if (!ChildFrontEndButton)
        {
            UButton* FallbackBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("FallbackBtn"));
            UTextBlock* FallbackTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FallbackTxt"));
            if (FallbackTxt)
            {
                FallbackTxt->SetText(ButtonText);
                FallbackTxt->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 20));
                FallbackTxt->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            }
            if (FallbackBtn)
            {
                if (FallbackTxt) FallbackBtn->AddChild(FallbackTxt);
                RootVBox->AddChildToVerticalBox(FallbackBtn);
            }
        }

        // 2. Construct sleek Inline IP Editor row
        UHorizontalBox* IPRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("IPRow"));
        if (IPRow)
        {
            // Label
            UTextBlock* IPLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("IPLabel"));
            if (IPLabel)
            {
                IPLabel->SetText(FText::FromString(TEXT("IP: ")));
                IPLabel->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 12));
                IPLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.5f, 0.1f, 1.0f))); // FICSIT Orange
                UHorizontalBoxSlot* LabelSlot = IPRow->AddChildToHorizontalBox(IPLabel);
                if (LabelSlot)
                {
                    LabelSlot->SetVerticalAlignment(VAlign_Center);
                    LabelSlot->SetPadding(FMargin(4.0f, 0.0f, 4.0f, 0.0f));
                }
            }

            // Editable Text Box for IP
            IPEditBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("IPEditBox"));
            if (IPEditBox)
            {
                IPEditBox->SetText(FText::FromString(TargetIP));
                IPEditBox->SetHintText(FText::FromString(TEXT("192.168.1.89")));
                UHorizontalBoxSlot* BoxSlot = IPRow->AddChildToHorizontalBox(IPEditBox);
                if (BoxSlot)
                {
                    BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
                    BoxSlot->SetVerticalAlignment(VAlign_Center);
                    BoxSlot->SetPadding(FMargin(2.0f, 0.0f, 4.0f, 0.0f));
                }
            }

            // Save Button
            SaveButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("SaveButton"));
            SaveButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SaveButtonText"));
            if (SaveButtonText)
            {
                SaveButtonText->SetText(FText::FromString(TEXT("Save")));
                SaveButtonText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 11));
                SaveButtonText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            }
            if (SaveButton)
            {
                if (SaveButtonText) SaveButton->AddChild(SaveButtonText);
                UHorizontalBoxSlot* SaveSlot = IPRow->AddChildToHorizontalBox(SaveButton);
                if (SaveSlot)
                {
                    SaveSlot->SetVerticalAlignment(VAlign_Center);
                    SaveSlot->SetPadding(FMargin(2.0f, 0.0f, 4.0f, 0.0f));
                }
            }

            UVerticalBoxSlot* RowSlot = RootVBox->AddChildToVerticalBox(IPRow);
            if (RowSlot)
            {
                RowSlot->SetPadding(FMargin(8.0f, 2.0f, 8.0f, 6.0f));
            }
        }
    }

    return Super::RebuildWidget();
}

void ULocalServerConnectButton::NativeConstruct()
{
    Super::NativeConstruct();

    FString ConfiguredIP = ULocalServerConnectConfig::GetConfiguredServerIP();
    if (!ConfiguredIP.IsEmpty())
    {
        TargetIP = ConfiguredIP;
    }

    if (IPEditBox)
    {
        IPEditBox->SetText(FText::FromString(TargetIP));
        IPEditBox->OnTextCommitted.RemoveAll(this);
        IPEditBox->OnTextCommitted.AddDynamic(this, &ULocalServerConnectButton::HandleTextCommitted);
    }

    if (SaveButton)
    {
        SaveButton->OnClicked.RemoveAll(this);
        SaveButton->OnClicked.AddDynamic(this, &ULocalServerConnectButton::HandleSaveClicked);
    }

    if (ChildFrontEndButton)
    {
        if (FMulticastDelegateProperty* DelegateProp = CastField<FMulticastDelegateProperty>(ChildFrontEndButton->GetClass()->FindPropertyByName(FName(TEXT("OnClicked")))))
        {
            FScriptDelegate Delegate;
            Delegate.BindUFunction(this, FName(TEXT("HandleButtonClicked")));
            DelegateProp->AddDelegate(Delegate, ChildFrontEndButton);
        }
    }
}

void ULocalServerConnectButton::HandleSaveClicked()
{
    if (IPEditBox)
    {
        FString NewIP = IPEditBox->GetText().ToString().TrimStartAndEnd();
        if (!NewIP.IsEmpty())
        {
            SetTargetIP(NewIP);
            if (SaveButtonText)
            {
                SaveButtonText->SetText(FText::FromString(TEXT("Saved!")));
            }
        }
    }
}

void ULocalServerConnectButton::HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter || CommitMethod == ETextCommit::OnUserMovedFocus)
    {
        FString NewIP = Text.ToString().TrimStartAndEnd();
        if (!NewIP.IsEmpty())
        {
            SetTargetIP(NewIP);
            if (SaveButtonText)
            {
                SaveButtonText->SetText(FText::FromString(TEXT("Saved!")));
            }
        }
    }
}

void ULocalServerConnectButton::HandleButtonClicked()
{
    ExecuteServerConnect();
}

void ULocalServerConnectButton::ExecuteServerConnect()
{
    // Sync with edit box if modified right before click
    if (IPEditBox)
    {
        FString BoxIP = IPEditBox->GetText().ToString().TrimStartAndEnd();
        if (!BoxIP.IsEmpty())
        {
            TargetIP = BoxIP;
            ULocalServerConnectConfig::SaveConfiguredServerIP(TargetIP);
        }
    }

    ConnectToConfiguredServer(this);
}

void ULocalServerConnectButton::ConnectToConfiguredServer(UObject* WorldContextObject)
{
    FString EffectiveIP = ULocalServerConnectConfig::GetConfiguredServerIP();
    if (EffectiveIP.TrimStartAndEnd().IsEmpty())
    {
        EffectiveIP = TEXT("192.168.1.89");
    }

    UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
    APlayerController* PC = World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr;

    if (PC)
    {
        PC->ClientTravel(EffectiveIP, ETravelType::TRAVEL_Absolute);
    }
    else if (GEngine)
    {
        FString Command = FString::Printf(TEXT("open %s"), *EffectiveIP);
        GEngine->Exec(World, *Command);
    }
}

void ULocalServerConnectButton::SetTargetIP(const FString& InIP)
{
    TargetIP = InIP.TrimStartAndEnd();
    ULocalServerConnectConfig::SaveConfiguredServerIP(TargetIP);
}
