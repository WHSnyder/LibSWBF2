#include "stdafx.h"
#include "Container.h"
#include "InternalHelpers.h"
#include "Hashing.h"
#include "FileReader.h"
#include "Chunks/BNK/BNK.h"
#include <vector>
#include <future>
#include <map>

namespace LibSWBF2
{
	using LibSWBF2::Chunks::GenericBaseChunk;
	using LibSWBF2::Chunks::LVL::LVL;
	using LibSWBF2::Chunks::BNK::BNK;

	struct LoadStatus
	{
		ELoadStatus m_LoadStatus = ELoadStatus::Uninitialized;
		GenericBaseChunk* m_Chunk = nullptr;
		Level* m_Level = nullptr;
		SoundBank* m_SoundBank = nullptr;
		uint64_t m_FileSize = 0;

#ifdef _DEBUG
		std::string m_LVLPath;
#endif
	};

	class ContainerMembers
	{
	public:
		std::mutex m_StatusLock;
		std::vector<std::future<void>> m_Processes;
		std::vector<LoadStatus> m_Statuses;
		std::vector<Schedule> m_Scheduled;

		// TODO: rework once we allow modifications (add / delete) in Levels
		std::unordered_map<std::string, const Texture*> m_TextureDB;
		std::unordered_map<std::string,	const Model*> m_ModelDB;
		std::unordered_map<std::string, const World*> m_WorldDB;
		std::unordered_map<std::string, const Terrain*> m_TerrainDB;
		std::unordered_map<std::string, const Script*> m_ScriptDB;
		std::unordered_map<std::string, const Light*> m_LightDB;
		std::unordered_map<std::string, const EntityClass*> m_EntityClassDB;

		std::unordered_map<FNVHash, const Sound*> m_SoundDB;
		std::unordered_map<std::string, List<const Localization*>> m_LocalizationDB;

		List<const World*> m_Worlds;
	};

	// const version
	template<class T1, class T2>
	void CopyMap(std::unordered_map<T2, size_t>& levelMap, const List<T1>& list, std::unordered_map<T2, const T1*>& containerMap)
	{
		for (auto& it : levelMap)
		{
			containerMap.emplace(it.first, &list[it.second]);
		}
	}

	// non const version
	template<class T1, class T2>
	void CopyMap(std::unordered_map<T2, size_t>& levelMap, List<T1>& list, std::unordered_map<T2, T1*>& containerMap)
	{
		for (auto& it : levelMap)
		{
			containerMap.emplace(it.first, &list[it.second]);
		}
	}

	template<class T>
	void CopyList(const List<T>& srcList, List<const T*>& dstList)
	{
		for (size_t i = 0; i < srcList.Size(); ++i)
		{
			dstList.Add(&srcList[i]);
		}
	}

