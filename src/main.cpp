#include "main.h"
#include "../editor/editor_app.h"

// LEARNING MAP
// This file is deliberately tiny. It only starts the engine and supplies an
// application. The actual example game lives in editor/editor_app.cpp.
// Learn next: src/engine.cpp, then editor/editor_app.cpp.

eclipse::App* CreateApp() { return new eclipse::editor::EditorApp; }
