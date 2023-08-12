#pragma once

#define N 100
#define ITER 10
#define SCALE 10

class Fluid {
	unsigned int colorMode = 0;
	unsigned int size;
	float dt;
	float diff;
	float visc;

	float* s = nullptr;
	float* density = nullptr;

	float* Vx = nullptr;
	float* Vy = nullptr;

	float* Vx0 = nullptr;
	float* Vy0 = nullptr;

	float* fillArr();

  // Fluid operations
  const int IX(int x, int y) const;
  void set_bnd(const int b, float x[]);
  void lin_solve(const int b, float x[], const float x0[], const float a, const float c);
  void diffuse(const int b, float x[], const float x0[], const float diff, const float dt);
  void project(float velocX[], float velocY[], float p[], float div[]);
  void advect(const int b, float d[], const float d0[], const float velocX[], const float velocY[], const float dt);

  public:

    Fluid(float dt, float diffusion, float viscosity);
    ~Fluid();

    void step();
    void addDensity(const int x, const int y, const float amount);
    void addVelocity(const int x, const int y, const float amountX, const float amountY);
    void fadeDensity();
    void changeColorMode();

    const float getDensity(const int x, const int y) const;
    const int getColorMode() const;
    const float getVelX(const int x, const int y) const;
    const float getVelY(const int x, const int y) const;
};

