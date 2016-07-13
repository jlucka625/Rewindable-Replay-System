#include "RewindableReplayExtensionEditorPrivatePCH.h"
#include "BaseEditorTool.h"
#include "ReplayTool.h"
#include "ReplayStyle.h"
#include "ReplayManager.h"
#include "PropertyEditorModule.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "ReplayTools"

class FRewindableReplayExtensionEditorModule : public IModuleInterface
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static void HandleTestCommandExcute();
	static bool HandleTestCommandCanExcute();

	static void OnToolWindowClosed(const TSharedRef<SWindow>& Window, UBaseEditorTool* Instance);

	TSharedPtr<FUICommandList> CommandList;
};

IMPLEMENT_MODULE(FRewindableReplayExtensionEditorModule, RewindableReplayExtension)

void FRewindableReplayExtensionEditorModule::StartupModule()
{
	FReplayCommands::Register();
	CommandList = MakeShareable(new FUICommandList);

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

	CommandList->Append(LevelEditorModule.GetGlobalLevelEditorActions());

	CommandList->MapAction(
		FReplayCommands::Get().ReplayCommand,
		FExecuteAction::CreateStatic(&FRewindableReplayExtensionEditorModule::HandleTestCommandExcute),
		FCanExecuteAction::CreateStatic(&FRewindableReplayExtensionEditorModule::HandleTestCommandCanExcute)
		);

	struct Local
	{
		static void AddToolbarCommands(FToolBarBuilder& ToolbarBuilder)
		{
			FSlateIcon icon = FSlateIcon(FReplayStyle::GetStyleSetName(), "RecordButton");
			ToolbarBuilder.AddToolBarButton(FReplayCommands::Get().ReplayCommand, NAME_None, TAttribute<FText>(), TAttribute<FText>(), icon);
		}
	};

	TSharedRef<FExtender> ToolbarExtender(new FExtender());
	ToolbarExtender->AddToolBarExtension(
		TEXT("Game"),
		EExtensionHook::After,
		CommandList.ToSharedRef(),
		FToolBarExtensionDelegate::CreateStatic(&Local::AddToolbarCommands));

	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);

	FReplayStyle::Initialize();
}

void FRewindableReplayExtensionEditorModule::ShutdownModule()
{
	FReplayCommands::Unregister();
	FReplayStyle::Shutdown();
}

void FRewindableReplayExtensionEditorModule::HandleTestCommandExcute()
{
	AReplayManager::GetInstance()->ExecuteReplaySystem();
}

bool FRewindableReplayExtensionEditorModule::HandleTestCommandCanExcute()
{
	return true;
}

void FRewindableReplayExtensionEditorModule::OnToolWindowClosed(const TSharedRef<SWindow>& Window, UBaseEditorTool* Instance)
{
	Instance->RemoveFromRoot();
}

#undef LOCTEXT_NAMESPACE