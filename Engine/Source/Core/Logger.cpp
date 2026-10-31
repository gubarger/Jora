#include <Jora/Core/Logger.hpp>

#include <print>
#include <iostream>
#include <cassert>

namespace jora
{
	std::pair<std::ostream&, std::string_view> MessageTypeToString(const MessageType type)
	{
		switch (type)
		{
		case MessageType::Default:	return { std::cout, "Default" };
		case MessageType::Success:	return { std::cout, "Success" };
		case MessageType::Error:	return { std::cerr, "Error" };
		case MessageType::Warning:	return { std::cerr, "Warning" };

		default: return { std::cerr, "Unknown" };
		}
	}

	void MessageLogging::Message(const std::string_view message, const MessageType type)
	{
		const auto [ostream, strType] = MessageTypeToString(type);

		std::print(ostream, "[Jora Engine]\t-- {}: {}\n", strType, message);
	}

}