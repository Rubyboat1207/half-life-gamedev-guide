#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include<string>

class CInfoGuide : public CBaseAnimating
{
private:
	float m_animationTimer;

public:
	void Spawn() override;
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;
	void Precache() override;
	void EXPORT GuideThink();
};

LINK_ENTITY_TO_CLASS( info_guide, CInfoGuide );

void CInfoGuide::Precache()
{
	PRECACHE_MODEL("models/speech_bubble.mdl");

	// all sounds
	PRECACHE_SOUND("info_guide/hello.wav");
}

void CInfoGuide::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/speech_bubble.mdl");

	UTIL_SetSize(pev, {-52, -52, -52}, {52, 52, 52});

	UTIL_SetOrigin(pev, pev->origin);
	
	pev->nextthink = gpGlobals->time + 0.5;
	SetThink(&CInfoGuide::GuideThink);

	pev->frame = 0;
	ResetSequenceInfo();
	pev->sequence = 0;

	ALERT(at_console, "ready");
}

void CInfoGuide::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) {
	ALERT(at_console, "%s\n", STRING(pev->message));
	pev->frame = 0;
	ResetSequenceInfo();
	pev->sequence = 1;
	m_animationTimer = pev->health;
	EMIT_SOUND(ENT(pev), CHAN_VOICE, (std::string("info_guide/").append(STRING(pev->message)).append(".wav")).c_str(), 1, ATTN_NORM);
}

void CInfoGuide::GuideThink() {
	//ALERT(at_console, "THINKING remain: %f, time: %f\n", m_animationTimer, pev->health);

	if (m_animationTimer > 0) {
		m_animationTimer -= 0.1;

		if (m_animationTimer <= 0) {
			pev->frame = 0;
			ResetSequenceInfo();
			pev->sequence = 0;
		}
	}

	pev->nextthink = gpGlobals->time + 0.1;
}