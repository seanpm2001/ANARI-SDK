## Copyright 2023 The Khronos Group
## SPDX-License-Identifier: Apache-2.0

include(cmake/get_match3D.cmake)
set(match3D_CACHE_IN_SOURCE_TREE ON)
find_package(match3D REQUIRED stb_image nativefiledialog)

project(anariViewer)

project_add_executable()
project_sources(
PRIVATE
  windows/LightsEditor.cpp
  windows/SceneSelector.cpp
  windows/Viewport.cpp
  Orbit.cpp
  main.cpp
  ui_anari.cpp
  ui_layout.cpp
)
project_link_libraries(PRIVATE anari anari_test_scenes match3D::match3D)
