// MQ2PlatTracker.cpp :: RedGuides exclusive plugin to track platinum changes
// v1.00 :: Sym - 2012-11-11
// v1.01 :: Sym - 2013-01-03 - Added plat per hour average
// v1.02 :: Sym - 2013-01-27 - Fixed timer reset for plat per hour average

#include <mq/Plugin.h>
#include <chrono>

PLUGIN_VERSION(1.02);
PreSetup("MQ2PlatTracker");

// Ratio of hours to seconds, stored as a float
using FractionalHours = std::chrono::duration<float, std::ratio<3600, 1>>;

static std::chrono::steady_clock::time_point s_startTime = std::chrono::steady_clock::now();
static float s_startingCash = 0;
static float s_currentCash = 0;
static bool s_didInit = false;

static float CalculateTotalCash()
{
	// Convert to fractional platinum
	return static_cast<float>(pLocalPC->GetTotalCash()) / 1000;
}

static void ShowHelp()
{
	WriteChatf("\atMQ2PlatTracker :: v%1.2f :: by Sym for RedGuides.com\ax", MQ2Version);
	WriteChatf("/plat :: Lists command syntax");
	WriteChatf("/plat reset :: Resets starting amount of platinum to current platinum.");
	WriteChatf("/plat show :: Shows amount of platinum gained or lost since last reset along with the average per hour.");
}

void PlatTrackerCommand(PlayerClient* pChar, const char* szLine)
{
	char szTemp[MAX_STRING];
	GetArg(szTemp, szLine, 1);

	if (!_strnicmp(szTemp, "reset", 5))
	{
		s_startingCash = CalculateTotalCash();
		WriteChatf("MQ2PlatTracker :: \ayStarting amount reset to %.3f platinum\ax", s_startingCash);
		s_startTime = std::chrono::steady_clock::now();
		return;
	}

	if (!_strnicmp(szTemp, "show", 5))
	{
		s_currentCash = CalculateTotalCash();

	
		float runningTime = std::chrono::duration_cast<FractionalHours>(std::chrono::steady_clock::now() - s_startTime).count();

		if (s_currentCash > s_startingCash)
		{
			WriteChatf("MQ2PlatTracker :: \agYou have gained a total of %.3f platinum (%.3f platinum per hour)\ax", s_currentCash - s_startingCash, (s_currentCash - s_startingCash) / runningTime);
		}
		else if (s_currentCash < s_startingCash)
		{
			WriteChatf("MQ2PlatTracker :: \arYou have lost a total of %.3f platinum (%.3f platinum per hour)\ax", s_startingCash - s_currentCash, (s_startingCash - s_currentCash) / runningTime);
		}
		else
		{
			WriteChatf("MQ2PlatTracker :: \ayNo platinum changes detected yet\ax");
		}

		return;
	}

	ShowHelp();
}

PLUGIN_API void InitializePlugin()
{
	AddCommand("/plat", PlatTrackerCommand);
}

PLUGIN_API void ShutdownPlugin()
{
	RemoveCommand("/plat");
}

PLUGIN_API void SetGameState(DWORD GameState)
{
	if (GameState == GAMESTATE_INGAME && !s_didInit)
	{
		s_didInit = true;

		s_startTime = std::chrono::steady_clock::now();
		s_startingCash = CalculateTotalCash();
		WriteChatf("\atMQ2PlatTracker :: v%1.2f :: by Sym for RedGuides.com\ax", MQ2Version);
	}
}
