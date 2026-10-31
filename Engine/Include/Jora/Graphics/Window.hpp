#include <cstdint>
#include <utility>

struct GLFWwindow;

namespace jora
{

	/**
	 * @brief Обертка над glfw3 бибилотекой.
	 */

	class Window {
	public:

		~Window();

		/**
		 * @brief Создание GLFWwindow.
		 * @return int32_t код (из GLFW).
		 */

		int32_t CreateWindow();

		/**
		 * @brief
		 * @return
		 */

		bool WindowCloseCheck() const;

		/**
		 * @brief Выдает GLFWwindow из приватного поля.
		 * @return GLFWwindow*
		 */

		GLFWwindow* GetWindow() const;

	private:
		GLFWwindow* window_ = nullptr;
	};

	/**
	 * @brief Получить расширения системы.
	 * @return std::pair из количества расширений (uint32_t) и текста их (const char**).
	 */

	std::pair<uint32_t, const char**> GetWindowExtensions();

	/**
	 * @brief Обработка GLFW events.
	 */

	void HandlingEvens();

}