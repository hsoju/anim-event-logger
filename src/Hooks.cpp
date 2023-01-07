#include "Hooks.h"
#include "Commands.h"
#include "Events.h"

void Hooks::Install() {
	Anim::Commands::GetSingleton()->Register();
	Anim::Events::GetSingleton()->AddEventSink();
	logger::info("Installed all hooks.");
}
