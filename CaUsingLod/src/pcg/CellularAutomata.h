/*
* A class representing a cellular automata.
* The class also contains member functions used for analysing the generated results.
* The class contains functionality needed for implementing multiple layers of detail. This is done using the Scale function.
* The Scale function increases the size of the grid by some multiplier and reinitializing the cells.
*/

#ifndef PCG_CELLULARAUTOMATA_H
#define PCG_CELLULARAUTOMATA_H

#include <vector>
#include <cstdint>
#include <memory>
#include <functional>
#include <vec2.hpp>
#include <queue>
#include <unordered_set>

namespace pcg
{
    struct Direction
    {
        int32_t dx;
        int32_t dy;
        bool operator==(const Direction& other) const;
    };

    class CellularAutomata
    {
    private:
        struct AStarNode;
    public:
        using InitFunction = uint32_t(const CellularAutomata&, uint32_t, uint32_t);
        using RuleFunction = uint32_t(const CellularAutomata&, uint32_t, uint32_t);
        using CostFunction = uint32_t(const CellularAutomata& ca, const AStarNode& from, const glm::uvec2& to);

        struct Cell
        {
            uint32_t type;
            uint32_t x;
            uint32_t y;
        };

        struct GroupAnalysis
        {
            int32_t count = 0;
            int32_t cellType = std::numeric_limits<int32_t>::max();
            uint32_t minX = std::numeric_limits<uint32_t>::max();
            uint32_t maxX = 0u;
            uint32_t minY = std::numeric_limits<uint32_t>::max();
            uint32_t maxY = 0u;
            std::vector<glm::uvec2> positions;
        };

        struct BorderAnalysis
        {
            int32_t jaggedness = 0;
            int32_t length = 0;
            uint32_t cellType = 0u;
            std::vector<Direction> directions;
            std::vector<glm::uvec2> positions;
            uint32_t minX = 0u;
            uint32_t maxX = 0u;
            uint32_t minY = 0u;
            uint32_t maxY = 0u;
        };

        struct PathAnalysis
        {
            std::vector<glm::uvec2> path;
            std::vector<Direction> directions;
            uint32_t cost;
        };

        struct Analysis
        {
            std::vector<GroupAnalysis> groupAnalyses;
            std::vector<BorderAnalysis> borderAnalyses;
            std::vector<PathAnalysis> pathAnalyses;
        };
    private:
        std::vector<Cell> cells;
        uint32_t initWidth;
        uint32_t initHeight;
        uint32_t width;
        uint32_t height;
        std::function<InitFunction> initializer;
        std::function<RuleFunction> rule;
        std::function<CostFunction> costFunction;

        [[nodiscard]]
        uint32_t GetIndex(uint32_t x, uint32_t y) const;
        [[nodiscard]]
        std::vector<Cell> Scale(
            const std::vector<Cell>& cells,
            uint32_t width, uint32_t height, uint32_t multiplier) const;

        //Private analysis functions:
        [[nodiscard]]
        GroupAnalysis AnalyzeGroup(
            uint32_t x, uint32_t y,
            std::vector<bool>& explored) const;
        [[nodiscard]]
        BorderAnalysis AnalyzeBorder(
            int32_t x, int32_t y,
            std::vector<bool>& explored,
            uint32_t cellType,
            int32_t dx, int32_t dy,
            uint32_t startX, uint32_t startY,
            int32_t startDx, int32_t startDy) const;
        bool WithinGrid(int32_t x, int32_t y) const;

        struct AStarNode
        {
            glm::ivec2 position;
            glm::uvec2 previous;
            bool explored = false;
            uint32_t cost = std::numeric_limits<uint32_t>::max();
            uint32_t priority = 0u;
        };

        struct AStarCompare
        {
            bool operator()(
                const AStarNode& first,
                const AStarNode& second) const;
        };

        using PriorityQueue =
            std::priority_queue<
            AStarNode,
            std::vector<AStarNode>,
            AStarCompare>;
        
