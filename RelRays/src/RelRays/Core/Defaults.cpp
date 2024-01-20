#include "lfpch.h"
#include "RelRays/Core/Defaults.h"

#include "RelRays/Physics/Units.h"

namespace RelRays {

	namespace Defaults {

		void Init() {

			static bool isInitialized = false;
			if (isInitialized) return;

			// Data extracted from https://www.image-sensing-solutions.eu/FCB-EV7520A.pdf
			Sensors::SonyCamera.RedSpectrum = Spectrum::Create(
				346.0f * Units::nm,
				1042.0f * Units::nm,
				{
					0.002f,//NOT IN SOURCE 346nm
					0.01f,//NOT IN SOURCE
					0.03f,//NOT IN SOURCE
					0.05f,//NOT IN SOURCE
					0.07f,//NOT IN SOURCE
					0.09f,//NOT IN SOURCE
					0.10f,//NOT IN SOURCE
					0.10f,//NOT IN SOURCE
					0.09f,//0 400nm
					0.07f,
					0.06f,
					0.05f,
					0.04f,
					0.04f,
					0.04f,
					0.04f,
					0.03f,
					0.03f,
					0.04f,//10 460nm
					0.04f,
					0.04f,
					0.05f,
					0.05f,
					0.05f,
					0.06f,
					0.06f,
					0.07f,
					0.08f,
					0.09f,//20 520nm
					0.09f,
					0.08f,
					0.07f,
					0.06f,
					0.06f,
					0.07f,
					0.16f,
					0.27f,
					0.49f,
					0.67f,//30 580nm
					0.86f,
					0.95f,
					0.99f,
					1.0f,
					1.0f,
					0.99f,
					0.98f,
					0.97f,
					0.96f,
					0.94f,//40 640nm
					0.92f,
					0.90f,
					0.87f,
					0.85f,
					0.82f,
					0.79f,
					0.77f,
					0.77f,
					0.78f,
					0.79f,//50 700nm
					0.80f,
					0.81f,
					0.82f,
					0.82f,
					0.82f,
					0.81f,
					0.81f,
					0.80f,
					0.80f,
					0.79f,//60 760nm
					0.77f,
					0.75f,
					0.74f,
					0.72f,
					0.71f,
					0.70f,
					0.68f,
					0.68f,
					0.67f,
					0.66f,//70 820nm
					0.65f,
					0.64f,
					0.63f,
					0.62f,
					0.61f,
					0.59f,
					0.57f,
					0.56f,
					0.54f,
					0.52f,//80 880nm
					0.50f,
					0.48f,
					0.46f,
					0.44f,
					0.42f,
					0.40f,
					0.38f,
					0.36f,
					0.34f,
					0.33f,//90 940nm
					0.31f,
					0.29f,
					0.27f,
					0.25f,
					0.24f,
					0.22f,
					0.21f,
					0.19f,
					0.18f,//99 1000nm
					0.16f,//NOT IN SOURCE
					0.14f,//NOT IN SOURCE
					0.11f,//NOT IN SOURCE
					0.08f,//NOT IN SOURCE
					0.05f,//NOT IN SOURCE
					0.01f,//NOT IN SOURCE
					0.002f,//NOT IN SOURCE 1042nm
			}); // 100 real entries + 15 extension entries, range: 346-1042nm, spacing: 6nm
			Sensors::SonyCamera.GreenSpectrum = Spectrum::Create(
				346.0f * Units::nm,
				1042.0f * Units::nm,
				{
					0.001f,// NOT IN SOURCE 346nm
					0.005f,// NOT IN SOURCE
					0.01f,// NOT IN SOURCE
					0.02f,// NOT IN SOURCE
					0.03f,// NOT IN SOURCE
					0.04f,// NOT IN SOURCE
					0.05f,// NOT IN SOURCE
					0.06f,// NOT IN SOURCE
					0.06f,// NOT IN SOURCE
					0.06f,//0 400nm
					0.06f,
					0.05f,
					0.05f,
					0.04f,
					0.04f,
					0.05f,
					0.05f,
					0.06f,
					0.08f,
					0.15f,//10 460nm
					0.29f,
					0.44f,
					0.58f,
					0.70f,
					0.80f,
					0.85f,
					0.89f,
					0.92f,
					0.93f,
					0.95f,//20 520nm
					0.96f,
					0.96f,
					0.95f,
					0.94f,
					0.91f,
					0.89f,
					0.87f,
					0.84f,
					0.79f,
					0.75f,//30 580nm
					0.67f,
					0.59f,
					0.49f,
					0.42f,
					0.34f,
					0.29f,
					0.25f,
					0.22f,
					0.20f,
					0.18f,//40 640nm
					0.17f,
					0.16f,
					0.17f,
					0.18f,
					0.20f,
					0.23f,
					0.26f,
					0.30f,
					0.32f,
					0.35f,//50 700nm
					0.36f,
					0.36f,
					0.36f,
					0.35f,
					0.36f,
					0.37f,
					0.39f,
					0.41f,
					0.43f,
					0.45f,//60 760nm
					0.47f,
					0.49f,
					0.51f,
					0.52f,
					0.54f,
					0.55f,
					0.57f,
					0.58f,
					0.59f,
					0.59f,//70 820nm
					0.59f,
					0.59f,
					0.59f,
					0.58f,
					0.58f,
					0.57f,
					0.56f,
					0.55f,
					0.52f,
					0.51f,//80 880nm
					0.49f,
					0.47f,
					0.45f,
					0.43f,
					0.41f,
					0.40f,
					0.38f,
					0.36f,
					0.34f,
					0.33f,//90 940nm
					0.31f,
					0.29f,
					0.27f,
					0.25f,
					0.24f,
					0.22f,
					0.21f,
					0.19f,
					0.18f,//99 1000nm
					0.16f,//NOT IN SOURCE
					0.14f,//NOT IN SOURCE
					0.11f,//NOT IN SOURCE
					0.08f,//NOT IN SOURCE
					0.05f,//NOT IN SOURCE
					0.01f,//NOT IN SOURCE
					0.002f,//NOT IN SOURCE 1042nm
			}); // 100 real entries + 15 extension entries, range: 346-1042nm, spacing: 6nm
			Sensors::SonyCamera.BlueSpectrum = Spectrum::Create(
				346.0f * Units::nm,
				1042.0f * Units::nm,
				{
					0.01f,// NOT IN SOURCE 346nm
					0.05f,// NOT IN SOURCE
					0.10f,// NOT IN SOURCE
					0.15f,// NOT IN SOURCE
					0.20f,// NOT IN SOURCE
					0.25f,// NOT IN SOURCE
					0.30f,// NOT IN SOURCE
					0.35f,// NOT IN SOURCE
					0.40f,//0 400nm
					0.45f,
					0.50f,
					0.53f,
					0.57f,
					0.61f,
					0.64f,
					0.66f,
					0.68f,
					0.68f,
					0.69f,//10 460nm
					0.68f,
					0.67f,
					0.64f,
					0.60f,
					0.54f,
					0.47f,
					0.40f,
					0.32f,
					0.26f,
					0.21f,//20 520nm
					0.17f,
					0.15f,
					0.12f,
					0.10f,
					0.08f,
					0.07f,
					0.06f,
					0.05f,
					0.04f,
					0.04f,//30 580nm
					0.04f,
					0.03f,
					0.03f,
					0.02f,
					0.02f,
					0.03f,
					0.03f,
					0.03f,
					0.04f,
					0.05f,//40 640nm
					0.05f,
					0.06f,
					0.07f,
					0.07f,
					0.08f,
					0.08f,
					0.08f,
					0.09f,
					0.09f,
					0.09f,//50 700nm
					0.08f,
					0.08f,
					0.07f,
					0.07f,
					0.06f,
					0.06f,
					0.07f,
					0.07f,
					0.08f,
					0.09f,//60 760nm
					0.11f,
					0.15f,
					0.22f,
					0.30f,
					0.40f,
					0.48f,
					0.55f,
					0.59f,
					0.61f,
					0.61f,//70 820nm
					0.60f,
					0.60f,
					0.59f,
					0.58f,
					0.57f,
					0.56f,
					0.55f,
					0.54f,
					0.53f,
					0.50f,//80 880nm
					0.49f,
					0.47f,
					0.45f,
					0.43f,
					0.41f,
					0.40f,
					0.38f,
					0.36f,
					0.34f,
					0.33f,//90 940nm
					0.31f,
					0.29f,
					0.27f,
					0.25f,
					0.24f,
					0.22f,
					0.21f,
					0.19f,
					0.18f,//99 1000nm
					0.16f,//NOT IN SOURCE
					0.14f,//NOT IN SOURCE
					0.11f,//NOT IN SOURCE
					0.08f,//NOT IN SOURCE
					0.05f,//NOT IN SOURCE
					0.01f,//NOT IN SOURCE
					0.002f,//NOT IN SOURCE 1042nm
			}); // 100 real entries + 15 extension entries, range: 346-1042nm, spacing: 6nm
			Sensors::SonyCamera.ColorSpectra = ColorSpectra::Create(
				EmptySpectrum,
				Sensors::SonyCamera.RedSpectrum,
				Sensors::SonyCamera.GreenSpectrum,
				Sensors::SonyCamera.BlueSpectrum,
				glm::vec4(1.0f, 1.0f, 1.0f, 0.0f) / 3.0f // The data extracted resembled the relative sensitivity of the red, green and blue spectra.
			);

			// Data extracted from https://en.wikipedia.org/wiki/CIE_1931_color_space#/media/File:CIE1931_RGBCMF2.png
			Sensors::EyeCamera.RedSpectrum = Spectrum::Create(
				380.0f * Units::nm,
				770.0f * Units::nm, 
				{
						 0.000f,//380nm
						 0.000f,
						 0.000f,//400nm
						 0.000f,
						 0.003f,
						 0.003f,
						-0.003f,
						-0.012f,
						-0.026f,
						-0.040f,
						-0.050f,
						-0.060f,
						-0.071f,//500nm
						-0.090f,
						-0.092f,
						-0.071f,
						-0.030f,
						 0.020f,
						 0.090f,
						 0.170f,
						 0.240f,
						 0.310f,
						 0.345f,//600nm
						 0.340f,
						 0.300f,
						 0.225f,
						 0.160f,
						 0.100f,
						 0.060f,
						 0.032f,
						 0.018f,
						 0.009f,
						 0.005f,//700nm
						 0.002f,
						 0.001f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,//770nm
				}); // 40 real entries + 0 extension entries, range: 380-770nm, spacing: 10nm
			Sensors::EyeCamera.GreenSpectrum = Spectrum::Create(
				380.0f * Units::nm,
				770.0f * Units::nm,
				{
						 0.000f,//380nm
						 0.000f,
						 0.000f,//400nm
						 0.000f,
						 0.000f,
						-0.001f,
						 0.001f,
						 0.009f,
						 0.017f,
						 0.027f,
						 0.040f,
						 0.059f,
						 0.085f,
						 0.130f,
						 0.175f,
						 0.200f,
						 0.216f,
						 0.211f,
						 0.198f,
						 0.170f,
						 0.140f,
						 0.100f,
						 0.060f,//600nm
						 0.037f,
						 0.019f,
						 0.009f,
						 0.003f,
						 0.001f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,//700nm
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,//770nm
				}); // 40 real entries + 0 extension entries, range: 380-770nm, spacing: 10nm
			Sensors::EyeCamera.BlueSpectrum = Spectrum::Create(
				380.0f * Units::nm,
				770.0f * Units::nm,
				{
						 0.000f,//380nm
						 0.004f,
						 0.014f,//400nm
						 0.040f,
						 0.110f,
						 0.240f,
						 0.314f,
						 0.317f,
						 0.300f,
						 0.230f,
						 0.140f,
						 0.080f,
						 0.048f,//500nm
						 0.029f,
						 0.014f,
						 0.006f,
						 0.001f,
						 0.000f,
						-0.001f,
						-0.001f,
						 0.000f,
						 0.000f,
						 0.000f,//600nm
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,//700nm
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,
						 0.000f,//770nm
				}); // 40 real entries + 0 extension entries, range: 380-770nm, spacing: 10nm
			Sensors::EyeCamera.ColorSpectra = ColorSpectra::Create(
				EmptySpectrum,
				Sensors::EyeCamera.RedSpectrum,
				Sensors::EyeCamera.GreenSpectrum,
				Sensors::EyeCamera.BlueSpectrum,
				// glm::vec4(0.0f, 1.0f, 4.5907f, 0.0601f) // 1:4.5907:0.0601 ratio to represent source luminance and 72.0962:1.3791:1 for source radiance.
				// glm::vec4(0.0f, 72.0962f, 1.3791, 1.0f) // 1:4.5907:0.0601 ratio to represent source luminance and 72.0962:1.3791:1 for source radiance.
				glm::vec4(1.0f, 1.0f, 1.0f, 0.0f) / 1.0f
			);

			isInitialized = true;
		}

