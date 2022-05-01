#pragma once

class Fluid
{
public:
	Fluid(float dt, float diffusion, float viscosity);
	~Fluid();

	void step();
	void addDensity(const int x, const int y, const float amount);
	void addVelocity(const int x, const int y, const float amountX, const float amountY);
	const float* getDensity() const;
	void renderV();
	void fadeD();

private:
	int size;
	float dt;
	float diff;
	float visc;

	float* s;
	float* density;
	
	float* Vx;
	float* Vy;

	float* Vx0;
	float* Vy0;
};