        void AStarExplore(
            const AStarNode& from,
            glm::vec2 position,
            std::vector<AStarNode>& explored,
            PriorityQueue& frontier,
            glm::uvec2 goal) const;
        [[nodiscard]]
        PathAnalysis BuildPath(
            glm::uvec2 goal,
            const std::vector<AStarNode>& explored) const;
        [[nodiscard]]
        void BorderDistancePeaksAux(
            uint32_t x,
            uint32_t y,
            const BorderAnalysis& borderAnalysis,
            const std::vector<uint32_t>& borderDistances,
            std::vector<bool>& explored,
            std::vector<uint32_t>& distancePeaks) const;
        [[nodiscard]]
        bool IsGap(
            glm::uvec2 position, 
            uint32_t minGapDepth, 
            uint32_t platformCellType) const;
        void InsertIfGap(
            glm::uvec2 position,
            uint32_t minGapDepth,
            uint32_t platformCellType,
            std::unordered_set<glm::uvec2>& gaps,
            uint32_t& gapCount) const;
    public:
        CellularAutomata(uint32_t width, uint32_t height);

        void SetInitializer(std::function<InitFunction> initializer);
        void SetRule(std::function<RuleFunction> rule);
        void SetCostFunction(std::function<CostFunction> costFunction);
        void SetCell(uint32_t type, uint32_t x, uint32_t y);
        [[nodiscard]]
        const Cell& GetCell(uint32_t x, uint32_t y) const;
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        [[nodiscard]]
        std::vector<Cell> Moore(
            uint32_t x, 
            uint32_t y, 
            uint32_t m, 
            const std::vector<uint32_t>& cellTypes) const;
        [[nodiscard]] std::vector<Cell> VonNeumann(
            uint32_t x, uint32_t y, 
            uint32_t m, 
            const std::vector<uint32_t>& cellTypes) const;
        void Step();
        void Generate(uint32_t n);
        void Initialize();
        void Scale(uint32_t multiplier);
        void Clear();
        [[nodiscard]]
        const std::vector<Cell>& GetCells() const;

        //Public analysis functions:
        [[nodiscard]]
        std::vector<GroupAnalysis> AnalyzeGroups() const;
        [[nodiscard]]
        std::vector<BorderAnalysis> AnalyzeBorders() const;
        [[nodiscard]]
        PathAnalysis AnalyzePath(
            glm::uvec2 from,
            glm::uvec2 to) const;
        [[nodiscard]]
        std::vector<uint32_t> BorderDistancesGrid(
            const BorderAnalysis& borderAnalysis) const;
        [[nodiscard]]
        std::vector<uint32_t> BorderDistances(
            const BorderAnalysis& borderAnalysis) const;
        [[nodiscard]]
        std::vector<uint32_t> BorderDistancePeaks(
            const BorderAnalysis& borderAnalysis) const;
        [[nodiscard]]
        std::vector<uint32_t> BorderDistances(
            const std::vector<BorderAnalysis>& borderAnalyses,
            uint32_t cellType) const;
        [[nodiscard]]
        std::vector<uint32_t> BorderDistancePeaks(
            const std::vector<BorderAnalysis>& borderAnalyses,
            uint32_t cellType) const;
        [[nodiscard]]
        uint32_t InAir(const PathAnalysis& pathAnalysis, uint32_t airCellType) const;
        [[nodiscard]]
        uint32_t OnSurface(const PathAnalysis& pathAnalysis, uint32_t airCellType) const;

        struct Platform
        {
            uint32_t height;
            glm::uvec2 left;
            glm::uvec2 right;
        };

        [[nodiscard]]
        std::vector<Platform> Platforms(
            const GroupAnalysis& groupAnalysis,
            uint32_t platformCellType) const;
        [[nodiscard]]
        uint32_t GapCount(
            const GroupAnalysis& groupAnalysis, 
            uint32_t minGapDepth,
            uint32_t platformCellType) const;
    };

    [[nodiscard]]
    std::vector<uint32_t> straightLines(
        const std::vector<Direction>& directions);
}

#endif
