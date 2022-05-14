/*
* A wrapper class for the CellularAutomata class.
* Contains member functions for analysing the grid created by the cellular automata.
* Exists due to historical reasons.
*/

#ifndef PCG_GENERATOR_H
#define PCG_GENERATOR_H

#include "pcg/CellularAutomata.h"
#include <vec3.hpp>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <gtc/constants.hpp>
#include <algorithm>
#include "Hash.h"

namespace pcg
{
    class Generator;

    using CompType = double;

    using DataComponent =
        std::function<CompType(
            const Generator&,
            const CellularAutomata::Analysis&)>;

    using DataPointRange =
        std::function<std::unordered_map<uint32_t, uint32_t>(
            const CellularAutomata::Analysis&)>;

    class Generator
    {
    private:
        struct NoOptions {};
    protected:
        CellularAutomata ca;
        uint32_t initWidth;
        uint32_t initHeight;
    public:
        Generator(uint32_t width, uint32_t height);
        void SetCostFunction(CellularAutomata::CostFunction costFunction);
        virtual void Generate() = 0;
        [[nodiscard]]
        const std::vector<CellularAutomata::Cell>& GetResult() const;
        [[nodiscard]]
        virtual std::vector<glm::vec3> GetCellColors() const = 0;
        [[nodiscard]]
        uint32_t GetWidth() const;
        [[nodiscard]]
        uint32_t GetHeight() const;
        [[nodiscard]]
        uint32_t GetInitWidth() const;
        [[nodiscard]]
        uint32_t GetInitHeight() const;
        [[nodiscard]]
        std::vector<CellularAutomata::GroupAnalysis> AnalyzeGroups() const;
        [[nodiscard]]
        std::vector<CellularAutomata::BorderAnalysis> AnalyzeBorders() const;
        [[nodiscard]]
        std::vector<CellularAutomata::PathAnalysis> AnalyzePaths() const;
        [[nodiscard]]
        std::vector<uint32_t> BorderDistances(
            const CellularAutomata::BorderAnalysis& borderAnalysis) const;
        [[nodiscard]]
        std::vector<uint32_t> BorderDistancePeaks(
            const CellularAutomata::BorderAnalysis& borderAnalysis) const;
        [[nodiscard]]
        std::vector<uint32_t> BorderDistances(
            const std::vector<CellularAutomata::BorderAnalysis>& borderAnalyses,
            uint32_t cellType) const;
        [[nodiscard]]
        std::vector<uint32_t> BorderDistancePeaks(
            const std::vector<CellularAutomata::BorderAnalysis>& borderAnalyses,
            uint32_t cellType) const;
        [[nodiscard]]
        uint32_t InAir(
            const CellularAutomata::PathAnalysis& pathAnalysis,
            uint32_t airCellType) const;
        [[nodiscard]]
        uint32_t OnSurface(
            const CellularAutomata::PathAnalysis& pathAnalysis,
            uint32_t airCellType) const;
        [[nodiscard]]
        std::vector<CellularAutomata::Platform> Platforms(
            const CellularAutomata::GroupAnalysis& groupAnalysis,
            uint32_t platformCellType) const;
        [[nodiscard]]
        uint32_t GapCount(
            const CellularAutomata::GroupAnalysis& groupAnalysis,
            uint32_t minGapDepth,
            uint32_t platformCellType) const;
        
        static DataComponent StraightLines(
            uint32_t minLength = 0u,
            uint32_t maxLength = std::numeric_limits<uint32_t>::max());
        
        [[nodiscard]]
        static DataComponent DistinctGroupSizes(
            uint32_t minSize = 0u,
            uint32_t maxSize = std::numeric_limits<uint32_t>::max());
        
        [[nodiscard]]
        static DataComponent DistinctLineLengths(
            uint32_t minLength = 0u,
            uint32_t maxLength = std::numeric_limits<uint32_t>::max());
        