		// TODO: Look up real values
		const LoFox::Ref<Spectrum> AbsorptionSpectra::FakePaint::Infrared= Spectrum::Create(
			400.0f * Units::nm,
			1000.0f * Units::nm,
			{
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
			});
		const LoFox::Ref<Spectrum> AbsorptionSpectra::FakePaint::Red = Spectrum::Create(
			630.0f * Units::nm,
			645.0f * Units::nm,
			{
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
			}); // Should be 1 peak at 700nm
		const LoFox::Ref<Spectrum> AbsorptionSpectra::FakePaint::Green = Spectrum::Create(
			530.0f * Units::nm,
			545.0f * Units::nm,
			{
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
			}); // Should be 1 peak at 546.1nm
		const LoFox::Ref<Spectrum> AbsorptionSpectra::FakePaint::Blue = Spectrum::Create(
			430.0f * Units::nm,
			445.0f * Units::nm,
			{
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
			}); // Should be 1 peak at 435.8nm


		// TODO: Look up real values
		const LoFox::Ref<Spectrum> EmissionSpectra::LED::Infrared = Spectrum::Create(
			400.0f * Units::nm,
			1000.0f * Units::nm,
			{
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
			},
			true
		);
		// Data extracted from the relative spectral power distribution graph for the XP-G3 Photo Red Standard https://downloads.cree-led.com/files/ds/x/XLamp-XPG3.pdf
		const LoFox::Ref<Spectrum> EmissionSpectra::LED::Red = Spectrum::Create(
			380.0f * Units::nm,
			770 * Units::nm,
			{
				0.00f,//380nm
				0.00f,
				0.00f,//400nm
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,//500nm
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.01f,
				0.02f,
				0.03f,//600nm
				0.06f,
				0.13f,
				0.26f,
				0.58f,
				0.92f,
				0.46f,
				0.12f,
				0.03f,
				0.01f,
				0.00f,//700nm
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,
				0.00f,//770nm
			},
			true
		);
		const LoFox::Ref<Spectrum> EmissionSpectra::LED::Green = Spectrum::Create(
			530.0f * Units::nm,
			545.0f * Units::nm,
			{
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
			},
			true
		); // Should be 1 peak at 546.1nm
		const LoFox::Ref<Spectrum> EmissionSpectra::LED::Blue = Spectrum::Create(
			430.0f * Units::nm,
			445.0f * Units::nm,
			{
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
				1.0f,
			},
			true
		); // Should be 1 peak at 435.8nm
	}
}