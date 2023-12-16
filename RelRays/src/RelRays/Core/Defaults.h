#pragma once

#include "RelRays/Physics/Spectrum.h"

namespace RelRays {
	namespace Defaults {

		const static LoFox::Ref<Spectrum> EmptySpectrum = Spectrum::Create(0.0f, 1000000.0f, { 0.0f });

		class SonyCamera {
		public:
			const static LoFox::Ref<Spectrum> RedSensorSpectrum;
			const static LoFox::Ref<Spectrum> GreenSensorSpectrum;
			const static LoFox::Ref<Spectrum> BlueSensorSpectrum;
			const static LoFox::Ref<ColorSpectra> SensorColorSpectra;
		};

		class EyeCamera {
		public:
			const static LoFox::Ref<Spectrum> RedSensorSpectrum;
			const static LoFox::Ref<Spectrum> GreenSensorSpectrum;
			const static LoFox::Ref<Spectrum> BlueSensorSpectrum;
			const static LoFox::Ref<ColorSpectra> SensorColorSpectra;
		};

		class Colors {
		public:
			const static LoFox::Ref<Spectrum> InfraredSpectrum;
			const static LoFox::Ref<Spectrum> RedSpectrum;
			const static LoFox::Ref<Spectrum> GreenSpectrum;
			const static LoFox::Ref<Spectrum> BlueSpectrum;
		};

		// class InfraRedCamera {
		// 
		// };
	}
}