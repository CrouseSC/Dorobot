#include "pch.h"
#include "ui_menu.h"
#include "Dorobot.h"


void UI_Menu::menu(Dorobot* doroBot)
{
    if (!isEditing) {
        ImVec2 buttonSize(250, 100);
        ImGui::PushFont(doroBot->toxicFont);
        ImGui::SetNextWindowSize(ImVec2(1400, 700));
        ImGui::Begin("Dorobot", 0, ImGuiWindowFlags_NoResize);
        ImGui::Columns(2, 0, false);
        ImGui::SetColumnWidth(0, 300);
        ImGui::SetColumnWidth(1, 1000);

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        if (ImGui::Button("Movement", buttonSize)) {
            selectedTab = MOVEMENT;
        }
        if (ImGui::Button("Recorder", buttonSize)) {
            selectedTab = RECORDER;
        }
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        if (ImGui::Button("Runs", buttonSize)) {
            selectedTab = RUNS;
        }
        ImGui::PopItemFlag();

        if (ImGui::Button("Misc", buttonSize)) {
            selectedTab = MISC;
        }
        if (ImGui::Button("Binds", buttonSize)) {
            selectedTab = BINDS;
        }
        ImGui::PopStyleColor();
        ImGui::NextColumn();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 30.f);

        switch (selectedTab) {
        case MOVEMENT:
        {
            ImVec2 pos = ImGui::GetCursorPos();
            if (ImGui::Checkbox("Strafebot", &strafebot_toggle)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("Autofps", &autofps_toggle)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("PFPS", &pfps_toggle)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("Key on W", &keyOnW_toggle)) {
                doroBot->saveConfiguration();
            }
            ImGui::SameLine(); ImGui::PushItemWidth(250.f);
            std::string keyOnWPreview = keyOnWWDSelected ? "WD" : "WA";
            if (ImGui::BeginCombo("##KeyOnWKeySelect", keyOnWPreview.c_str())) {
                if (ImGui::Selectable("WD", keyOnWWDSelected)) keyOnWWDSelected = true;
                if (ImGui::Selectable("WA", !keyOnWWDSelected)) keyOnWWDSelected = false;
                ImGui::EndCombo();
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("Sprint to Prestrafe", &sprintToPrestrafe_toggle)) {
                doroBot->saveConfiguration();
            }
            ImGui::SameLine(); ImGui::PushItemWidth(250.f);
            std::string preview = sprintToPrestrafeIsWD ? "WD" : "WA";
            if (ImGui::BeginCombo("##DirectionSelect", preview.c_str())) {
                if (ImGui::Selectable("WD", sprintToPrestrafeIsWD)) sprintToPrestrafeIsWD = true;
                if (ImGui::Selectable("WA", !sprintToPrestrafeIsWD)) sprintToPrestrafeIsWD = false;
                ImGui::EndCombo();
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("No 200", &no200_toggle)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("Jump at Velocity", &jumpAtVelo_toggle)) {
                doroBot->saveConfiguration();
            }
            ImGui::SameLine(); ImGui::PushItemWidth(350.f);
            if (ImGui::SliderFloat("##VeloToJumpAt", &veloToJumpAt, 1.f, 364.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("Bhop", &bhop_toggle)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("Jump on RPG", &jumpOnRpg_toggle)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("Autopara", &autopara_toggle)) {
                doroBot->saveConfiguration();
            }
            ImGui::Indent(20.f);
            if (ImGui::Checkbox("Only once: ##autopara", &autoparaOnlyOnce)) {
                doroBot->saveConfiguration();
            }
            ImGui::Indent(-20.f);
            if (ImGui::Checkbox("Auto transfer zone-3 switch: ", &autoTransferzone3Spam_toggle)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("Auto 250", &auto250_toggle)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("Switch on bounce", &switchOnBounce_toggle)) {
                doroBot->saveConfiguration();
            }
            ImGui::Indent(-20.f);
            break;
        }
        case MISC:
        {
            if (ImGui::Checkbox("Speedometer", &velo_meter)) {
                doroBot->saveConfiguration();
            }
            ImGui::SameLine();
            ImGui::ColorButton("Color Button", color);
            if (ImGui::IsItemClicked()) {
                ImGui::OpenPopup("ColorPickerPopup");
            }
            if (ImGui::BeginPopup("ColorPickerPopup")) {
                ImGui::ColorPicker4("Color Picker", &color.x);
                ImGui::EndPopup();
                doroBot->saveConfiguration();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Lock Speed Position", &lock_velo_pos);
            if (ImGui::Checkbox("Sep Speedometer", &sep_velo)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::SliderFloat("Speed Size", &velo_scale, 0.01f, 10.f)) {
                doroBot->saveConfiguration();
            }

            if (ImGui::Checkbox("Anglehelper", &anglehelper_toggle)) {
                doroBot->saveConfiguration();
            }
            ImGui::SameLine();
            ImGui::ColorButton("Anglehelper Color", anglehelper_color);
            if (ImGui::IsItemClicked()) ImGui::OpenPopup("AnglehelperColorPickerPopup");
            if (ImGui::BeginPopup("AnglehelperColorPickerPopup")) {
                ImGui::ColorPicker4("Anglehelper Color Picker", &anglehelper_color.x);
                ImGui::EndPopup();
                doroBot->saveConfiguration();
            }

            if (ImGui::Checkbox("90 Lines", &lines_toggle)) {
                doroBot->saveConfiguration();
            }
            ImGui::SameLine();
            ImGui::ColorButton("90 Lines Color", lines_color);
            if (ImGui::IsItemClicked()) ImGui::OpenPopup("90LinesColorPickerPopup");
            if (ImGui::BeginPopup("90LinesColorPickerPopup")) {
                ImGui::ColorPicker4("90 Lines Color Picker", &lines_color.x);
                ImGui::EndPopup();
                doroBot->saveConfiguration();
            }

            if (ImGui::Checkbox("FPS Wheel", &fpswheel_toggle)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("FPS Wheel Centerline", &fpswheel_centerline_toggle)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::SliderFloat("FPS Wheel Size", &fpswheel_size, 1.f, 100.f)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::SliderFloat("FPS Wheel Y Offset", &fpswheel_offset_y, -200.f, 200.f)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::SliderFloat("FPS Wheel X Offset", &fpswheel_offset_x, 0.45f, 1.f)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::SliderFloat("Anglehelper Pixel Scale", &ah_pixel_scale, 0.3f, 1.f)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::SliderFloat("Wheel Anglehelper Pixel Scale", &wheel_ah_pixel_scale, 0.3f, 1.f)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::SliderFloat("Wheel Pixel Scale", &wheel_pixel_scale, 0.3f, 1.f)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("RPG Lookdown", &rpgLookdown_toggle)) {
                doroBot->saveConfiguration();
            }
			ImGui::Indent(20.f);
            if (ImGui::Checkbox("Shoot backwards", &rpgLookdownBackwards)) {
                doroBot->saveConfiguration();
            }
            ImGui::Indent(20.f);
            if (ImGui::SliderFloat("Pitch", &rpgLookdownPitch, -85.f, 85.f)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::SliderFloat("Yaw", &rpgLookdownYaw, 0.f, 360.f)) {
                doroBot->saveConfiguration();
            }
            ImGui::Indent(-40.f);
            if (ImGui::Checkbox("Yaw script", &yawScript_toggle)) {
                doroBot->saveConfiguration();
            }
            ImGui::Indent(20.f);
            if (ImGui::SliderFloat("Yaw##2", &yawScriptAngle, 0.f, 360.f)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("Display angles", &displayAngles_toggle)) {
                doroBot->saveConfiguration();
            }
            ImGui::Indent(-20.f);
            if (ImGui::Checkbox("Draw recorder progress bar", &progressbar_toggle)) {
                doroBot->saveConfiguration();
            }
            if (ImGui::Checkbox("DEBUG", &debugToggle)) {
                doroBot->saveConfiguration();
            }
            break;
        }
        case RECORDER:
        {
            Recording* recording = doroBot->recorder->getSelectedRecording();
            ImGui::PushItemWidth(400.f);
            auto a = ImGui::GetIO().KeysDown;
            if (ImGui::InputText("##Name", nameInputText, 32)) {
                doroBot->recorder->setSelectedRecordingName(std::string(nameInputText));
            }
            if (recording->packets.size() != 0) {
                ImGui::SameLine();
                ImGui::PushItemWidth(300.f);
                ImGui::Text(("Author: " + recording->author).c_str());
                if (ImGui::Button("Edit", ImVec2(120, 60))) {
                    isEditing = true;
                    trimFrames.x = 1;
                    trimFrames.y = recording->packets.size();
                    autofixRecording();
                }
                ImGui::SameLine();
                if (ImGui::Button("Save", ImVec2(120, 60))) {
                    doroBot->recorder->saveRecord();
                }
            }

            ImGui::Checkbox("Packet inspector", &packetInspector_toggle);

            ImGui::SetCursorPosY(300.f);
            ImGui::BeginListBox("##Recordings", ImVec2(800, 300));
            for (const Recording& recording : doroBot->recorder->recordings) {
                if (recording.mapName == doroBot->game->getMapName() && ImGui::Selectable(recording.name.c_str())) {
                    doroBot->recorder->setSelectedRecording(recording);
                    strcpy_s(nameInputText, recording.name.c_str());
                }
            }
            ImGui::EndListBox();
            break;
        }
        case BINDS:
        {
            for (int i = 0; i < doroBot->bindManager->binds.size(); i++) {
				Bind* bind = &doroBot->bindManager->binds[i];
                if (ImGui::BeginCombo(std::string("##" + std::to_string(i)).c_str(), bind->name.c_str())) {
                    for (const std::string& bindName : doroBot->bindManager->getPossibleBindNames()) {
                        if (ImGui::Selectable(bindName.c_str(), bindName == bind->name)) {
                            doroBot->bindManager->setBindName(*bind, bindName);
                        }
                    }
                    ImGui::EndCombo();
					ImGui::SameLine();
                }
                ImGui::SameLine();
                if (ImGui::Button(std::string("-##" + std::to_string(i)).c_str())) {
					capturingInputForBind = -1;
                    doroBot->bindManager->deleteBind(*bind);
                }
				ImGui::SameLine();
                std::string key = doroBot->bindManager->getNameForKey(bind->key);

                if (ImGui::Button((key + "##" + std::to_string(i)).c_str())) {
					capturingInputForBind = i;
                }
            }
            if (ImGui::Button("+")) {
                doroBot->bindManager->addBind("", -1);
                capturingInputForBind = -1;
            }
            if (capturingInputForBind != -1) {
                int key = doroBot->bindManager->getCurrentPressedKey();
                if (key != -1) {
					doroBot->bindManager->binds[capturingInputForBind].key = key;
                    capturingInputForBind = -1;
                    doroBot->saveConfiguration();
                }
			}
        }
        }
        ImGui::End();
        ImGui::PopFont();
    }

    if (isEditing) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 100);
        ImGui::SetNextWindowSize(ImVec2(600, 350));
        if (ImGui::Begin("Editor", &isEditing, ImGuiWindowFlags_NoResize)) {
            Recording* recording = doroBot->recorder->getSelectedRecording();

            if (ImGui::Button("<")) {
                selectedFrame--;
                if (selectedFrame < 1)
                    selectedFrame = 1;
            }
            ImGui::SameLine();
            if (ImGui::SliderInt("##Frame", &selectedFrame, 1, recording->packets.size())) {
                if (selectedFrame < trimFrames.x) {
                    selectedFrame = trimFrames.x;
                }
                if (selectedFrame > trimFrames.y) {
                    selectedFrame = trimFrames.y;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(">")) {
                selectedFrame++;
                if (selectedFrame > recording->packets.size())
                    selectedFrame = recording->packets.size();
            }
            ImGui::SameLine();
            ImGui::Text("Frame");

            if (ImGui::Button("<##2")) {
                trimFrames.x--;
                if (trimFrames.x < 1)
                    trimFrames.x = 1;
            }
            ImGui::SameLine();
            if (ImGui::SliderInt("##X", &trimFrames.x, 1, recording->packets.size())) {
                selectedFrame = trimFrames.x;
                if (trimFrames.x > trimFrames.y) {
                    trimFrames.y = trimFrames.x;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(">##2")) {
                trimFrames.x++;
                if (trimFrames.x > recording->packets.size())
                    trimFrames.x = recording->packets.size();
                if (trimFrames.x > trimFrames.y)
                    trimFrames.y = trimFrames.x;
            }

            if (ImGui::Button("<##3")) {
                trimFrames.y--;
                if (trimFrames.y < 1)
                    trimFrames.y = 1;
                if (trimFrames.y < trimFrames.x)
                    trimFrames.x = trimFrames.y;
            }
            ImGui::SameLine();
            if (ImGui::SliderInt("##Y", &trimFrames.y, 1, recording->packets.size())) {
                selectedFrame = trimFrames.y;
                if (trimFrames.y < trimFrames.x) {
                    trimFrames.x = trimFrames.y;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(">##3")) {
                trimFrames.y++;
                if (trimFrames.y > recording->packets.size())
                    trimFrames.y = recording->packets.size();
            }
            if (ImGui::Button("Autofix", ImVec2(100, 30))) {
                autofixRecording();
            }
            ImGui::SameLine();
            if (ImGui::Button("Trim", ImVec2(70, 30))) {
                doroBot->recorder->autofixRecording(Vec2<int>(trimFrames.x - 1, trimFrames.y - 1));
                selectedFrame = 1;
                isEditing = false;
            }
            ImGui::PopStyleVar();
            ImGui::SetWindowFocus();
            ImGui::End();
        }

        if (packetInspector_toggle) {
            ImGui::SetNextWindowSize(ImVec2(550, 275));
            if (ImGui::Begin("Packet inspector", &packetInspector_toggle)) {
                CommandPacket selectedPacket = doroBot->recorder->getSelectedRecording()->packets[selectedFrame - 1];
                std::string buttonsText = "Buttons: " + std::to_string(selectedPacket.cmd.buttons);
                ImGui::Text(buttonsText.c_str());
                std::string sideText = "Side: " + std::to_string(selectedPacket.cmd.side);
                ImGui::Text(sideText.c_str());
                std::string forwardText = "Forward: " + std::to_string(selectedPacket.cmd.forward);
                ImGui::Text(forwardText.c_str());
                std::string anglesText = "Angles X: " + std::to_string(selectedPacket.cmd.angles[0]) + " Y: " + std::to_string(selectedPacket.cmd.angles[1]);
                ImGui::Text(anglesText.c_str());
                std::string originText = "Origin X: " + std::to_string(selectedPacket.origin.x) + " Y: " + std::to_string(selectedPacket.origin.y) + " Z: " + std::to_string(selectedPacket.origin.z);
                ImGui::Text(originText.c_str());
                std::string veloText = "Velocity X: " + std::to_string(selectedPacket.velocity.x) + " Y: " + std::to_string(selectedPacket.velocity.y) + " Z: " + std::to_string(selectedPacket.velocity.z);
                ImGui::Text(veloText.c_str());
                std::string veloText2 = "Velocity: " + std::to_string(selectedPacket.velocity.Length2D());
                ImGui::Text(veloText2.c_str());
                std::string serverTimeText = "Servertime: " + std::to_string(selectedPacket.cmd.serverTime);
                ImGui::Text(serverTimeText.c_str());
                ImGui::End();
            }
        }
    }
}

void UI_Menu::render()
{
    Dorobot* doroBot = Dorobot::getInstance();

    if (doroBot->wantInput) menu(doroBot);

    if (debugToggle) {
        doroBot->uiDebug->render();
        doroBot->uiView->render();
        doroBot->saveConfiguration();
    }

    if (displayAngles_toggle) {
        doroBot->uiDebug->renderAngles();
    }

    if ((velo_meter || sep_velo) && doroBot->game->isConnected())
        doroBot->uiVelocity->render(doroBot, lock_velo_pos, velo_pos, velo_scale, color);

    if (anglehelper_toggle && doroBot->game->isConnected())
        doroBot->uiAnglehelper->render(doroBot, anglehelper_color);

    if (fpswheel_toggle && doroBot->game->isConnected()) {
        doroBot->uiFpsWheel->render(doroBot);
    }
}

void UI_Menu::ImGuiImageButton(const std::string& label, const ImVec2& size, const TextureWrapper* texture)
{
    ImGui::Button(label.c_str(), size);
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - size.x);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + size.y / 4);
    ImGui::Image(texture->getId(), ImVec2(50, 50));
}

void UI_Menu::autofixRecording()
{
    Vec2<int> newTrim = Dorobot::getInstance()->recorder->autoTrimRecording(trimFrames.y - 1);
    newTrim.x++;
    newTrim.y++;
    trimFrames = newTrim;
}

UI_Menu::UI_Menu(Dorobot* doroBot) 
{
    doroBot->render->addCallback([this]() { this->render(); });
}

UI_Menu::~UI_Menu() {}
