#ifndef _TOAST_
#define _TOAST_

typedef int ToastPhase;

enum ToastPhase_
{
	ToastPhase_FadeIn,
	ToastPhase_Wait,
	ToastPhase_FadeOut,
	ToastPhase_Expired,
	ToastPhase_COUNT
};

class ToastInfo
{
private:
	int duration;
	char text[512];
	uint64_t creationTime;

public:
	auto get_title() const -> std::string { return this->text; }
	auto get_duration() const -> int { return this->duration; }
	auto get_elapsed_time() const -> uint64_t { return GetTickCount64() - this->creationTime; }
	auto get_creation_time() const -> uint64_t { return this->creationTime; }

	auto get_phase() const -> ToastPhase
	{
		const auto elapsed = get_elapsed_time();
		const float fade = cvar.notifications_fade > 1.0f ? cvar.notifications_fade : 1.0f;

		if (elapsed > fade + this->duration + fade)
			return ToastPhase_Expired;

		if (elapsed > fade + this->duration)
			return ToastPhase_FadeOut;

		if (elapsed > fade)
			return ToastPhase_Wait;

		return ToastPhase_FadeIn;
	}

	auto get_fade_percent() const -> float
	{
		const auto phase = get_phase();
		const float elapsed = static_cast<float>(get_elapsed_time());
		const float fade = cvar.notifications_fade > 1.0f ? cvar.notifications_fade : 1.0f;

		if (phase == ToastPhase_FadeIn)
		{
			float value = elapsed / fade;
			if (value < 0.0f) value = 0.0f;
			if (value > 1.0f) value = 1.0f;
			return value;
		}

		if (phase == ToastPhase_FadeOut)
		{
			float value = 1.0f - ((elapsed - fade - static_cast<float>(this->duration)) / fade);
			if (value < 0.0f) value = 0.0f;
			if (value > 1.0f) value = 1.0f;
			return value;
		}

		return 1.0f;
	}

	auto get_progress() const -> float
	{
		const float elapsed = static_cast<float>(get_elapsed_time());
		const float fade = cvar.notifications_fade > 1.0f ? cvar.notifications_fade : 1.0f;
		float activeElapsed = elapsed - fade;
		if (activeElapsed < 0.0f) activeElapsed = 0.0f;
		if (activeElapsed > static_cast<float>(this->duration)) activeElapsed = static_cast<float>(this->duration);
		float value = 1.0f - activeElapsed / static_cast<float>(this->duration);
		if (value < 0.0f) value = 0.0f;
		if (value > 1.0f) value = 1.0f;
		return value;
	}

	ToastInfo(int displaySeconds, const char* text)
	{
		strncpy_s(this->text, sizeof(this->text), text ? text : "", _TRUNCATE);
		this->duration = (displaySeconds > 0 ? displaySeconds : 1) * 1000;
		this->creationTime = GetTickCount64();
	}
};

extern std::vector<ToastInfo> toasts;

namespace Toast
{
	void Create(int displaySeconds, const char* text, ...);
	void Remove(int toastIndex);
	void Render();
}

#endif
