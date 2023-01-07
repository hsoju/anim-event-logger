#pragma once

namespace Anim
{
	class Commands
	{
	public:
		static Commands* GetSingleton() {
			static Commands singleton;
			return &singleton;
		}

		static void Register();

	protected:
		Commands() = default;
		Commands(const Commands&) = delete;
		Commands(Commands&&) = delete;
		virtual ~Commands() = default;

		Commands& operator=(const Commands&) = delete;
		Commands& operator=(Commands&&) = delete;

	private:
		static bool Exec(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj, 
			RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr);

		static constexpr char _DELIM{ '\t' };
	};
}
