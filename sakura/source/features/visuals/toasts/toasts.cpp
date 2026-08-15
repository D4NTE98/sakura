#include "../../../client.h"

std::vector<ToastInfo> toasts;

void Toast::Create(int displaySeconds, const char* text, ...)
{
	va_list va_alist;
	char textbf[1024] = {};

	va_start(va_alist, text);
	vsnprintf(textbf, sizeof(textbf), text ? text : "", va_alist);
	va_end(va_alist);

	toasts.emplace_back(displaySeconds, textbf);
}

void Toast::Remove(int toastIndex)
{
	if (toastIndex < 0 || toastIndex >= static_cast<int>(toasts.size()))
		return;

	toasts.erase(toasts.begin() + toastIndex);
}

void Toast::Render()
{
	if (!cvar.notifications_enable || toasts.empty())
		return;

	ImDrawList* draw = ImGui::GetCurrentWindow()->DrawList;
	const ImVec2 display = ImGui::GetIO().DisplaySize;
	const float centerX = display.x * 0.5f;
	float stackY = cvar.notifications_y;

	for (int i = static_cast<int>(toasts.size()) - 1; i >= 0; --i)
	{
		ToastInfo& toast = toasts[i];

		if (toast.get_phase() == ToastPhase_Expired)
		{
			Toast::Remove(i);
			continue;
		}

		const std::string message = toast.get_title();
		const float opacity = toast.get_fade_percent();
		const float progress = toast.get_progress();
		const ImVec2 messageSize = ImGui::CalcTextSize(message.c_str());

		if (cvar.notifications_text_only)
		{
			float x = cvar.notifications_x;
			if (cvar.notifications_x > centerX + messageSize.x * 0.5f)
				x -= messageSize.x;
			else if (cvar.notifications_x >= centerX - messageSize.x * 0.5f)
				x = centerX - messageSize.x * 0.5f;

			draw->AddText(
				ImVec2(x, stackY),
				ImColor(
					cvar.notifications_text_color[0],
					cvar.notifications_text_color[1],
					cvar.notifications_text_color[2],
					opacity
				),
				message.c_str()
			);

			stackY += messageSize.y + 7.0f;
			continue;
		}

		const float width = ImMax(250.0f, messageSize.x + 42.0f);
		const float height = 54.0f;
		const float slide = (1.0f - opacity) * 18.0f;
		float x = cvar.notifications_x;

		if (cvar.notifications_x < centerX - width * 0.5f)
			x -= slide;
		else if (cvar.notifications_x > centerX + width * 0.5f)
			x = cvar.notifications_x - width + slide;
		else
			x = centerX - width * 0.5f;

		x = Sakura::Math::Clamp(x, 4.0f, ImMax(4.0f, display.x - width - 4.0f));

		const ImVec2 boxMin(x, stackY);
		const ImVec2 boxMax(x + width, stackY + height);
		const ImColor accent = Sakura::Menu::GetMenuColor(opacity);
		const ImColor background(
			cvar.notifications_bg_color[0],
			cvar.notifications_bg_color[1],
			cvar.notifications_bg_color[2],
			opacity * 0.97f
		);
		const ImColor progressColor(
			cvar.notifications_bg_time_color[0],
			cvar.notifications_bg_time_color[1],
			cvar.notifications_bg_time_color[2],
			opacity
		);
		const ImColor textColor(
			cvar.notifications_text_color[0],
			cvar.notifications_text_color[1],
			cvar.notifications_text_color[2],
			opacity
		);

		draw->AddRectFilled(
			ImVec2(boxMin.x + 3.0f, boxMin.y + 5.0f),
			ImVec2(boxMax.x + 3.0f, boxMax.y + 5.0f),
			ImColor(0.0f, 0.0f, 0.0f, opacity * 0.28f),
			9.0f
		);
		draw->AddRectFilled(boxMin, boxMax, background, 9.0f);
		draw->AddRect(boxMin, boxMax, ImColor(1.0f, 1.0f, 1.0f, opacity * 0.08f), 9.0f, ImDrawCornerFlags_All, 1.0f);
		draw->AddRectFilled(ImVec2(boxMin.x, boxMin.y + 8.0f), ImVec2(boxMin.x + 3.0f, boxMax.y - 8.0f), accent, 2.0f);
		draw->AddCircleFilled(ImVec2(boxMin.x + 17.0f, boxMin.y + 17.0f), 3.0f, accent);
		draw->AddText(ImVec2(boxMin.x + 27.0f, boxMin.y + 9.0f), accent, "SAKURA");
		draw->AddText(ImVec2(boxMin.x + 14.0f, boxMin.y + 29.0f), textColor, message.c_str());
		draw->AddRectFilled(
			ImVec2(boxMin.x + 10.0f, boxMax.y - 6.0f),
			ImVec2(boxMax.x - 10.0f, boxMax.y - 4.0f),
			ImColor(1.0f, 1.0f, 1.0f, opacity * 0.07f),
			1.0f
		);
		draw->AddRectFilled(
			ImVec2(boxMin.x + 10.0f, boxMax.y - 6.0f),
			ImVec2(boxMin.x + 10.0f + (width - 20.0f) * progress, boxMax.y - 4.0f),
			progressColor,
			1.0f
		);

		stackY += height + 8.0f;
	}
}
