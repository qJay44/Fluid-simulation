#include "Fluid.h"
#include "FluidOperations.h"
#include <iostream>

Fluid::Fluid(float dt, float diffusion, float viscosity)
{
	this->size = Operations.GetN() * Operations.GetN();
	this->dt = dt;
	this->diff = diffusion;
	this->visc = viscosity;

	this->s = fillArr();
	this->density = fillArr();
	
	this->Vx = fillArr();
	this->Vy = fillArr();

	this->Vx0 = fillArr();
	this->Vy0 = fillArr();
}

Fluid::~Fluid()
{
	delete[] this->s;
	delete[] this->density;

	delete[] this->Vx;
	delete[] this->Vy;

	delete[] this->Vx0;
	delete[] this->Vy0;
}

void Fluid::step()
{
	Operations.diffuse(1, this->Vx0, this->Vx, this->visc, this->dt);
	Operations.diffuse(2, this->Vy0, this->Vy, this->visc, this->dt);
	
	Operations.project(this->Vx0, this->Vy0, this->Vx, this->Vy);

	Operations.advect(1, this->Vx, this->Vx0, this->Vx0, this->Vy0, this->dt);
	Operations.advect(2, this->Vy, this->Vy0, this->Vx0, this->Vy0, this->dt);

	Operations.project(this->Vx, this->Vy, this->Vx0, this->Vy0);

	Operations.diffuse(0, this->s, this->density, this->diff, this->dt);
	Operations.advect(0, this->density, this->s, this->Vx, this->Vy, this->dt);
}

void Fluid::addDensity(const int x, const int y, const float amount)
{
	this->density[Operations.IX(x, y)] += amount;
}

void Fluid::addVelocity(const int x, const int y, const float amountX, const float amountY)
{
	this->Vx[Operations.IX(x, y)] += amountX;
	this->Vy[Operations.IX(x, y)] += amountY;
}

void Fluid::fadeDensity()
{
	for (size_t i = 0; i < this->size; i++)
		this->density[i] = this->density[i] - 0.05f < 0.f ? 0.f : this->density[i] - 0.05f;
}

float* Fluid::fillArr()
{
	float* arr = new float[this->size];
	for (size_t i = 0; i < this->size; i++)
		arr[i] = 0.f;

	return arr;
}

const float Fluid::getDensity(const int x, const int y) const
{
	return this->density[Operations.IX(x, y)];
}

void Fluid::changeColorMode()
{
	this->colorMode == 2 ? this->colorMode = 0 : this->colorMode++;
}

const int Fluid::getColorMode() const { return this->colorMode; }
const float Fluid::getVelX(const int x, const int y) const { return this->Vx[Operations.IX(x, y)]; }
const float Fluid::getVelY(const int x, const int y) const { return this->Vy[Operations.IX(x, y)]; }
const int Fluid::getScale() const { return Operations.GetScale(); }
const int Fluid::getN() const { return Operations.GetN(); }
