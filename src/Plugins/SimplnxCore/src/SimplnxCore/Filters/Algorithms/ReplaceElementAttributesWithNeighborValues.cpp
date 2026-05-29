#include "ReplaceElementAttributesWithNeighborValues.hpp"

#include "simplnx/DataStructure/DataArray.hpp"
#include "simplnx/DataStructure/Geometry/ImageGeom.hpp"
#include "simplnx/Utilities/FilterUtilities.hpp"
#include "simplnx/Utilities/NeighborUtilities.hpp"

using namespace nx::core;

namespace
{
const int32 k_GreaterThanIndex = 1;

template <typename T>
class IComparisonFunctor
{
public:
  IComparisonFunctor() = default;
  virtual ~IComparisonFunctor() = default;

  IComparisonFunctor(const IComparisonFunctor&) = delete;            // Copy Constructor Not Implemented
  IComparisonFunctor(IComparisonFunctor&&) = delete;                 // Move Constructor Not Implemented
  IComparisonFunctor& operator=(const IComparisonFunctor&) = delete; // Copy Assignment Not Implemented
  IComparisonFunctor& operator=(IComparisonFunctor&&) = delete;      // Move Assignment Not Implemented

  [[nodiscard]] virtual bool compare(T left, T right) const = 0;
  [[nodiscard]] virtual bool compare1(T left, T right) const = 0;
  [[nodiscard]] virtual bool compare2(T left, T right) const = 0;
};

template <typename T>
class LessThanComparison : public IComparisonFunctor<T>
{
public:
  LessThanComparison() = default;
  ~LessThanComparison() override = default;

  LessThanComparison(const LessThanComparison&) = delete;            // Copy Constructor Not Implemented
  LessThanComparison(LessThanComparison&&) = delete;                 // Move Constructor Not Implemented
  LessThanComparison& operator=(const LessThanComparison&) = delete; // Copy Assignment Not Implemented
  LessThanComparison& operator=(LessThanComparison&&) = delete;      // Move Assignment Not Implemented

  [[nodiscard]] bool compare(T left, T right) const override
  {
    return left < right;
  }
  [[nodiscard]] bool compare1(T left, T right) const override
  {
    return left >= right;
  }
  [[nodiscard]] bool compare2(T left, T right) const override
  {
    return left > right;
  }
};

template <typename T>
class GreaterThanComparison : public IComparisonFunctor<T>
{
public:
  GreaterThanComparison() = default;
  ~GreaterThanComparison() override = default;
  GreaterThanComparison(const GreaterThanComparison&) = delete;            // Copy Constructor Not Implemented
  GreaterThanComparison(GreaterThanComparison&&) = delete;                 // Move Constructor Not Implemented
  GreaterThanComparison& operator=(const GreaterThanComparison&) = delete; // Copy Assignment Not Implemented
  GreaterThanComparison& operator=(GreaterThanComparison&&) = delete;      // Move Assignment Not Implemented

  [[nodiscard]] bool compare(T left, T right) const override
  {
    return left > right;
  }
  [[nodiscard]] bool compare1(T left, T right) const override
  {
    return left <= right;
  }
  [[nodiscard]] bool compare2(T left, T right) const override
  {
    return left < right;
  }
};

struct ExecuteTemplate
{
  template <typename T>
  void CompareValues(std::shared_ptr<IComparisonFunctor<T>>& comparator, const AbstractDataStore<T>& inputArray, int64 neighbor, float thresholdValue, float32& best,
                     std::vector<int64_t>& bestNeighbor, size_t i) const
  {
    if(comparator->compare1(inputArray[neighbor], thresholdValue) && comparator->compare2(inputArray[neighbor], best))
    {
      best = inputArray[neighbor];
      bestNeighbor[i] = neighbor;
    }
  }

