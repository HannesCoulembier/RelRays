#pragma once

#include "RelRays/Physics/Spectrum.h"
#include "RelRays/Core/Camera.h"

namespace RelRays {
	namespace Defaults {

		void Init();

		const static LoFox::Ref<Spectrum> EmptySpectrum = Spectrum::Create(0.0f, 1000000.0f, { 0.0f });

		namespace Sensors {

			inline Sensor SonyCamera;
			inline Sensor EyeCamera;

			// Sensor InfraRedCamera;
		}

		class Colors {
		public:
			const static LoFox::Ref<Spectrum> InfraredSpectrum;
			const static LoFox::Ref<Spectrum> RedSpectrum;
			const static LoFox::Ref<Spectrum> GreenSpectrum;
			const static LoFox::Ref<Spectrum> BlueSpectrum;
		};
	}
}