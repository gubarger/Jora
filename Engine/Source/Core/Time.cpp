#include <Jora/Core/Time.hpp>
#include <Jora/Core/Logger.hpp>

namespace jora
{

	void GlobalTime::View() const noexcept
	{
		DebugMessenger::Print({
			.message = "Global time | DeltaTime = " + std::to_string(deltaTime_) +
			" | Last FPS = " + std::to_string(lastFps_.time_since_epoch().count()) +
			" | Frame Count = " + std::to_string(frameCount_), .type = MessageType::Info });
	}

}