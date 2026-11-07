#include <Jora/Core/Time.hpp>
#include <Jora/Core/Logger.hpp>

namespace jora
{

	ScopedTimer::~ScopedTimer()
	{
		const auto ms = std::chrono::duration<double, std::milli>(
			std::chrono::steady_clock::now() - start_).count();

		DebugMessenger::Print({
			.message = std::format("{} took {:.3f} ms", loc_.function_name(), ms),
			.type = MessageType::Info });
	}

}