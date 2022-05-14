#include "CellularAutomata.h"
#include "Hash.h"
#include <cmath>
#include <iostream>
#include <stack>
#include <map>
#include "Heuristic.h"

namespace pcg
{
    CellularAutomata::CellularAutomata(uint32_t width, uint32_t height)
        : width(width), height(height), initWidth(width), initHeight(height)
    {
        cells.resize(width * height);
    }

    uint32_t getIndex(uint32_t x, uint32_t y, uint32_t width)
    {
        return x + y * width;
    }

    uint32_t CellularAutomata::GetIndex(uint32_t x, uint32_t y) const
    {
        return getIndex(x, y, width);
    }

    void CellularAutomata::SetInitializer(std::function<InitFunction> initializer)
    {
        this->initializer = initializer;
    }

    void CellularAutomata::SetRule(std::function<RuleFunction> rule)
    {
        this->rule = rule;
    }

    void CellularAutomata::SetCostFunction(std::function<CostFunction> costFunction)
    {
        this->costFunction = costFunction;
    }

    void CellularAutomata::SetCell(uint32_t type, uint32_t x, uint32_t y)
    {
        cells[GetIndex(x, y)] = { type, x, y };
    }

    const CellularAutomata::Cell& CellularAutomata::GetCell(uint32_t x, uint32_t y) const
    {
        return cells[GetIndex(x, y)];
    }

    uint32_t CellularAutomata::GetWidth() const
    {
        return width;
    }

    uint32_t CellularAutomata::GetHeight() const
    {
        return height;
    }

    static bool contains(const std::vector<uint32_t>& cells, uint32_t type)
    {
        for (size_t i = 0; i < cells.size(); i++)
            if (cells[i] == type)
                return true;
        return false;
    }

    std::vector<CellularAutomata::Cell> CellularAutomata::Moore(
        uint32_t x, 
        uint32_t y, 
        uint32_t m, 
        const std::vector<uint32_t>& cellTypes) const
    {
        int32_t sx = static_cast<int32_t>(x);
        int32_t sy = static_cast<int32_t>(y);
        int32_t sm = static_cast<int32_t>(m);

        std::vector<Cell> neighbourhood;
        for (int32_t x0 = sx - sm; x0 <= sx + sm; x0++)
            for (int32_t y0 = sy - sm; y0 <= sy + sm; y0++)
            {
                if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
                {
                    const auto& cell = GetCell(x0, y0);
                    if (contains(cellTypes, cell.type))
                        neighbourhood.push_back(cell);
                }
            }
        return neighbourhood;
    }

    std::vector<CellularAutomata::Cell> CellularAutomata::VonNeumann(
        uint32_t x,
        uint32_t y,
        uint32_t r,
        const std::vector<uint32_t>& cellTypes) const
    {
        int32_t sx = static_cast<int32_t>(x);
        int32_t sy = static_cast<int32_t>(y);

        int32_t sr = static_cast<int32_t>(r);

        std::vector<Cell> neighbourhood;
        for (int32_t y0 = -sr; y0 <= sr; y0++)
        {
            int32_t y1 = sy + y0;
            if (y1 < 0 || y1 >= height)
                continue;
            int32_t d = std::abs(y0) - sr;
            for (int32_t x0 = d; x0 <= -d; x0++)
            {
                int32_t x1 = sx + x0;
                if (x1 < 0 || x1 >= width)
                    continue;
                const auto& cell = GetCell(x1, y1);
                if (contains(cellTypes, cell.type))
                    neighbourhood.push_back(cell);
            }
        }
        return neighbourhood;
    }

    void CellularAutomata::Step()
    {
        std::vector<Cell> newCells;
        newCells.resize(cells.size());

        for (uint32_t x = 0; x < width; x++)
        {
            for (uint32_t y = 0; y < height; y++)
            {
                uint32_t newCell = rule(*this, x, y);
                newCells[GetIndex(x, y)] = { newCell, x, y };
            }
        }
        cells = std::move(newCells);
    }

    void CellularAutomata::Generate(uint32_t n)
    {
        for (size_t i = 0; i < n; i++)
            Step();
    }

