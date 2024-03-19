#include "mgpch.h"

#include "LayerStack.h"

namespace Moongoose {

	LayerStack::LayerStack() {}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
		layer->onAttach();
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto stackedLayer = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (stackedLayer != m_Layers.end())
		{
			m_Layers.erase(stackedLayer);
			m_LayerInsertIndex--;
			(*stackedLayer)->onDetach();
		}
	}

}