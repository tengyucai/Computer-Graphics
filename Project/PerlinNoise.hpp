//----------------------------------------------------------------------------
// PerlinNoise.hpp
// 
// http://www.ics.uci.edu/~gopi/CS211B/RayTracing%20tutorial.pdf
//----------------------------------------------------------------------------

#pragma once

class PerlinNoise {
private:
   int p[512];

   double fade(double);
   double lerp(double, double, double);
   double grad(int, double, double, double);

public:
   PerlinNoise();
   double noise(double, double, double);
};
