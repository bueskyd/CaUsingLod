#include "Generator.h"
#include <geometric.hpp>
#include "Heuristic.h"

namespace pcg
{
    Generator::Generator(uint32_t width, uint32_t height)
        : initWidth(width), initHeight(height), ca(width, height) { }

    void Generator::SetCostFunction(CellularAutomata::CostFunction costFunction)
    {
        ca.SetCostFunction(costFunction);
    }

    const std::vector<CellularAutomata::Cell>& Generator::GetResult() const
    {
        return ca.GetCells();
    }

    uint32_t Generator::GetWidth() const
    {
        return ca.GetWidth();
    }

    uint32_t Generator::GetHeight() const
    {
        return ca.GetHeight();
    }

    uint32_t Generator::GetInitWidth() const
    {
        return initWidth;
    }

    uint32_t Generator::GetInitHeight() const
    {
        return initHeight;
    }

    std::vector<CellularAutomata::GroupAnalysis> Generator::AnalyzeGroups() const
    {
        return ca.AnalyzeGroups();
    }

    std::vector<CellularAutomata::BorderAnalysis> Generator::AnalyzeBorders() const
    {
        return ca.AnalyzeBorders();
    }

    std::vector<CellularAutomata::PathAnalysis> Generator::AnalyzePaths() const
    {
        std::vector<std::pair<glm::uvec2, glm::uvec2>> pathEndPoints
        {
            { { 0u, 0u }, { GetWidth() - 1u, 0u } }, //Top row
            { { 0u, GetHeight() / 2u}, { GetWidth() - 1u, GetHeight() / 2u } }, //Middle row
            { { 0u, GetHeight() - 1u}, { GetWidth() - 1u, GetHeight() - 1u } }, //Bottom row

            { { 0u, 0u }, {0u, GetHeight() - 1u}}, //Left column
            { { GetWidth() / 2u, 0u }, { GetWidth() / 2u, GetHeight() - 1u } }, //Middle column
            { { GetWidth() - 1u, 0u }, { GetWidth() - 1u, GetHeight() - 1u } } //Right column
        };

        std::vector<CellularAutomata::PathAnalysis> analyses;
        for (const auto& [from, to] : pathEndPoints)
            analyses.push_back(ca.AnalyzePath(from, to));
        return analyses;

    }

    std::vector<uint32_t> Generator::BorderDistances(
        const CellularAutomata::BorderAnalysis& borderAnalysis) const
    {
        return ca.BorderDistances(borderAnalysis);
    }

    std::vector<uint32_t> Generator::BorderDistancePeaks(
        const CellularAutomata::BorderAnalysis& borderAnalysis) const
    {
        return ca.BorderDistancePeaks(borderAnalysis);
    }

    std::vector<uint32_t> Generator::BorderDistances(
        const std::vector<CellularAutomata::BorderAnalysis>& borderAnalyses,
        uint32_t cellType) const
    {
        return ca.BorderDistances(borderAnalyses, cellType);
    }

    std::vector<uint32_t> Generator::BorderDistancePeaks(
        const std::vector<CellularAutomata::BorderAnalysis>& borderAnalyses,
        uint32_t cellType) const
    {
        return ca.BorderDistancePeaks(borderAnalyses, cellType);
    }

    uint32_t Generator::InAir(
        const CellularAutomata::PathAnalysis& pathAnalysis,
        uint32_t airCellType) const
    {
        return ca.InAir(pathAnalysis, airCellType);
    }

    uint32_t Generator::OnSurface(
        const CellularAutomata::PathAnalysis& pathAnalysis,
        uint32_t airCellType) const
    {
        return ca.OnSurface(pathAnalysis, airCellType);
    }

    std::vector<CellularAutomata::Platform> Generator::Platforms(
        const CellularAutomata::GroupAnalysis& groupAnalysis,
        uint32_t platformCellType) const
    {
        return ca.Platforms(groupAnalysis, platformCellType);
    }

    uint32_t Generator::GapCount(
        const CellularAutomata::GroupAnalysis& groupAnalysis,
        uint32_t gapMinDepth,
        uint32_t platformCellType) const
    {
        return ca.GapCount(groupAnalysis, gapMinDepth, platformCellType);
    }

