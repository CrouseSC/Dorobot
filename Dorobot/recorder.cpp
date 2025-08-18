#include "pch.h"
#include "recorder.h"

Recorder::Recorder(Dorobot* doroBot)
{
	this->doroBot = doroBot;
	loadRecordings();
}

Recorder::~Recorder()
{
}

void Recorder::cycle()  //CALLED BEFORE CREATEMOVE
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s cmd = *input->GetUserCmd(input->currentCmdNum);
	playerState_s* ps = doroBot->game->getPmoveCurrent()->ps;
	if (!ps) return;

	Vec3<float> origin = ps->origin;
	Vec3<float> velocity = ps->velocity;
	cmd.angles[0] += ANGLE2SHORT(ps->DeltaAngles.x);
	cmd.angles[1] += ANGLE2SHORT(ps->DeltaAngles.y);
	cmd.angles[2] += ANGLE2SHORT(ps->DeltaAngles.z);

	packets.add(CommandPacket(cmd, origin, velocity, doroBot->game->getView()));

	static bool saved = false;
	if (doroBot->bindManager->bindActive("Save recording")) {
		setSelectedRecord();
		saved = true;
	}
}

void Recorder::registerBinds()
{
	doroBot->bindManager->registerBindName("Save recording", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Play recording", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Stop playing recording", BIND_TYPE_HOLD);
}

void Recorder::renderProgressbar()
{
	if (doroBot->recorder->isPlayingRecording && doroBot->uiMenu->progressbar_toggle) {
		ImGui::SetNextWindowBgAlpha(0.f);
		ImGui::SetNextWindowSize(ImVec2(300, 30));
		ImGui::Begin("ProgressBar", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration);
		ImGui::ProgressBar( (float)doroBot->recorder->atFrame / (float)doroBot->recorder->getSelectedRecording()->packets.size());
		ImGui::End();
	}
}

void Recorder::cycleEditing(refdef_t* refDef)
{
	/*doroBot->uiDebug->addDebuginfo("X", doroBot->game->getOrigin().x);
	doroBot->uiDebug->addDebuginfo("Y", doroBot->game->getOrigin().y);
	doroBot->uiDebug->addDebuginfo("Z", doroBot->game->getOrigin().z);
	doroBot->uiDebug->addDebuginfo("Xcam", refDef->Origin.x);
	doroBot->uiDebug->addDebuginfo("Ycam", refDef->Origin.y);
	doroBot->uiDebug->addDebuginfo("Zcam", refDef->Origin.z);*/
	if (doroBot->uiMenu->isEditing) {  //ideally we'd set viewaxis here, but idk how
		CommandPacket packet = selectedRecording.packets[doroBot->uiMenu->selectedFrame-1];
		refDef->Origin = packet.origin;
		refDef->Origin.z += 60.f;
	}
}

void Recorder::cycleEditingAfterCreatemove()
{
	if (doroBot->uiMenu->isEditing) {
		CommandPacket packet = selectedRecording.packets[doroBot->uiMenu->selectedFrame - 1];
		doroBot->game->setNormalView(packet.normalView);
		doroBot->game->getCg_t()->PredictedPlayerState.origin = packet.origin;
		doroBot->game->getCg_t()->CurrentSnap->ps.origin = packet.origin;
	}

	if (doroBot->bindManager->bindActive("Play recording") && !doroBot->uiMenu->isEditing) {
		replay();
	}
	if (doroBot->bindManager->bindActive("Stop playing recording")) {
		stopReplay();
	}

	if (isPlayingRecording) {
		input_s* input = doroBot->game->getInput_s();
		usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

		if (selectedRecording.packets.size() == 0) {
			isPlayingRecording = false;
			return;
		}

		if (atFrame == 0 && !doroBot->positioning->moveToPosition(mm::truncate_vector(selectedRecording.packets[0].origin), 0.005f)) {
			return;
		}

		if (selectedRecording.packets.size() != atFrame) {
			if (atFrame == 0) {
				baseCmdTime = cmd->serverTime;
			}
			CommandPacket packet = selectedRecording.packets[atFrame];
			*cmd = packet.cmd;
			Vec3<float> angles = doroBot->game->toCodAngles(packet.normalView);
			doroBot->game->setView(angles);
			cmd->angles[0] -= ANGLE2SHORT(doroBot->game->getPmoveCurrent()->ps->DeltaAngles.x);
			cmd->angles[1] -= ANGLE2SHORT(doroBot->game->getPmoveCurrent()->ps->DeltaAngles.y);
			cmd->angles[2] -= ANGLE2SHORT(doroBot->game->getPmoveCurrent()->ps->DeltaAngles.z);
			cmd->serverTime = baseCmdTime;
			if (atFrame != 0) {
				int msec = packet.cmd.serverTime - selectedRecording.packets[atFrame - 1].cmd.serverTime;
				//doroBot->uiDebug->addDebuginfo("msec", msec);
				cmd->serverTime += (packet.cmd.serverTime - selectedRecording.packets[atFrame - 1].cmd.serverTime);
				doroBot->game->setFps((int)(1000/msec));
				baseCmdTime = cmd->serverTime;
			}
			atFrame++;

			if (selectedRecording.packets.size() == atFrame) {
				atFrame = 0;
				isPlayingRecording = false;
			}
		}
	}
}

