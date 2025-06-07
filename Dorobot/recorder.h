#pragma once
#include <filesystem>
#include "pch.h"
#include "ringbuffer.hpp"
#include "recording.h"
#include "json_converters.h"

class Recorder
{
public:
	Recorder(class Dorobot* doroBot);
	~Recorder();
	void cycle();
	void cycleEditing(refdef_t* refDef);
	void cycleEditingAfterCreatemove();
	void replay();
	void stopReplay();
	Recording* getSelectedRecording();
	void autofixRecording(Vec2<int> trimFrames);
	Vec2<int> autoTrimRecording(int right);
	void loadRecordings();
	void addRecording(const Recording& recording);
	std::vector<Recording> recordings;
	void setSelectedRecordingName(const std::string& name);
	void saveRecord();
	void setSelectedRecording(const Recording& recording);
	void registerBinds();
	bool isPlayingRecording = false;
	static constexpr char recordFolderPath[] = "Dorobot/Recordings/";
	static constexpr char recordingExtension[] = ".doro";

private:
	static constexpr int RINGBUFFER_SIZE = 16384;
	Dorobot* doroBot;
	RingBuffer<CommandPacket, RINGBUFFER_SIZE> packets;
	Recording selectedRecording;
	int atFrame = 0;
	int baseCmdTime = 0;
	void setSelectedRecord();

};
