// Copyright 2023 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "SceneSelector.h"
// anari
#include <anari_test_scenes.h>

namespace windows {

static bool UI_callback(void *_stringList, int index, const char **out_text)
{
  auto &stringList = *(std::vector<std::string> *)_stringList;
  *out_text = stringList[index].c_str();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SceneSelector::SceneSelector(const char *name) : Window(name, true)
{
  m_categories = anari::scenes::getAvailableSceneCategories();
  m_scenes.resize(m_categories.size());
  for (int i = 0; i < m_categories.size(); i++) {
    m_scenes[i] =
        anari::scenes::getAvailableSceneNames(m_categories[i].c_str());
  }
}

void SceneSelector::buildUI()
{
  bool newCategory = ImGui::Combo("category##whichCategory",
      &m_currentCategory,
      UI_callback,
      &m_categories,
      m_categories.size());

  if (newCategory)
    m_currentScene = 0;

  auto &sceneList = m_scenes[m_currentCategory];

  bool newScene = ImGui::Combo("scene##whichScene",
      &m_currentScene,
      UI_callback,
      &sceneList,
      sceneList.size());

  if (newCategory || newScene)
    notify();
}

void SceneSelector::setCallback(SceneSelectionCallback cb)
{
  m_callback = cb;
  notify();
}

void SceneSelector::notify()
{
  if (m_callback) {
    const char *c = m_categories[m_currentCategory].c_str();
    const char *s = m_scenes[m_currentCategory][m_currentScene].c_str();
    m_callback(c, s);
  }
}

} // namespace windows