void Recorder::replay()
{
	isPlayingRecording = true;
}

void Recorder::stopReplay()
{
	isPlayingRecording = false;
	atFrame = 0;
	baseCmdTime = 0;
}

Recording* Recorder::getSelectedRecording()
{
	return &selectedRecording;
}

void Recorder::autofixRecording(Vec2<int> trimFrames)
{
	selectedRecording.packets = std::vector<CommandPacket>(selectedRecording.packets.begin() + trimFrames.x, selectedRecording.packets.begin() + trimFrames.y + 1);
	updateSelectedRecordingPoints();
}

Vec2<int> Recorder::autoTrimRecording(int right)
{
	int startPos = 0;
	int endPos = right;
	if (selectedRecording.packets[right].velocity.Length3D() == 0) {
		for (int i = right; i >= 0; i--) {
			if (selectedRecording.packets[i].velocity.Length3D() != 0) {
				right = i;
				endPos = i;
				break;
			}
		}
	}
	for (int i = right; i >= 0; i--) {
		if (selectedRecording.packets[i].velocity.Length3D() == 0) {
			startPos = i;
			break;
		}
	}

	return Vec2<int>(startPos, endPos);
}

void Recorder::loadRecordings()
{
	namespace fs = std::filesystem;

	for (auto& p : fs::recursive_directory_iterator(recordFolderPath)) {
		if (p.path().extension() == recordingExtension) {
			try {
				std::ifstream file(p.path());
				std::string line;
				std::getline(file, line);
				if (line == "1.0") {  //version
					Recording recording;
					std::getline(file, recording.name);
					std::getline(file, recording.author);
					std::getline(file, recording.mapName);
					CommandPacket packet;

					while (std::getline(file, line)) {
						packet.cmd.angles[0] = std::stoi(line);
						std::getline(file, line);
						packet.cmd.angles[1] = std::stoi(line);
						std::getline(file, line);
						packet.cmd.angles[2] = std::stoi(line);
						std::getline(file, line);
						packet.cmd.buttons = std::stoi(line);
						std::getline(file, line);
						packet.cmd.forward = std::stoi(line);
						std::getline(file, line);
						packet.cmd.meleeChargeDist = std::stoi(line);
						std::getline(file, line);
						packet.cmd.meleeChargeYaw = std::stof(line);
						std::getline(file, line);
						packet.cmd.offHandIndex = std::stoi(line);
						std::getline(file, line);
						packet.cmd.selectedLocation[0] = std::stoi(line);
						std::getline(file, line);
						packet.cmd.selectedLocation[1] = std::stoi(line);
						std::getline(file, line);
						packet.cmd.serverTime = std::stoi(line);
						std::getline(file, line);
						packet.cmd.side = std::stoi(line);
						std::getline(file, line);
						packet.cmd.weapon = std::stoi(line);
						std::getline(file, line);
						packet.origin.x = std::stof(line);
						std::getline(file, line);
						packet.origin.y = std::stof(line);
						std::getline(file, line);
						packet.origin.z = std::stof(line);
						std::getline(file, line);
						packet.velocity.x = std::stof(line);
						std::getline(file, line);
						packet.velocity.y = std::stof(line);
						std::getline(file, line);
						packet.velocity.z = std::stof(line);
						std::getline(file, line);
						packet.normalView.x = std::stof(line);
						std::getline(file, line);
						packet.normalView.y = std::stof(line);
						std::getline(file, line);
						packet.normalView.z = std::stof(line);
						recording.packets.push_back(packet);
					}
					recordings.push_back(recording);
				}
			}
			catch (const std::exception& e) {
				std::cerr << "Failed to parse recording: " << e.what() << std::endl;
				continue;
			}
		}
	}
}