  /**
   * @brief
   * @param
   * @return Returns the number of voxels to be changed.
   */
  template <typename T>
  usize findBestNeighbors(const std::array<int64, 3>& dims, const AbstractDataStore<T>& inputStore, std::shared_ptr<IComparisonFunctor<T>> comparator, float32 thresholdValue,
                          const std::array<int64, 6>& neighborVoxelIndexOffsets, std::vector<int64_t>& bestNeighbor, int64& prog, int64 progIncrement,
                          const IFilter::MessageHandler& messageHandler)
  {
    const usize totalPoints = inputStore.getNumberOfTuples();
    int64 neighbor = 0;
    usize count = 0;

    int64 column = 0;
    int64 row = 0;
    int64 plane = 0;

    // Iterate over the voxels and compare neighbors
    for(usize voxelIndex = 0; voxelIndex < totalPoints; voxelIndex++)
    {
      if(comparator->compare(inputStore[voxelIndex], thresholdValue))
      {
        column = voxelIndex % dims[0];
        row = (voxelIndex / dims[0]) % dims[1];
        plane = voxelIndex / (dims[0] * dims[1]);
        count++;
        float32 best = inputStore[voxelIndex];

        neighbor = static_cast<int64>(voxelIndex) + neighborVoxelIndexOffsets[0];
        if(plane != 0)
        {
          CompareValues<T>(comparator, inputStore, neighbor, thresholdValue, best, bestNeighbor, voxelIndex);
        }
        neighbor = static_cast<int64>(voxelIndex) + neighborVoxelIndexOffsets[1];
        if(row != 0)
        {
          CompareValues<T>(comparator, inputStore, neighbor, thresholdValue, best, bestNeighbor, voxelIndex);
        }
        neighbor = static_cast<int64>(voxelIndex) + neighborVoxelIndexOffsets[2];
        if(column != 0)
        {
          CompareValues<T>(comparator, inputStore, neighbor, thresholdValue, best, bestNeighbor, voxelIndex);
        }
        neighbor = static_cast<int64>(voxelIndex) + neighborVoxelIndexOffsets[3];
        if(column != (dims[0] - 1))
        {
          CompareValues<T>(comparator, inputStore, neighbor, thresholdValue, best, bestNeighbor, voxelIndex);
        }
        neighbor = static_cast<int64>(voxelIndex) + neighborVoxelIndexOffsets[4];
        if(row != (dims[1] - 1))
        {
          CompareValues<T>(comparator, inputStore, neighbor, thresholdValue, best, bestNeighbor, voxelIndex);
        }
        neighbor = static_cast<int64>(voxelIndex) + neighborVoxelIndexOffsets[5];
        if(plane != (dims[2] - 1))
        {
          CompareValues<T>(comparator, inputStore, neighbor, thresholdValue, best, bestNeighbor, voxelIndex);
        }
      }
      if(voxelIndex > prog)
      {
        int64 progressInt = static_cast<int64>(((float)voxelIndex / totalPoints) * 100.0f);
        const std::string progressMessage = fmt::format("Processing Loop({}) Progress: {}% Complete", count, progressInt);
        messageHandler(IFilter::ProgressMessage{IFilter::Message::Type::Progress, progressMessage, static_cast<int32_t>(progressInt)});
        prog += progIncrement;
      }
    }

    return count;
  }

  /**
  * @brief 
  */
  void replaceAttributesWithNeighbor(const usize totalPoints, const std::vector<int64>& bestNeighbor, int64& prog, int64 count, const AttributeMatrix& attrMatrix,
                                     const IFilter::MessageHandler& messageHandler)
  {
    // For each voxel, copy tuple values from the best neighbor
    const int64 progIncrement = static_cast<int64>(totalPoints / 50);
    //int64 prog = 1;
    int64 progressInt = 0;
    int64 neighbor;

    for(int64 voxelIndex = 0; voxelIndex < totalPoints; voxelIndex++)
    {
      // Track and report progress
      if(voxelIndex > prog)
      {
        progressInt = static_cast<int64>(((float)voxelIndex / totalPoints) * 100.0f);
        const std::string progressMessage = fmt::format("Transferring Loop({}) Progress: {}% Complete", count, progressInt);
        messageHandler(IFilter::ProgressMessage{IFilter::Message::Type::Progress, progressMessage, static_cast<int32_t>(progressInt)});
        prog += progIncrement;
      }

      // Copies the best neighbor tuple
      neighbor = bestNeighbor[voxelIndex];
      if(neighbor != -1)
      {
        for(const auto& [dataId, dataObject] : attrMatrix)
        {
          auto& dataArray = dynamic_cast<IDataArray&>(*dataObject);
          dataArray.copyTuple(neighbor, voxelIndex);
        }
      }
    }
  }