    double spaceCoverage(
        const AnalysisData& data,
        CompType minX, CompType minY, CompType maxX, CompType maxY)
    {
        if (data.dataPoints.size() == 0ull)
            return {};
        std::unordered_set<glm::uvec2> coveredPoints;
        for (const auto& dataPoint : data.dataPoints)
        {
            CompType mx = dataPoint.x - minX;
            CompType my = dataPoint.y - minY;
            coveredPoints.insert({ mx, my });
        }
        uint32_t coveredSpace = coveredPoints.size();
        uint32_t xRange = static_cast<uint32_t>(maxX + 0.5) - static_cast<uint32_t>(minX + 0.5) + 1;
        uint32_t yRange = static_cast<uint32_t>(maxY + 0.5) - static_cast<uint32_t>(minY + 0.5) + 1;
        uint32_t spaceSize = xRange * yRange;
        return (static_cast<double>(coveredSpace) / spaceSize) * 100.0;
    }

    CompType directionAngle(Direction t0, Direction t1)
    {
        auto cross = t0.dx * t1.dy - t0.dy * t1.dx;
        auto angle = std::asin(cross);
        return angle;
    }

    CompType clustering(const AnalysisData& analysisData)
    {
        CompType distanceSum{};
        size_t count = 0ull;
        if (analysisData.dataPoints.empty())
            return {};
        for (size_t i = 0; i < analysisData.dataPoints.size(); i++)
        {
            const auto& dataPoint0 = analysisData.dataPoints[i];
            for (size_t j = i + 1; j < analysisData.dataPoints.size(); j++)
            {
                const auto& dataPoint1 = analysisData.dataPoints[j];
                auto difference = dataPoint1 - dataPoint0;
                auto distance = glm::length(difference);
                distanceSum += distance;
                count++;
            }
        }
        CompType clustering = distanceSum / static_cast<double>(count);
        return clustering;
    }

    static CompType distance(
        glm::dvec2 direction,
        glm::uvec2 startPoint,
        glm::uvec2 point)
    {
        if (direction.x == 0.0)
            return
            static_cast<int32_t>(startPoint.x) -
            static_cast<int32_t>(point.x);
        CompType a = direction.y / direction.x;
        CompType b =
            static_cast<CompType>(startPoint.y) -
            a * static_cast<CompType>(startPoint.x);
        CompType distance = std::abs(
            a * static_cast<CompType>(point.x) +
            b -
            static_cast<CompType>(point.y)) /
            std::sqrt(a * a + 1.0);
        return distance;
    }

    static uint32_t manhattenToClosestPoint(
        glm::dvec2 direction,
        glm::ivec2 startPoint,
        glm::ivec2 point)
    {
        CompType cross =
            (point.x - startPoint.x) * direction.y -
            (point.y - startPoint.y) * direction.x;
        if (cross == 0.0)
            return 0u;
        glm::dvec2 orthogonal(-direction.y, direction.x);
        orthogonal = glm::normalize(orthogonal);
        orthogonal *= cross / std::abs(cross);
        orthogonal *= distance(direction, startPoint, point);
        glm::dvec2 closestPoint = static_cast<glm::dvec2>(point) + orthogonal;
        int32_t cx = static_cast<int32_t>(closestPoint.x + 0.5);
        int32_t cy = static_cast<int32_t>(closestPoint.y + 0.5);
        return manhattan(glm::ivec2(cx, cy), static_cast<glm::ivec2>(point));
    }

    static bool straightLineFit(
        std::vector<glm::uvec2>::const_iterator begin,
        std::vector<glm::uvec2>::const_iterator end)
    {
        const auto& startPoint = *begin;
        const auto& endPoint = *(end - 1);
        glm::dvec2 direction =
            static_cast<glm::dvec2>(endPoint) -
            static_cast<glm::dvec2>(startPoint);
        direction /= glm::length(direction);
        uint32_t distanceToLineSum = 0u;
        for (auto it = begin; it != end; ++it)
        {
            uint32_t distanceToLine = manhattenToClosestPoint(direction, startPoint, *it);
            distanceToLineSum += distanceToLine;
        }
        uint32_t optimalLength = manhattan(startPoint, endPoint);
        uint32_t pathLength = std::distance(begin, end) - 1u;
        CompType lengthDifference = static_cast<CompType>(pathLength - optimalLength);
        CompType averageDistanceToLine =
            static_cast<CompType>(distanceToLineSum) /
            static_cast<CompType>(pathLength);
        CompType fit = lengthDifference + averageDistanceToLine;
        return fit;
    }

