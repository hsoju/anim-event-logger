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
