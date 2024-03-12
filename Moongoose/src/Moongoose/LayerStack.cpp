#include "mgpch.h"

#include "LayerStack.h"

namespace Moongoose {

	LayerStack::LayerStack()
	{
		m_LayerInsert = m_Layers.begin();
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers) delete layer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer);
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto stackedLayer = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (stackedLayer != m_Layers.end())
		{
			m_Layers.erase(stackedLayer);
			m_LayerInsert--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto stackedOverlay = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (stackedOverlay != m_Layers.end())
		{
			m_Layers.erase(stackedOverlay);
		}
	}

}