#pragma once

#include "Constants.h"

class Fluid
{
public:
	unsigned int colorMode;

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

private:
	const unsigned int size = N;
	float dt;
	float diff;
	float visc;

	float* s = new float[N * N];
	float* density = new float[N * N];
	
	float* Vx = new float[N * N];
	float* Vy = new float[N * N];

	float* Vx0 = new float[N * N];
	float* Vy0 = new float[N * N];

	void fillArr(float arr[], const unsigned int size);
};
