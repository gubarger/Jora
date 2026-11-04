#include <Jora/Graphics/CommandPools.hpp>

namespace jora
{

	class Context;

	/**
	 * @brief Базовый класс рендеринга.
	 */

	class Renderer
	{
	public:

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		/**
		 * @brief Передача Vulkan контекста.
		 * @param context Vulkan *(графический контекст)*.
		 */

		explicit Renderer(Context& context) noexcept : context_(context) {}

		~Renderer();

		/**
		 * @brief Удаление перемещения из-за this и потоков.
		 */

		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		/**
		 * @brief Инициализирует коммандные пулы.
		 * @return Код создания пулов *(vk::Result)*.
		 */

		vk::Result Init() noexcept;

		/**
		 * @brief Записывает кадр.
		 */

		void Record();

	private:
		Context& context_;
		CommandPools commandPools_;
	};

}