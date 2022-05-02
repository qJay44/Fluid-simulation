#include "Fluid.h"
#include "FluidOperations.h"

Fluid::Fluid(float dt, float diffusion, float viscosity)
{
	this->dt = dt;
	this->diff = diffusion;
	this->visc = viscosity;

	this->fillArr(this->s, N * N);
	this->fillArr(this->density, N * N);

	this->fillArr(this->Vx, N * N);
	this->fillArr(this->Vy, N * N);

	this->fillArr(this->Vx0, N * N);
	this->fillArr(this->Vy0, N * N);
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
	diffuse(1, this->Vx0, this->Vx, this->visc, this->dt);
	diffuse(2, this->Vy0, this->Vy, this->visc, this->dt);
	
	project(this->Vx0, this->Vy0, this->Vx, this->Vy);

	advect(1, this->Vx, this->Vx0, this->Vx0, this->Vy0, this->dt);
	advect(2, this->Vy, this->Vy0, this->Vx0, this->Vy0, this->dt);

	project(this->Vx, this->Vy, this->Vx0, this->Vy0);

	diffuse(0, this->s, this->density, this->diff, this->dt);
	advect(0, this->density, this->s, this->Vx, this->Vy, this->dt);
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

void Fluid::fadeDensity()
{
	for (size_t i = 0; i < N * N; i++)
	{
		this->density[i] = this->density[i] - 0.05f < 0 ? 0 : this->density[i] - 0.05f;
	}
}
void Fluid::fillArr(float arr[], const unsigned int size)
{
	for (size_t i = 0; i < size; i++)
	{
		arr[i] = 0.f;
	}
}

const float Fluid::getDensity(const int x, const int y) const
{
	return this->density[IX(x, y)];
}

void Fluid::changeColorMode()
{
	this->colorMode == 2 ? this->colorMode = 0 : this->colorMode++;
}

const int Fluid::getColorMode() const { return this->colorMode; }
const float Fluid::getVelX(const int x, const int y) const { return this->Vx[IX(x, y)]; }
const float Fluid::getVelY(const int x, const int y) const { return this->Vy[IX(x, y)]; }
