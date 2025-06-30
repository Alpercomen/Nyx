#pragma once
#include <Application/Core/Core.h>
#include <Application/Core/Input/InputEvent.h>

namespace Nyx {
	class InputQueue {
	public:
		void PushEvent(const InputEvent& event) { m_inputQueue.push(event); }
		bool8 PopEvent(InputEvent& event) 
		{
			if (!m_inputQueue.empty())
			{
				event = m_inputQueue.front();
				m_inputQueue.pop();
				return true;
			}
			return false; 
		}

		void Clear() 
		{
			while (!m_inputQueue.empty())
				m_inputQueue.pop();
		}
		bool8 IsEmpty() const { return m_inputQueue.empty(); }

	private:
		Queue<InputEvent> m_inputQueue;
	};
}