  template <typename T>
  void operator()(const ImageGeom& imageGeom, IDataArray* inputIDataArray, int32 comparisonAlgorithm, float thresholdValue, bool loopUntilDone, const std::atomic_bool& shouldCancel,
                  const IFilter::MessageHandler& messageHandler)
  {
    const auto& inputStore = inputIDataArray->template getIDataStoreRefAs<AbstractDataStore<T>>();

    const usize totalPoints = inputStore.getNumberOfTuples();

    Vec3 udims = imageGeom.getDimensions();
    std::array<int64, 3> dims = {
        static_cast<int64>(udims[0]),
        static_cast<int64>(udims[1]),
        static_cast<int64>(udims[2]),
    };

    std::array<int64, 6> neighborVoxelIndexOffsets = initializeFaceNeighborOffsets(dims);
    std::vector<int64_t> bestNeighbor(totalPoints, -1);

    usize count = 0;
    bool keepGoing = true;

    // Pick the appropriate comparison functor for the algorithm
    std::shared_ptr<IComparisonFunctor<T>> comparator = std::make_shared<LessThanComparison<T>>();
    if(comparisonAlgorithm == k_GreaterThanIndex)
    {
      comparator = std::make_shared<GreaterThanComparison<T>>();
    }

    // Get the Cell Data AttributeMatrix from the ImageGeom
    // This is used to edit all of the arrays based on the best neighbor value.
    const AttributeMatrix& attrMatrix = *imageGeom.getCellData();

    // Loop until the algorithm stops finding changes to make
    while(keepGoing)
    {
      keepGoing = false;
      if(shouldCancel)
      {
        break;
      }

      auto progIncrement = static_cast<int64>(totalPoints / 50);
      int64 prog = 1;

      count = findBestNeighbors<T>(dims, inputStore, comparator, thresholdValue, neighborVoxelIndexOffsets, bestNeighbor, prog, progIncrement, messageHandler);

      // Allow breaking out of the algorithm
      if(shouldCancel)
      {
        break;
      }

      replaceAttributesWithNeighbor(totalPoints, bestNeighbor, prog, count, attrMatrix, messageHandler);

      // Check if the algorithm should continue looping
      if(loopUntilDone && count > 0)
      {
        keepGoing = true;
      }
    }
  }
};

} // namespace

// -----------------------------------------------------------------------------
ReplaceElementAttributesWithNeighborValues::ReplaceElementAttributesWithNeighborValues(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                                                                       ReplaceElementAttributesWithNeighborValuesInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
ReplaceElementAttributesWithNeighborValues::~ReplaceElementAttributesWithNeighborValues() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& ReplaceElementAttributesWithNeighborValues::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> ReplaceElementAttributesWithNeighborValues::operator()()
{

  auto* srcIDataArray = m_DataStructure.getDataAs<IDataArray>(m_InputValues->InputArrayPath);
  const auto& imageGeom = m_DataStructure.getDataRefAs<ImageGeom>(m_InputValues->SelectedImageGeometryPath);

  ExecuteDataFunction(ExecuteTemplate{}, srcIDataArray->getDataType(), imageGeom, srcIDataArray, m_InputValues->SelectedComparison, m_InputValues->MinConfidence, m_InputValues->Loop, m_ShouldCancel,
                      m_MessageHandler);

  return {};
}
