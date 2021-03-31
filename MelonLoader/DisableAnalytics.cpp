#include "DisableAnalytics.h"
#include "HookManager.h"
#include "Logger.h"

gethostbyname_t DisableAnalytics::Original_gethostbyname = NULL;
getaddrinfo_t DisableAnalytics::Original_getaddrinfo = NULL;
std::list<std::string> DisableAnalytics::URL_Blacklist = {
	"amplitude.com",
	"api.uca.cloud.unity3d.com",
	"config.uca.cloud.unity3d.com",
	"perf-events.cloud.unity3d.com",
	"public.cloud.unity3d.com",
	"cdp.cloud.unity3d.com",
	"data-optout-service.uca.cloud.unity3d.com",
	"oculus.com",
	"oculuscdn.com",
	"facebook-hardware.com",
	"facebook.net",
	"facebook.com",
	"graph.facebook.com",
	"fbcdn.com",
	"fbsbx.com",
	"fbcdn.net",
	"fb.me",
	"fb.com",
	"crashlytics.com",
	"discordapp.com",
	"dropbox.com",
	"pastebin.com",
	"gluehender-aluhut.de",
	"softlight.at.ua"
};

void DisableAnalytics::Setup()
{
	HMODULE wsock32 = GetModuleHandle("wsock32.dll");
	if (wsock32 != NULL)
	{
		Original_gethostbyname = (gethostbyname_t)GetProcAddress(wsock32, "gethostbyname");
		if (Original_gethostbyname != NULL)
			HookManager::Hook(&(LPVOID&)Original_gethostbyname, Hooked_gethostbyname);
		else
			Logger::DebugLogWarning("Failed to GetProcAddress ( gethostbyname ) for [ DisableAnalytics.cpp | DisableAnalytics::Setup ]");
	}
	else
		Logger::DebugLogWarning("Failed to GetModuleHandle ( wsock32.dll ) for [ DisableAnalytics.cpp | DisableAnalytics::Setup ]");

	HMODULE ws2_32 = GetModuleHandle("ws2_32");
	if (ws2_32 != NULL)
	{
		Original_getaddrinfo = (getaddrinfo_t)GetProcAddress(ws2_32, "getaddrinfo");
		if (Original_getaddrinfo != NULL)
			HookManager::Hook(&(LPVOID&)Original_getaddrinfo, Hooked_getaddrinfo);
		else
			Logger::DebugLogWarning("Failed to GetProcAddress ( getaddrinfo ) for [ DisableAnalytics.cpp | DisableAnalytics::Setup ]");
	}
	else
		Logger::DebugLogWarning("Failed to GetModuleHandle ( ws2_32 ) for [ DisableAnalytics.cpp | DisableAnalytics::Setup ]");
}

bool DisableAnalytics::CheckBlacklist(std::string url)
{
    std::transform(url.begin(), url.end(), url.begin(), [](unsigned char c){ return std::tolower(c); });
	bool url_found = (std::find(URL_Blacklist.begin(), URL_Blacklist.end(), url) != URL_Blacklist.end());
	if (url_found)
		Logger::DebugLog("Analytics URL Blocked: " + url);
	return url_found;
}

void* DisableAnalytics::Hooked_gethostbyname(const char* name)
{
	try
	{
		if ((name != NULL) && CheckBlacklist(name))
			return Original_gethostbyname("localhost");
		return Original_gethostbyname(name);
	}
	catch (...)
	{
		Logger::LogError("Exception caught in gethostbyname! Returning NULL");
		return NULL;
	}
}

int DisableAnalytics::Hooked_getaddrinfo(PCSTR pNodeName, PCSTR pServiceName, void* pHints, void* ppResult)
{
	try
	{
		if ((pNodeName != NULL) && CheckBlacklist(pNodeName))
			return Original_getaddrinfo("localhost", pServiceName, pHints, ppResult);
		return Original_getaddrinfo(pNodeName, pServiceName, pHints, ppResult);
	}
	catch (...)
	{
		Logger::LogError("Exception caught in getaddrinfo! Returning WSATRY_AGAIN");
		return WSATRY_AGAIN;
	}
}