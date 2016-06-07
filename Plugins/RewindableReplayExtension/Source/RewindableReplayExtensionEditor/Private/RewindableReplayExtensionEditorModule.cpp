#include "RewindableReplayExtensionEditorPrivatePCH.h"
#include "BaseEditorTool.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "ReplayTools"

class FRewindableReplayExtensionEditorModule : public IModuleInterface
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static void HandleTestCommandExcute();
	static bool HandleTestCommandCanExcute();

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
		FReplayCommands::Get().TestCommand,
		FExecuteAction::CreateStatic(&FRewindableReplayExtensionEditorModule::HandleTestCommandExcute),
		FCanExecuteAction::CreateStatic(&FRewindableReplayExtensionEditorModule::HandleTestCommandCanExcute)
		);

	struct Local
	{
		static void AddToolbarCommands(FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.AddToolBarButton(FReplayCommands::Get().TestCommand);
		}
	};

	TSharedRef<FExtender> ToolbarExtender(new FExtender());
	ToolbarExtender->AddToolBarExtension(
		TEXT("Game"),
		EExtensionHook::After,
		CommandList.ToSharedRef(),
		FToolBarExtensionDelegate::CreateStatic(&Local::AddToolbarCommands));

	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

void FRewindableReplayExtensionEditorModule::ShutdownModule()
{
	FReplayCommands::Unregister();
}

void FRewindableReplayExtensionEditorModule::HandleTestCommandExcute()
{
	FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Test Command Excuted!!!"), TEXT("TestCommand"));
}

bool FRewindableReplayExtensionEditorModule::HandleTestCommandCanExcute()
{
	return true;
}

#undef LOCTEXT_NAMESPACE