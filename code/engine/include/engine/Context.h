#pragma once
#include <string>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace gl3::engine::context
{
 /**
  * @brief Event triggered when the window bounds were recomputed.
  */
 struct WindowBoundsRecomputeEvent
 {
  int newWidth;
  int newHeight;
  std::vector<float>* windowBounds; ///< Pointer to the world window bounds vector.
 };

 /**
  * @brief Event for mouse scroll input.
  */
 struct MouseScrollEvent
 {
  double xOffset; ///< Horizontal scroll offset.
  double yOffset; ///< Vertical scroll offset.
 };

 /**
  * @brief The main rendering context for the game.
  *
  * Handles window creation, camera position, zoom, clear color, and window bounds calculations.
  * Is part of the main application loop.
  */
 class Context final
 {
 public:
  /// User-defined callback type for update loop.
  using Callback = std::function<void(Context&)>;

  /**
   * @brief Create the rendering context.
   * @param width Initial window width.
   * @param height Initial window height.
   * @param title Window title.
   * @param camPos Initial camera position.
   * @param camZoom Initial camera zoom.
   */
  explicit Context(int width = 0, int height = 0, const std::string& title = "Game",
                   glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 0.0f), float camZoom = 1.0f);

  /**
   * @brief Destroy the rendering context and free resources.
   */
  ~Context();

  /**
   * @brief Runs the main loop, repeatedly calling the update callback.
   * @param update A function (the game's update) that is called every frame with the Context as an argument.
   */
  void run(const Callback& update);

  /**
   * @brief Set the camera position and center and recalculate the world window bounds.
   * @param position The new camera position.
   * @param center The new camera center.
   */
  void setCameraPosAndCenter(glm::vec3 position, glm::vec3 center);

  /**
   * @brief Move the camera along the X axis, then recompute world window bounds.
   * @param dx The distance to move.
   */
  void moveCameraX(float dx);

  /// @return The GLFW window handle.
  [[nodiscard]] GLFWwindow* getWindow() const { return window; }

  /// @return The current camera position.
  [[nodiscard]] glm::vec3 getCameraPos() const { return cameraPosition; }

  /// @return The camera center point.
  [[nodiscard]] glm::vec3 getCameraCenter() const { return cameraCenter; }

  /// @return The current zoom level.
  [[nodiscard]] float getCurrentZoom() const { return zoom; }

  /**
   * @brief Set the OpenGL clear color.
   * @param color The new clear color.
   */
  void setClearColor(const glm::vec4& color) { clearColor = color; }

  /**
   * @brief Calculate the world-space window bounds based on the camera position.
   */
  void calculateWorldWindowBounds();

  /**
   * @brief Get the current world window bounds.
   * @return Reference to the window bounds vector:
   * {left, right, top, bottom} in world coordinates.
   */
  [[nodiscard]] std::vector<float>& getWorldWindowBounds() { return windowBounds; }

  /**
   * @brief Check if a point is within the visible window bounds.
   * @param position The point to test.
   * @param scale The object's scale.
   * @param margin Optional margin.
   * @return True if the point is inside the visible area.
   */
  [[nodiscard]] bool isInVisibleWindow(const glm::vec2& position, glm::vec2 scale,
                                       float margin = 0.f) const;

 private:
  /**
   * @brief Called when the application exits.
   */
  void onExitApplication() const;

  /**
   * @brief GLFW framebuffer resize callback.
   */
  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

  /**
   * @brief GLFW scroll callback.
   */
  static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

  GLFWwindow* window = nullptr; ///< The GLFW window handle.
  float zoom; ///< Current zoom level.
  glm::vec3 cameraPosition; ///< Camera position.
  glm::vec3 cameraCenter{0.0f, 0.0f, 0.0f}; ///< Camera look-at center.
  glm::vec4 clearColor = {1, 1, 1, 1}; ///< OpenGL clear color.
  std::vector<float> windowBounds; ///< World window bounds: {left, right, top, bottom}.
 };
} // namespace gl3::engine::context
