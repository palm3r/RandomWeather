#pragma once

#include <array>
#include <random>
#include <numeric>

namespace perlin {

	class noise
	{
	public:
		noise(unsigned int seed = 0);
		noise(double zero, double amplitude, double min, double max, double frequency, int octave, unsigned int seed = 0);

		double zero() const { return zero_; }
		double amplitude() const { return amplitude_; }
		double min_value() const { return min_; }
		double max_value() const { return max_; }
		double frequency() const { return frequency_; }
		int octave() const { return octave_; }

		void set_zero(double zero) { zero_ = zero; }
		void set_amplitude(double amplitude) { amplitude_ = amplitude; }
		void set_seed(unsigned int seed);
		void set_min_value(double min) { min_ = min; }
		void set_max_value(double max) { max_ = max; }
		void set_frequency(double frequency) { frequency_ = frequency; }
		void set_octave(int octave) { octave_ = octave; }

		double operator() (double x) const;
		double operator() (double x, double y) const;
		double operator() (double x, double y, double z) const;

	private:
		std::array<int, 512> p;
		double zero_;
		double amplitude_;
		double min_;
		double max_;
		double frequency_;
		int octave_;

		double get(double x, double y, double z) const;
	};

} // namespace perlin