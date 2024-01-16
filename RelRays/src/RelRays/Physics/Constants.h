#pragma once

#include "RelRays/Physics/Units.h"

namespace Constants {

	// Speed of light
	inline float c	= 299792458 * Units::m / Units::s;
	// Planck constant
	inline float h	= 6.62607015 * 0.0001f * Prefix::q * Units::J * Units::s;
	// Boltzmann constant
	inline float k	= 1.380649 * 0.01f * Prefix::z * Units::J / Units::K;
	// Avogadro constant
	inline float NA	= 6.02214076 * 100.0f * Prefix::Z / Units::mol;

	// Pi
	inline float pi	= 3.141592653589793238462643383f;
	// e
	inline float e	= 2.7182818284590452353602874f;
}