void Recorder::addRecording(const Recording& recording)
{
	for (Recording& rec : recordings) {
		if (rec.name == recording.name) {
			rec = recording;
			return;
		}
	}
	recordings.push_back(recording);
}

void Recorder::setSelectedRecordingName(const std::string& name)
{
	this->selectedRecording.name = name;
}

void Recorder::saveRecord()
{
	std::string path = recordFolderPath + selectedRecording.name + recordingExtension;
	std::ofstream out(path);
	out << "1.0" << std::endl;
	out << selectedRecording.name << std::endl;
	out << selectedRecording.author << std::endl;
	out << selectedRecording.mapName << std::endl;
	
	for (auto& packet : selectedRecording.packets) {
		if (packet.cmd.serverTime == 0) continue;  //at the start of a session the servertime is 0
		out << std::to_string(packet.cmd.angles[0]) << std::endl;
		out << std::to_string(packet.cmd.angles[1]) << std::endl;
		out << std::to_string(packet.cmd.angles[2]) << std::endl;
		out << std::to_string(packet.cmd.buttons) << std::endl;
		out << std::to_string(packet.cmd.forward) << std::endl;
		out << std::to_string(packet.cmd.meleeChargeDist) << std::endl;
		out << std::to_string(packet.cmd.meleeChargeYaw) << std::endl;
		out << std::to_string(packet.cmd.offHandIndex) << std::endl;
		out << std::to_string(packet.cmd.selectedLocation[0]) << std::endl;
		out << std::to_string(packet.cmd.selectedLocation[1]) << std::endl;
		out << std::to_string(packet.cmd.serverTime) << std::endl;
		out << std::to_string(packet.cmd.side) << std::endl;
		out << std::to_string(packet.cmd.weapon) << std::endl;
		out << std::to_string(packet.origin.x) << std::endl;
		out << std::to_string(packet.origin.y) << std::endl;
		out << std::to_string(packet.origin.z) << std::endl;
		out << std::to_string(packet.velocity.x) << std::endl;
		out << std::to_string(packet.velocity.y) << std::endl;
		out << std::to_string(packet.velocity.z) << std::endl;
		out << std::to_string(packet.normalView.x) << std::endl;
		out << std::to_string(packet.normalView.y) << std::endl;
		out << std::to_string(packet.normalView.z) << std::endl;
	}

	addRecording(selectedRecording);
}

void Recorder::setSelectedRecording(const Recording& recording)
{
	selectedRecording = recording;
	updateSelectedRecordingPoints();
}

void Recorder::setSelectedRecord()  //TODO: make Dorobot, Recordings folders if they don't exist probably right after dll entry
{
	Recording recording;
	recording.author = doroBot->game->getPlayerName();
	recording.mapName = doroBot->game->getMapName();
	recording.name = "ots_hard_roof";
	recording.packets = packets.readAll();

	selectedRecording = recording;
	updateSelectedRecordingPoints();
}

void Recorder::renderTrail()
{
	constexpr float MAX_DIST = 5000.f;

	if (!doroBot->uiMenu->renderTrail_toggle)
		return;

	if (selectedRecordingPoints.size() < 2)
		return;

	std::unique_lock<std::mutex> lock(pointMutex);
	std::vector<Vec3<float>> points = selectedRecordingPoints;  //the vector can be accessed from multiple threads so we make a copy
	lock.unlock();

	for (int i = 0; i < points.size()-1; i++) {
		auto& point1 = points[i];
		auto& point2 = points[i+1];
		Vec2<float> screen1;
		Vec2<float> screen2;
		if (point1.Dist(doroBot->game->getOrigin()) < MAX_DIST) {
			bool onScreen = doroBot->game->worldToScreen(point1, &screen1.x, &screen1.y);
			onScreen = onScreen && doroBot->game->worldToScreen(point2, &screen2.x, &screen2.y);
			if (onScreen) {
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(screen1.x, screen1.y), ImVec2(screen2.x, screen2.y), ImGui::GetColorU32(ImVec4(128, 255, 0, 255)), 2.f);
			}
		}
	}
}

void Recorder::updateSelectedRecordingPoints()
{
	std::lock_guard<std::mutex> lock(pointMutex);
	selectedRecordingPoints.clear();
	for (const CommandPacket& packet : selectedRecording.packets) {
		selectedRecordingPoints.push_back(packet.origin);
	}
}