    static bool isStraightLine(
        std::vector<glm::uvec2>::const_iterator begin,
        std::vector<glm::uvec2>::const_iterator end)
    {
        CompType fit = straightLineFit(begin, end);
        return fit < 0.01;
    }

    static std::vector<uint32_t> straightLines(
        std::vector<glm::uvec2>::const_iterator begin,
        std::vector<glm::uvec2>::const_iterator end)
    {
        std::vector<uint32_t> lines;
        for (; begin != end; ++begin)
        {
            uint32_t length = manhattan(*begin, *(end - 1));
            if (isStraightLine(begin, end))
                lines.push_back(length);
        }
        return lines;
    }

    DataComponent Generator::StraightLines(
        uint32_t minLength, uint32_t maxLength)
    {
        return[minLength, maxLength](
            const auto& generator, const auto& analysis)
        {
            uint32_t lines = 0u;
            for (const auto& borderAnalysis : analysis.borderAnalyses)
            {
                auto sl = straightLines(
                    borderAnalysis.positions.begin(),
                    borderAnalysis.positions.end());
                for (uint32_t length : sl)
                    if (length >= minLength && length <= maxLength)
                        ++lines;
            }
            return lines;
        };
    }

    DataComponent Generator::DistinctGroupSizes(
        uint32_t minSize, uint32_t maxSize)
    {
        return[minSize, maxSize](
            const auto& generator, const auto& analysis)
        {
            std::unordered_set<uint32_t> sizes;
            for (const auto& groupAnalysis : analysis.groupAnalyses)
                if (groupAnalysis.count >= minSize && groupAnalysis.count <= maxSize)
                    sizes.insert(groupAnalysis.count);
            return sizes.size();
        };
    }

    DataComponent Generator::DistinctLineLengths(
        uint32_t minLength, uint32_t maxLength)
    {
        return[minLength, maxLength](
            const auto& generator, const auto& analysis)
        {
            std::unordered_set<uint32_t> lengths;
            for (const auto& borderAnalysis : analysis.borderAnalyses)
            {
                auto sl = straightLines(
                    borderAnalysis.positions.begin(),
                    borderAnalysis.positions.end());
                for (uint32_t length : sl)
                    lengths.insert(length);
            }
            uint32_t distinctLineLengths =
                static_cast<uint32_t>(lengths.size());
            return distinctLineLengths;
        };
    }

    DataComponent Generator::PercentageOfType(uint32_t cellType)
    {
        return[cellType](const auto& generator, const auto& analysis)
        {
            uint32_t totalMatches = 0u;
            uint32_t totalCellCount = 0u;
            for (const auto& groupAnalysis : analysis.groupAnalyses)
            {
                if (groupAnalysis.cellType == cellType)
                    totalMatches += groupAnalysis.count;
                totalCellCount += groupAnalysis.count;
            }
            float percentage =
                static_cast<float>(totalMatches) /
                static_cast<float>(totalCellCount) * 100.0f;
            return percentage;
        };
    }

    DataComponent Generator::PercentageReachable(
        uint32_t reachableCellType)
    {
        return[reachableCellType](const auto& generator, const auto& analysis)
        {
            uint32_t reachableCellCount = 0u;
            uint32_t caves = 0u;
            for (const auto& groupAnalysis : analysis.groupAnalyses)
                if (groupAnalysis.cellType == reachableCellType)
                {
                    reachableCellCount += groupAnalysis.count;
                    caves++;
                }
            if (caves == 0u)
                return 0.0;

            CompType percentageSum{};
            for (const auto& groupAnalysis : analysis.groupAnalyses)
                if (groupAnalysis.cellType == reachableCellType)
                {
                    CompType percentage =
                        static_cast<CompType>(groupAnalysis.count) /
                        static_cast<CompType>(reachableCellCount) * 100;
                    percentageSum += percentage;
                }
            CompType averagePercentage = percentageSum / static_cast<CompType>(caves);
            return averagePercentage;
        };
    }