    void CellularAutomata::Initialize()
    {
        for (uint32_t y = 0; y < height; y++)
            for (uint32_t x = 0; x < width; x++)
                cells[GetIndex(x, y)] = { initializer(*this, x, y), x, y };
    }

    std::vector<CellularAutomata::Cell> CellularAutomata::Scale(
        const std::vector<Cell>& cells,
        uint32_t width, uint32_t height, uint32_t multiplier) const
    {
        uint32_t scaledWidth = width * multiplier;
        uint32_t scaledHeight = height * multiplier;
        std::vector<Cell> newCells;
        newCells.resize(scaledWidth * scaledHeight);

        for (uint32_t y = 0u; y < scaledHeight; y++)
        {
            for (uint32_t x = 0u; x < scaledWidth; x++)
            {
                uint32_t x0 = x / multiplier;
                uint32_t y0 = y / multiplier;
                newCells[getIndex(x, y, scaledWidth)] = cells[getIndex(x0, y0, width)];
            }
        }
        return newCells;
    }

    void CellularAutomata::Scale(uint32_t multiplier)
    {
        cells = Scale(cells, width, height, multiplier);
        width *= multiplier;
        height *= multiplier;
    }

    void CellularAutomata::Clear()
    {
        width = initWidth;
        height = initHeight;
        cells.resize(width * height);
        for (uint32_t y = 0; y < height; y++)
            for (uint32_t x = 0; x < width; x++)
                cells[GetIndex(x, y)] = { 0u, x, y };
    }

    const std::vector<CellularAutomata::Cell>& CellularAutomata::GetCells() const
    {
        return cells;
    }

    CellularAutomata::GroupAnalysis CellularAutomata::AnalyzeGroup(
        uint32_t x, uint32_t y,
        std::vector<bool>& explored) const
    {
        if (explored[x + y * width])
            return {};

        GroupAnalysis analysis{};
        std::stack<Cell> frontier;
        const Cell& initialCell = GetCell(x, y);
        analysis.cellType = initialCell.type;
        frontier.push(initialCell);
        while (!frontier.empty())
        {
            const Cell& cell = frontier.top();
            uint32_t cx = cell.x;
            uint32_t cy = cell.y;
            analysis.minX = std::min(cx, analysis.minX);
            analysis.maxX = std::max(cx, analysis.maxX);
            analysis.minY = std::min(cy, analysis.minY);
            analysis.maxY = std::max(cy, analysis.maxY);
            frontier.pop();
            if (explored[cx + cy * width])
                continue;
            explored[cx + cy * width] = true;
            analysis.positions.push_back({ cx, cy });
            analysis.count++;
            std::vector<Cell> neighbourhood = VonNeumann(cx, cy, 1u, { initialCell.type });
            for (const Cell& neighbour : neighbourhood)
                frontier.push(neighbour);
        }
        return analysis;
    }

    std::vector<CellularAutomata::GroupAnalysis> CellularAutomata::AnalyzeGroups() const
    {
        std::vector<bool> explored;
        explored.resize(width * height);

        std::vector<GroupAnalysis> analyses;

        for (size_t y = 0; y < height; y++)
            for (size_t x = 0; x < width; x++)
            {
                GroupAnalysis groupAnalysis = AnalyzeGroup(x, y, explored);
                if (groupAnalysis.count > 0u)
                    analyses.push_back(groupAnalysis);
            }

        return analyses;
    }

    static bool firstExploredCell(
        int32_t x, int32_t y,
        int32_t dx, int32_t dy,
        uint32_t startX, uint32_t startY,
        int32_t startDx, int32_t startDy)
    {
        return x == startX && y == startY && dx == startDx && dy == startDy;
    }

    static bool withinGrid(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        return x >= 0 && x < width && y >= 0 && y < height;
    }

    bool CellularAutomata::WithinGrid(int32_t x, int32_t y) const
    {
        return withinGrid(x, y, width, height);
    }

