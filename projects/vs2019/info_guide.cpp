#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include<string>
#include <unordered_map>
#include "filesystem_utils.h"

struct InfoGuideSubtitle;
struct InfoGuideSubtitle
{
	float startTime;
	char* text;
	InfoGuideSubtitle* next;
	InfoGuideSubtitle* first;

	InfoGuideSubtitle(float startTime, char* text);
	InfoGuideSubtitle* set_next(InfoGuideSubtitle* next);
	InfoGuideSubtitle* latest();
};



class CInfoGuide : public CBaseAnimating
{
private:
	float m_animationTimer;
	InfoGuideSubtitle* subtitle;
	std::vector<const char*> paths;
	hudtextparms_t params{};
	// redundant, i know.
	std::unordered_map<std::string, InfoGuideSubtitle*> guides{};

public:
	void Spawn() override;
	void InitDialogue();
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;
	void Precache() override;
	void EXPORT GuideThink();
};





InfoGuideSubtitle::InfoGuideSubtitle(float startTime, char* text)
{
	this->startTime = startTime;
	this->text = text;
	next = nullptr;
	first = this;
}

InfoGuideSubtitle* InfoGuideSubtitle::set_next(InfoGuideSubtitle* next)
{
	this->next = next;
	next->first = this->first;

	return this->next;
}

InfoGuideSubtitle* InfoGuideSubtitle::latest()
{
	// Check if the list is empty or this is the last element.
	if (this->next == nullptr)
	{
		return this;
	}

	InfoGuideSubtitle* next = this->next;
	int max_iter = 50; // Consider adjusting this if the list can be longer.
	int i = 0;

	while (i < max_iter && next != nullptr)
	{
		if (next->next == nullptr)
		{
			// Found the last element in the list.
			return next;
		}
		next = next->next;
		i++;
	}

	// If the list is longer than max_iter, return the element at max_iter.
	// This is a fallback and might need rethinking based on your use case.
	return next;
}


LINK_ENTITY_TO_CLASS( info_guide, CInfoGuide );

void CInfoGuide::Precache()
{
	PRECACHE_MODEL("models/speech_bubble.mdl");

	for (const auto& sound : guides)
	{
		// Calculate the length of the required string
		//size_t length = strlen("info_guide/") + sound.first.length() + strlen(".wav") + 1; // +1 for the null terminator
		char* file = new char[255];

		// Construct the file path
		strcpy(file, "info_guide/");
		strcat(file, sound.first.c_str());
		strcat(file, ".wav");

		// Use the file path as needed
		ALERT(at_console, file);
		PRECACHE_SOUND(file);

		// Store the dynamically allocated string in the vector
		paths.push_back(file);
	}
	
}

void CInfoGuide::Spawn()
{
	InitDialogue();

	Precache();
	SET_MODEL(ENT(pev), "models/speech_bubble.mdl");

	UTIL_SetSize(pev, {-52, -52, -52}, {52, 52, 52});

	UTIL_SetOrigin(pev, pev->origin);
	
	pev->nextthink = gpGlobals->time + 0.5;
	SetThink(&CInfoGuide::GuideThink);

	pev->frame = 0;
	ResetSequenceInfo();
	pev->sequence = 0;

	params.x = -1;
	params.y = 0.7f;
	params.effect = 0; // fade in-out
	// main color
	params.r1 = 255;
	params.g1 = 255;
	params.b1 = 255;
	params.a1 = 255;
	// highlight color
	params.r2 = 255;
	params.g2 = 255;
	params.b2 = 255;
	params.a2 = 255;
	params.fxTime = 0;
	params.channel = 3;


	ALERT(at_console, "spawn done\n");
}

void CInfoGuide::InitDialogue() {
	guides["hello"] = new InfoGuideSubtitle(0.f, "Hello and again,");
	guides["hello"]->set_next(new InfoGuideSubtitle(1.5f, "welcome to the apeture science"));
	guides["hello"]->next->set_next(new InfoGuideSubtitle(3.5f, "computer aided enrichment center"));

	guides["intro"] = new InfoGuideSubtitle(0.1f, "Welcome to the Half-Life 1 guided tour");
	guides["intro"]->set_next(new InfoGuideSubtitle(2.6f, "these speech bubbles will be how I"));
	guides["intro"]->next->set_next(new InfoGuideSubtitle(4.8f, "Rudy Soliz"));
	guides["intro"]->latest()->set_next(new InfoGuideSubtitle(5.8f, "Will explain the various examples"));
	guides["intro"]->latest()->set_next(new InfoGuideSubtitle(7.4f, "during this tour."));
	guides["intro"]->latest()->set_next(new InfoGuideSubtitle(8.4f, ""));
	guides["intro"]->latest()->set_next(new InfoGuideSubtitle(8.9f, "If you stand still for more than 30 seconds"));
	guides["intro"]->latest()->set_next(new InfoGuideSubtitle(11.f, "there will be a countdown and then the map will reset."));


	guides["example"] = new InfoGuideSubtitle(0.1f, "Here is an example of something interesting");
	guides["example"]->set_next(new InfoGuideSubtitle(2.4f, "maybe something that ties in with my paper"));
	guides["example"]->next->set_next(new InfoGuideSubtitle(4.8f, "i will explain it in more detail than is probably needed"));
	guides["example"]->latest()->set_next(new InfoGuideSubtitle(7.6f, "then ask the user to move on to the next bubble"));
}

void CInfoGuide::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) {
	ALERT(at_console, "%s\n", STRING(pev->message));
	pev->frame = 0;
	ResetSequenceInfo();
	pev->sequence = 1;
	m_animationTimer = pev->health;
	EMIT_SOUND(ENT(pev), CHAN_VOICE, (std::string("info_guide/").append(STRING(pev->message)).append(".wav")).c_str(), 1, ATTN_NORM);
	subtitle = guides[(std::string(STRING(pev->message)))]->first;
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

		const float time_sice_start = pev->health - m_animationTimer;

		if (subtitle != nullptr && time_sice_start > subtitle->startTime)
		{
			auto new_sub = subtitle->next;
			ALERT(at_console, "subtitle \"%s\"at: %f\n", subtitle->text, time_sice_start);
			if (new_sub == nullptr)
			{
				params.holdTime = m_animationTimer;
			}
			else
			{
				ALERT(at_console, "Next subtitle \"%s\" at: %f\n", new_sub->text, new_sub->startTime);

				params.holdTime = (new_sub->startTime - subtitle->startTime);
			}
			ALERT(at_console, "Hold Time: %f\n", params.holdTime);


			UTIL_HudMessageAll(params, subtitle->text);
			subtitle = new_sub;
		}
	}

	pev->nextthink = gpGlobals->time + 0.1;
}