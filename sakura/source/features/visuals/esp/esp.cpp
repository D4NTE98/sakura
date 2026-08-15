#include "../../../client.h"

std::deque<playeresp_t> PlayerEsp;
std::deque<worldesp_t> WorldEsp;
std::deque<worldespprev_t> WorldEspPrev;

namespace
{
	DWORD lastLocalShotTime = 0;
	DWORD lastDamageTime[33] = {};
	int lastDamage[33] = {};
}

void Sakura::Esp::Player::RegisterLocalShot()
{
	lastLocalShotTime = GetTickCount();
}

void Sakura::Esp::Player::RegisterDamage(const int index, const int damage)
{
	if (index < 1 || index > 32 || damage <= 0)
		return;

	if (g_Player[index].iTeam == g_Local.iTeam)
		return;

	const DWORD now = GetTickCount();

	if (now - lastLocalShotTime > 550)
		return;

	if (now - lastDamageTime[index] <= 240)
		lastDamage[index] += damage;
	else
		lastDamage[index] = damage;

	if (lastDamage[index] > 999)
		lastDamage[index] = 999;

	lastDamageTime[index] = now;
}

void Rect(Vector2D Pos, Vector2D Size, ImU32 color)
{
	ImGui::GetCurrentWindow()->DrawList->AddRect({ Pos.x, Pos.y }, { Pos.x + Size.x, Pos.y + Size.y }, color);
}

void Corners(Vector2D Pos, Vector2D Size, ImU32 color)
{
	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x, Pos.y }, { Pos.x + (Size.x / 4), Pos.y }, color);
	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x, Pos.y }, { Pos.x, Pos.y + (Size.x / 4) }, color);

	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x, Pos.y + Size.y }, { Pos.x + (Size.x / 4), Pos.y + Size.y }, color);
	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x, Pos.y + Size.y }, { Pos.x, Pos.y + Size.y - (Size.x / 4) }, color);

	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x + Size.x, Pos.y }, { Pos.x + Size.x - (Size.x / 4), Pos.y }, color);
	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x + Size.x, Pos.y }, { Pos.x + Size.x, Pos.y + (Size.x / 4) }, color);

	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x + Size.x, Pos.y + Size.y }, { Pos.x + Size.x - (Size.x / 4), Pos.y + Size.y }, color);
	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x + Size.x, Pos.y + Size.y }, { Pos.x + Size.x, Pos.y + Size.y - (Size.x / 4) }, color);
}

void Coal(Vector2D Pos, Vector2D Size, ImU32 color)
{
	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x, Pos.y }, { Pos.x, Pos.y + Size.y }, color);
	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x, Pos.y }, { Pos.x + (Size.x / 4), Pos.y }, color);
	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x, Pos.y + Size.y }, { Pos.x + (Size.x / 4), Pos.y + Size.y }, color);

	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x + Size.x, Pos.y }, { Pos.x + Size.x, Pos.y + Size.y }, color);
	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x + Size.x, Pos.y }, { Pos.x + Size.x - (Size.x / 4), Pos.y }, color);
	ImGui::GetCurrentWindow()->DrawList->AddLine({ Pos.x + Size.x, Pos.y + Size.y }, { Pos.x + Size.x - (Size.x / 4), Pos.y + Size.y }, color);
}

