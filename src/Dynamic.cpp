#include <algorithm>
#include <sstream>

#include <toml++/toml.h>

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

	void Anim::Dynamic::GenerateNode(std::stringstream& a_stream, const char* a_key, const char* a_val, bool use_quotes, bool use_brackets) {
		a_stream << a_key << " = ";
		if (!use_quotes && !use_brackets) {
			a_stream << a_val;
		} else {
			if (use_quotes) {
				a_stream << "\"" << a_val << "\"";
			} else {
				if (use_brackets) {
					a_stream << "[" << a_val << "]";
				}
			}
		}
		a_stream << "\n";
	}

	const std::string Anim::Dynamic::GenerateDynamicSpell(const char* a_event, const char* a_spellID, const char* a_spell_mod) {
		auto config = toml::table();
		std::stringstream output;
		output << "[[event]]\n";
		GenerateNode(output, "AnimationEvent", a_event, true);
		GenerateNode(output, "HasRaceFormID", "-1");
		GenerateNode(output, "RaceEspName", "", true);
		GenerateNode(output, "HasActorFormID", "-1");
		GenerateNode(output, "ActorEspName", "", true);
		GenerateNode(output, "IsEquippedRightFormID", "-1");
		GenerateNode(output, "IsEquippedLeftFormID", "-1");
		GenerateNode(output, "HasWeaponType", "-1");
		GenerateNode(output, "WeaponEspName", "", true);
		GenerateNode(output, "HasEffectFormID", "-1");
		GenerateNode(output, "EffectEspName", "", true);
		GenerateNode(output, "HasKeywordFormID", "-1");
		GenerateNode(output, "KeywordEspName", "", true);
		if (a_spellID != nullptr) {
			GenerateNode(output, "SpellFormIDs", a_spellID, false, true);
		} else {
			GenerateNode(output, "SpellFormIDs", "", false, true);
		}
		if (a_spell_mod != nullptr) {
			GenerateNode(output, "SpellEspName", a_spell_mod, true);
		} else {
			GenerateNode(output, "SpellEspName", "Skyrim.esm", true);
		}
		GenerateNode(output, "TargetCaster", "false");
		GenerateNode(output, "HealthCost", "0.00");
		GenerateNode(output, "StaminaCost", "0.00");
		GenerateNode(output, "MagickaCost", "0.00");
		const std::string result = output.str();
		output.clear();
		return result;
	}

	void Anim::Dynamic::CopyDynamicSpell(const std::string& a_toml) {
		OpenClipboard(nullptr);
		EmptyClipboard();
		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, a_toml.size() + 1);
		if (!hg) {
			CloseClipboard();
		} else {
			memcpy(GlobalLock(hg), a_toml.c_str(), a_toml.size() + 1);
			GlobalUnlock(hg);
			SetClipboardData(CF_TEXT, hg);
			CloseClipboard();
			GlobalFree(hg);
		}
	}

	void Anim::Dynamic::SetDynamicSpell(RE::PlayerCharacter* a_player, bool a_leftHand) {
		auto a_events = Anim::Events::GetSingleton();
		const char* eventName = a_events->dynamicEvent.c_str();
		
		const auto  c_log = RE::ConsoleLog::GetSingleton();

		auto spell = a_player->GetEquippedObject(a_leftHand);
		if (spell && spell->As<RE::MagicItem>()) {
			a_events->dynamicSpell = spell->As<RE::MagicItem>();
			const char* spellName = a_events->dynamicSpell->GetName();
			std::string formID = fmt::format("{:x}", a_events->dynamicSpell->GetLocalFormID());
			std::transform(formID.begin(), formID.end(), formID.begin(), [](unsigned char c) { return std::toupper(c); });
			formID.insert(0, "0x");
			const char* spellID = formID.c_str();
			const char* spellMod = a_events->dynamicSpell->GetDescriptionOwnerFile()->GetFilename().data();
			if (c_log) {
				c_log->Print("%s registered to %s", spellName, eventName);
			}
			logger::info("{} registered to {}", spellName, eventName);
			CopyDynamicSpell(GenerateDynamicSpell(eventName, spellID, spellMod));
		} else {
			if (c_log) {
				c_log->Print("Unable to register spell to %s", eventName);
			}
			logger::info("Unable to register spell to {}", eventName);
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
