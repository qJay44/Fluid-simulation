#include <cmath>
#include "utils.hpp"

#define WIDTH 1600
#define HEIGHT 720
#define SCALE 8 // Width and height of grid squares
#define ITER 10  // How accurate liquid will be calculated (more better but slower)

#define ROWS ((int)((HEIGHT) / (SCALE)))
#define COLUMNS ((int)((WIDTH) / (SCALE)))
#define PIXELS (ROWS) * (COLUMNS) * 4

#define IX(x, y) ((x) + (y) * (COLUMNS))

class Fluid {
	int colorMode = 0;
	int size;

	float *dt = nullptr;
	float *diff = nullptr;
	float *visc = nullptr;

	float* s = nullptr;
	float* density = nullptr;

	float* Vx = nullptr;
	float* Vy = nullptr;

	float* Vx0 = nullptr;
	float* Vy0 = nullptr;

  float* fillArr() {
    float* arr = new float[size];
    for (int i = 0; i < size; i++)
      arr[i] = 0.f;

    return arr;
  }

  void set_bnd(int b, float x[]) {
    for (int i = 1; i < COLUMNS - 1; i++) {
      x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
      x[IX(i, ROWS - 1)] = b == 2 ? -x[IX(i, ROWS - 2)] : x[IX(i, ROWS - 2)];
    }

    for (int j = 1; j < ROWS - 1; j++) {
      x[IX(0, j)] = b == 1 ? -x[IX(1, j)] : x[IX(1, j)];
      x[IX(COLUMNS - 1, j)] = b == 1 ? -x[IX(COLUMNS - 2, j)] : x[IX(COLUMNS - 2, j)];
    }

    x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
    x[IX(0, ROWS - 1)] = 0.5f * (x[IX(1, ROWS - 1)] + x[IX(0, ROWS - 2)]);
    x[IX(COLUMNS - 1, 0)] = 0.5f * (x[IX(COLUMNS - 2, 0)] + x[IX(COLUMNS - 1, 1)]);
    x[IX(COLUMNS - 1, ROWS - 1)] = 0.5f * (x[IX(COLUMNS - 2, ROWS - 1)] + x[IX(COLUMNS - 1, ROWS - 2)]);
  }

  void lin_solve(int b, float x[], float x0[], float a, float c) {
    const float cRecip = 1.0f / c;
    for (int t = 0; t < ITER; t++) {
      for (int j = 1; j < ROWS - 1; j++) {
        for (int i = 1; i < COLUMNS - 1; i++) {
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

  void diffuse(int b, float x[], float x0[], float* diff) {
    const float a = *dt * *diff * (COLUMNS - 2) * (ROWS - 2);
    lin_solve(b, x, x0, a, 1 + 4 * a);
  }

  void project(float velocX[], float velocY[], float p[], float div[]) {
    for (int j = 1; j < ROWS - 1; j++) {
      for (int i = 1; i < COLUMNS - 1; i++) {
        div[IX(i, j)] = -0.5f * (
          velocX[IX(i + 1, j)]
          - velocX[IX(i - 1, j)]
          + velocY[IX(i, j + 1)]
          - velocY[IX(i, j - 1)]
          ) / ((ROWS + COLUMNS) / 2.f); // NOTE: Probably wrong

        p[IX(i, j)] = 0.f;
      }
    }

    set_bnd(0, div);
    set_bnd(0, p);
    lin_solve(0, p, div, 1, 4);

    for (int j = 1; j < ROWS - 1; j++) {
      for (int i = 1; i < COLUMNS - 1; i++) {
        velocX[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) * COLUMNS;
        velocY[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) * ROWS;
      }
    }
    set_bnd(1, velocX);
    set_bnd(2, velocY);
  }

  void advect(int b, float d[], float d0[], float velocX[], float velocY[], float* dt) {
    int i0, i1, j0, j1;

    float dtx = *dt * (COLUMNS - 2);
    float dty = *dt * (ROWS - 2);

    float s0, s1, t0, t1;
    float tmp1, tmp2, x, y;

    float ifloat, jfloat;
    int i, j;

    for (j = 1, jfloat = 1.f; j < ROWS - 1; j++, jfloat++) {
      for (i = 1, ifloat = 1.f; i < COLUMNS - 1; i++, ifloat++) {
        tmp1 = dtx * velocX[IX(i, j)];
        tmp2 = dty * velocY[IX(i, j)];
        x = ifloat - tmp1;
        y = jfloat - tmp2;

        if (x < 0.5f) x = 0.5f;
        if (x > COLUMNS - 2 + 0.5f) x = COLUMNS - 2 + 0.5f;

        i0 = floor(x);
        i1 = i0 + 1;

        if (y < 0.5f) y = 0.5f;
        if (y > ROWS - 2 + 0.5f) y = ROWS - 2 + 0.5f;

        j0 = floor(y);
        j1 = j0 + 1;

        s1 = x - i0;
        s0 = 1.0f - s1;
        t1 = y - j0;
        t0 = 1.0f - t1;

        int i0i = i0;
        int i1i = i1;
        int j0i = j0;
        int j1i = j1;

        d[IX(i, j)] =
          s0 * (t0 * d0[IX(i0i, j0i)] + t1 * d0[IX(i0i, j1i)]) +
          s1 * (t0 * d0[IX(i1i, j0i)] + t1 * d0[IX(i1i, j1i)]);
      }
    }
    set_bnd(b, d);
  }

  public:
    Fluid() {}

    ~Fluid() {
      delete[] s;
      delete[] density;

      delete[] Vx;
      delete[] Vy;

      delete[] Vx0;
      delete[] Vy0;
    }

    // @param dt: time step
    // @param diffusion: how fast fluid will spread (disappear)
    // @param viscosity: how thick the fluid is
    void setup(float& deltatime, float& diffusion, float& viscosity) {
      size = ROWS * COLUMNS;
      dt = &deltatime;
      diff = &diffusion;
      visc = &viscosity;

      s = fillArr();
      density = fillArr();

      Vx = fillArr();
      Vy = fillArr();

      Vx0 = fillArr();
      Vy0 = fillArr();
    }

    void step() {
      diffuse(1, Vx0, Vx, visc);
      diffuse(2, Vy0, Vy, visc);

      project(Vx0, Vy0, Vx, Vy);

      advect(1, Vx, Vx0, Vx0, Vy0, dt);
      advect(2, Vy, Vy0, Vx0, Vy0, dt);

      project(Vx, Vy, Vx0, Vy0);

      diffuse(0, s, density, diff);
      advect(0, density, s, Vx, Vy, dt);
    }

    void fadeDensity() {
      for (int i = 0; i < size; i++)
        density[i] = density[i] - 0.05f < 0.f ? 0.f : density[i] - 0.05f;
    }

    void addDensity(int x, int y, float amount) {
      density[IX(x, y)] += amount;
    }

    void addVelocity(int x, int y, float amountX, float amountY) {
      Vx[IX(x, y)] += amountX;
      Vy[IX(x, y)] += amountY;
    }

    inline void changeColorMode() {
      colorMode == 2 ? colorMode = 0 : colorMode++;
    }

    inline const float getDensity(int index) const {
      return density[index];
    }

    inline const int getColorMode() const { return colorMode; }
    inline const float getVelX(int index) const { return Vx[index]; }
    inline const float getVelY(int index) const { return Vy[index]; }
};

