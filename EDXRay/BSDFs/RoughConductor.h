#pragma once

#include "../Core/BSDF.h"

namespace EDX
{
	namespace RayTracer
	{
		class RoughConductor : public BSDF
		{
		private:
			float mRoughness;

		public:
			RoughConductor(const Color& reflectance = Color::WHITE, float roughness = 1.0f)
				: BSDF(ScatterType(BSDF_REFLECTION | BSDF_GLOSSY), BSDFType::RoughConductor, reflectance)
				, mRoughness(roughness)
			{
			}
			RoughConductor(const RefPtr<Texture2D<Color>>& pTex, float roughness = 1.0f)
				: BSDF(ScatterType(BSDF_REFLECTION | BSDF_GLOSSY), BSDFType::RoughConductor, pTex)
				, mRoughness(roughness)
			{
			}
			RoughConductor(const char* pFile, float roughness = 1.0f)
				: BSDF(ScatterType(BSDF_REFLECTION | BSDF_GLOSSY), BSDFType::RoughConductor, pFile)
				, mRoughness(roughness)
			{
			}

			Color SampleScattered(const Vector3& _wo,
				const Sample& sample,
				const DifferentialGeom& diffGeom,
				Vector3* pvIn, float* pPdf,
				ScatterType types = BSDF_ALL,
				ScatterType* pSampledTypes = NULL) const;

		private:
			float Pdf(const Vector3& wo, const Vector3& wi, ScatterType types = BSDF_ALL) const
			{
				if (BSDFCoordinate::CosTheta(wo) * BSDFCoordinate::CosTheta(wi) <= 0.0f)
					return 0.0f;

				Vector3 wh = Math::Normalize(wo + wi);

				if (BSDFCoordinate::CosTheta(wh) < 0.0f)
					wh *= -1.0f;

				float dwh_dwi = 1.0f / (4.0f * Math::Dot(wi, wh));
				float whProb = GGX_Pdf(wh, mRoughness);

				return Math::Abs(whProb * dwh_dwi);
			}

			float Eval(const Vector3& wo, const Vector3& wi, ScatterType types = BSDF_ALL) const
			{
				if (BSDFCoordinate::CosTheta(wo) * BSDFCoordinate::CosTheta(wi) <= 0.0f)
					return 0.0f;

				Vector3 wh = Math::Normalize(wo + wi);

				float D = GGX_D(wh, mRoughness);
				if (D == 0.0f)
					return 0.0f;

				float F = BSDF::FresnelConductor(Math::Dot(wo, wh), 0.4f, 1.6f);
				float G = GGX_G(wo, wi, wh, mRoughness);

				return F * D * G / (4.0f * BSDFCoordinate::AbsCosTheta(wi) * BSDFCoordinate::AbsCosTheta(wo));
			}

		public:
			int GetParameterCount() const { return BSDF::GetParameterCount() + 1; }
			string GetParameterName(const int idx) const
			{
				if (idx < BSDF::GetParameterCount())
					return BSDF::GetParameterName(idx);

				if (idx == GetParameterCount() - 1)
					return "Roughness";

				return "";
			}
			bool GetParameter(const string& name, float* pVal, float* pMin = nullptr, float* pMax = nullptr) const
			{
				if (BSDF::GetParameter(name, pVal, pMin, pMax))
					return true;

				if (name == "Roughness")
				{
					*pVal = this->mRoughness;
					if (pMin)
						*pMin = 0.001f;
					if (pMax)
						*pMax = 1.0f;
					return true;
				}

				return false;
			}
			void SetParameter(const string& name, const float value)
			{
				BSDF::SetParameter(name, value);

				if (name == "Roughness")
					this->mRoughness = value;

				return;
			}
		};
	}
}