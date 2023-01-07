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
	if (!showNames) {
		if (log) {
			log->Print(a_event);
		}
		logger::info("{}", a_event);
	} else {
		if (log) {
			log->Print("%s: %s", holder, a_event);
		}
		logger::info("{}: {}", holder, a_event);
	}
}

void Anim::Events::CheckDynamicEvent(RE::BSAnimationGraphEvent& a_event) {
	if (useDynamic && a_event.tag == dynamicEvent) {
		RE::Actor* const actor = const_cast<RE::Actor*>(a_event.holder->As<RE::Actor>());
		if (actor == dynamicActor) {
			auto caster = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
			if (caster) {
				if (!useAlternateSpell) {
					caster->CastSpellImmediate(dynamicSpell, false, nullptr, 1.0f, false, 0.0f, actor);
				} else {
					auto left = actor->GetEquippedObject(true);
					if (left && left->As<RE::MagicItem>()) {
						caster->CastSpellImmediate(left->As<RE::MagicItem>(), false, nullptr, 1.0f, false, 0.0f, actor);
					} else {
						auto right = actor->GetEquippedObject(false);
						if (right && right->As<RE::MagicItem>()) {
							caster->CastSpellImmediate(right->As<RE::MagicItem>(), false, nullptr, 1.0f, false, 0.0f, actor);
						} else {
							const auto& favSpells = RE::MagicFavorites::GetSingleton()->spells;
							uint32_t favSize = favSpells.size();
							if (favSize > 0) {
								auto spell = favSpells[favoriteIdx % favSize]->As<RE::MagicItem>();
								caster->CastSpellImmediate(spell, false, nullptr, 1.0f, false, 0.0f, actor);
								favoriteIdx += 1;
							}
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
						 PrintEvent("Player", a_event.tag.c_str());
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
	enabled = ini.GetBoolValue("Global", "bAutoEnabled", false);
	showNames = ini.GetBoolValue("Global", "bShowNames", false);
	trackMount = ini.GetBoolValue("Global", "bTrackMount", false);
}
