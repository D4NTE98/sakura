#include "../../client.h"

CSequences g_Sequences;

SequenceList::iterator CSequences::begin()
{
	return m_sequences.begin();
}

SequenceList::iterator CSequences::end()
{
	return m_sequences.end();
}

void CSequences::Update()
{
	if (!client_static || !pmove)
		return;

	const int incomingSequence = client_static->netchan.incoming_sequence;

	if (incomingSequence <= 0)
		return;

	if (incomingSequence > m_iLastIncomingSequence)
	{
		m_sequences.push_front(CIncomingSequence{ incomingSequence, GetTickCount() });
		m_iLastIncomingSequence = incomingSequence;
	}

	while (m_sequences.size() > 2048)
		m_sequences.pop_back();
}

void CSequences::Clear()
{
	m_sequences.clear();
	m_iLastIncomingSequence = 0;
}

float Sakura::Math::Clamp(float value, float minimumValue, float maximumValue)
{
	if (maximumValue < minimumValue)
		return maximumValue;

	if (value < minimumValue)
		return minimumValue;

	if (value > maximumValue)
		return maximumValue;

	return value;
}

bool Sakura::Backtrack::FindSpanningContexts(cl_entity_t* ent, float targettime, position_history_t** newer, position_history_t** older)
{
	if (!newer || !older)
		return true;

	*newer = nullptr;
	*older = nullptr;

	if (!ent)
		return true;

	const int current = ent->current_position & HISTORY_MASK;
	position_history_t* newest = &ent->ph[current];

	if (newest->animtime <= 0.0f)
		return true;

	if (targettime >= newest->animtime)
	{
		*newer = newest;
		*older = newest;
		return true;
	}

	position_history_t* lastValid = newest;

	for (int i = 1; i < HISTORY_MAX; ++i)
	{
		position_history_t* currentEntry = &ent->ph[(current - i) & HISTORY_MASK];

		if (currentEntry->animtime <= 0.0f)
			break;

		if (currentEntry->animtime <= targettime && lastValid->animtime >= targettime)
		{
			*newer = lastValid;
			*older = currentEntry;
			return false;
		}

		lastValid = currentEntry;
	}

	*newer = lastValid;
	*older = lastValid;
	return true;
}

bool Sakura::Backtrack::Player(cl_entity_s* pGameEntity, int lerp_msec, Vector& origin)
{
	if (!pGameEntity || !client_state || !g_Engine.pfnGetCvarPointer)
		return false;

	static cvar_t* sv_unlag = g_Engine.pfnGetCvarPointer("sv_unlag");
	static cvar_t* cl_lw = g_Engine.pfnGetCvarPointer("cl_lw");
	static cvar_t* cl_lc = g_Engine.pfnGetCvarPointer("cl_lc");
	static cvar_t* cl_updaterate = g_Engine.pfnGetCvarPointer("cl_updaterate");
	static cvar_t* sv_maxunlag = g_Engine.pfnGetCvarPointer("sv_maxunlag");
	static cvar_t* ex_interp = g_Engine.pfnGetCvarPointer("ex_interp");
	static cvar_t* sv_unlagpush = g_Engine.pfnGetCvarPointer("sv_unlagpush");

	if (sv_unlag && sv_unlag->value <= 0.0f)
		return false;

	if (cl_lw && cl_lw->value <= 0.0f)
		return false;

	if (cl_lc && cl_lc->value <= 0.0f)
		return false;

	const double fakeLatency = cvar.misc_fakelatency ? Sakura::Math::Clamp(cvar.misc_fakelatency_amount, 0.0f, 1000.0f) / 1000.0 : 0.0;
	const int frameIndex = client_state->parsecountmod;
	double latency = client_state->frames[frameIndex].latency + fakeLatency;
	latency = Sakura::Math::Clamp(static_cast<float>(latency), 0.0f, 1.5f);

	double updateInterval = 0.05;
	if (cl_updaterate && cl_updaterate->value > 10.0f)
		updateInterval = 1.0 / static_cast<double>(cl_updaterate->value);

	double correct = latency;
	if (correct > 1.0)
		correct = 1.0;

	if (sv_maxunlag && sv_maxunlag->value > 0.0f)
		correct = min(correct, static_cast<double>(sv_maxunlag->value));

	const double realtime = g_Engine.GetClientTime();
	double targettime = realtime - correct;
	double interptime = lerp_msec >= 0 ? lerp_msec / 1000.0 : (ex_interp ? ex_interp->value : 0.1);

	if (interptime < 0.0)
		interptime = 0.0;

	if (interptime > 0.1)
		interptime = 0.1;

	if (updateInterval > interptime)
		interptime = updateInterval;

	targettime -= interptime;

	if (sv_unlagpush)
		targettime += sv_unlagpush->value;

	if (targettime > realtime)
		targettime = realtime;

	position_history_t* newer = nullptr;
	position_history_t* older = nullptr;
	FindSpanningContexts(pGameEntity, static_cast<float>(targettime), &newer, &older);

	if (!newer || !older)
		return false;

	float fraction = 0.0f;
	const float deltaTime = newer->animtime - older->animtime;

	if (fabs(deltaTime) > 0.0001f)
		fraction = Sakura::Math::Clamp(static_cast<float>((targettime - older->animtime) / deltaTime), 0.0f, 1.0f);

	Vector delta = newer->origin - older->origin;

	if (delta.LengthSqr() > 4096.0f)
		return false;

	origin = older->origin + delta * fraction;
	return true;
}

void Sakura::Fakelatency::Logic()
{
	if (!cvar.misc_fakelatency || !client_static)
		return;

	const DWORD now = GetTickCount();
	const DWORD targetDelay = static_cast<DWORD>(Sakura::Math::Clamp(cvar.misc_fakelatency_amount, 0.0f, 1000.0f));

	for (const auto& sequence : g_Sequences)
	{
		if (now - sequence.time >= targetDelay)
		{
			client_static->netchan.incoming_sequence = sequence.seq;
			return;
		}
	}
}

void __cdecl Netchan_TransmitBits(netchan_t* chan, int length, byte* data)
{
	if (!client_static && chan)
		client_static = reinterpret_cast<client_static_s*>(reinterpret_cast<uintptr_t>(chan) - offsetof(client_static_s, netchan));

	if (Netchan_TransmitBits_s)
		Netchan_TransmitBits_s(chan, length, data);
}
