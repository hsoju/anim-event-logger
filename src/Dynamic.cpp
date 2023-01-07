#include "Dynamic.h"
#include "Events.h"

namespace Anim
{
	void Anim::Dynamic::SetDynamicActor(RE::Actor* a_player) {
		auto a_events = Anim::Events::GetSingleton();
		if (a_events->tdmLoaded) {
			auto selected = a_events->tdmInterface->GetCurrentTarget();
			if (selected) {
				a_events->dynamicActor = selected.get().get();
			} else {
				a_events->dynamicActor = a_player;
			}
		} else {
			a_events->dynamicActor = a_player;
		}
	}

	void GenerateDynamicSpell() {

	}

	void CopyDynamicSpell() {
		std::string okay("alright");
		OpenClipboard(nullptr);
		EmptyClipboard();
		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, okay.size() + 1);
		if (!hg) {
			CloseClipboard();
		} else {
			memcpy(GlobalLock(hg), okay.c_str(), okay.size() + 1);
			GlobalUnlock(hg);
			SetClipboardData(CF_TEXT, hg);
			CloseClipboard();
			GlobalFree(hg);
		}
	}

	void Anim::Dynamic::SetDynamicSpell(RE::PlayerCharacter* a_player, bool a_leftHand) {
		auto a_events = Anim::Events::GetSingleton();
		const auto c_log = RE::ConsoleLog::GetSingleton();
		auto spell = a_player->GetEquippedObject(a_leftHand);
		if (spell && spell->As<RE::MagicItem>()) {
			a_events->dynamicSpell = spell->As<RE::MagicItem>();
			if (c_log) {
				c_log->Print("%s registered to %s", a_events->dynamicSpell->GetName(), a_events->dynamicEvent.c_str());
			}
			logger::info("{} registered to {}", a_events->dynamicSpell->GetName(), a_events->dynamicEvent.c_str());
			
		} else {
			if (c_log) {
				c_log->Print("Unable to register spell to %s", a_events->dynamicEvent.c_str());
			}
			logger::info("Unable to register spell to {}", a_events->dynamicEvent.c_str());
		}
	}

	void Anim::Dynamic::SetDynamicEvent(RE::SCRIPT_FUNCTION::StringChunk* a_param, std::string_view a_param_parsed) {
		const auto a_events = Anim::Events::GetSingleton();
		const auto c_log = RE::ConsoleLog::GetSingleton();

		a_events->useDynamic = true;
		a_events->dynamicEvent = RE::BSFixedString(a_param_parsed);

		auto playerChar = RE::PlayerCharacter::GetSingleton();
		SetDynamicActor(playerChar->As<RE::Actor>());
		
		auto hand = a_param->GetNext()->AsString();
		auto handParsed = hand->GetString();
		
		if (handParsed == "L") {
			SetDynamicSpell(playerChar, true);
		} else {
			if (handParsed == "R") {
				SetDynamicSpell(playerChar, false);
			} else {
				a_events->useAlternateSpell = true;
				if (c_log) {
					c_log->Print("Alternate spell registered to %s", a_events->dynamicEvent.c_str());
				}
				logger::info("Alternate spell registered to {}", a_events->dynamicEvent.c_str());
			}
		}
	}
}
