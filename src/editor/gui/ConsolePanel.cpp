#include "ConsolePanel.hpp"
#include "imgui.h"
#include "../../utils/Logger.hpp"

namespace {
	void renderLogTab(LOG::Channel channel) {
		const std::vector<LOG::Entry>& entries = channel == LOG::Channel::GAME ? LOG::gameEntries : LOG::editorEntries;

		if (ImGui::Button("Clear Logs")) {
			LOG::clear();
		}
		ImGui::Separator();

		ImGui::BeginChild("LogEntries", ImVec2(0, 0), ImGuiChildFlags_None,
			ImGuiWindowFlags_HorizontalScrollbar);
		for (const LOG::Entry& entry : entries) {
			if (entry.count > 1) {
				ImGui::Text("[%s] %s (x%zu)", LOG::getLevelName(entry.level),
					entry.message.c_str(), entry.count);
			} else {
				ImGui::Text("[%s] %s", LOG::getLevelName(entry.level), entry.message.c_str());
			}
		}
		ImGui::EndChild();
	}
}

void ConsolePanel::Render() {
	ImGui::Begin("Console");
	if (ImGui::BeginTabBar("ConsoleTabs")) {
		if (ImGui::BeginTabItem("Game Logs")) {
			renderLogTab(LOG::Channel::GAME);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Editor Logs")) {
			renderLogTab(LOG::Channel::EDITOR);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}
