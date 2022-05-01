#pragma once

const int IX(int x, int y)
{
	x = x < 0 ? 0 : x;
	x = x > N - 1 ? N - 1 : x;

	y = y < 0 ? 0 : y;
	y = y > N - 1 ? N - 1 : y;

	return x + (y * N);
}
