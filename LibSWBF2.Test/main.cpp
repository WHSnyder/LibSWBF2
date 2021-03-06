// LibSWBF2.Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "../LibSWBF2/LibSWBF2.h"
#include "../LibSWBF2/FileWriter.h"
#include <iostream>
#include <fstream>
#include <string>

using LibSWBF2::Types::String;
using LibSWBF2::Types::List;
using LibSWBF2::Chunks::MSH::MSH;
using LibSWBF2::Wrappers::SoundBank;
using LibSWBF2::Wrappers::Sound;
using LibSWBF2::Logging::Logger;
using LibSWBF2::Logging::LoggerEntry;
using LibSWBF2::FileWriter;

void libLog(const LoggerEntry* log)
{
	//std::cout << log->ToString().Buffer() << std::endl;
}

int main()
{
	Logger::SetLogCallback(&libLog);

	SoundBank* soundBank = SoundBank::FromFile("F:\\SteamLibrary\\steamapps\\common\\Star Wars Battlefront II\\GameData\\data\\_lvl_pc\\sound\\common.bnk");

	//if (soundBank == nullptr)
	//{
	//	std::cout << "ERROR" << std::endl;
	//	return 0;
	//}

	//const List<Sound>& sounds = soundBank->GetSounds();
	//for (size_t i = 0; i < sounds.Size(); ++i)
	//{
	//	String name = sounds[i].GetName();
	//	uint32_t sampleRate;
	//	uint32_t sampleCount;
	//	uint8_t blockAlign;
	//	const uint8_t* data;

	//	if (!sounds[i].GetData(sampleRate, sampleCount, blockAlign, data))
	//	{
	//		std::cout << "Error reading clip no " << i << " '" << name.Buffer() << "'" << std::endl;
	//		continue;
	//	}

	//	std::string clipName = "[" +std::to_string(i) + "] " + std::string(name.Buffer()) + ".wav";
	//	std::cout << "Exporting clip " << clipName << std::endl;

	//	uint32_t dataSize = sampleCount * blockAlign;
	//	uint32_t fileSize = dataSize + 36;

	//	FileWriter wav;
	//	wav.Open(("F:\\SteamLibrary\\steamapps\\common\\Star Wars Battlefront II\\GameData\\data\\_lvl_pc\\sound\\common\\" + clipName).c_str());
	//	wav.WriteString("RIFF", 4);
	//	wav.WriteUInt32(fileSize);
	//	wav.WriteString("WAVE", 4);
	//	wav.WriteString("fmt ", 4);
	//	wav.WriteUInt32(16);				// fmt header size
	//	wav.WriteUInt16(1);					// PCM format
	//	wav.WriteUInt16(1);					// num of audio channels
	//	wav.WriteUInt32(sampleRate);
	//	wav.WriteUInt32(sampleRate * 2);	// frame size
	//	wav.WriteUInt16(blockAlign);
	//	wav.WriteUInt16(16);				// bit depth
	//	wav.WriteString("data", 4);
	//	wav.WriteUInt32(dataSize);
	//	wav.WriteBytes(data, dataSize);

	//	wav.Close();
	//}

	//SoundBank::Destroy(soundBank);
	return 0;
}