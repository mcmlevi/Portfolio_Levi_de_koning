#pragma once
#include <glm/vec4.hpp>
#include <Components/TransformComponent.h>

namespace Th
{
	namespace Components
	{		
		enum E_LightType
		{
			LT_Directional = 0,
			LT_Spot = 1,
			LT_Point = 2
		};

		/// <summary>
		/// The point light component is used to enlighten sections defined by a radius within the scene.
		/// </summary>
		struct LightComponent
		{
			
		private:

			glm::vec4 m_Color;				// 16 bytes.
			// -----------------------------// Boundry. 
			E_LightType m_LightType;		// 4 bytes.
			float m_Intensity;				// 4 bytes.
			float m_Radius;					// 4 bytes.
			float m_SpotAngle;				// 4 bytes.
			// -----------------------------// Boundry.
			float m_ConstantAttenuation;	// 4 bytes.
			float m_LinearAttenuation;		// 4 bytes.
			float m_QuadraticAttenuation;	// 4 bytes.
			// -----------------------------// 4 bytes remaining before boundry.
			// Total 60 bytes, missing 4.

		public:

			LightComponent();
			~LightComponent() = default;

			/// <summary>
			/// </summary>
			/// <returns>The color of this lightcomponent.</returns>
			glm::vec4 GetColor() const;

			/// <summary>
			/// Set the color of this lightcomponent.
			/// </summary>
			void SetColor(const glm::vec4&);

			/// <summary>
			/// </summary>
			/// <returns>The type of this lightcomponent. Types are: Directional, Spot, Point.</returns>
			E_LightType GetLightType() const;

			/// <summary>
			/// Set the type of this lightcomponent. Types are: Directional, Spot, Point.
			/// </summary>
			void SetLightType(const E_LightType&);

			/// <summary>
			/// </summary>
			/// <returns>The intensity of this lightcomponent.</returns>
			float GetIntesity() const;

			/// <summary>
			/// Set the intensity of this lightcomponent.
			/// </summary>
			void SetIntensity(const float&);

			/// <summary>
			/// </summary>
			/// <returns>The radius of this lightcomponent</returns>
			float GetRadius() const;

			/// <summary>
			/// Set the radius of this lightcomponent.
			/// </summary>
			void SetRadius(const float&);

			/// <summar>
			/// </summary>
			/// <returns>The spot angle of this lightcomponent.</returns>
			float GetSpotAngle() const;

			/// <summary>
			/// Set the spot angle of this lightcomponent.
			/// </summary>
			void SetSpotAngle(const float&);

			/// <summary> 
			/// </summary>
			/// <param name="Distance"></param>
			/// <returns>The attentuation using the constant, linear and quadratic attentuation.</returns>
			float GetAttenuation(const float&) const;

			/// <summary>
			/// Set the attentiations.
			/// </summary>
			/// <param name="Constant"></param>
			/// <param name="Linear"></param>
			/// <param name="Quadratic"></param>
			void SetAttentuation(const float&, const float&, const float&);

			/// <summary>
			/// Deprecated.
			/// </summary>
			/// <param name="">Transform of this LightComponent.</param>
			/// <param name="">Array with 20 entries.</param>
			void GetHLSLBoundry(const TransformComponent&, float*);
		};
	}
}