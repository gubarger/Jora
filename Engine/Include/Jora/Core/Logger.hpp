#pragma once

#include <string>
#include <vector>
#include <optional>

#ifdef NDEBUG
inline constexpr bool MessengerEnabled = false;
#else
inline constexpr bool MessengerEnabled = true;
#endif

namespace jora
{

	/**
	 * @brief Перечисление для уровня логирования.
	 */

	enum class MessageType : uint32_t
	{
		Debug = 1,
		Info = 2,
		Warning = 3,
		Error = 4,
		Critical = 5,

		_Null = 0 // Используется лишь для проверки (ести ли значение).
	};

	/**
	 * @brief Отдает тип лога в виде проэкции строки.
	 * @param type Тип лога.
	 * @return проэкцию строки (`std::string_view`)
	 */

	constexpr std::string_view ViewMessageType(const MessageType type)
	{
		switch (type)
		{
		case MessageType::Debug:	return "Debug";
		case MessageType::Info:		return "Info";
		case MessageType::Warning:	return "Warning";
		case MessageType::Error:	return "Error";
		case MessageType::Critical: return "Critical";
		[[unlikely]] case MessageType::_Null:	return "";
		default: std::unreachable(); // Переданы неверные данные.
		}
	}

	struct MessageInfo
	{
		std::string message; //< Сообщение лога
		MessageType type; //< Тип лога

		/**
		 * @brief Проверяет пустой ли type (`_Null`) и строка.
		 * @return true или false (в зависимости от того, есть ли данные).
		 */

		constexpr const bool Empty() const noexcept
		{
			return (static_cast<uint32_t>(type) == 0) && message.empty();
		}
	};

	/**
	 * @brief Пустой логгер над `BasicMessenger`
	 */

	class NullMessenger
	{
	public:
		constexpr static void Print([[maybe_unused]] const MessageInfo&) noexcept {}

		constexpr void Say([[maybe_unused]] const MessageInfo&) const noexcept {}

		constexpr void AddInfo([[maybe_unused]] const MessageInfo&) const noexcept {}

		constexpr void Destroy() const noexcept {}

		const std::vector<MessageInfo>& GetHistory() const noexcept
		{
			static const std::vector<MessageInfo> empty;
			return empty;
		}

		const std::optional<MessageInfo>& LastInfo() const noexcept
		{
			static MessageInfo empty;
			return empty;
		}
	};

	/**
	 * @brief Базовый класс логирования.
	 */

	class BasicMessenger
	{
	public:

		BasicMessenger(BasicMessenger&&) noexcept = default;
		BasicMessenger& operator=(BasicMessenger&&) noexcept = default;

		BasicMessenger() = default;

		~BasicMessenger() { Destroy(); }

		BasicMessenger(const BasicMessenger&) = delete;
		BasicMessenger& operator=(const BasicMessenger&) = delete;

		/**
		 * @brief Отдает историю из приватного поля класса.
		 * @return Массив из `MessageInfo`
		 */

		constexpr const std::vector<MessageInfo>& GetHistory() noexcept
		{
			return container_;
		}

		/**
		 * @brief Вывести информацию разом без сохранения.
		 * @param info Информация о логе.
		 */

		static void Print(const MessageInfo& info);

		/**
		 * @brief Вывести информацию разом c сохранением истории.
		 * @param info Информация о логе.
		 */

		void Say(const MessageInfo& info);

		/**
		 * @brief Удаляет весь массив.
		 */

		constexpr void Destroy() noexcept
		{
			if (container_.empty()) { return; }
			container_.clear();
		}

		/**
		 * @brief Добовляет лог в массив истории (если нужно).
		 * @param info Информация о логе.
		 */

		constexpr void AddInfo(const MessageInfo& info) noexcept
		{
			container_.push_back(info);
		}

		/**
		 * @brief Отдает последний лог в списке.
		 * @return `std::nullopt`, если массив пуст. В противном случае ссылку на MessageInfo.
		 */

		constexpr const std::optional<MessageInfo>& LastInfo() noexcept
		{
			if (container_.empty()) { return std::nullopt; }
			return container_.back();
		}

	private:
		std::vector<MessageInfo> container_; //< История логов.
	};

	/**
	 * @brief Базовый класс для Debug логов.
	 * @tparam _EnabledDebug
	 */

	template<bool _EnabledDebug>
	class BasicDebugMessenger
	{
	private:
		/**
		 * @brief Хранилище для логов. В Release занимает 0 байт.
		 */

		using Storage = std::conditional_t<_EnabledDebug, BasicMessenger, NullMessenger>;

	public:
		BasicDebugMessenger() = default;

		/**
		 * @brief Переносит основной логгер к BasicDebugMessenger.
		 * @param base Базовый класс логера.
		 */

		explicit BasicDebugMessenger([[maybe_unused]] BasicMessenger&& base)
		{
			if constexpr (_EnabledDebug)
				storage_ = std::move(base);
		}

		/**
		 * @brief Вывести информацию разом без сохранения. Работает только в Debug.
		 * @param info Информация о логе.
		 */

		static void Print([[maybe_unused]] const MessageInfo& info)
		{
			Storage::Print(info); // Для Release пусто
		}

		/**
		 * @brief Выводит и сохраняет лог. Работает только в Debug.
		 * @param info Информация о логе.
		 */

		void Say([[maybe_unused]] const MessageInfo& info)
		{
			storage_.Say(info); // Для Release пусто
		}

		/**
		 * @brief Выдает Базовый класс логирования.
		 * @return Измененный тип хранилища.
		 */

		Storage* GetHandle() noexcept { return &storage_; }

	private:
		[[no_unique_address]] Storage storage_; //< Хранилище истории для Debug.
	};

	using DebugMessenger = BasicDebugMessenger<MessengerEnabled>;
}