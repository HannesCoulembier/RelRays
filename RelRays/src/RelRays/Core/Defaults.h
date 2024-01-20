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

		namespace EmissionSpectra {

			class LED {
			public:
				const static LoFox::Ref<Spectrum> Infrared;
				const static LoFox::Ref<Spectrum> Red;
				const static LoFox::Ref<Spectrum> Green;
				const static LoFox::Ref<Spectrum> Blue;
			};
		}
		namespace AbsorptionSpectra {

			class FakePaint {
			public:
				const static LoFox::Ref<Spectrum> Infrared;
				const static LoFox::Ref<Spectrum> Red;
				const static LoFox::Ref<Spectrum> Green;
				const static LoFox::Ref<Spectrum> Blue;
			};
		}
	}
}