#include "RewindableReplayExtensionEditorPrivatePCH.h"
#include "ReplayStyle.h"
#include "SlateStyle.h"
#include "IPluginManager.h"

#define IMAGE_PLUGIN_BRUSH( RelativePath, ... ) FSlateImageBrush( FReplayStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )
#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

FString FReplayStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("RewindableReplayExtension"))->GetContentDir();
	return (ContentDir / RelativePath) + Extension;
}

void FReplayStyle::Initialize()
{
	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());

	const FVector2D size(860.0f, 150.0f);
	const FVector2D sliderSize(500.0f, 12.0f);
	const FVector2D icon128(80.0f, 80.0f);
	const FVector2D icon40(40.0f, 40.0f);

	{
		StyleSet->Set("UIBorder", new IMAGE_BRUSH(TEXT("Icons/BlankIcon"), size));
		StyleSet->Set("PlayButton", new IMAGE_BRUSH(TEXT("Icons/icon_playInSelectedViewport_40x"), icon128));
		StyleSet->Set("PauseButton", new IMAGE_BRUSH(TEXT("Icons/icon_pause_40x"), icon128));
		StyleSet->Set("BeginButton", new IMAGE_BRUSH(TEXT("Icons/icon_step_back_40x"), icon128));
		StyleSet->Set("EndButton", new IMAGE_BRUSH(TEXT("Icons/icon_step_40x"), icon128));
		StyleSet->Set("RewindButton", new IMAGE_BRUSH(TEXT("Icons/icon_Cascade_LowestLOD_40x"), icon128));
		StyleSet->Set("FastForwardButton", new IMAGE_BRUSH(TEXT("Icons/icon_Cascade_HighestLOD_40x"), icon128));
		StyleSet->Set("RecordButton", new IMAGE_BRUSH(TEXT("Icons/icon_Persona_StartRecord_40x"), icon40));
		StyleSet->Set("ExitButton", new IMAGE_BRUSH(TEXT("Icons/icon_Cascade_DeleteLOD_40x"), icon40));
	}

	{
		StyleSet->Set("SliderStyle", FSliderStyle().SetNormalBarImage(*StyleSet->GetBrush("SliderBrush")));
	}
}

void FReplayStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}

TSharedPtr< class FSlateStyleSet > FReplayStyle::StyleSet = nullptr;

TSharedPtr< class ISlateStyle > FReplayStyle::Get()
{
	return StyleSet;
}

FName FReplayStyle::GetStyleSetName()
{
	return TEXT("ReplayStyle");
}