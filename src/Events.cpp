#include <SimpleIni.h>
#include "Events.h"

void Anim::Events::AddEventSink() {
	REL::Relocation<uintptr_t> PCProcessAnimGraphEventVtbl{ RE::VTABLE_PlayerCharacter[2] };
	REL::Relocation<uintptr_t> NPCProcessAnimGraphEventVtbl{ RE::VTABLE_Character[2] };
	_PCProcessEvent = PCProcessAnimGraphEventVtbl.write_vfunc(0x1, &PCProcessEvent);
	_NPCProcessEvent = NPCProcessAnimGraphEventVtbl.write_vfunc(0x1, &NPCProcessEvent);
}

void Anim::Events::PrintEvent(const char* holder, const char* a_event) {
	const auto log = RE::ConsoleLog::GetSingleton();
	if (log) {
		log->Print(a_event);
	}
	logger::info("{}", a_event);
}

void Anim::Events::CheckDynamicEvent(RE::BSAnimationGraphEvent& a_event) {
	if (useDynamic && a_event.tag == dynamicEvent) {
		RE::Actor* const actor = const_cast<RE::Actor*>(a_event.holder->As<RE::Actor>());
		if (actor == dynamicActor) {
			auto caster = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
			if (caster) {
				logger::info("1");
				if (!useAlternateSpell) {
					logger::info("2");
					caster->CastSpellImmediate(dynamicSpell, false, nullptr, 1.0f, false, 0.0f, actor);
				} else {
					logger::info("3");
					auto left = actor->GetEquippedObject(true)->As<RE::MagicItem>();
					if (left) {
						logger::info("4");
						caster->CastSpellImmediate(left, false, nullptr, 1.0f, false, 0.0f, actor);
					} else {
						logger::info("5");
						auto right = actor->GetEquippedObject(false)->As<RE::MagicItem>();
						if (right) {
							logger::info("6");
							caster->CastSpellImmediate(right, false, nullptr, 1.0f, false, 0.0f, actor);
						}
					}
				}
			}
		}
	}
}

RE::BSEventNotifyControl Anim::Events::ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event,
	RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src) {
	if (enabled) {
		if (a_event.tag != NULL && a_event.holder != NULL) {
			if (a_event.holder->IsPlayerRef()) {
				if (tdmLoaded) {
					auto selected = tdmInterface->GetCurrentTarget();
					if (!selected) {
						 //PrintEvent("Player", a_event.tag.c_str());
					}
				} else {
					PrintEvent("Player", a_event.tag.c_str());
				}
			} else {
				auto actor = a_event.holder->As<RE::Actor>();
				if (trackMount && actor->IsBeingRidden()) {
					PrintEvent(actor->GetName(), a_event.tag.c_str());
					return RE::BSEventNotifyControl::kContinue;
				}
				if (tdmLoaded) {
					auto selected = tdmInterface->GetCurrentTarget();
					if (selected) {
						if (selected != lastSelected) {
							lastSelected = selected;
							selectedActor = selected.get().get();
						}
						if (selectedActor && selectedActor == actor) {
							PrintEvent(actor->GetName(), a_event.tag.c_str());
						}
					}
				}
			}
			CheckDynamicEvent(a_event);
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}

RE::BSEventNotifyControl Anim::Events::PCProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event,
	RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src) {
	ProcessEvent(a_this, a_event, a_src);
	return _PCProcessEvent(a_this, a_event, a_src);
}

RE::BSEventNotifyControl Anim::Events::NPCProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event,
	RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src) {
	ProcessEvent(a_this, a_event, a_src);
	return _NPCProcessEvent(a_this, a_event, a_src);
}

void Anim::Events::GetSettings()
{
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\AnimationEventLogger.ini");
	enabled = ini.GetBoolValue("Global", "bEnabled", false);
	showNames = ini.GetBoolValue("Global", "bShowNames", false);
	trackMount = ini.GetBoolValue("Global", "bTrackMount", false);
}
