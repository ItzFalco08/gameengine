#pragma once


class AssetsRegistryPanel {
public:
	void Render();

	void Activate();
private:
	bool active = false;
	bool openRequested = false;
};