        [[nodiscard]]
        static DataComponent PercentageOfType(uint32_t cellType);
        
        [[nodiscard]]
        static DataComponent PercentageReachable(
            uint32_t reachableCellType);
        
        [[nodiscard]]
        static DataComponent GroupCount(
            uint32_t cellType,
            uint32_t minSize = 0u,
            uint32_t maxSize = std::numeric_limits<uint32_t>::max());
        
        [[nodiscard]]
        static DataComponent OptimalPathPercentageAverage();
        
        [[nodiscard]]
        static DataComponent OptimalPathLineFit();
        
        [[nodiscard]]
        static DataComponent AverageBorderDistancePeaks(
            uint32_t cellType);
        
        [[nodiscard]]
        static DataComponent DistinctBorderDistancePeaks(
            uint32_t cellType);
        
        [[nodiscard]]
        static DataComponent AverageHeightDifference(
            uint32_t platformCellType);
        
        [[nodiscard]]
        static DataComponent GapCount(
            uint32_t minGapDepth,
            uint32_t platformCellType);

        [[nodiscard]]
        static DataComponent AverageWalkablePercentage(
            uint32_t platformCellType);

        [[nodiscard]]
        static DataComponent InAirPercentage(uint32_t airCellType);
        [[nodiscard]]
        static DataComponent OnSurfacePercentage(uint32_t airCellType);

        static DataPointRange LineLengthDistribution(
            uint32_t minLength = 0u,
            uint32_t maxLength = std::numeric_limits<uint32_t>::max());
        static DataPointRange GroupSizeDistribution();
    };

    struct AnalysisData
    {
        std::vector<glm::dvec2> dataPoints;
        CompType minX = std::numeric_limits<CompType>::infinity();
        CompType minY = std::numeric_limits<CompType>::infinity();
        CompType maxX = 0.0;
        CompType maxY = 0.0;
    };

    double spaceCoverage(
        const AnalysisData& data,
        CompType minX, CompType minY, CompType maxX, CompType maxY);

    CompType directionAngle(Direction t0, Direction t1);

    CompType clustering(const AnalysisData& analysisData);

    template<typename GeneratorType>
    inline AnalysisData dataPoints(
        GeneratorType& generator,
        uint32_t optionsCount,
        DataComponent calculateX,
        DataComponent calculateY,
        uint32_t iterations,
        std::function<void(GeneratorType&, uint32_t)> optionSetter,
        bool printProgress = false)
    {
        AnalysisData data;
        for (size_t o = 0ull; o < optionsCount; o++)
        {
            optionSetter(generator, o);

            for (size_t i = 0ull; i < iterations; i++)
            {
                generator.Generate();
                auto groupAnalyses = generator.AnalyzeGroups();
                auto bordersAnalyses = generator.AnalyzeBorders();
                auto pathAnalyses = generator.AnalyzePaths();
                CellularAutomata::Analysis analysis
                {
                    groupAnalyses,
                    bordersAnalyses,
                    pathAnalyses
                };
                CompType dataPointX = calculateX(generator, analysis);
                CompType dataPointY = calculateY(generator, analysis);
                data.minX = std::min(data.minX, dataPointX);
                data.minY = std::min(data.minY, dataPointY);
                data.maxX = std::max(data.maxX, dataPointX);
                data.maxY = std::max(data.maxY, dataPointY);
                data.dataPoints.push_back({ dataPointX, dataPointY });

                if (printProgress)
                {
                    uint32_t finishedWork = i + o * iterations + 1u;
                    uint32_t totalWork = (iterations * optionsCount);
                    std::cout <<
                        "Progress: " <<
                        finishedWork <<
                        " / " <<
                        totalWork <<
                        " iterations completed (" <<
                        static_cast<float>(finishedWork) / totalWork * 100u <<
                        "%)\n";
                }
            }
        }
        return data;
    }
}

#endif
