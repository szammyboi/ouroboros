#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <type_traits>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "window.h"
#include "mem.h"

class View
{
public:
	virtual ~View() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate() = 0;

	template <typename PanelType, typename... Args>
	static inline Shared<View> CreateView(const Window& window, Args&&... args)
	{
		static_assert(std::is_base_of<View, PanelType>::value, "PanelType must be a Panel");

		PanelType* ptr = new PanelType(window, std::forward<Args>(args)...);

		Shared<View> view(ptr, [](View* v)
		{
			v->OnDetach();
			delete v;
		});

		view->OnAttach();
		return view;
	}

	template <typename PanelType, typename... Args>
	inline void AddView(Args&&... args)
	{
		std::type_index type_id = typeid(PanelType);

		if (m_Views.find(type_id) != m_Views.end())
			return;

		Shared<View> view = View::CreateView<PanelType>(m_Window, std::forward<Args>(args)...);
		m_Views[type_id] = std::move(view);
	}

	template <typename PanelType, typename... Args>
	inline void RemoveView(Args&&... args)
	{
		static_assert(std::is_base_of<View, PanelType>::value, "PanelType must be a Panel");
		std::type_index type_id = typeid(PanelType);

		auto iterator = m_Views.find(type_id);

		if (iterator == m_Views.end())
			return;

		//iterator->second->OnDetach();
		m_Views.erase(iterator);
	}

	template <typename PanelType, typename... Args>
	inline Shared<PanelType> GetView(Args&&... args)
	{
		static_assert(std::is_base_of<View, PanelType>::value, "PanelType must be a Panel");
		std::type_index type_id = typeid(PanelType);

		auto iterator = m_Views.find(type_id);

		if (iterator == m_Views.end())
			return nullptr;

		return std::dynamic_pointer_cast<PanelType>(iterator->second);
	}

	template <typename PanelType, typename... Args>
	inline void ReplaceView(Args&&... args)
	{
		RemoveView<PanelType>();
		AddView<PanelType>(std::forward<Args>(args)...);
	}

	inline void Slot()
	{
		for (const auto& [type, view] : m_Views)
		{
			view->OnUpdate();
		}
	}

	template <typename Callback>
	inline void Slot(Callback&& func)
	{
		int i = 0;
		for (const auto& [type, view] : m_Views)
		{
			if constexpr (std::is_invocable_v<Callback, int, View&>)
				func(i, *view);
			else if constexpr (std::is_invocable_v<Callback, int>)
				func(i);
			else if constexpr (std::is_invocable_v<Callback, View&>)
				func(*view);
			else if constexpr (std::is_invocable_v<Callback>)
				func();
			else
				static_assert(false, "Invalid Slot Callback Signature");

			view->OnUpdate();
			i++;
		}
	}

protected:
	View(const Window& window)
	    : m_Window(window) {}

protected:
	const Window& m_Window;
	std::unordered_map<std::type_index, Shared<View>> m_Views;
};