    CellularAutomata::BorderAnalysis CellularAutomata::AnalyzeBorder(
        int32_t x, int32_t y,
        std::vector<bool>& explored,
        uint32_t cellType,
        int32_t dx, int32_t dy,
        uint32_t startX, uint32_t startY,
        int32_t startDx, int32_t startDy) const
    {
        BorderAnalysis analysis{};
        std::vector<Direction> directions;
        std::vector<glm::uvec2> positions;
        int32_t newDx = 0;
        int32_t newDy = 0;
        bool first = true;
        do
        {
            if (x < 0 || x > width - 1u)
                break;
            if (y < 0 || y > height - 1u)
                break;

            if (
                firstExploredCell(
                    x, y,
                    dx, dy,
                    startX, startY,
                    startDx, startDy) && !first)
                break;
            first = false;

            explored[x + y * width] = true;
            positions.emplace_back(x, y);
            analysis.minX = std::min<uint32_t>(x, analysis.minX);
            analysis.maxX = std::max<uint32_t>(x, analysis.maxX);
            analysis.minY = std::min<uint32_t>(y, analysis.minY);
            analysis.maxY = std::max<uint32_t>(y, analysis.maxY);

            //Try to turn left:
            int32_t neighbourX = x - dy;
            int32_t neighbourY = y + dx;
            if (WithinGrid(neighbourX, neighbourY) &&
                cells[GetIndex(neighbourX, neighbourY)].type == cellType)
            {
                analysis.jaggedness++;
                x = neighbourX;
                y = neighbourY;
                newDx = -dy;
                newDy = dx;
                dx = newDx;
                dy = newDy;
                directions.push_back({ dx, dy });
                continue;
            }

            //Try to go straight ahead:
            neighbourX = x + dx;
            neighbourY = y + dy;
            if (WithinGrid(neighbourX, neighbourY) &&
                cells[GetIndex(neighbourX, neighbourY)].type == cellType)
            {
                analysis.length++;
                x = neighbourX;
                y = neighbourY;
                directions.push_back({ dx, dy });
                continue;
            }

            //Try to turn right:
            neighbourX = x + dy;
            neighbourY = y - dx;
            if (WithinGrid(neighbourX, neighbourY) &&
                cells[GetIndex(neighbourX, neighbourY)].type == cellType)
            {
                analysis.jaggedness++;
                analysis.length++;
                newDx = dy;
                newDy = -dx;
                dx = newDx;
                dy = newDy;
                directions.push_back({ dx, dy });
                continue;
            }

            analysis.jaggedness += 2u;
            analysis.length += 2u;
            dx = -dx;
            dy = -dy;
            directions.push_back({ dx, dy });
        } while (true);
        analysis.directions = std::move(directions);
        analysis.positions = std::move(positions);
        return analysis;
    }