    DataComponent Generator::GroupCount(
        uint32_t cellType, uint32_t minSize, uint32_t maxSize)
    {
        return[cellType, minSize, maxSize](
            const auto& generator, const auto& analysis)
        {
            uint32_t groupCount = 0u;
            for (const auto& groupAnalysis : analysis.groupAnalyses)
                if (groupAnalysis.cellType == cellType &&
                    groupAnalysis.count >= minSize &&
                    groupAnalysis.count <= maxSize)
                    groupCount++;
            return groupCount;
        };
    }

    DataComponent Generator::OptimalPathPercentageAverage()
    {
        return[](const auto& generator, const auto& analysis)
        {
            float percentageSum = 0.0f;
            uint32_t cells = generator.GetWidth() * generator.GetHeight();
            for (const auto& pathAnalsis : analysis.pathAnalyses)
            {
                uint32_t optimalPathLength = pathAnalsis.path.size();
                float percentage =
                    (static_cast<float>(optimalPathLength) /
                        static_cast<float>(cells)) *
                    100.0f;
                percentageSum += percentage;
            }
            float average = percentageSum / analysis.pathAnalyses.size();
            return average;
        };
    }

    DataComponent Generator::OptimalPathLineFit()
    {
        return[](const auto& generator, const auto& analysis)
        {
            struct PositionDistanceWeightTuple
            {
                glm::ivec2 position;
                CompType distance;
                uint32_t weight;
            };
            auto compare = [](const auto& first, const auto& second)
            {
                return first.distance < second.distance;
            };
            CompType pathFitSum{};
            for (const auto& pathAnalysis : analysis.pathAnalyses)
            {
                const auto& start = pathAnalysis.path.back();
                const auto& end = pathAnalysis.path.front();
                glm::dvec2 direction =
                    static_cast<glm::dvec2>(end) -
                        static_cast<glm::dvec2>(start);
                direction /= glm::length(direction);
                uint32_t distanceToLineSum = 0u;
                for (const auto& point : pathAnalysis.path)
                {
                    uint32_t distanceToLine = manhattenToClosestPoint(direction, start, point);
                    distanceToLineSum += distanceToLine;
                }
                uint32_t optimalLength = manhattan(start, end);
                uint32_t pathLength = pathAnalysis.path.size() - 1u;
                CompType lengthDifference = static_cast<CompType>(pathLength - optimalLength);
                CompType averageDistanceToLine =
                    static_cast<CompType>(distanceToLineSum) /
                    static_cast<CompType>(pathLength);
                CompType pathFit = lengthDifference + averageDistanceToLine;
                pathFitSum += pathFit;
            }
            CompType averagePathFit =
                pathFitSum /
                static_cast<CompType>(analysis.pathAnalyses.size());
            return averagePathFit;
        };
    }

    DataComponent Generator::AverageBorderDistancePeaks(uint32_t cellType)
    {
        return[cellType](const auto& generator, const auto& analysis)
        {
            uint32_t distancePeakSum = 0u;
            auto distancePeaks = generator.BorderDistancePeaks(analysis.borderAnalyses, cellType);
            for (uint32_t peak : distancePeaks)
                distancePeakSum += peak;
            float average =
                static_cast<float>(distancePeakSum) /
                static_cast<float>(distancePeaks.size());
            return average;
        };
    }

    DataComponent Generator::DistinctBorderDistancePeaks(uint32_t cellType)
    {
        return[cellType](const auto& generator, const auto& analysis)
        {
            std::unordered_set<uint32_t> distinctDistancePeaks;
            auto distancePeaks = generator.BorderDistancePeaks(analysis.borderAnalyses, cellType);
            for (uint32_t peak : distancePeaks)
                distinctDistancePeaks.insert(peak);
            return distinctDistancePeaks.size();
        };
    }

    DataComponent Generator::AverageHeightDifference(
        uint32_t platformCellType)
    {
        return[platformCellType](
            const auto& generator, const auto& analysis)
        {
            uint32_t heightDifferenceSum = 0u;
            for (const auto& groupAnalysis : analysis.groupAnalyses)
            {
                uint32_t minHeight = std::numeric_limits<uint32_t>::max();
                uint32_t maxHeight = 0u;
                auto platformHeights = generator.Platforms(
                    groupAnalysis, platformCellType);
                for (const auto& platform : platformHeights)
                {
                    minHeight = std::min(minHeight, platform.height);
                    maxHeight = std::max(maxHeight, platform.height);
                }
                uint32_t heightDifference = maxHeight - minHeight;
                heightDifferenceSum += heightDifference;
            }
            float averageHeightDifference =
                static_cast<float>(heightDifferenceSum) /
                static_cast<float>(analysis.groupAnalyses.size());
            return averageHeightDifference;
        };
    }

