#include "reload.h"

SpellItemVectorClass* SpellItemVectorClass::GetSingleton()
{
	static SpellItemVectorClass singleton;
	return std::addressof(singleton);
}

void SpellItemVectorClass::resetSpells(std::shared_ptr<spdlog::logger> r)
{
	if (!_spellItemFormIDVector.empty() && !_spellItemGravityVector.empty() && !_spellItemSpeedVector.empty()) {
		for (const auto* spell : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::SpellItem>()) {
			if (spell) {
				if (const auto* sES = spell->GetAVEffect(); sES) {
					if (auto* sP = sES->data.projectileBase; sP) {
						for (decltype(_spellItemFormIDVector.size()) i = 0; i < _spellItemFormIDVector.size(); i++) {
							if (spell->GetFormID() == _spellItemFormIDVector[i]) {
								sP->data.speed = _spellItemSpeedVector[i];
								sP->data.gravity = _spellItemGravityVector[i];
								r->debug(" set spell with FormID : {:08X} speed = {} , gravity = {}", sP->GetFormID(), sP->data.speed, sP->data.gravity);
							}
						}
					}
				}
			}
		}
	}
}

void SpellItemVectorClass::loadVectors()
{
	if (_spellItemFormIDVector.empty() && _spellItemGravityVector.empty() && _spellItemSpeedVector.empty()) {
		auto& BSTSpellItemArray = RE::TESDataHandler::GetSingleton()->GetFormArray<RE::SpellItem>();
		auto size = BSTSpellItemArray.size();

		_spellItemFormIDVector.reserve(size);
		_spellItemGravityVector.reserve(size);
		_spellItemSpeedVector.reserve(size);

		for (const auto* spells : BSTSpellItemArray) {
			if (spells) {
				if (const auto* spellEffectSettings = spells->GetAVEffect(); spellEffectSettings) {
					if (const auto* spellProjectiles = spellEffectSettings->data.projectileBase; spellProjectiles) {
						auto ak = spells->GetRawFormID();
						_spellItemFormIDVector.push_back(ak);
						auto dj = spellProjectiles->data.gravity;
						_spellItemGravityVector.push_back(dj);
						auto djs = spellProjectiles->data.speed;
						_spellItemSpeedVector.push_back(djs);
					}
				}
			}
		}
		_spellItemFormIDVector.shrink_to_fit();
		_spellItemGravityVector.shrink_to_fit();
		_spellItemSpeedVector.shrink_to_fit();
	}
}
