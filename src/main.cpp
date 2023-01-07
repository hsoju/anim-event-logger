#include <stddef.h>

#include "TrueDirectionalMovementAPI.h"
#include "Events.h"
#include "Hooks.h"

using namespace RE::BSScript;
using namespace SKSE;
using namespace SKSE::log;

namespace {
    void InitializeLogging(const SKSE::PluginDeclaration* plugin) {
#ifndef NDEBUG
        auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
        auto path = logger::log_directory();
        if (!path) {
            SKSE::stl::report_and_fail("Unable to lookup SKSE logs directory.");
        }

        *path /= fmt::format(FMT_STRING("{}.log"), plugin->GetName());
        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
        const auto level = spdlog::level::trace;
#else
        const auto level = spdlog::level::info;
#endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
        log->set_level(level);
        log->flush_on(level);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern("[%l] %v"s);
		/*spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");*/
    }

    void InitializeHooking() {
        log::trace("Initializing trampoline...");
        auto& trampoline = GetTrampoline();
        trampoline.create(14);
        log::trace("Trampoline initialized.");
		Anim::Events::GetSingleton()->GetSettings();
		Hooks::Install();
    }

    void InitializeMessaging() {
        if (!GetMessagingInterface()->RegisterListener([](MessagingInterface::Message* message) {
                switch (message->type) {
                    // Skyrim lifecycle events.
                    case MessagingInterface::kPostLoad:  // Called after all plugins have finished running
                                                         // SKSEPlugin_Load. It is now safe to do multithreaded
                                                         // operations, or operations against other plugins.
						Anim::Events::GetSingleton()->tdmInterface = reinterpret_cast<TDM_API::IVTDM1*>(TDM_API::RequestPluginAPI(TDM_API::InterfaceVersion::V1));
						if (Anim::Events::GetSingleton()->tdmInterface) {
							Anim::Events::GetSingleton()->tdmLoaded = true;
						}
						break;
                    case MessagingInterface::kPostPostLoad:  // Called after all kPostLoad message handlers have run.
                    case MessagingInterface::kInputLoaded:   // Called when all game data has been found.
                        break;
                    case MessagingInterface::kDataLoaded:  // All ESM/ESL/ESP plugins have loaded, main menu is now
                                                           // active.
                        // It is now safe to access form data.
                        InitializeHooking();
                        break;

                    // Skyrim game events.
                    case MessagingInterface::kNewGame:      // Player starts a new game from main menu.
                    case MessagingInterface::kPreLoadGame:  // Player selected a game to load, but it hasn't loaded yet.
                                                            // Data will be the name of the loaded save.
                    case MessagingInterface::kPostLoadGame:  // Player's selected save game has finished loading.
                                                             // Data will be a boolean indicating whether the load was
                                                             // successful.
                    case MessagingInterface::kSaveGame:  // The player has saved a game.
                                                         // Data will be the save name.
                    case MessagingInterface::kDeleteGame:  // The player deleted a saved game from within the load menu.
                        break;
                }
            })) {
            SKSE::stl::report_and_fail("Unable to register message listener.");
        }
    }
}

extern "C" [[maybe_unused]] DLLEXPORT constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v;
    v.PluginVersion(Plugin::VERSION);
    v.PluginName(Plugin::NAME);
	v.UsesAddressLibrary(true);
    //v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST_AE, SKSE::RUNTIME_LATEST_VR});
    v.HasNoStructUse(true);
    return v;
}();

extern "C" [[maybe_unused]] DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo * pluginInfo) {
    pluginInfo->name = SKSEPlugin_Version.pluginName;
    pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
    pluginInfo->version = SKSEPlugin_Version.pluginVersion;
    return true;
}

extern "C" [[maybe_unused]] DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* skse) {
    auto* plugin = PluginDeclaration::GetSingleton();
    auto version = plugin->GetVersion();

	InitializeLogging(plugin);
    log::info("{} {} is loading...", plugin->GetName(), version);

    SKSE::Init(skse);

    InitializeMessaging();

    log::info("{} has finished loading.", plugin->GetName());
    return true;
}
