#include <iostream>
#include "scene_lua.hpp"

int main(int argc, char** argv)
{
  std::string filename = "Assets/room2.lua";
  if (argc >= 2) {
    filename = argv[1];
  }

  Settings *s = new Settings();
  int i = 2;
  while (i < argc) {
  	std::string arg(argv[i]);
  	if (arg == "-NUM_P_THREADS") {
  		s->num_threads_photon = std::atoi(argv[i+1]);
  	} else if (arg == "-NUM_R_THREADS") {
  		s->num_threads_render = std::atoi(argv[i+1]);
  	} else if (arg == "-SAMPLE_SIZE") {
  		s->sample_size = std::atoi(argv[i+1]);
  	}
  	i += 2;
  }

  if (!run_lua(filename, s)) {
    std::cerr << "Could not open " << filename << std::endl;
    return 1;
  }
}
