#pragma once

#include <cmath>

struct
{
public:
	inline const int GetAmount() const { return this->N; }
	inline const int GetIter() const { return this->iter; }
	inline const float GetScale() const { return this->scale; }
	inline const float GetFieldSize() const { return this->fieldSize; }

	inline void SetAmount(const int num) { this->N = num * this->scale; }
	inline void SetIter(const int num) { this->iter = num; }
	inline void SetScale(const float num) { this->scale = num; }
	inline void SetFieldSize(const int num) { this->fieldSize = num; }

	const int IX(int x, int y)
	{
		x = x < 0 ? 0 : x;
		x = x > N - 1 ? N - 1 : x;

		y = y < 0 ? 0 : y;
		y = y > N - 1 ? N - 1 : y;

		return x + (y * N);
	}

	void set_bnd(const int b, float x[])
	{
		for (size_t i = 1; i < N - 1; i++)
		{
			x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
			x[IX(i, N - 1)] = b == 2 ? -x[IX(i, N - 2)] : x[IX(i, N - 2)];
		}

		for (size_t j = 1; j < N - 1; j++)
		{
			x[IX(0, j)] = b == 1 ? -x[IX(1, j)] : x[IX(1, j)];
			x[IX(N - 1, j)] = b == 1 ? -x[IX(N - 2, j)] : x[IX(N - 2, j)];
		}

		x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
		x[IX(0, N - 1)] = 0.5f * (x[IX(1, N - 1)] + x[IX(0, N - 2)]);
		x[IX(N - 1, 0)] = 0.5f * (x[IX(N - 2, 0)] + x[IX(N - 1, 1)]);
		x[IX(N - 1, N - 1)] = 0.5f * (x[IX(N - 2, N - 1)] + x[IX(N - 1, N - 2)]);
	}

	void lin_solve(const int b, float x[], const float x0[], const float a, const float c)
	{
		const float cRecip = 1.0f / c;
		for (size_t t = 0; t < iter; t++)
		{
			for (size_t j = 1; j < N - 1; j++)
			{
				for (size_t i = 1; i < N - 1; i++)
				{
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

	void diffuse(const int b, float x[], const float x0[], const float diff, const float dt)
	{
		const float a = dt * diff * (N - 2) * (N - 2);
		lin_solve(b, x, x0, a, 1 + 4 * a);
	}

	void project(float velocX[], float velocY[], float p[], float div[])
	{
		for (size_t j = 1; j < N - 1; j++)
		{
			for (size_t i = 1; i < N - 1; i++)
			{
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

		for (size_t j = 1; j < N - 1; j++)
		{
			for (size_t i = 1; i < N - 1; i++)
			{
				velocX[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) * N;
				velocY[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) * N;
			}
		}
		set_bnd(1, velocX);
		set_bnd(2, velocY);
	}

	void advect(const int b, float d[], const float d0[], const float velocX[], const float velocY[], const float dt)
	{
		float i0, i1, j0, j1;

		float dtx = dt * (N - 2);
		float dty = dt * (N - 2);

		float s0, s1, t0, t1;
		float tmp1, tmp2, x, y;

		float Nfloat = N;
		float ifloat, jfloat;
		int i, j;

		for (j = 1, jfloat = 1.f; j < N - 1; j++, jfloat++)
		{
			for (i = 1, ifloat = 1.f; i < N - 1; i++, ifloat++)
			{
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

private:
	float scale;
	unsigned int iter = 10;
	unsigned int fieldSize;
	unsigned int N;

} Operations;
