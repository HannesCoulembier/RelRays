#pragma once

namespace Prefix {

	inline float Q = 1000000000000000000000000000000.0f;	// 1.0e30
	inline float R = 1000000000000000000000000000.0f;		// 1.0e27
	inline float Y = 1000000000000000000000000.0f;			// 1.0e24
	inline float Z = 1000000000000000000000.0f;				// 1.0e21
	inline float E = 1000000000000000000.0f;				// 1.0e18
	inline float P = 1000000000000000.0f;					// 1.0e15
	inline float T = 1000000000000.0f;						// 1.0e12
	inline float G = 1000000000.0f;							// 1.0e9
	inline float M = 1000000.0f;							// 1.0e6
	inline float k = 1000.0f;								// 1.0e3
	inline float h = 100.0f;								// 1.0e2
	inline float da= 10.0f;									// 1.0e1
	inline float d = 0.1f;									// 1.0e-1
	inline float c = 0.01f;									// 1.0e-2
	inline float m = 0.001f;								// 1.0e-3
	inline float µ = 0.000001f;								// 1.0e-6
	inline float n = 0.000000001f;							// 1.0e-9
	inline float p = 0.000000000001f;						// 1.0e-12
	inline float f = 0.000000000000001f;					// 1.0e-15
	inline float a = 0.000000000000000001f;					// 1.0e-18
	inline float z = 0.000000000000000000001f;				// 1.0e-21
	inline float y = 0.000000000000000000000001f;			// 1.0e-24
	inline float r = 0.000000000000000000000000001f;		// 1.0e-27
	inline float q = 0.000000000000000000000000000001f;		// 1.0e-30
}

namespace Units {

	// Defines what units are used for raw float values
	namespace Base {

		// 1 means that the float value 1 corresponds to 1 meter, 1000 means float value 1 corresponds to 1 millimeter, ...
		inline float Meter		= 1.0f;
		inline float Second		= 1.0f;
		inline float Gram		= 1.0f;
		inline float Kilogram	= Prefix::k * Gram;
		inline float Kelvin		= 1.0f;
		inline float Ampere		= 1.0f;
		inline float Mole		= 1.0f;
		inline float Candela	= 1.0f;
	}

	inline float mm		= Prefix::m	* Base::Meter;
	inline float cm		= Prefix::c * Base::Meter;
	inline float dm		= Prefix::d * Base::Meter;
	inline float m		= 1.0f		* Base::Meter;
	inline float km		= Prefix::k	* Base::Meter;

	inline float s		= 1.0f		* Base::Second;
	inline float h		= 3600.0f	* Base::Second;

	inline float g		= 1.0f		* Base::Gram;
	inline float kg		= 1.0f		* Base::Kilogram;

	inline float A		= 1.0f		* Base::Ampere;

	inline float cd		= 1.0f		* Base::Candela;

	inline float mol	= 1.0f		* Base::Mole;

	inline float K		= 1.0f		* Base::Kelvin;

	// Derived units
	inline float rad		= m / m;
	inline float Hz			= 1.0f / s;
	inline float N			= (kg * m) / (s * s);
	inline float Pa			= N / (m * m);
	inline float J			= N * m;
	inline float W			= J / s;
	inline float C			= s * A;
	inline float V			= W / A;
	inline float F			= C / V;
	inline float Ohm		= V / A;
	inline float S			= 1.0f / Ohm;
	inline float Wb			= V * s;
	inline float T			= Wb / (m * m);
	inline float H			= Wb / A;
	inline float Celsius	= K;
	inline float lm			= cd * (m * m) / (m * m);
	inline float lx			= lm / (m * m);
	inline float Bq			= 1.0f / s;
	inline float Gy			= J / kg;
	inline float Sv			= J / kg;
	inline float kat		= mol / s;
}