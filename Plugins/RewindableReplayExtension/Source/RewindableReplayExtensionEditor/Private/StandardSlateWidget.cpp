#include "RewindableReplayExtensionEditorPrivatePCH.h"
#include "ReplayStyle.h"
#include "ReplayManager.h"
#include "StandardSlateWidget.h"

#define LOCTEXT_NAMESPACE "ReplayTools"

void SStandardSlateWidget::Construct(const FArguments& InArgs)
{
	//OwnerHUD = InArgs._OwnerHUD;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////If the code below doesn't look like C++ to you it's because it (sort-of) isn't,
	/////Slate makes extensive use of the C++ Prerocessor(macros) and operator overloading,
	/////Epic is trying to make our lives easier, look-up the macro/operator definitions to see why.

	struct Local
	{
		static void ValueChanged(float rate)
		{
			AReplayManager::GetInstance()->Scrub(rate < 0.0f);
		}
	};

	ChildSlot
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SOverlay)
			+ SOverlay::Slot().VAlign(VAlign_Bottom).HAlign(HAlign_Center)
			[
				SNew(SImage).Image(FReplayStyle::Get()->GetBrush("UIBorder"))
			]
			+ SOverlay::Slot().VAlign(VAlign_Bottom).HAlign(HAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(FMargin(20.0f, 0.0f, 20.0f, 0.0f))
					[
						SNew(SButton).ButtonColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
						.OnClicked_Lambda([]()->FReply {AReplayManager::GetInstance()->Begin(); return FReply::Handled(); })
						[
							SNew(SImage).Image(FReplayStyle::Get()->GetBrush("BeginButton"))
						]
					]
					/*+ SHorizontalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(FMargin(20.0f, 0.0f, 20.0f, 0.0f))
					[
						SNew(SButton).ButtonColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
						[
							SNew(SImage).Image(FReplayStyle::Get()->GetBrush("PauseButton"))
						]
					]*/
					+ SHorizontalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(FMargin(60.0f, 0.0f, 60.0f, 0.0f))
					[
						SNew(SButton).ButtonColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
						.OnClicked_Lambda([]()->FReply {AReplayManager::GetInstance()->TogglePlayback(); return FReply::Handled(); })
						[
							SAssignNew(PlayButtonImage, SImage).Image(FReplayStyle::Get()->GetBrush("PlayButton"))
						]
					]
					+ SHorizontalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(FMargin(20.0f, 0.0f, 20.0f, 0.0f))
					[
						SNew(SButton).ButtonColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
						.OnClicked_Lambda([]()->FReply {AReplayManager::GetInstance()->End(); return FReply::Handled(); })
						[
							SNew(SImage).Image(FReplayStyle::Get()->GetBrush("EndButton"))
						]
					]
				]
				+ SVerticalBox::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left)
				[
					SNew(SBox).WidthOverride(500.0f).Padding(FMargin(15.0f, 0.0f, 0.0f, 20.0f))
					[
						SAssignNew(Scrubber, SSlider).SliderBarColor(FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)))
						.OnValueChanged_Static(&Local::ValueChanged)
					]
				]
			]
		];
}

TSharedPtr<SSlider> SStandardSlateWidget::GetScrubber()
{
	return Scrubber;
}

TSharedPtr<SImage> SStandardSlateWidget::GetPlayButtonImage()
{
	return PlayButtonImage;
}

#undef LOCTEXT_NAMESPACE