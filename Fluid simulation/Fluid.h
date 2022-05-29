#pragma once

class Fluid
{
public:
	unsigned int colorMode;

	Fluid(float dt, float diffusion, float viscosity, float, int);
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
	int size;
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
};
