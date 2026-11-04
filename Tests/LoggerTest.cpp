#include <gtest/gtest.h>

#include <Jora/Core/Logger.hpp>

TEST(BaseLogicLoggerTest, NullMessageInfo)
{
	jora::MessageInfo nullInfo = { "", jora::MessageType::_Null };
	bool isEmptyInfo = nullInfo.Empty();

	EXPECT_EQ(static_cast<int>(nullInfo.message.empty()) & ~static_cast<int>(nullInfo.type), isEmptyInfo)
		<< "The result of evaluating the expression should be \"true\" (a zero jora::MessageInfo isEmptyInfo)";

	nullInfo.message = "Not null";

	isEmptyInfo = nullInfo.Empty();

	EXPECT_EQ(static_cast<int>(nullInfo.message.empty()) & ~static_cast<int>(nullInfo.type), isEmptyInfo)
		<< "The result of evaluating the expression should be \"true\" (a not zero jora::MessageInfo isEmptyInfo)";
}