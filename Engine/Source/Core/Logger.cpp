#include <Jora/Core/Logger.hpp>

#include <print>
#include <iostream>

namespace jora
{

	void BasicMessenger::Print(const MessageInfo& info)
	{
		if (info.Empty())
		{
			std::print(std::cout, "Jora Engine | [Error]: MessageInfo is null, and it is impossible to show.\n");
			return;
		}

		std::print(std::cout, "Jora Engine | [{}]: {}\n", ViewMessageType(info.type), info.message);
	}

	void BasicMessenger::Say(const MessageInfo& info)
	{
		Print(info);
		container_.push_back(info);
	}

}