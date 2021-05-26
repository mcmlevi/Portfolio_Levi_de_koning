#include "Thpch.h"
#include <Components/LightComponent.h>
#include <Components/TransformComponent.h>
namespace Th
{
    namespace Components
    {
        
        LightComponent::LightComponent()
        {
            m_Color         = glm::vec4(255, 255, 255, 255);
            m_LightType     = LT_Spot;
            m_Intensity     = 10;
            m_Radius        = 10;
            m_SpotAngle     = 90;
            m_ConstantAttenuation   = 1;
            m_LinearAttenuation     = 1;
            m_QuadraticAttenuation  = 1;
        }

        glm::vec4 LightComponent::GetColor() const
        {
            return m_Color;
        }

        void LightComponent::SetColor(const glm::vec4& a_Color)
        {
            m_Color = a_Color;
        }

        E_LightType LightComponent::GetLightType() const
        {
            return m_LightType;
        }

        void LightComponent::SetLightType(const E_LightType& a_LightType)
        {
            m_LightType = a_LightType;
        }

        float LightComponent::GetIntesity() const
        {
            return m_Intensity;
        }

        void LightComponent::SetIntensity(const float& a_Intensity)
        {
            m_Intensity = a_Intensity;
        }

        float LightComponent::GetRadius() const
        {
            return m_Radius;
        }

        void LightComponent::SetRadius(const float& a_Radius)
        {
            m_Radius = a_Radius;
        }

        float LightComponent::GetSpotAngle() const
        {
            return m_SpotAngle;
        }

        void LightComponent::SetSpotAngle(const float& a_SpotAngle)
        {
            m_SpotAngle = a_SpotAngle;
        }

        float LightComponent::GetAttenuation(const float& a_Distance) const
        {
            return 1.0f / (m_ConstantAttenuation + m_LinearAttenuation * a_Distance + m_QuadraticAttenuation * a_Distance * a_Distance);
        }

        void LightComponent::SetAttentuation(const float& a_Constant, const float& a_Linear, const float& a_Quadratic)
        {
            m_ConstantAttenuation = a_Constant;
            m_LinearAttenuation = a_Linear;
            m_QuadraticAttenuation = a_Quadratic;
        }

        void LightComponent::GetHLSLBoundry(const TransformComponent& a_Transform, float* a_HLSLBoundry)
        {
            a_HLSLBoundry[0] = a_Transform.GetPosition().x;
            a_HLSLBoundry[1] = a_Transform.GetPosition().y;
            a_HLSLBoundry[2] = a_Transform.GetPosition().z;
            a_HLSLBoundry[3] = 0;

            a_HLSLBoundry[4] = a_Transform.GetForward().x;
            a_HLSLBoundry[5] = a_Transform.GetForward().y;
            a_HLSLBoundry[6] = a_Transform.GetForward().z;
            a_HLSLBoundry[7] = 0;

            a_HLSLBoundry[8] = m_Color.r;
            a_HLSLBoundry[9] = m_Color.g;
            a_HLSLBoundry[10] = m_Color.b;
            a_HLSLBoundry[11] = m_Color.a;

            a_HLSLBoundry[12] = m_LightType;
            a_HLSLBoundry[13] = m_Intensity;
            a_HLSLBoundry[14] = m_Radius;
            a_HLSLBoundry[15] = m_SpotAngle;

            a_HLSLBoundry[16] = m_ConstantAttenuation;
            a_HLSLBoundry[17] = m_LinearAttenuation;
            a_HLSLBoundry[18] = m_QuadraticAttenuation;
            a_HLSLBoundry[19] = 0;
        }
    }
}