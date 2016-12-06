#pragma once

class Settings {
public:
	unsigned int num_threads_photon;
	unsigned int num_threads_render;
	unsigned int sample_size;

	Settings():num_threads_photon(4), num_threads_render(2), sample_size(1) {}
};
