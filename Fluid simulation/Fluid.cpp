#include "Fluid.h"
#include "FluidOperations.h"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include <iostream>

Fluid::Fluid(float dt, float diffusion, float viscosity)
{
	this->size = N;
	this->dt = dt;
	this->diff = diffusion;
	this->visc = viscosity;

	this->s = new float[N * N];
	this->density = new float[N * N];

	this->Vx = new float[N * N];
	this->Vy = new float[N * N];

	this->Vx0 = new float[N * N];
	this->Vy0 = new float[N * N];

}

Fluid::~Fluid()
{
	delete this->s;
	delete this->density;

	delete this->Vx;
	delete this->Vy;

	delete this->Vx0;
	delete this->Vy0;
}

void Fluid::step()
{
	const int N = this->size;
	const float visc = this->visc;
	const float dt = this->dt;
	float* Vx = this->Vx;
	float* Vy = this->Vy;
	float* Vx0 = this->Vx0;
	float* Vy0 = this->Vx0;
	float* s = this->s;
	float* density = this->density;

	diffuse(1, Vx0, Vx, visc, dt);
	diffuse(2, Vy0, Vy, visc, dt);
	
	project(Vx0, Vy0, Vx, Vy);

	advect(1, Vx, Vx0, Vx0, Vy0, dt);
	advect(2, Vy, Vy0, Vx0, Vy0, dt);

	project(Vx, Vy, Vx0, Vy0);

	diffuse(0, s, density, diff, dt);
	advect(0, density, s, Vx, Vy, dt);
}

void Fluid::addDensity(const int x, const int y, const float amount)
{
	this->density[IX(x, y)] += amount;
}

void Fluid::addVelocity(const int x, const int y, const float amountX, const float amountY)
{
	this->Vx[IX(x, y)] += amountX;
	this->Vy[IX(x, y)] += amountY;
}

const float* Fluid::getDensity() const
{
	return this->density;
}
