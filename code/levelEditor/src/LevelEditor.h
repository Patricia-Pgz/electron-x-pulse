#pragma once
#include "engine/Game.h"

namespace gl3 {

class LevelEditor : public engine::Game {

  public:
    LevelEditor(int width, int height, const std::string& title, const glm::vec3& camPos, float camZoom);
    ~LevelEditor() override;
};

} // gl3
