#include "Commands.h"
#include "Events.h"
#include "Dynamic.h"

namespace Anim
{
	bool Anim::Commands::Exec(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR*, RE::Script*, 
		RE::ScriptLocals*, double&, std::uint32_t&)
	{
		const auto a_events = Anim::Events::GetSingleton();
		const auto c_log = RE::ConsoleLog::GetSingleton();

		auto param = a_scriptData->GetStringChunk();
		auto paramParsed = param->GetString();
		
		if (paramParsed == "On") {
			a_events->enabled = true;
			if (c_log) {
				c_log->Print("AnimEvent logging enabled");
			}
			logger::info("AnimEvent logging enabled");
		} else {
			if (paramParsed == "Off") {
				a_events->enabled = false;
				a_events->useDynamic = false;
				if (c_log) {
					c_log->Print("AnimEvent logging disabled");
				}
				logger::info("AnimEvent logging disabled");
			} else {
				Anim::Dynamic::GetSingleton()->SetDynamicEvent(param, paramParsed);
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
