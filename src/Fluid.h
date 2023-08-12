#pragma once

#define N 100    // Amount of squares in row and column
#define ITER 1  // How accurate liquid will be calculated (more better but slower)
#define SCALE 10 // Width and height of grid squares

class Fluid {
	unsigned int colorMode = 0;
	unsigned int size;
	float *dt = nullptr;
	float *diff = nullptr;
	float *visc = nullptr;

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
  void diffuse(const int b, float x[], const float x0[], const float* diff, const float* dt);
  void project(float velocX[], float velocY[], float p[], float div[]);
  void advect(const int b, float d[], const float d0[], const float velocX[], const float velocY[], const float* dt);

  public:

    // @param dt: time step
    // @param diffusion: how fast fluid will spread (disappear)
    // @param viscosity: how thick the fluid is
    Fluid(float& dt, float& diffusion, float& viscosity);
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