	void Container::LoadLevelAsync(const Schedule& scheduled)
	{
		// do not globally lock in order to not block
		// while performing ReadFromFile!

		LVL* lvl = nullptr;
		{
			LOCK(m_ThreadSafeMembers->m_StatusLock);
			LoadStatus& status = m_ThreadSafeMembers->m_Statuses[scheduled.m_Handle];

			FileReader reader;
			if (!reader.Open(scheduled.m_Path))
			{
				status.m_LoadStatus = ELoadStatus::Failed;
				return;
			}
			else
			{
				status.m_FileSize = reader.GetFileSize();
				reader.Close();
			}

			lvl = LVL::Create();
			status.m_Chunk = lvl;
			status.m_LoadStatus = ELoadStatus::Loading;
		}

		if (lvl->ReadFromFile(scheduled.m_Path, scheduled.m_SubLVLsToLoad.Size() > 0 ? &scheduled.m_SubLVLsToLoad : nullptr))
		{
			Level* level = Level::FromChunk(lvl, this);
			LOCK(m_ThreadSafeMembers->m_StatusLock);
			LoadStatus& status = m_ThreadSafeMembers->m_Statuses[scheduled.m_Handle];
			status.m_Level = level;
			if (level != nullptr)
			{
				if (scheduled.bRegisterContents)
				{
					CopyMap(level->m_NameToIndexMaps->TextureNameToIndex,		level->m_Textures,		m_ThreadSafeMembers->m_TextureDB);
					CopyMap(level->m_NameToIndexMaps->ModelNameToIndex,			level->m_Models,		m_ThreadSafeMembers->m_ModelDB);
					CopyMap(level->m_NameToIndexMaps->WorldNameToIndex,			level->m_Worlds,		m_ThreadSafeMembers->m_WorldDB);
					CopyMap(level->m_NameToIndexMaps->TerrainNameToIndex,		level->m_Terrains,		m_ThreadSafeMembers->m_TerrainDB);
					CopyMap(level->m_NameToIndexMaps->ScriptNameToIndex,		level->m_Scripts,		m_ThreadSafeMembers->m_ScriptDB);
					CopyMap(level->m_NameToIndexMaps->LightNameToIndex,			level->m_Lights,		m_ThreadSafeMembers->m_LightDB);
					CopyMap(level->m_NameToIndexMaps->EntityClassTypeToIndex,	level->m_EntityClasses, m_ThreadSafeMembers->m_EntityClassDB);
					CopyList(level->m_Worlds, m_ThreadSafeMembers->m_Worlds);

					for (auto& it : level->m_NameToIndexMaps->LocalizationNameToIndex)
					{
						auto find = m_ThreadSafeMembers->m_LocalizationDB.find(it.first);
						if (find == m_ThreadSafeMembers->m_LocalizationDB.end())
						{
							List<const Localization*> list;
							list.Add(&level->m_Localizations[it.second]);
							m_ThreadSafeMembers->m_LocalizationDB.emplace(it.first, list);
						}
						else
						{
							find->second.Add(&level->m_Localizations[it.second]);
						}
					}
				}

				level->m_FullPath = scheduled.m_Path;
				status.m_LoadStatus = ELoadStatus::Loaded;
			}
			else
			{
				status.m_LoadStatus = ELoadStatus::Failed;
				LVL::Destroy(lvl);
			}
		}
		else
		{
			LOCK(m_ThreadSafeMembers->m_StatusLock);
			LoadStatus& status = m_ThreadSafeMembers->m_Statuses[scheduled.m_Handle];
			status.m_LoadStatus = ELoadStatus::Failed;
			LVL::Destroy(lvl);
		}
		LOCK(m_ThreadSafeMembers->m_StatusLock);
		LoadStatus& status = m_ThreadSafeMembers->m_Statuses[scheduled.m_Handle];
		status.m_Chunk = nullptr;
	}

	void Container::LoadSoundBankAsync(const Schedule& scheduled)
	{
		// do not globally lock in order to not block
		// while performing ReadFromFile!

		BNK* bnk = nullptr;
		{
			LOCK(m_ThreadSafeMembers->m_StatusLock);
			LoadStatus& status = m_ThreadSafeMembers->m_Statuses[scheduled.m_Handle];

			FileReader reader;
			if (!reader.Open(scheduled.m_Path))
			{
				status.m_LoadStatus = ELoadStatus::Failed;
				return;
			}
			else
			{
				status.m_FileSize = reader.GetFileSize();
				reader.Close();
			}

			bnk = BNK::Create();
			status.m_Chunk = bnk;
			status.m_LoadStatus = ELoadStatus::Loading;
		}

		if (bnk->ReadFromFile(scheduled.m_Path))
		{
			SoundBank* soundBank = SoundBank::FromChunk(bnk);
			LOCK(m_ThreadSafeMembers->m_StatusLock);
			LoadStatus& status = m_ThreadSafeMembers->m_Statuses[scheduled.m_Handle];
			status.m_SoundBank = soundBank;
			if (soundBank != nullptr && scheduled.bRegisterContents)
			{
				CopyMap(soundBank->m_NameToIndexMaps->SoundHashToIndex, soundBank->m_Sounds, m_ThreadSafeMembers->m_SoundDB);
				status.m_LoadStatus = ELoadStatus::Loaded;
			}
			else if (soundBank == nullptr)
			{
				status.m_LoadStatus = ELoadStatus::Failed;
				BNK::Destroy(bnk);
			}
		}
		else
		{
			LOCK(m_ThreadSafeMembers->m_StatusLock);
			LoadStatus& status = m_ThreadSafeMembers->m_Statuses[scheduled.m_Handle];
			status.m_LoadStatus = ELoadStatus::Failed;
			BNK::Destroy(bnk);
		}
		LOCK(m_ThreadSafeMembers->m_StatusLock);
		LoadStatus& status = m_ThreadSafeMembers->m_Statuses[scheduled.m_Handle];
		status.m_Chunk = nullptr;
	}

