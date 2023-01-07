#pragma once
#include "TrueDirectionalMovementAPI.h"

namespace Anim
{
	class Events {
	
	public:
		static Events* GetSingleton() {
			static Events singleton;
			return &singleton;
		}

		static RE::BSEventNotifyControl ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src);
		
		static inline REL::Relocation<decltype(ProcessEvent)> _PCProcessEvent;
		static inline REL::Relocation<decltype(ProcessEvent)> _NPCProcessEvent;
		
		static RE::BSEventNotifyControl PCProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src);
		static RE::BSEventNotifyControl NPCProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src);
		
		static inline bool enabled = false;
		static inline bool showNames = false;
		static inline bool trackMount = false;

		static inline bool tdmLoaded = false;
		static inline TDM_API::IVTDM1* tdmInterface;
		static inline TDM_API::ActorHandle lastSelected;
		static inline RE::Actor* selectedActor;

		static inline bool useDynamic = false;
		static inline RE::BSFixedString dynamicEvent;
		static inline RE::Actor* dynamicActor;
		static inline RE::MagicItem* dynamicSpell;
		static inline bool useAlternateSpell = false;
		static inline uint32_t favoriteIdx = 0;

		static void PrintEvent(const char* holder, const char* a_event);
		static void CheckDynamicEvent(RE::BSAnimationGraphEvent& a_event);

		void AddEventSink();
		void GetSettings();

	protected:
		Events() = default;
		Events(const Events&) = delete;
		Events(Events&&) = delete;
		virtual ~Events() = default;

		auto operator=(const Events&) -> Events& = delete;
		auto operator=(Events&&) -> Events& = delete;
	};
}