    std::vector<CellularAutomata::BorderAnalysis> CellularAutomata::AnalyzeBorders() const
    {
        std::vector<BorderAnalysis> analyses;

        std::vector<bool> explored;
        explored.resize(width * height);
        int32_t prevCellType = -1;
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                int32_t cellType =
                    static_cast<int32_t>(GetCell(x, y).type);
                if (cellType == prevCellType)
                    continue;
                prevCellType = cellType;
                if (explored[x + y * width])
                    continue;

                uint32_t startX = x;
                uint32_t startY = y;
                int32_t dx = 0;
                int32_t dy = 0;
                if (
                    y + 1 < height &&
                    GetCell(x, y + 1).type == cellType)
                    dy = 1;
                else if (x + 1 < width &&
                    GetCell(x + 1, y).type != cellType)
                    dy = -1;
                else
                    dx = 1;
                BorderAnalysis analysis = AnalyzeBorder(
                    x, y, explored, cellType, dx, dy, x, y, dx, dy);
                analysis.cellType = cellType;
                analyses.push_back(std::move(analysis));
            }
            prevCellType = -1;
        }
        return analyses;
    }

    bool CellularAutomata::AStarCompare::operator()(
        const AStarNode& first,
        const AStarNode& second) const
    {
        return first.priority > second.cost + second.priority;
    }

    void CellularAutomata::AStarExplore(
        const AStarNode& from,
        glm::vec2 next,
        std::vector<AStarNode>& explored,
        PriorityQueue& frontier,
        glm::uvec2 goal) const
    {
        if (
            WithinGrid(next.x, next.y) &&
            !explored[next.x + next.y * width].explored)
        {
            uint32_t cost = costFunction(*this, from, next);
            AStarNode node
            {
                .position = next,
                .previous = from.position,
                .explored = true,
                .cost = cost,
                .priority = cost + manhattan(from.position, next)
            };
            frontier.push(node);
            explored[next.x + next.y * width] = node;
        }
    }

    CellularAutomata::PathAnalysis CellularAutomata::BuildPath(
        glm::uvec2 goal,
        const std::vector<AStarNode>& explored) const
    {
        PathAnalysis analysis;
        const AStarNode* node = &explored[goal.x + goal.y * width];
        analysis.cost = node->cost;
        while (static_cast<glm::ivec2>(node->position) != static_cast<glm::ivec2>(node->previous))
        {
            analysis.path.push_back(node->position);
            auto direction =
                node->position -
                static_cast<glm::ivec2>(node->previous);
            analysis.directions.push_back(
                Direction
                {
                    .dx = direction.x,
                    .dy = direction.y
                });
            node = &explored[node->previous.x + node->previous.y * width];
        }
        return analysis;
    }

    CellularAutomata::PathAnalysis CellularAutomata::AnalyzePath(
        glm::uvec2 from,
        glm::uvec2 to) const
    {
        AStarNode initial
        {
            .position = from,
            .previous = from,
            .explored = true,
            .cost = 0u,
            .priority = manhattan(from, to)
        };
        PriorityQueue frontier;
        frontier.push(initial);
        std::vector<AStarNode> explored(cells.size());
        explored[from.x + from.y * width] = initial;
        while (!frontier.empty())
        {
            AStarNode next = frontier.top();
            if (next.position == static_cast<glm::ivec2>(to))
                return BuildPath(to, explored);
            frontier.pop();
            glm::vec2 left(next.position.x - 1, next.position.y);
            AStarExplore(next, left, explored, frontier, to);
            glm::vec2 right(next.position.x + 1, next.position.y);
            AStarExplore(next, right, explored, frontier, to);
            glm::vec2 top(next.position.x, next.position.y + 1);
            AStarExplore(next, top, explored, frontier, to);
            glm::vec2 bottom(next.position.x, next.position.y - 1);
            AStarExplore(next, bottom, explored, frontier, to);
        }
        return
        {
            .path = {},
            .cost = std::numeric_limits<uint32_t>::max()
        };
    }

    static size_t straightLines(
        const Direction& lineDirection,
        size_t directionIndexJump,
        const std::vector<Direction>& directions,
        size_t directionIndex,
        std::vector<uint32_t>& lines)
    {
        while (directionIndex < directions.size())
        {
            uint32_t lineLength = 0u;
            const Direction* direction = &directions[directionIndex];
            while (*direction == lineDirection)
            {
                directionIndex += directionIndexJump;
                lineLength++;
                if (directionIndex >= directions.size())
                    break;
            }
            directionIndex++;
            lines.push_back(lineLength);
        }
        return directionIndex;
    }

    std::vector<uint32_t> straightLines(
        const std::vector<Direction>& directions)
    {
        std::vector<uint32_t> lines;
        //Horizontal:
        straightLines({ -1, 0}, 1ull, directions, 0ull, lines);
        straightLines({ 1, 0 }, 1ull, directions, 0ull, lines);
        //Vertical:
        straightLines({ 0, -1 }, 1ull, directions, 0ull, lines);
        straightLines({ 0, 1 }, 1ull, directions, 0ull, lines);
        //Diagonals:
        straightLines({ -1, -1 }, 2ull, directions, 0ull, lines);
        straightLines({ 1, -1 }, 2ull, directions, 0ull, lines);
        straightLines({ -1, 1 }, 2ull, directions, 0ull, lines);
        straightLines({ 1, 1 }, 2ull, directions, 0ull, lines);
        return lines;
    }

    bool Direction::operator==(const Direction& other) const
    {
        return dx == other.dx && dy == other.dy;
    }

    [[nodiscard]]
    static uint32_t firstMatch(
        const Direction& lineDirection,
        const std::vector<Direction>& directions)
    {
        size_t directionIndex = 0ull;
        bool stopAtNextMatch = true;
        if (directions[0] == lineDirection)
        {
            stopAtNextMatch = false;
            directionIndex++;
        }
        while (directionIndex < directions.size())
        {
            if (directions[directionIndex] == lineDirection)
            {
                if (stopAtNextMatch)
                    break;
            }
            else
                stopAtNextMatch = true;
            directionIndex++;
        }
        return directionIndex;
    }

    static std::vector<uint32_t> straightLinesMerged(
        const Direction& lineDirection,
        size_t directionIndexJump,
        const std::vector<Direction>& directions)
    {
        size_t firstMatchIndex = firstMatch(lineDirection, directions);
        std::vector<uint32_t> lines;
        size_t directionIndex = straightLines(
            lineDirection,
            directionIndexJump,
            directions,
            firstMatchIndex,
            lines);
        //Merge first and last match if possible:
        if (directionIndex == firstMatchIndex && lines.size() > 1u)
        {
            uint32_t last = lines.back();
            lines.front() += last;
            lines.pop_back();
        }
        return lines;
    }

    std::vector<uint32_t> straightLinesMerged(const std::vector<Direction>& directions)
    {
        std::vector<uint32_t> lines;
        //Horizontal:
        straightLinesMerged({ -1, 0 }, 1ull, directions);
        straightLinesMerged({ 1, 0 }, 1ull, directions);
        //Vertical:
        straightLinesMerged({ 0, -1 }, 1ull, directions);
        straightLinesMerged({ 0, 1 }, 1ull, directions);
        //Diagonals:
        straightLinesMerged({ -1, -1 }, 2ull, directions);
        straightLinesMerged({ 1, -1 }, 2ull, directions);
        straightLinesMerged({ -1, 1 }, 2ull, directions);
        straightLinesMerged({ 1, 1 }, 2ull, directions);
        return lines;
    }

    std::vector<uint32_t> CellularAutomata::BorderDistancesGrid(
        const BorderAnalysis& borderAnalysis) const
    {
        struct PositionDistancePair
        {
            glm::uvec2 position;
            uint32_t distance;
        };

        std::queue<PositionDistancePair> frontier;
        for (const auto& position : borderAnalysis.positions)
            frontier.push({ position, 0u });
        uint32_t gridSizeX = borderAnalysis.maxX - borderAnalysis.minX + 1u;
        uint32_t gridSizeY = borderAnalysis.maxY - borderAnalysis.minY + 1u;
        uint32_t gridSize = gridSizeX * gridSizeY;
        std::vector<uint32_t> borderDistances(gridSize, 0u);
        std::unordered_set<glm::uvec2> explored;
        while (!frontier.empty())
        {
            PositionDistancePair pair = frontier.front();
            frontier.pop();
            if (explored.contains(pair.position))
                continue;
            uint32_t xOffset = pair.position.x - borderAnalysis.minX;
            uint32_t yOffset = pair.position.y - borderAnalysis.minY;
            if (!withinGrid(xOffset, yOffset, gridSizeX, gridSizeY))
                continue;
            uint32_t index = xOffset + yOffset * gridSizeX;
            borderDistances[index] = pair.distance;
            explored.insert(pair.position);
            std::vector<Cell> neighbourhood = VonNeumann(
                pair.position.x, pair.position.y, 1u, { borderAnalysis.cellType });
            for (const Cell& neighbour : neighbourhood)
                frontier.push({ { neighbour.x, neighbour.y }, pair.distance + 1u });
        }
        return borderDistances;
    }

    std::vector<uint32_t> CellularAutomata::BorderDistances(
        const BorderAnalysis& borderAnalysis) const
    {
        auto borderDistanceGrid = BorderDistancesGrid(borderAnalysis);
        std::vector<uint32_t> borderDistances;
        for (uint32_t distance : borderDistanceGrid)
            if (distance)
                borderDistances.push_back(distance);
        return borderDistances;
    }

    static void explore(
        glm::ivec2 to,
        uint32_t fromIndex,
        const std::vector<uint32_t>& borderDistances,
        std::stack<glm::ivec2>& frontier,
        uint32_t sizeX,
        uint32_t sizeY,
        bool& isPeak)
    {
        uint32_t index = static_cast<uint32_t>(to.x + to.y * sizeX);
        bool isWithinGrid = withinGrid(to.x, to.y, sizeX, sizeY);
        if (isWithinGrid)
        {
            auto toDistance = borderDistances[index];
            auto fromDistance = borderDistances[fromIndex];
            if (toDistance == fromDistance)
                frontier.push(to);
            else if (toDistance > fromDistance)
                isPeak = false;
        }
    }

    void CellularAutomata::BorderDistancePeaksAux(
        uint32_t x,
        uint32_t y,
        const BorderAnalysis& borderAnalysis,
        const std::vector<uint32_t>& borderDistances,
        std::vector<bool>& explored,
        std::vector<uint32_t>& distancePeaks) const
    {
        uint32_t sizeX = borderAnalysis.maxX - borderAnalysis.minX + 1u;
        uint32_t sizeY = borderAnalysis.maxY - borderAnalysis.minY + 1u;
        std::stack<glm::ivec2> frontier;
        frontier.push({x, y});
        while (!frontier.empty())
        {
            glm::ivec2 position = frontier.top();
            frontier.pop();
            uint32_t index =
                static_cast<uint32_t>(position.x + position.y * sizeX);
            if (explored[index])
                continue;
            explored[index] = true;
            bool isPeak = true;
            glm::ivec2 left(position.x - 1, position.y);
            explore(left, index, borderDistances, frontier, sizeX, sizeY, isPeak);
            glm::ivec2 right(position.x + 1, position.y);
            explore(right, index, borderDistances, frontier, sizeX, sizeY, isPeak);
            glm::ivec2 bottom(position.x, position.y - 1);
            explore(bottom, index, borderDistances, frontier, sizeX, sizeY, isPeak);
            glm::ivec2 top(position.x, position.y + 1);
            explore(top, index, borderDistances, frontier, sizeX, sizeY, isPeak);

            if (isPeak)
            {
                uint32_t peak = borderDistances[index];
                distancePeaks.push_back(peak);
            }
        }
    }

    std::vector<uint32_t> CellularAutomata::BorderDistancePeaks(
        const BorderAnalysis& borderAnalysis) const
    {
        auto borderDistances = BorderDistancesGrid(borderAnalysis);
        std::vector<bool> explored(borderDistances.size());
        std::vector<uint32_t> distancePeaks;
        int32_t sizeX = borderAnalysis.maxX - borderAnalysis.minX + 1;
        int32_t sizeY = borderAnalysis.maxY - borderAnalysis.minY + 1;
        for (int32_t y = 0; y < sizeY; y++)
            for (int32_t x = 0; x < sizeX; x++)
                if (!explored[x + y * sizeX])
                    BorderDistancePeaksAux(
                        x,
                        y,
                        borderAnalysis, 
                        borderDistances, 
                        explored, 
                        distancePeaks);
        return distancePeaks;
    }

    static CellularAutomata::BorderAnalysis combine(
        const std::vector<CellularAutomata::BorderAnalysis>& borderAnalyses,
        uint32_t cellType)
    {
        CellularAutomata::BorderAnalysis combined;
        combined.cellType = cellType;
        for (const auto& borderAnalysis : borderAnalyses)
        {
            if (borderAnalysis.cellType == cellType)
            {
                combined.minX = std::min(combined.minX, borderAnalysis.minX);
                combined.maxX = std::max(combined.maxX, borderAnalysis.maxX);
                combined.minY = std::min(combined.minY, borderAnalysis.minY);
                combined.maxY = std::max(combined.maxY, borderAnalysis.maxY);
                combined.jaggedness += borderAnalysis.jaggedness;
                combined.length += borderAnalysis.length;
                combined.positions.insert(
                    combined.positions.end(),
                    borderAnalysis.positions.begin(),
                    borderAnalysis.positions.end());
                combined.directions.insert(
                    combined.directions.end(),
                    borderAnalysis.directions.begin(),
                    borderAnalysis.directions.end());
            }
        }
        return combined;
    }

    std::vector<uint32_t> CellularAutomata::BorderDistances(
        const std::vector<BorderAnalysis>& borderAnalyses,
        uint32_t cellType) const
    {
        BorderAnalysis combined = combine(borderAnalyses, cellType);
        return BorderDistances(combined);
    }

    std::vector<uint32_t> CellularAutomata::BorderDistancePeaks(
        const std::vector<BorderAnalysis>& borderAnalyses,
        uint32_t cellType) const
    {
        BorderAnalysis combined = combine(borderAnalyses, cellType);
        return BorderDistancePeaks(combined);
    }

    uint32_t CellularAutomata::InAir(
        const PathAnalysis& pathAnalysis,
        uint32_t airCellType) const
    {
        uint32_t inAir = 0u;
        for (const auto& position : pathAnalysis.path)
            if (position.y == 0u || GetCell(position.x, position.y - 1u).type == airCellType)
                inAir++;
        return inAir;
    }

    uint32_t CellularAutomata::OnSurface(
        const PathAnalysis& pathAnalysis,
        uint32_t airCellType) const
    {
        uint32_t onSurface = 0u;
        for (const auto& position : pathAnalysis.path)
            if (position.y != 0u &&
                GetCell(position.x, position.y).type == airCellType &&
                GetCell(position.x, position.y - 1u).type != airCellType)
                onSurface++;
        return onSurface;
    }

    std::vector<CellularAutomata::Platform> CellularAutomata::Platforms(
        const GroupAnalysis& groupAnalysis,
        uint32_t platformCellType) const
    {
        std::vector<Platform> platforms;
        std::unordered_set<glm::uvec2> positionMap(
            groupAnalysis.positions.begin(),
            groupAnalysis.positions.end());
        for (size_t y = groupAnalysis.minY; y <= groupAnalysis.maxY; y++)
        {
            if (y == 0u)
                continue;
            int32_t platformStartIndex = -1;
            for (size_t x = groupAnalysis.minX; x <= groupAnalysis.maxX; x++)
            {
                if (!positionMap.contains({ x, y }))
                    continue;
                if (x >= GetWidth())
                    continue;
                const Cell& bottom = GetCell(x, y - 1u);
                const Cell& top = GetCell(x, y);
                bool isPlatform =
                    top.type != platformCellType &&
                    bottom.type == platformCellType;
                bool buildingPlatform = platformStartIndex != -1;
                if (isPlatform && !buildingPlatform)
                    platformStartIndex = static_cast<int32_t>(x);
                else if (!isPlatform && buildingPlatform)
                {
                    uint32_t platformHeight = y - groupAnalysis.minY;
                    glm::uvec2 left(platformStartIndex, y);
                    glm::uvec2 right(x, y);
                    platforms.emplace_back(platformHeight, left, right);
                    platformStartIndex = -1;
                }
            }
        }
        return platforms;
    }

    bool CellularAutomata::IsGap(
        glm::uvec2 position, 
        uint32_t minGapDepth, 
        uint32_t platformCellType) const
    {
        const Cell* cell = nullptr;
        uint32_t fallen = 0u;
        do
        {
            cell = &GetCell(position.x, position.y - fallen);
            if (fallen >= minGapDepth)
                return true;
            fallen++;
            if (
                static_cast<int32_t>(position.y) -
                static_cast<int32_t>(fallen) < 0)
                break;
        } while (cell->type != platformCellType);
        return false;
    }

    void CellularAutomata::InsertIfGap(
        glm::uvec2 position,
        uint32_t minGapDepth,
        uint32_t platformCellType,
        std::unordered_set<glm::uvec2>& gaps,
        uint32_t& gapCount) const
    {
        bool isGap = IsGap(
            position, minGapDepth, platformCellType);
        if (!gaps.contains(position) && isGap)
        {
            gaps.insert(position);
            gapCount++;
        }
    }

    uint32_t CellularAutomata::GapCount(
        const GroupAnalysis& groupAnalysis, 
        uint32_t minGapDepth,
        uint32_t platformCellType) const
    {
        uint32_t gapCount = 0u;
        auto platforms = Platforms(groupAnalysis, platformCellType);
        std::unordered_set<glm::uvec2> gaps;
        for (const auto& platform : platforms)
        {
            auto left = static_cast<glm::ivec2>(platform.left) - glm::ivec2(1, 0);
            if (left.x >= 0)
                InsertIfGap(
                    left,
                    minGapDepth,
                    platformCellType,
                    gaps,
                    gapCount);
                
            InsertIfGap(
                platform.right,
                minGapDepth,
                platformCellType,
                gaps,
                gapCount);
        }
        return gapCount;
    }
}
