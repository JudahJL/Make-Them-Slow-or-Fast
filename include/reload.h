#pragma once

class SpellItemVectorClass
{  // used to store all spell data
public:
	static SpellItemVectorClass* GetSingleton();

	void resetSpells(std::shared_ptr<spdlog::logger> r);

	void loadVectors();

private:
	std::vector<RE::FormID> _spellItemFormIDVector;
	std::vector<float>      _spellItemGravityVector;
	std::vector<float>      _spellItemSpeedVector;
};