    [[nodiscard]]
    DataComponent Generator::GapCount(
        uint32_t minGapDepth,
        uint32_t platformCellType)
    {
        return[minGapDepth, platformCellType](
            const auto& generator, const auto& analysis)
        {
            uint32_t totalGapCount = 0u;
            for (const auto& groupAnalysis : analysis.groupAnalyses)
            {
                auto groupGaps = generator.GapCount(
                    groupAnalysis, minGapDepth, platformCellType);
                totalGapCount += groupGaps;
            }
            return totalGapCount;
        };
    }

    DataComponent Generator::AverageWalkablePercentage(uint32_t platformCellType)
    {
        return[platformCellType](
            const auto& generator,
            const auto& analysis)
        {
            CompType percentageSum{};
            uint32_t validGroups = 0u;
            for (const auto& groupAnalysis : analysis.groupAnalyses)
            {
                if (groupAnalysis.cellType == platformCellType)
                    continue;
                auto platforms = generator.Platforms(
                    groupAnalysis, platformCellType);
                uint32_t totalPlatformSurfaceArea = 0u;
                for (const auto& platform : platforms)
                    totalPlatformSurfaceArea += platform.right.x - platform.left.x;
                    
                CompType walkablePercentage =
                    static_cast<CompType>(totalPlatformSurfaceArea) /
                    static_cast<CompType>(groupAnalysis.count) * 100.0;
                percentageSum += walkablePercentage;
                validGroups++;
            }
            CompType average =
                validGroups == 0u ?
                0.0 :
                percentageSum / static_cast<CompType>(validGroups);
            return average;
        };
    }

    DataComponent Generator::InAirPercentage(uint32_t airCellType)
    {
        return[airCellType](const auto& generator, const auto& analysis)
        {
            CompType percentageSum{};
            for (const auto& pathAnalysis : analysis.pathAnalyses)
            {
                uint32_t inAir = generator.InAir(pathAnalysis, airCellType);
                CompType percentage =
                    static_cast<CompType>(inAir) /
                    static_cast<CompType>(pathAnalysis.path.size()) * 100.0;
                percentageSum += percentage;
            }
            CompType percentageAverage = percentageSum / analysis.pathAnalyses.size();
            return percentageAverage;
        };
    }

    DataComponent Generator::OnSurfacePercentage(uint32_t airCellType)
    {
        return[airCellType](const auto& generator, const auto& analysis)
        {
            CompType percentageSum{};
            for (const auto& pathAnalysis : analysis.pathAnalyses)
            {
                uint32_t onSurface = generator.OnSurface(pathAnalysis, airCellType);
                CompType percentage =
                    static_cast<CompType>(onSurface) /
                    static_cast<CompType>(pathAnalysis.path.size()) * 100.0;
                percentageSum += percentage;
            }
            CompType percentageAverage = percentageSum / analysis.pathAnalyses.size();
            return percentageAverage;
        };
    }

    DataPointRange Generator::LineLengthDistribution(
        uint32_t minLength, uint32_t maxLength)
    {
        return[minLength, maxLength](const auto& analysis)
        {
            std::unordered_map<uint32_t, uint32_t> distribution;
            for (const auto& borderAnalysis : analysis.borderAnalyses)
            {
                auto counts = straightLines(borderAnalysis.directions);
                for (uint32_t count : counts)
                    if (count >= minLength && count <= maxLength)
                        distribution[count]++;
            }
            return distribution;
        };
    }

    DataPointRange Generator::GroupSizeDistribution()
    {
        return [](const auto& analysis)
        {
            std::unordered_map<uint32_t, uint32_t> distribution;
            for (const auto& groupAnalysis : analysis.groupAnalyses)
                distribution[groupAnalysis.count]++;
            return distribution;
        };
    }
}
