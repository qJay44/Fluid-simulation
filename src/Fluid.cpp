#include "Fluid.h"
#include <cmath>

Fluid::Fluid(float dt, float diffusion, float viscosity) {
	this->size = N * N;
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

Fluid::~Fluid() {
	delete[] this->s;
	delete[] this->density;

	delete[] this->Vx;
	delete[] this->Vy;

	delete[] this->Vx0;
	delete[] this->Vy0;
}

void Fluid::step() {
	diffuse(1, this->Vx0, this->Vx, this->visc, this->dt);
	diffuse(2, this->Vy0, this->Vy, this->visc, this->dt);

	project(this->Vx0, this->Vy0, this->Vx, this->Vy);

	advect(1, this->Vx, this->Vx0, this->Vx0, this->Vy0, this->dt);
	advect(2, this->Vy, this->Vy0, this->Vx0, this->Vy0, this->dt);

	project(this->Vx, this->Vy, this->Vx0, this->Vy0);

	diffuse(0, this->s, this->density, this->diff, this->dt);
	advect(0, this->density, this->s, this->Vx, this->Vy, this->dt);
}

void Fluid::addDensity(const int x, const int y, const float amount) {
	this->density[IX(x, y)] += amount;
}

void Fluid::addVelocity(const int x, const int y, const float amountX, const float amountY) {
	this->Vx[IX(x, y)] += amountX;
	this->Vy[IX(x, y)] += amountY;
}

void Fluid::fadeDensity() {
	for (int i = 0; i < this->size; i++)
		this->density[i] = this->density[i] - 0.05f < 0.f ? 0.f : this->density[i] - 0.05f;
}

float* Fluid::fillArr() {
	float* arr = new float[this->size];
	for (int i = 0; i < this->size; i++)
		arr[i] = 0.f;

	return arr;
}

const int Fluid::IX(int x, int y) const {
  x = x < 0 ? 0 : x;
  x = x > N - 1 ? N - 1 : x;

  y = y < 0 ? 0 : y;
  y = y > N - 1 ? N - 1 : y;

  return x + (y * N);
}

void Fluid::set_bnd(const int b, float x[]) {
  for (int i = 1; i < N - 1; i++) {
    x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
    x[IX(i, N - 1)] = b == 2 ? -x[IX(i, N - 2)] : x[IX(i, N - 2)];
  }

  for (int j = 1; j < N - 1; j++) {
    x[IX(0, j)] = b == 1 ? -x[IX(1, j)] : x[IX(1, j)];
    x[IX(N - 1, j)] = b == 1 ? -x[IX(N - 2, j)] : x[IX(N - 2, j)];
  }

  x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
  x[IX(0, N - 1)] = 0.5f * (x[IX(1, N - 1)] + x[IX(0, N - 2)]);
  x[IX(N - 1, 0)] = 0.5f * (x[IX(N - 2, 0)] + x[IX(N - 1, 1)]);
  x[IX(N - 1, N - 1)] = 0.5f * (x[IX(N - 2, N - 1)] + x[IX(N - 1, N - 2)]);
}

void Fluid::lin_solve(const int b, float x[], const float x0[], const float a, const float c) {
  const float cRecip = 1.0f / c;
  for (int t = 0; t < ITER; t++) {
    for (int j = 1; j < N - 1; j++) {
      for (int i = 1; i < N - 1; i++) {
        x[IX(i, j)] =
          (x0[IX(i, j)] + a * (
            x[IX(i + 1, j)]
            + x[IX(i - 1, j)]
            + x[IX(i, j + 1)]
            + x[IX(i, j - 1)]
            )) * cRecip;
      }
    }
    set_bnd(b, x);
  }
}

void Fluid::diffuse(const int b, float x[], const float x0[], const float diff, const float dt) {
  const float a = dt * diff * (N - 2) * (N - 2);
  lin_solve(b, x, x0, a, 1 + 4 * a);
}

void Fluid::project(float velocX[], float velocY[], float p[], float div[]) {
  for (int j = 1; j < N - 1; j++) {
    for (int i = 1; i < N - 1; i++) {
      div[IX(i, j)] = -0.5f * (
        velocX[IX(i + 1, j)]
        - velocX[IX(i - 1, j)]
        + velocY[IX(i, j + 1)]
        - velocY[IX(i, j - 1)]
        ) / N;

      p[IX(i, j)] = 0.f;
    }
  }

  set_bnd(0, div);
  set_bnd(0, p);
  lin_solve(0, p, div, 1, 4);

  for (int j = 1; j < N - 1; j++) {
    for (int i = 1; i < N - 1; i++) {
      velocX[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) * N;
      velocY[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) * N;
    }
  }
  set_bnd(1, velocX);
  set_bnd(2, velocY);
}

void Fluid::advect(const int b, float d[], const float d0[], const float velocX[], const float velocY[], const float dt) {
  float i0, i1, j0, j1;

  float dtx = dt * (N - 2);
  float dty = dt * (N - 2);

  float s0, s1, t0, t1;
  float tmp1, tmp2, x, y;

  float Nfloat = (float)N;
  float ifloat, jfloat;
  unsigned int i, j;

  for (j = 1, jfloat = 1.f; j < N - 1; j++, jfloat++) {
    for (i = 1, ifloat = 1.f; i < N - 1; i++, ifloat++) {
      tmp1 = dtx * velocX[IX(i, j)];
      tmp2 = dty * velocY[IX(i, j)];
      x = ifloat - tmp1;
      y = jfloat - tmp2;

      if (x < 0.5f) x = 0.5f;
      if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;

      i0 = floor(x);
      i1 = i0 + 1.0f;

      if (y < 0.5f) y = 0.5f;
      if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;

      j0 = floor(y);
      j1 = j0 + 1.0f;

      s1 = x - i0;
      s0 = 1.0f - s1;
      t1 = y - j0;
      t0 = 1.0f - t1;

      const int i0i = (int)i0;
      const int i1i = (int)i1;
      const int j0i = (int)j0;
      const int j1i = (int)j1;

      d[IX(i, j)] =
        s0 * (t0 * d0[IX(i0i, j0i)] + t1 * d0[IX(i0i, j1i)]) +
        s1 * (t0 * d0[IX(i1i, j0i)] + t1 * d0[IX(i1i, j1i)]);
    }
  }
  set_bnd(b, d);
}


const float Fluid::getDensity(const int x, const int y) const {
	return this->density[IX(x, y)];
}

void Fluid::changeColorMode() {
	this->colorMode == 2 ? this->colorMode = 0 : this->colorMode++;
}

const int Fluid::getColorMode() const { return this->colorMode; }
const float Fluid::getVelX(const int x, const int y) const { return this->Vx[IX(x, y)]; }
const float Fluid::getVelY(const int x, const int y) const { return this->Vy[IX(x, y)]; }

