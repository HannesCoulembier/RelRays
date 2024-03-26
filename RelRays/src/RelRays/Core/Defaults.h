#pragma once

#include "RelRays/Physics/Spectrum.h"
#include "RelRays/Core/Camera.h"
#include "RelRays/Physics/Units.h"

namespace RelRays {
	namespace Defaults {

		void Init();

		const static LoFox::Ref<Spectrum> EmptySpectrum = Spectrum::Create(0.0f*Units::nm, 1000000.0f*Units::nm, { 0.0f });

		namespace Sensors {

			inline Sensor SonyCamera;
			inline Sensor EyeCamera;
			inline Sensor BlockCamera;

			// Sensor InfraRedCamera;
		}

		namespace EmissionSpectra {

			class BlockLED {
			public:
				const static LoFox::Ref<Spectrum> Infrared;
				const static LoFox::Ref<Spectrum> Red;
				const static LoFox::Ref<Spectrum> Green;
				const static LoFox::Ref<Spectrum> Blue;
			};
		}
		namespace AbsorptionSpectra {

			class BlockPaint {
			public:
				const static LoFox::Ref<Spectrum> Infrared;
				const static LoFox::Ref<Spectrum> Red;
				const static LoFox::Ref<Spectrum> Green;
				const static LoFox::Ref<Spectrum> Blue;
			};
		}
	}
}