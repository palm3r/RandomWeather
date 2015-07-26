#include "perlin.h"
#include "misc.h"

using namespace perlin;

noise::noise(unsigned int seed)
	: min_(-1), max_(1), frequency_(3600), octave_(1)
{
	set_seed(seed);
}

noise::noise(double zero, double amplitude, double min, double max, double frequency, int octave, unsigned int seed)
	: zero_(zero), amplitude_(amplitude), min_(min), max_(max), frequency_(frequency), octave_(octave)
{
	set_seed(seed);
}

void noise::set_seed(unsigned int seed)
{
	if (seed == 0)
	{
		std::random_device rnd;
		seed = rnd();
	}
	std::mt19937 mt(seed);
	auto mid = p.begin() + 256;
	std::iota(p.begin(), mid, 0);
	std::shuffle(p.begin(), mid, mt);
	std::copy(p.begin(), mid, mid);
}

double noise::operator() (double x) const
{
	double total = zero_, amplitude = amplitude_;
	auto freq = frequency_;
	for (int i = 0; i < octave_; ++i)
	{
		total += get(x * freq, 0, 0) * amplitude;
		amplitude *= 0.5;
		freq *= 2;
	}
	return clamp<double>(total, min_, max_);
}

double noise::operator() (double x, double y) const
{
	double total = zero_, amplitude = amplitude_;
	auto freq = frequency_;
	for (int i = 0; i < octave_; ++i)
	{
		total += get(x * freq, y * freq, 0) * amplitude;
		amplitude *= 0.5;
		freq *= 2;
	}
	return clamp<double>(total, min_, max_);
}

double noise::operator() (double x, double y, double z) const
{
	double total = zero_, amplitude = amplitude_;
	auto freq = frequency_;
	for (int i = 0; i < octave_; ++i)
	{
		total += get(x * freq, y * freq, z * freq) * amplitude;
		amplitude *= 0.5;
		freq *= 2;
	}
	return clamp<double>(total, min_, max_);
}

double fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double lerp(double t, double a, double b)
{
	return a + t * (b - a);
}

double grad(int hash, double x, double y, double z)
{
	auto h = hash & 15;
	auto u = h < 8 ? x : y;
	auto v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double noise::get(double x, double y, double z) const
{
	auto const X = static_cast<int>(std::floor(x)) & 255;
	auto const Y = static_cast<int>(std::floor(y)) & 255;
	auto const Z = static_cast<int>(std::floor(z)) & 255;

	x -= std::floor(x);
	y -= std::floor(y);
	z -= std::floor(z);

	auto const u = fade(x);
	auto const v = fade(y);
	auto const w = fade(z);

	auto const A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
	auto const B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

	auto result = lerp(w,
		lerp(v,
			lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)),
			lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))),
		lerp(v,
			lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)),
			lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1)))
		);

	return result;
}