	Container::Container()
	{
		m_ThreadSafeMembers = new ContainerMembers();
	}

	Container::~Container()
	{
		FreeAll(true);
		delete m_ThreadSafeMembers;
		m_ThreadSafeMembers = nullptr;
	}

	Container* Container::Create()
	{
		return new Container();
	}

	void Container::Delete(Container* instance)
	{
		delete instance;
	}

	SWBF2Handle Container::AddLevel(const String& path, const List<String>* subLVLsToLoad, bool bRegisterContents)
	{
		LOCK(m_ThreadSafeMembers->m_StatusLock);
		LoadStatus& status = m_ThreadSafeMembers->m_Statuses.emplace_back();
#ifdef _DEBUG
		status.m_LVLPath = path.Buffer();
#endif
		SWBF2Handle handle = (SWBF2Handle)m_ThreadSafeMembers->m_Statuses.size() - 1;
		m_ThreadSafeMembers->m_Scheduled.push_back(
		{ 
			handle, 
			path, 
			subLVLsToLoad != nullptr ? *subLVLsToLoad : List<String>(), 
			false, 
			bRegisterContents
		});
		return handle;
	}

	SWBF2Handle Container::AddSoundBank(const String& path, bool bRegisterContents)
	{
		LOCK(m_ThreadSafeMembers->m_StatusLock);
		LoadStatus& status = m_ThreadSafeMembers->m_Statuses.emplace_back();
#ifdef _DEBUG
		status.m_LVLPath = path.Buffer();
#endif
		SWBF2Handle handle = (SWBF2Handle)m_ThreadSafeMembers->m_Statuses.size() - 1;
		m_ThreadSafeMembers->m_Scheduled.push_back(
		{ 
			handle,
			path, 
			List<String>(), 
			true, 
			bRegisterContents 
		});
		return handle;
	}

	void Container::StartLoading()
	{
		LOCK(m_ThreadSafeMembers->m_StatusLock);
		if (m_ThreadSafeMembers->m_Scheduled.size() == 0)
		{
			LOG_WARN("No levels scheduled to load!");
			return;
		}

		m_OverallSize = 0;
		for (Schedule& scheduled : m_ThreadSafeMembers->m_Scheduled)
		{
			if (scheduled.m_bIsSoundBank)
			{
				m_ThreadSafeMembers->m_Processes.push_back(std::async(std::launch::async, &Container::LoadSoundBankAsync, this, scheduled));
			}
			else
			{
				m_ThreadSafeMembers->m_Processes.push_back(std::async(std::launch::async, &Container::LoadLevelAsync, this, scheduled));
			}
		}
		m_ThreadSafeMembers->m_Scheduled.clear();
	}

	void Container::FreeAll(bool bForce)
	{
		if (!bForce && m_ThreadSafeMembers->m_Statuses.size() == 0)
		{
			LOG_WARN("Nothing to free!");
			return;
		}

		if (!IsDone())
		{
			if (bForce)
			{
				for (std::future<void>& process : m_ThreadSafeMembers->m_Processes)
				{
					// join all processes... you cannot just abort another thread in c++, see:
					// https://stackoverflow.com/questions/13678155/cancel-a-c-11-async-task
					process.wait();
				}
			}
			else
			{
				LOG_WARN("Cannot free while still running!");
				return;
			}
		}

		{
			LOCK(m_ThreadSafeMembers->m_StatusLock);
			for (LoadStatus& status : m_ThreadSafeMembers->m_Statuses)
			{
				if (status.m_Level != nullptr)
				{
					Level::Destroy(status.m_Level);
					status.m_Level = nullptr;
				}
				if (status.m_SoundBank != nullptr)
				{
					SoundBank::Destroy(status.m_SoundBank);
					status.m_SoundBank = nullptr;
				}
			}
		}

		delete m_ThreadSafeMembers;
		m_ThreadSafeMembers = new ContainerMembers();
	}

	bool Container::IsDone() const
	{
		LOCK(m_ThreadSafeMembers->m_StatusLock);
		if (m_ThreadSafeMembers->m_Statuses.size() == 0)
		{
			return true;
		}

		bool bIsDone = true;
		for (LoadStatus& status : m_ThreadSafeMembers->m_Statuses)
		{
			bIsDone = status.m_LoadStatus != ELoadStatus::Loading && status.m_LoadStatus != ELoadStatus::Uninitialized && bIsDone;
		}
		return bIsDone;
	}

