#include "Commands.h"
#include "Events.h"

namespace Anim
{
	void Anim::Commands::SetDynamicActor(RE::Actor* a_player) {
		auto evt = Anim::Events::GetSingleton();
		if (evt->tdmLoaded) {
			auto selected = evt->tdmInterface->GetCurrentTarget();
			if (selected) {
				evt->dynamicActor = selected.get().get();
			} else {
				evt->dynamicActor = a_player;
			}
		} else {
			evt->dynamicActor = a_player;
		}
	}

	void Anim::Commands::SetDynamicSpell(RE::PlayerCharacter* a_player, bool a_leftHand) {
		auto evt = Anim::Events::GetSingleton();
		auto spell = a_player->GetEquippedObject(a_leftHand)->As<RE::MagicItem>();
		if (spell) {
			evt->dynamicSpell = spell;
			logger::info("{} attached to {}", evt->dynamicSpell->GetName(), evt->dynamicEvent.c_str());
		} else {
			logger::info("Unable to attach spell to {}", evt->dynamicEvent.c_str());
		}
	}


	bool Anim::Commands::Exec(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR*, RE::Script*, 
		RE::ScriptLocals*, double&, std::uint32_t&)
	{
		auto evt = Anim::Events::GetSingleton();
		auto param = a_scriptData->GetStringChunk();
		auto paramParsed = param->GetString();
		if (paramParsed == "On") {
			evt->enabled = true;
		} else {
			if (paramParsed == "Off") {
				evt->enabled = false;
				evt->useDynamic = false;
			} else {
				evt->useDynamic = true;
				evt->dynamicEvent = RE::BSFixedString(paramParsed);
				auto playerChar = RE::PlayerCharacter::GetSingleton();
				SetDynamicActor(playerChar->As<RE::Actor>());
				auto hand = param->GetNext()->AsString();
				auto handParsed = hand->GetString();
				if (handParsed == "L") {
					SetDynamicSpell(playerChar, true);
				} else {
					if (handParsed == "R") {
						SetDynamicSpell(playerChar, false);
					} else {
						evt->useAlternateSpell = true;
					}
				}
			}
		}
		return true;
	}

	void Anim::Commands::Register()
	{
		using Type = RE::SCRIPT_PARAM_TYPE;

		auto info = RE::SCRIPT_FUNCTION::LocateConsoleCommand("RecvAnimEvent");  // Unused
		if (info) {
			info->referenceFunction = false;
			info->executeFunction = Exec;
			info->conditionFunction = nullptr;
			info->editorFilter = false;
			info->invalidatesCellList = false;
			logger::debug("Replaced RecvAnimEvent Console Command Successfully!");
		} else {
			logger::error("Failed to Locate RecvAnimEvent Console Command!");
		}
	}

}
