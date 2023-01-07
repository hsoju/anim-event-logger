#pragma once

namespace Anim
{
	class Dynamic
	{
	public:
		static Dynamic* GetSingleton() {
			static Dynamic singleton;
			return &singleton;
		}

		void GenerateNode(std::stringstream& a_stream, const char* a_key, const char* a_val, bool use_quotes=false, bool use_brackets=false);
		const std::string GenerateDynamicSpell(const char* a_event, const char* a_spellID, const char* a_spell_mod);
		void CopyDynamicSpell(const std::string& a_toml);

		void SetDynamicActor(RE::Actor* a_player);
		void SetDynamicSpell(RE::PlayerCharacter* a_player, bool a_leftHand);
		void SetDynamicEvent(RE::SCRIPT_FUNCTION::StringChunk* a_param, std::string_view a_param_parse);

	protected:
		Dynamic() = default;
		Dynamic(const Dynamic&) = delete;
		Dynamic(Dynamic&&) = delete;
		virtual ~Dynamic() = default;

		Dynamic& operator=(const Dynamic&) = delete;
		Dynamic& operator=(Dynamic&&) = delete;
	};
}