	List<SWBF2Handle> Container::GetLoadedLevels() const
	{
		List<SWBF2Handle> handles;
		LOCK(m_ThreadSafeMembers->m_StatusLock);
		for (size_t i = 0; i < m_ThreadSafeMembers->m_Statuses.size(); ++i)
		{
			if (m_ThreadSafeMembers->m_Statuses[i].m_Level != nullptr)
			{
				handles.Add((SWBF2Handle)i);
			}
		}
		return handles;
	}

	ELoadStatus Container::GetLevelStatus(SWBF2Handle handle) const
	{
		if (handle >= m_ThreadSafeMembers->m_Processes.size())
		{
			LOG_WARN("Given Level Handle '{}' is illegal!", handle);
			return ELoadStatus::Uninitialized;
		}

		LOCK(m_ThreadSafeMembers->m_StatusLock);
		return m_ThreadSafeMembers->m_Statuses[handle].m_LoadStatus;
	}

	float_t Container::GetLevelProgress(SWBF2Handle handle) const
	{
		if (handle >= m_ThreadSafeMembers->m_Processes.size())
		{
			LOG_WARN("Given Level Handle '{}' is illegal!", handle);
			return 0.0f;
		}

		LOCK(m_ThreadSafeMembers->m_StatusLock);
		LoadStatus& status = m_ThreadSafeMembers->m_Statuses[handle];
		if (status.m_LoadStatus != ELoadStatus::Loading)
		{
			return 1.0f;
		}
		return status.m_Chunk->GetReadingProgress();
	}

	Level* Container::GetLevel(SWBF2Handle handle) const
	{
		if (handle >= m_ThreadSafeMembers->m_Processes.size())
		{
			LOG_WARN("Given Level Handle '{}' is illegal!", handle);
			return nullptr;
		}

		LOCK(m_ThreadSafeMembers->m_StatusLock);
		return m_ThreadSafeMembers->m_Statuses[handle].m_Level;
	}

	Level* Container::TryGetWorldLevel() const
	{
		LOCK(m_ThreadSafeMembers->m_StatusLock);
		size_t num = m_ThreadSafeMembers->m_Statuses.size();
		if (num == 0)
		{
			return nullptr;
		}
		for (size_t i = 0; i < num; ++i)
		{
			LoadStatus& status = m_ThreadSafeMembers->m_Statuses[i];
			if (status.m_LoadStatus == ELoadStatus::Loaded && status.m_Level != nullptr && status.m_Level->IsWorldLevel())
			{
				return status.m_Level;
			}
		}
		return nullptr;
	}

	float_t Container::GetOverallProgress()
	{
		LOCK(m_ThreadSafeMembers->m_StatusLock);
		size_t num = m_ThreadSafeMembers->m_Statuses.size();
		if (m_OverallSize == 0)
		{
			for (size_t i = 0; i < num; ++i)
			{
				LoadStatus& status = m_ThreadSafeMembers->m_Statuses[i];
				if (status.m_LoadStatus == ELoadStatus::Uninitialized)
				{
					// wait until all tasks started
					m_OverallSize = 0;
					return 0.0f;
				}
				m_OverallSize += status.m_FileSize;
			}
		}

		float_t progress = 0.0f;
		for (size_t i = 0; i < num; ++i)
		{
			LoadStatus& status = m_ThreadSafeMembers->m_Statuses[i];
			if (status.m_FileSize > 0)
			{
				float lvlProgress = status.m_Chunk != nullptr ? status.m_Chunk->GetReadingProgress() : 1.0f;
				lvlProgress *= status.m_FileSize / (float_t)m_OverallSize;
				progress += lvlProgress;
			}
		}
		return progress;
	}

	const List<const World*>& Container::GetWorlds()
	{
		if (!IsDone())
		{
			LOG_WARN("Cannot lookup all Worlds while still loading!");
			static const List<const World*> tmp;
			return tmp;
		}
		return m_ThreadSafeMembers->m_Worlds;
	}

