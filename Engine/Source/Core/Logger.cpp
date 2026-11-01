#include <Jora/Core/Logger.hpp>

#include <print>
#include <iostream>
#include <cassert>

namespace
{

	/**
	 * @brief Обертка над дублирующим кодом для вывода логов.
	 * @param queue Приоритетная очередь.
	 */

	void PrintMessage(const std::priority_queue<jora::MessageBox>& queue)
	{}

}

namespace jora
{
	std::pair<std::ostream&, std::string_view> MessageTypeToString(const MessageType type)
	{
		switch (type)
		{
		case MessageType::Average:	return { std::cout, "Average" };
		case MessageType::Debug:	return { std::cout, "Debug" };
		case MessageType::Default:	return { std::cerr, "Default" };
		case MessageType::Highest:	return { std::cerr, "Highest" };

		default: return { std::cerr, "Unknown" };
		}
	}

	void MessageSplittingManager::Message(const MessageBox messageInfo)
	{
		AddMessageInQueue(messageInfo);

		const auto& currentMessage = queue_.top();
		const auto currentTypeInfo = jora::MessageTypeToString(currentMessage.type.value());
		std::print(currentTypeInfo.first
			, "[Jora Engine]\t-- {}: {}\n", currentTypeInfo.second, currentMessage.message.value());
	}

	std::string ConstructMessage(const std::initializer_list<std::string_view> messages)
	{
		std::string result;

		result.reserve(messages.size());

		for (auto message : messages)
		{
			result += message;
		}

		return result;
	}
}