void Sakura::Esp::Player::DrawBox(const float x, const float y, const float w, const float h, const ImRGBA color)
{
	if (!cvar.visual_box)
		return;

	if (cvar.visual_box_fill > 0) ImGui::GetCurrentWindow()->DrawList->AddRectFilled({ x, y }, { x + w, y + h }, ImColor(color.r, color.g, color.b, cvar.visual_box_fill / 100.f));

	switch ((int)cvar.visual_box_type)
	{
	case 0:
		Rect({ x - 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Rect({ x - 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Rect({ x + 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Rect({ x + 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Rect({ x, y }, { w, h }, ImColor(color.r, color.g, color.b, color.a));
		break;
	case 1:
		Corners({ x - 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Corners({ x - 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Corners({ x + 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Corners({ x + 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Corners({ x, y }, { w, h }, ImColor(color.r, color.g, color.b, color.a));
		break;
	case 2:
		Coal({ x - 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Coal({ x - 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Coal({ x + 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Coal({ x + 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Coal({ x, y }, { w, h }, ImColor(color.r, color.g, color.b, color.a));
	}
}

void Sakura::Esp::Player::DrawHealth(const int index, const float x, const float y, const float h)
{
	if (!cvar.visual_health)
		return;

	static int playerHealth[33];
	static double hp[33][Sakura::Animation::max_count];
	static double changeTimestamp[33] = {};

	playerHealth[index] = g_Player[index].iHealth;

	if (playerHealth[index] < 0)
		playerHealth[index] = 0;
	if (playerHealth[index] > 100)
		playerHealth[index] = 100;

	Sakura::Animation::Calculate(hp[index], changeTimestamp[index], playerHealth[index], 100, 0.65);

	float displayedHealth = static_cast<float>(hp[index][Sakura::Animation::calculated]);
	if (displayedHealth < 0.0f)
		displayedHealth = 0.0f;
	if (displayedHealth > 100.0f)
		displayedHealth = 100.0f;

	const float ratio = displayedHealth / 100.0f;
	const float barWidth = 5.0f;
	const float barX = x - 11.0f;
	const float fillTop = y + h - (h * ratio);

	float red;
	float green;

	if (ratio < 0.5f)
	{
		red = 1.0f;
		green = ratio * 2.0f;
	}
	else
	{
		red = (1.0f - ratio) * 2.0f;
		green = 1.0f;
	}

	ImDrawList* drawList = ImGui::GetCurrentWindow()->DrawList;

	drawList->AddRectFilled(
		ImVec2(barX - 2.0f, y - 2.0f),
		ImVec2(barX + barWidth + 2.0f, y + h + 2.0f),
		ImColor(0.025f, 0.027f, 0.035f, 0.92f),
		3.0f
	);

	drawList->AddRectFilled(
		ImVec2(barX, fillTop),
		ImVec2(barX + barWidth, y + h),
		ImColor(red, green, 0.08f, 1.0f),
		2.0f
	);

	if (playerHealth[index] < 100)
	{
		char healthText[8] = {};
		sprintf_s(healthText, "%d", static_cast<int>(displayedHealth + 0.5f));

		const ImVec2 textSize = ImGui::CalcTextSize(healthText);
		float textY = fillTop - textSize.y * 0.5f;

		if (textY < y)
			textY = y;
		if (textY + textSize.y > y + h)
			textY = y + h - textSize.y;

		const ImVec2 textPos(barX - textSize.x - 7.0f, textY);

		drawList->AddRectFilled(
			ImVec2(textPos.x - 4.0f, textPos.y - 2.0f),
			ImVec2(textPos.x + textSize.x + 4.0f, textPos.y + textSize.y + 2.0f),
			ImColor(0.025f, 0.027f, 0.035f, 0.88f),
			3.0f
		);

		drawList->AddText(textPos, ImColor(0.94f, 0.95f, 0.98f, 1.0f), healthText);
	}
}

void Sakura::Esp::Player::DrawDamage(const int index, const float x, const float y, const float w, const float h)
{
	if (!cvar.visual_damage || index < 1 || index > 32 || lastDamage[index] <= 0)
		return;

	const DWORD now = GetTickCount();
	const DWORD age = now - lastDamageTime[index];
	const float duration = 1350.0f;

	if (age >= static_cast<DWORD>(duration))
		return;

	const float progress = static_cast<float>(age) / duration;
	const float alpha = 1.0f - progress;
	const float rise = progress * 18.0f;

	char damageText[24] = {};
	sprintf_s(damageText, "-%d HP", lastDamage[index]);

	const ImVec2 textSize = ImGui::CalcTextSize(damageText);
	const ImVec2 textPos(x + w + 14.0f, y + h * 0.36f - rise);
	ImDrawList* drawList = ImGui::GetCurrentWindow()->DrawList;

	drawList->AddRectFilled(
		ImVec2(textPos.x - 7.0f, textPos.y - 4.0f),
		ImVec2(textPos.x + textSize.x + 8.0f, textPos.y + textSize.y + 4.0f),
		ImColor(0.035f, 0.037f, 0.047f, 0.88f * alpha),
		5.0f
	);

	drawList->AddRectFilled(
		ImVec2(textPos.x - 7.0f, textPos.y - 4.0f),
		ImVec2(textPos.x - 4.0f, textPos.y + textSize.y + 4.0f),
		ImColor(0.94f, 0.22f, 0.36f, alpha),
		5.0f
	);

	drawList->AddText(textPos, ImColor(1.0f, 0.86f, 0.89f, alpha), damageText);
}

void Sakura::Esp::Player::DrawVip(const int index, const float x, const float y)
{
	if (!cvar.visual_vip || !g_Player[index].bVip)
		return;

	float label_size = ImGui::CalcTextSize("VIP", NULL, true).x / 2.0f;
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size, y }, ImColor(1.f, 1.f, 0.f, 1.f), "VIP");
}

void Sakura::Esp::Player::DrawReload(const int index, const int sequence, const float x, const float y)
{
	int seqinfo = Cstrike_SequenceInfo[sequence];

	if (!cvar.visual_reload_bar || seqinfo != 2)
		return;

	float label_size = ImGui::CalcTextSize("Reloading", NULL, true).x / 2.0f;
	float y2 = cvar.visual_vip && g_Player[index].bVip ? 8 : 0;
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size, y + y2 }, ImColor(0.f, 1.f, 0.f, 1.f), "Reloading");
}

void Sakura::Esp::Player::DrawName(const int index, const float x, const float y)
{
	if (!cvar.visual_name)
		return;

	player_info_s* player = g_Studio.PlayerInfo(index - 1);

	if (!player || !(lstrlenA(player->name) > 0))
		return;

	float label_size = ImGui::CalcTextSize(player->name, NULL, true).x / 2.0f;
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size, y - 16 }, ImColor(1.f, 1.f, 1.f, 1.f), player->name);
}

void Sakura::Esp::Player::DrawWeapon(const int weaponId, const float x, const float y)
{
	model_s* mdl = g_Studio.GetModelByIndex(weaponId);

	if (!cvar.visual_weapon || !mdl)
		return;

	char weapon[256];
	sprintf(weapon, Sakura::Strings::getfilename(mdl->name).c_str() + 2);

	float label_size = ImGui::CalcTextSize(weapon, NULL, true).x / 2.0f;
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size, y }, ImColor(1.f, 1.f, 1.f, 1.f), weapon);
}

void Sakura::Esp::Player::DrawModel(const char* model, const float x, const float y)
{
	if (!cvar.visual_model)
		return;

	float label_size = ImGui::CalcTextSize(model, NULL, true).x / 2.0f;
	float y2 = cvar.visual_weapon ? 8 : 0;
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size, y + y2 }, ImColor(1.f, 1.f, 1.f, 1.f), model);
}

bool bCalcScreen(playeresp_t Esp, float& x, float& y, float& w, float& h, float& xo, float& yo)
{
	float vOrigin[2];
	if (WorldToScreen(Esp.origin, vOrigin))
	{
		xo = IM_ROUND(vOrigin[0]);
		yo = IM_ROUND(vOrigin[1]);
		float x0 = vOrigin[0], x1 = vOrigin[0], y0 = vOrigin[1], y1 = vOrigin[1];
		for (esphitbox_t Hitbox : Esp.PlayerEspHitbox)
		{
			for (unsigned int i = 0; i < 8; i++)
			{
				float vHitbox[2];
				if (WorldToScreen(Hitbox.HitboxMulti[i], vHitbox))
				{
					x0 = min(x0, vHitbox[0]);
					x1 = max(x1, vHitbox[0]);
					y0 = min(y0, vHitbox[1]);
					y1 = max(y1, vHitbox[1]);
				}
			}
		}
		x = IM_ROUND(x0);
		y = IM_ROUND(y0);
		w = IM_ROUND(x1) - IM_ROUND(x0) + 1;
		h = IM_ROUND(y1) - IM_ROUND(y0) + 1;
		return true;
	}
	return false;
}

void Sakura::Esp::Player::Draw()
{
	for (playeresp_t Esp : PlayerEsp)
	{
		if (cvar.visual_idhook_only && IdHook::FirstKillPlayer[Esp.index] == IDHOOK_PLAYER_OFF)
			continue;

		if (!cvar.visual_visual_team && g_Player[Esp.index].iTeam == g_Local.iTeam)
			continue;

		if (!Sakura::Player::IsAlive(Esp.index))
			continue;

		ImRGBA boxColor = Sakura::Colors::GetCustomizedTeamColor(Esp.index, cvar.visual_box_color_tt, cvar.visual_box_color_ct,
			cvar.rainbow_box_tt, cvar.rainbow_box_ct);

		float x, y, w, h, xo, yo;

		if (bCalcScreen(Esp, x, y, w, h, xo, yo))
		{
			DrawBox(x, y, w, h, boxColor);
			DrawHealth(Esp.index, x, y, h);
			DrawDamage(Esp.index, x, y, w, h);
			DrawReload(Esp.sequence, xo + w, y, Esp.index);
			DrawName(Esp.index, x + (w / 2), y);
			DrawModel(Esp.model, x + (w / 2), y + h);
			DrawWeapon(Esp.weaponmodel, x + (w / 2), y + h);
			DrawVip(Esp.index, x + w, y);
		}
	}
}

void Sakura::Esp::World::DrawBox(const float x, const float y, const float w, const float h)
{
	if (!cvar.visual_box_world) return;

	ImRGBA color = Sakura::Colors::GetCustomizedColor(cvar.visual_box_world_color, cvar.rainbow_world_box);

	if (cvar.visual_box_world_fill > 0) ImGui::GetCurrentWindow()->DrawList->AddRectFilled({ x, y }, { x + w, y + h }, ImColor(color.r, color.g, color.b, cvar.visual_box_world_fill / 100.f));

	switch ((int)cvar.visual_box_world_type)
	{
	case 0:
		Rect({ x - 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Rect({ x - 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Rect({ x + 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Rect({ x + 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Rect({ x, y }, { w, h }, ImColor(color.r, color.g, color.b, color.a));
		break;
	case 1:
		Corners({ x - 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Corners({ x - 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Corners({ x + 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Corners({ x + 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Corners({ x, y }, { w, h }, ImColor(color.r, color.g, color.b, color.a));
		break;
	case 2:
		Coal({ x - 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Coal({ x - 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Coal({ x + 1, y - 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Coal({ x + 1, y + 1 }, { w, h }, ImColor(0.f, 0.f, 0.f, color.a));
		Coal({ x, y }, { w, h }, ImColor(color.r, color.g, color.b, color.a));
		break;
	}
}

void Sakura::Esp::World::DrawName(const int index, const float x, const float y, const ImU32 color)
{
	if (index < 1 || index > g_Engine.GetMaxClients())
		return;

	if (!cvar.visual_name_world)
		return;

	player_info_s* player = g_Studio.PlayerInfo(index - 1);

	if (!player || !(lstrlenA(player->name) > 0)) 
		return;

	float label_size = ImGui::CalcTextSize(player->name, NULL, true).x / 2;
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size, y }, color, player->name);

	return;
}

void Sakura::Esp::World::DrawModel(const char* name, const float x, const float y, const ImU32 white)
{
	if (!cvar.visual_model_world)
		return;

	char world[256];
	sprintf(world, Sakura::Strings::getfilename(name).c_str() + 2);

	float label_size = ImGui::CalcTextSize(world, NULL, true).x / 2;
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size, y - 16 }, white, world);
}

bool bCalcScreenWorld(worldesp_t Esp, float& x, float& y, float& w, float& h, float& xo, float& yo)
{
	float vOrigin[2];
	if (WorldToScreen(Esp.origin, vOrigin))
	{
		xo = IM_ROUND(vOrigin[0]);
		yo = IM_ROUND(vOrigin[1]);
		float x0 = vOrigin[0], x1 = vOrigin[0], y0 = vOrigin[1], y1 = vOrigin[1];
		for (esphitbox_t Hitbox : Esp.WorldEspHitbox)
		{
			for (unsigned int i = 0; i < 8; i++)
			{
				float vHitbox[2];
				if (WorldToScreen(Hitbox.HitboxMulti[i], vHitbox))
				{
					x0 = min(x0, vHitbox[0]);
					x1 = max(x1, vHitbox[0]);
					y0 = min(y0, vHitbox[1]);
					y1 = max(y1, vHitbox[1]);
				}
			}
		}
		x = IM_ROUND(x0);
		y = IM_ROUND(y0);
		w = IM_ROUND(x1) - IM_ROUND(x0) + 1;
		h = IM_ROUND(y1) - IM_ROUND(y0) + 1;
		return true;
	}
	return false;
}

void Sakura::Esp::World::Draw()
{
	for (worldesp_t Esp : WorldEsp)
	{
		if (!strstr(Esp.name, "w_"))
			continue;

		float x, y, w, h, xo, yo;
		if (bCalcScreenWorld(Esp, x, y, w, h, xo, yo))
		{
			ImU32 teamColor = g_Player[Esp.index].iTeam == 1 ? Sakura::Colors::Red() : Sakura::Colors::Blue();

			DrawBox(x, y, w, h);
			DrawName(Esp.index, x + (w / 2), y + h, teamColor);
			DrawModel(Esp.name, x + (w / 2), y, Sakura::Colors::White());
		}
	}
}