	const Light* Container::FindLight(String lightName) const
	{
		if (lightName.IsEmpty())
		{
			return nullptr;
		}

		LOCK(m_ThreadSafeMembers->m_StatusLock);
		auto it = m_ThreadSafeMembers->m_LightDB.find(ToLower(lightName));
		if (it != m_ThreadSafeMembers->m_LightDB.end())
		{
			return it->second;
		}

		return nullptr;
	}

	const Model* Container::FindModel(String modelName) const
	{
		if (modelName.IsEmpty())
		{
			return nullptr;
		}

		LOCK(m_ThreadSafeMembers->m_StatusLock);
		auto it = m_ThreadSafeMembers->m_ModelDB.find(ToLower(modelName));
		if (it != m_ThreadSafeMembers->m_ModelDB.end())
		{
			return it->second;
		}

		return nullptr;
	}

	const Texture* Container::FindTexture(String textureName) const
	{
		if (textureName.IsEmpty())
		{
			return nullptr;
		}

		LOCK(m_ThreadSafeMembers->m_StatusLock);
		auto it = m_ThreadSafeMembers->m_TextureDB.find(ToLower(textureName));
		if (it != m_ThreadSafeMembers->m_TextureDB.end())
		{
			return it->second;
		}

		return nullptr;
	}

	const World* Container::FindWorld(String worldName) const
	{
		if (worldName.IsEmpty())
		{
			return nullptr;
		}

		LOCK(m_ThreadSafeMembers->m_StatusLock);
		auto it = m_ThreadSafeMembers->m_WorldDB.find(ToLower(worldName));
		if (it != m_ThreadSafeMembers->m_WorldDB.end())
		{
			return it->second;
		}

		return nullptr;
	}

	const Terrain* Container::FindTerrain(String terrainName) const
	{
		if (terrainName.IsEmpty())
		{
			return nullptr;
		}

		LOCK(m_ThreadSafeMembers->m_StatusLock);
		auto it = m_ThreadSafeMembers->m_TerrainDB.find(ToLower(terrainName));
		if (it != m_ThreadSafeMembers->m_TerrainDB.end())
		{
			return it->second;
		}

		return nullptr;
	}

	const Script* Container::FindScript(String scriptName) const
	{
		if (scriptName.IsEmpty())
		{
			return nullptr;
		}

		LOCK(m_ThreadSafeMembers->m_StatusLock);
		auto it = m_ThreadSafeMembers->m_ScriptDB.find(ToLower(scriptName));
		if (it != m_ThreadSafeMembers->m_ScriptDB.end())
		{
			return it->second;
		}

		return nullptr;
	}

	const List<const Localization*>* Container::FindLocalizations(String languageName) const
	{
		if (languageName.IsEmpty())
		{
			return nullptr;
		}

		LOCK(m_ThreadSafeMembers->m_StatusLock);
		auto it = m_ThreadSafeMembers->m_LocalizationDB.find(ToLower(languageName));
		if (it != m_ThreadSafeMembers->m_LocalizationDB.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	const EntityClass* Container::FindEntityClass(String typeName) const
	{
		if (typeName.IsEmpty())
		{
			return nullptr;
		}

		LOCK(m_ThreadSafeMembers->m_StatusLock);
		auto it = m_ThreadSafeMembers->m_EntityClassDB.find(ToLower(typeName));
		if (it != m_ThreadSafeMembers->m_EntityClassDB.end())
		{
			return it->second;
		}

		return nullptr;
	}

	const Sound* Container::FindSound(String soundName) const
	{
		if (soundName.IsEmpty())
		{
			return nullptr;
		}
		return FindSound(FNV::Hash(soundName));
	}

	const Sound* Container::FindSound(FNVHash hashedSoundName) const
	{
		LOCK(m_ThreadSafeMembers->m_StatusLock);
		auto it = m_ThreadSafeMembers->m_SoundDB.find(hashedSoundName);
		if (it != m_ThreadSafeMembers->m_SoundDB.end())
		{
			return it->second;
		}
		return nullptr;
	}

	bool Container::GetLocalizedWideString(const String& language, const String& path, uint16_t*& chars, uint32_t& count) const
	{
		const List<const Localization*>* locals = FindLocalizations(language);
		if (locals == nullptr)
		{
			LOG_WARN("Cannot find language '{}'!", language);
			return false;
		}
		for (size_t i = 0; i < locals->Size(); ++i)
		{
			if ((*locals)[i]->GetLocalizedWideString(path, chars, count))
			{
				return true;
			}
		}
		return false;
	}
}