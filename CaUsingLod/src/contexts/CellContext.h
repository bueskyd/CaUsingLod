/*
* A class for setting up and working with cellular automata.
* Reads user input and renders CA on window.
*/

#ifndef PCG_CELLCONTEXT_H
#define PCG_CELLCONTEXT_H

#include <vec2.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "graphics/renderers/CellRenderer.h"
#include "graphics/Camera2d.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Window.h"
#include "pcg/ui/HistogramHeatMap.h"
#include "pcg/Generators/CaveGenerator.h"
#include "pcg/Generators/CaveLodGenerator.h"

namespace pcg
{
    enum class GameplayType
    {
        TopDown, SideScroller
    };

    class CellContext
    {
    private:
        using GeneratorType0 = CaveGenerator;
        using GeneratorType1 = CaveLodGenerator;

        Window* window;
        std::unique_ptr<CellRenderer> renderer;
        std::unique_ptr<ShaderProgram> shaderProgram;
        std::unique_ptr<Camera2d> camera;
        std::unique_ptr<GeneratorType0> generator0;
        std::unique_ptr<GeneratorType1> generator1;
        std::unique_ptr<HistogramHeatMap> heatMap;

        void Initialize();
        void DrawGenerated(const Generator& generator);
        void Analyze(GameplayType gameplayType);
    public:
        explicit CellContext(Window* window);
        void Update();
        void ClearCells();
    };
}

#endif
