#include "pch.h"
#include "..\Include\InteractAbleComponent.h"

InteractAbleComponent::InteractAbleComponent() {
	m_Initialized = true;
}

InteractAbleComponent::~InteractAbleComponent() {

}

void InteractAbleComponent::SetInteractableType(int type) {
	m_CurrentInteractableType = type;
}
