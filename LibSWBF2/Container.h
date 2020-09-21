#pragma once
#include "Wrappers/Level.h"
#include "Wrappers/SoundBank.h"
#include "Types/LibString.h"
#include "Types/List.h"
#include "Types/Enums.h"

namespace LibSWBF2
{
	using LibSWBF2::Types::String;
	using LibSWBF2::Types::List;
	using LibSWBF2::Wrappers::Level;
	using LibSWBF2::Wrappers::SoundBank;
	using LibSWBF2::Wrappers::Sound;
	using LibSWBF2::Wrappers::Model;
	using LibSWBF2::Wrappers::Texture;
	using LibSWBF2::Wrappers::World;
	using LibSWBF2::Wrappers::Terrain;
	using LibSWBF2::Wrappers::Script;
	using LibSWBF2::Wrappers::Light;
	using LibSWBF2::Wrappers::Localization;
	using LibSWBF2::Wrappers::EntityClass;

	struct Schedule
	{
		String m_Path;
		List<String> m_SubLVLsToLoad;
		bool m_bIsSoundBank = false;
		bool bRegisterContents = true;
	};

	class LIBSWBF2_API Container
	{
	private:
		Container();
		~Container();

		class ContainerMembers* m_ThreadSafeMembers = nullptr;
		uint64_t m_OverallSize = 0;

		void LoadLevelAsync(size_t index, const Schedule& scheduled);
		void LoadSoundBankAsync(size_t index, const Schedule& scheduled);

	public:
		static Container* Create();
		static void Delete(Container* instance);

		SWBF2Handle AddLevel(const String& path, const List<String>* subLVLsToLoad = nullptr, bool bRegisterContents=true);
		SWBF2Handle AddSoundBank(const String& path, bool bRegisterContents = true);
		void StartLoading();
		void FreeAll();
		bool IsDone() const;
		ELoadStatus GetLevelStatus(SWBF2Handle handle) const;
		float_t GetLevelProgress(SWBF2Handle handle) const;
		Level* GetLevel(SWBF2Handle handle) const;
		float_t GetOverallProgress();

		// will return the first encountered world LVL, if existent
		Level* TryGetWorldLevel() const;

		const List<const World*>& GetWorlds();

		const Light* FindLight(String lightName) const;
		const Model* FindModel(String modelName) const;
		const Texture* FindTexture(String textureName) const;
		const World* FindWorld(String worldName) const;
		const Terrain* FindTerrain(String terrainName) const;
		const Script* FindScript(String scriptName) const;
		const Localization* FindLocalization(String loclName) const;
		const EntityClass* FindEntityClass(String typeName) const;
		const Sound* FindSound(String soundName) const;
		const Sound* FindSound(FNVHash hashedSoundName) const;

		// count is number of wide chars, NOT number of bytes!
		bool GetLocalizedWideString(const String& language, const String& path, uint16_t*& chars, uint32_t& count) const;
	};
}