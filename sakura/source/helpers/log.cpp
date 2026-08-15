#include "../client.h"

void Sakura::Log::File(const char* text, ...)
{
	va_list va_alist;
	char buf[2048] = {};
	char logbuf[2304] = {};
	char cDirectory[600] = {};
	FILE* file;
	struct tm* current_tm;
	time_t current_time;

	time(&current_time);
	current_tm = localtime(&current_time);

	if (!current_tm)
		return;

	snprintf(
		logbuf,
		sizeof(logbuf),
		"[%02d-%02d-%02d %02d:%02d:%02d] ",
		current_tm->tm_mday,
		current_tm->tm_mon + 1,
		current_tm->tm_year + 1900,
		current_tm->tm_hour,
		current_tm->tm_min,
		current_tm->tm_sec
	);

	va_start(va_alist, text);
	vsnprintf(buf, sizeof(buf), text, va_alist);
	va_end(va_alist);

	strcat_s(logbuf, sizeof(logbuf), buf);
	strcat_s(logbuf, sizeof(logbuf), "\n");
	strcpy_s(cDirectory, sizeof(cDirectory), Sakura::CheatDir);
	strcat_s(cDirectory, sizeof(cDirectory), "sakura.log");

	if (fopen_s(&file, cDirectory, "a+") == 0 && file)
	{
		fputs(logbuf, file);
		fclose(file);
	}
}

void Sakura::Log::Console(const char* text, ...)
{
	va_list va_alist;
	char buf[2048] = {};

	va_start(va_alist, text);
	vsnprintf(buf, sizeof(buf), text, va_alist);
	va_end(va_alist);

	strcat_s(buf, sizeof(buf), "\n");
	g_Engine.Con_DPrintf("%s", buf);
}

void Sakura::Lua::Error(const char* errorMsg, ...)
{
	va_list va_alist;
	char buf[2048] = {};

	va_start(va_alist, errorMsg);
	vsnprintf(buf, sizeof(buf), errorMsg, va_alist);
	va_end(va_alist);

	Sakura::Log::File("%s", buf);

	if (g_Engine.pfnGetCvarFloat && g_Engine.Con_DPrintf)
	{
		float isInDeveloperMode = g_Engine.pfnGetCvarFloat("developer");

		if (isInDeveloperMode)
			Sakura::Log::Console("%s", buf);
	}
}
