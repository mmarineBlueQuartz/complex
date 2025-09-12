#include "CropGeometryUtilities.hpp"

#include "simplnx/DataStructure/AbstractDataStore.hpp"
#include "simplnx/DataStructure/DataArray.hpp"
#include "simplnx/DataStructure/NeighborList.hpp"
#include "simplnx/DataStructure/StringArray.hpp"

namespace nx::core
{
namespace CropGeometry
{
namespace Generic
{
std::array<uint64, 6> getBounds(const ImageGeom& geom, const std::array<bool, 3>& isBound, const IntVec3& minBounds, const IntVec3& maxBounds)
{
  auto dimensions = geom.getDimensions();
  std::array<uint64, 6> output = {0, dimensions[0], 0, dimensions[1], 0, dimensions[2]};

  for(usize i = 0; i < 3; i++)
  {
    if(isBound[i])
    {
      output[2 * i + 0] = minBounds[i];
      output[2 * i + 1] = maxBounds[i];
    }
  }

  return output;
}

Result<> CropVoxelArray(const ImageGeom& geom, const IArray& inputArray, IArray& outputArray, const std::array<bool, 3>& isBound, const IntVec3& minBounds, const IntVec3& maxBounds)
{
  auto inputArrayType = inputArray.getArrayType();
  auto outputArrayType = outputArray.getArrayType();
  
  if(IArray::ArrayType::DataArray == inputArrayType && IArray::ArrayType::DataArray == outputArrayType)
  {
    return CropVoxelDataArray(geom, static_cast<const IDataArray&>(inputArray), static_cast<IDataArray&>(outputArray), isBound, minBounds, maxBounds);
  }
  else if(IArray::ArrayType::NeighborListArray == inputArrayType && IArray::ArrayType::NeighborListArray == outputArrayType)
  {
    return CropVoxelNeighborList(geom, static_cast<const INeighborList&>(inputArray), static_cast<INeighborList&>(outputArray), isBound, minBounds, maxBounds);
  }
  else if(IArray::ArrayType::StringArray == inputArrayType && IArray::ArrayType::StringArray == outputArrayType)
  {
    return CropVoxelStringArray(geom, static_cast<const StringArray&>(inputArray), static_cast<StringArray&>(outputArray), isBound, minBounds, maxBounds);
  }

  return MakeErrorResult(-5540, "Arrays are not of compatible types");
}

Result<> CropPhysicalArray(const ImageGeom& imageGeom, const IArray& inputArray, IArray& outputArray, const std::array<bool, 3>& isBound, const FloatVec3& minBoundPhysical,
                           const FloatVec3& maxBoundPhysical)
{
  IntVec3 minBoundVoxels = ConvertPhysicalToVoxel(imageGeom, minBoundPhysical);
  IntVec3 maxBoundVoxels = ConvertPhysicalToVoxel(imageGeom, maxBoundPhysical);
  return CropVoxelArray(imageGeom, inputArray, outputArray, isBound, minBoundVoxels, maxBoundVoxels);
}
} // namespace Generic

namespace Algorithms
{
/**
 * @brief
 * @tparam T
 */
template <typename T>
class CropImageGeomDataArray
{
public:
  CropImageGeomDataArray(const IDataArray& oldCellArray, IDataArray& newCellArray, const ImageGeom& srcImageGeom, std::array<uint64, 6> bounds, const std::atomic_bool& shouldCancel)
  : m_OldCellStore(oldCellArray.template getIDataStoreRefAs<AbstractDataStore<T>>())
  , m_NewCellStore(newCellArray.template getIDataStoreRefAs<AbstractDataStore<T>>())
  , m_SrcImageGeom(srcImageGeom)
  , m_Bounds(bounds)
  , m_ShouldCancel(shouldCancel)
  {
  }

  ~CropImageGeomDataArray() = default;

  CropImageGeomDataArray(const CropImageGeomDataArray&) = default;
  CropImageGeomDataArray(CropImageGeomDataArray&&) noexcept = default;
  CropImageGeomDataArray& operator=(const CropImageGeomDataArray&) = delete;
  CropImageGeomDataArray& operator=(CropImageGeomDataArray&&) noexcept = delete;

  void operator()() const
  {
    convert();
  }

protected:
  void convert() const
  {
    size_t numComps = m_OldCellStore.getNumberOfComponents();

    m_NewCellStore.fill(static_cast<T>(-1));

    auto srcDims = m_SrcImageGeom.getDimensions();

    uint64 destTupleIndex = 0;
    for(uint64 zIndex = m_Bounds[4]; zIndex < m_Bounds[5]; zIndex++)
    {
      if(m_ShouldCancel)
      {
        return;
      }
      for(uint64 yIndex = m_Bounds[2]; yIndex < m_Bounds[3]; yIndex++)
      {
        for(uint64 xIndex = m_Bounds[0]; xIndex < m_Bounds[1]; xIndex++)
        {
          uint64 srcIndex = (srcDims[0] * srcDims[1] * zIndex) + (srcDims[0] * yIndex) + xIndex;
          for(size_t compIndex = 0; compIndex < numComps; compIndex++)
          {
            m_NewCellStore.setValue(destTupleIndex * numComps + compIndex, m_OldCellStore.getValue(srcIndex * numComps + compIndex));
          }
          destTupleIndex++;
        }
      }
    }
  }

private:
  const AbstractDataStore<T>& m_OldCellStore;
  AbstractDataStore<T>& m_NewCellStore;
  const ImageGeom& m_SrcImageGeom;
  std::array<uint64, 6> m_Bounds;
  const std::atomic_bool& m_ShouldCancel;
};

/**
 * @brief
 * @tparam T
 */
template <typename T>
class CropImageGeomNeighborList
{
public:
  CropImageGeomNeighborList(const NeighborList<T>& oldCellArray, NeighborList<T>& newCellArray, const ImageGeom& srcImageGeom, std::array<uint64, 6> bounds, const std::atomic_bool& shouldCancel)
  : m_OldCellStore(oldCellArray.getStoreRef())
  , m_NewCellStore(newCellArray.getStoreRef())
  , m_SrcImageGeom(srcImageGeom)
  , m_Bounds(bounds)
  , m_ShouldCancel(shouldCancel)
  {
  }

  ~CropImageGeomNeighborList() = default;

  CropImageGeomNeighborList(const CropImageGeomNeighborList&) = default;
  CropImageGeomNeighborList(CropImageGeomNeighborList&&) noexcept = default;
  CropImageGeomNeighborList& operator=(const CropImageGeomNeighborList&) = delete;
  CropImageGeomNeighborList& operator=(CropImageGeomNeighborList&&) noexcept = delete;

  void operator()() const
  {
    convert();
  }

protected:
  void convert() const
  {
    size_t numComps = 1;
    auto srcDims = m_SrcImageGeom.getDimensions();

    uint64 destTupleIndex = 0;
    for(uint64 zIndex = m_Bounds[4]; zIndex < m_Bounds[5]; zIndex++)
    {
      if(m_ShouldCancel)
      {
        return;
      }
      for(uint64 yIndex = m_Bounds[2]; yIndex < m_Bounds[3]; yIndex++)
      {
        for(uint64 xIndex = m_Bounds[0]; xIndex < m_Bounds[1]; xIndex++)
        {
          uint64 srcIndex = (srcDims[0] * srcDims[1] * zIndex) + (srcDims[0] * yIndex) + xIndex;
          for(size_t compIndex = 0; compIndex < numComps; compIndex++)
          {
            m_NewCellStore.setList(destTupleIndex * numComps + compIndex, m_OldCellStore.at(srcIndex * numComps + compIndex));
          }
          destTupleIndex++;
        }
      }
    }
  }

private:
  const AbstractListStore<T>& m_OldCellStore;
  AbstractListStore<T>& m_NewCellStore;
  const ImageGeom& m_SrcImageGeom;
  std::array<uint64, 6> m_Bounds;
  const std::atomic_bool& m_ShouldCancel;
};

class CropImageGeomStringArray
{
public:
  CropImageGeomStringArray(const StringArray& oldCellArray, StringArray& newCellArray, const ImageGeom& srcImageGeom, std::array<uint64, 6> bounds, const std::atomic_bool& shouldCancel)
  : m_OldCellStore(oldCellArray.getStoreRef())
  , m_NewCellStore(newCellArray.getStoreRef())
  , m_SrcImageGeom(srcImageGeom)
  , m_Bounds(bounds)
  , m_ShouldCancel(shouldCancel)
  {
  }

  ~CropImageGeomStringArray() = default;

  CropImageGeomStringArray(const CropImageGeomStringArray&) = default;
  CropImageGeomStringArray(CropImageGeomStringArray&&) noexcept = default;
  CropImageGeomStringArray& operator=(const CropImageGeomStringArray&) = delete;
  CropImageGeomStringArray& operator=(CropImageGeomStringArray&&) noexcept = delete;

  void operator()() const
  {
    convert();
  }

protected:
  void convert() const
  {
    m_NewCellStore.fill("");

    auto srcDims = m_SrcImageGeom.getDimensions();

    uint64 destTupleIndex = 0;
    for(uint64 zIndex = m_Bounds[4]; zIndex < m_Bounds[5]; zIndex++)
    {
      if(m_ShouldCancel)
      {
        return;
      }
      for(uint64 yIndex = m_Bounds[2]; yIndex < m_Bounds[3]; yIndex++)
      {
        for(uint64 xIndex = m_Bounds[0]; xIndex < m_Bounds[1]; xIndex++)
        {
          uint64 srcIndex = (srcDims[0] * srcDims[1] * zIndex) + (srcDims[0] * yIndex) + xIndex;
          m_NewCellStore.setValue(destTupleIndex, m_OldCellStore.getValue(srcIndex));
          destTupleIndex++;
        }
      }
    }
  }

private:
  const AbstractStringStore& m_OldCellStore;
  AbstractStringStore& m_NewCellStore;
  const ImageGeom& m_SrcImageGeom;
  std::array<uint64, 6> m_Bounds;
  const std::atomic_bool& m_ShouldCancel;
};
} // namespace Algorithms

template <typename T>
Result<> CopyDataStore(const IDataArray* inputArray, IDataArray* outputArray)
{
  const auto* inputTypedArray = static_cast<const DataArray<T>*>(inputArray);
  auto* outputTypedArray = static_cast<DataArray<T>*>(outputArray);

  const AbstractDataStore<T>& inputStore = inputTypedArray->getDataStoreRef();
  AbstractDataStore<T>& outputStore = outputTypedArray->getDataStoreRef();

  if(outputStore.copy(inputStore))
  {
    return {};
  }
  return MakeErrorResult(-5554, fmt::format("Failed to copy data from DataArray '{}' to '{}'", inputArray->getName(), outputArray->getName()));
}

Result<> CopyDataArray(DataStructure& dataStructure, const DataPath& inputArrayPath, const DataPath& outputArrayPath)
{
  const IDataArray* inputArray = dataStructure.getDataAs<IDataArray>(inputArrayPath);
  IDataArray* outputArray = dataStructure.getDataAs<IDataArray>(outputArrayPath);

  if(inputArray == nullptr)
  {
    return MakeErrorResult(-5550, fmt::format("Failed to copy data. Input DataArray at path '{}' not found.", inputArrayPath.toString()));
  }
  if(outputArray == nullptr)
  {
    return MakeErrorResult(-5551, fmt::format("Failed to copy data. Output DataArray at path '{}' not found.", outputArrayPath.toString()));
  }

  auto dataType = inputArray->getDataType();

  switch(dataType)
  {
  case DataType::boolean:
    return CopyDataStore<bool>(inputArray, outputArray);
  case DataType::int8:
    return CopyDataStore<int8>(inputArray, outputArray);
  case DataType::int16:
    return CopyDataStore<int16>(inputArray, outputArray);
  case DataType::int32:
    return CopyDataStore<int32>(inputArray, outputArray);
  case DataType::int64:
    return CopyDataStore<int64>(inputArray, outputArray);
  case DataType::uint8:
    return CopyDataStore<uint8>(inputArray, outputArray);
  case DataType::uint16:
    return CopyDataStore<uint16>(inputArray, outputArray);
  case DataType::uint32:
    return CopyDataStore<uint32>(inputArray, outputArray);
  case DataType::uint64:
    return CopyDataStore<uint64>(inputArray, outputArray);
  case DataType::float32:
    return CopyDataStore<float32>(inputArray, outputArray);
  case DataType::float64:
    return CopyDataStore<float64>(inputArray, outputArray);
  default:
    return MakeErrorResult(-5552, fmt::format("Failed to copy data. Input DataArray at path '{}' has invalid data type.", inputArrayPath.toString()));
  }
}

template <typename T>
Result<> CopyListStore(const INeighborList* inputArray, INeighborList* outputArray)
{
  const auto* inputTypedArray = static_cast<const NeighborList<T>*>(inputArray);
  auto* outputTypedArray = static_cast<NeighborList<T>*>(outputArray);

  const AbstractListStore<T>& inputStore = inputTypedArray->getStoreRef();
  AbstractListStore<T>& outputStore = outputTypedArray->getStoreRef();

  outputStore.copy(inputStore);
  return {};
}

Result<> CopyListArray(DataStructure& dataStructure, const DataPath& inputArrayPath, const DataPath& outputArrayPath)
{
  const INeighborList* inputArray = dataStructure.getDataAs<INeighborList>(inputArrayPath);
  INeighborList* outputArray = dataStructure.getDataAs<INeighborList>(outputArrayPath);

  if(inputArray == nullptr)
  {
    return MakeErrorResult(-5560, fmt::format("Failed to copy data. Input NeighborList at path '{}' not found.", inputArrayPath.toString()));
  }
  if(outputArray == nullptr)
  {
    return MakeErrorResult(-5561, fmt::format("Failed to copy data. Output NeighborList at path '{}' not found.", outputArrayPath.toString()));
  }

  auto dataType = inputArray->getDataType();

  switch(dataType)
  {
  case DataType::int8:
    return CopyListStore<int8>(inputArray, outputArray);
  case DataType::int16:
    return CopyListStore<int16>(inputArray, outputArray);
  case DataType::int32:
    return CopyListStore<int32>(inputArray, outputArray);
  case DataType::int64:
    return CopyListStore<int64>(inputArray, outputArray);
  case DataType::uint8:
    return CopyListStore<uint8>(inputArray, outputArray);
  case DataType::uint16:
    return CopyListStore<uint16>(inputArray, outputArray);
  case DataType::uint32:
    return CopyListStore<uint32>(inputArray, outputArray);
  case DataType::uint64:
    return CopyListStore<uint64>(inputArray, outputArray);
  case DataType::float32:
    return CopyListStore<float32>(inputArray, outputArray);
  case DataType::float64:
    return CopyListStore<float64>(inputArray, outputArray);
  default:
    return MakeErrorResult(-5562, fmt::format("Failed to copy data. Input NeighborList at path '{}' has invalid data type.", inputArrayPath.toString()));
  }
}

Result<> CopyStringArray(DataStructure& dataStructure, const DataPath& inputArrayPath, const DataPath& outputArrayPath)
{
  const StringArray* inputArray = dataStructure.getDataAs<StringArray>(inputArrayPath);
  StringArray* outputArray = dataStructure.getDataAs<StringArray>(outputArrayPath);

  if(inputArray == nullptr)
  {
    return MakeErrorResult(-5560, fmt::format("Failed to copy data. Input StringArray at path '{}' not found.", inputArrayPath.toString()));
  }
  if(outputArray == nullptr)
  {
    return MakeErrorResult(-5561, fmt::format("Failed to copy data. Output StringArray at path '{}' not found.", outputArrayPath.toString()));
  }

  usize size = outputArray->getSize();
  for(usize i = 0; i < size; i++)
  {
    std::string value = inputArray->at(i);
    outputArray->setValue(i, value);
  }

  return {};
}

Result<> CopyArray(DataStructure& dataStructure, const DataPath& inputArrayPath, const DataPath& outputArrayPath)
{
  auto& inputArrayRef = dataStructure.getDataRefAs<IArray>(inputArrayPath);
  auto arrayType = inputArrayRef.getArrayType();

  switch(arrayType)
  {
  case IArray::ArrayType::DataArray:
    return CopyDataArray(dataStructure, inputArrayPath, outputArrayPath);
  case IArray::ArrayType::NeighborListArray:
    return CopyListArray(dataStructure, inputArrayPath, outputArrayPath);
  case IArray::ArrayType::StringArray:
    return CopyStringArray(dataStructure, inputArrayPath, outputArrayPath);
  }

  return MakeErrorResult(-5540, fmt::format("Failed to copy data. DataType at '{}' not supported.", inputArrayPath.toString()));
}

Result<> CropArray(DataStructure& dataStructure, const DataPath& geomPath, const DataPath& inputArrayPath, const DataPath& outputArrayPath, const CropGeometryParameter::ValueType& cropValues)
{
  using TypeEnum = CropGeometryParameter::ValueType::TypeEnum;

  ImageGeom& imageGeom = dataStructure.getDataRefAs<ImageGeom>(geomPath);
  const IArray& inputArray = dataStructure.getDataRefAs<IArray>(inputArrayPath);
  IArray& outputArray = dataStructure.getDataRefAs<IArray>(outputArrayPath);

  std::array<bool, 3> isBound{cropValues.cropX, cropValues.cropY, cropValues.cropZ};
  IntVec3 minBoundVoxels{cropValues.xBoundVoxels[0], cropValues.yBoundVoxels[0], cropValues.zBoundVoxels[0]};
  IntVec3 maxBoundVoxels{cropValues.xBoundVoxels[1], cropValues.yBoundVoxels[1], cropValues.zBoundVoxels[1]};

  FloatVec3 minBoundPhysical{cropValues.xBoundPhysical[0], cropValues.yBoundPhysical[0], cropValues.zBoundPhysical[0]};
  FloatVec3 maxBoundPhysical{cropValues.xBoundPhysical[1], cropValues.yBoundPhysical[1], cropValues.zBoundPhysical[1]};

  switch(cropValues.type)
  {
  case TypeEnum::EntireVolume:
    return CopyArray(dataStructure, inputArrayPath, outputArrayPath);
  case TypeEnum::PhysicalSubvolume:
    minBoundVoxels = ConvertPhysicalToVoxel(imageGeom, minBoundPhysical);
    maxBoundVoxels = ConvertPhysicalToVoxel(imageGeom, maxBoundPhysical);
    [[fallthrough]];
  case TypeEnum::VoxelSubvolume:
    return Generic::CropVoxelArray(imageGeom, inputArray, outputArray, isBound, minBoundVoxels, maxBoundVoxels);
  }

  return {};
}

IntVec3 ConvertPhysicalToVoxel(const ImageGeom& geom, const FloatVec3& physical)
{
  const FloatVec3 spacing = geom.getSpacing();
  IntVec3 voxels;
  for(usize i = 0; i < 3; i++)
  {
    voxels[i] = physical[i] * spacing[i];
  }
  return voxels;
}

usize ConvertVoxelToTupleIndex(const SizeVec3& dimensions, const IntVec3& voxel)
{
  return (dimensions[0] * dimensions[1] * voxel[2]) + (dimensions[0] * voxel[1]) + voxel[0];
}

usize ConvertPhysicalToTupleIndex(const ImageGeom& geom, const FloatVec3& physical)
{
  const IntVec3 voxelIndex = ConvertPhysicalToVoxel(geom, physical);
  return ConvertVoxelToTupleIndex(geom.getDimensions(), voxelIndex);
}

Result<> CropVoxelDataArray(const ImageGeom& geom, const IDataArray& inputArray, IDataArray& outputArray, const std::array<bool, 3>& isBound, const IntVec3& minBounds, const IntVec3& maxBounds)
{
  std::atomic_bool shouldCancel = false;
  auto bounds = Generic::getBounds(geom, isBound, minBounds, maxBounds);
  auto dataType = inputArray.getDataType();

  switch(dataType)
  {
  case DataType::int8:
    Algorithms::CropImageGeomDataArray<int8>(inputArray, outputArray, geom, bounds, shouldCancel)();
    break;
  case DataType::int16:
    Algorithms::CropImageGeomDataArray<int16>(inputArray, outputArray, geom, bounds, shouldCancel)();
    break;
  case DataType::int32:
    Algorithms::CropImageGeomDataArray<int32>(inputArray, outputArray, geom, bounds, shouldCancel)();
    break;
  case DataType::int64:
    Algorithms::CropImageGeomDataArray<int64>(inputArray, outputArray, geom, bounds, shouldCancel)();
    break;
  case DataType::uint8:
    Algorithms::CropImageGeomDataArray<uint8>(inputArray, outputArray, geom, bounds, shouldCancel)();
    break;
  case DataType::uint16:
    Algorithms::CropImageGeomDataArray<uint16>(inputArray, outputArray, geom, bounds, shouldCancel)();
    break;
  case DataType::uint32:
    Algorithms::CropImageGeomDataArray<uint32>(inputArray, outputArray, geom, bounds, shouldCancel)();
    break;
  case DataType::uint64:
    Algorithms::CropImageGeomDataArray<uint64>(inputArray, outputArray, geom, bounds, shouldCancel)();
    break;
  case DataType::boolean:
    Algorithms::CropImageGeomDataArray<bool>(inputArray, outputArray, geom, bounds, shouldCancel)();
    break;
  case DataType::float32:
    Algorithms::CropImageGeomDataArray<float32>(inputArray, outputArray, geom, bounds, shouldCancel)();
    break;
  case DataType::float64:
    Algorithms::CropImageGeomDataArray<float64>(inputArray, outputArray, geom, bounds, shouldCancel)();
    break;
  default:
    return MakeErrorResult(-5546, fmt::format("Failed to crop DataArray by voxels. Invali array type '{}'", static_cast<uint8>(dataType)));
  }

  return {};
}

Result<> CropVoxelNeighborList(const ImageGeom& geom, const INeighborList& inputArray, INeighborList& outputArray, const std::array<bool, 3>& isBound, const IntVec3& minBounds, const IntVec3& maxBounds)
{
  std::atomic_bool shouldCancel = false;
  auto bounds = Generic::getBounds(geom, isBound, minBounds, maxBounds);
  auto dataType = inputArray.getDataType();

  switch(dataType)
  {
  case DataType::int8:
    Algorithms::CropImageGeomNeighborList<int8>(dynamic_cast<const NeighborList<int8>&>(inputArray), dynamic_cast<NeighborList<int8>&>(outputArray), geom, bounds, shouldCancel)();
    break;
  case DataType::int16:
    Algorithms::CropImageGeomNeighborList<int16>(dynamic_cast<const NeighborList<int16>&>(inputArray), dynamic_cast<NeighborList<int16>&>(outputArray), geom, bounds, shouldCancel)();
    break;
  case DataType::int32:
    Algorithms::CropImageGeomNeighborList<int32>(dynamic_cast<const NeighborList<int32>&>(inputArray), dynamic_cast<NeighborList<int32>&>(outputArray), geom, bounds, shouldCancel)();
    break;
  case DataType::int64:
    Algorithms::CropImageGeomNeighborList<int64>(dynamic_cast<const NeighborList<int64>&>(inputArray), dynamic_cast<NeighborList<int64>&>(outputArray), geom, bounds, shouldCancel)();
    break;
  case DataType::uint8:
    Algorithms::CropImageGeomNeighborList<uint8>(dynamic_cast<const NeighborList<uint8>&>(inputArray), dynamic_cast<NeighborList<uint8>&>(outputArray), geom, bounds, shouldCancel)();
    break;
  case DataType::uint16:
    Algorithms::CropImageGeomNeighborList<uint16>(dynamic_cast<const NeighborList<uint16>&>(inputArray), dynamic_cast<NeighborList<uint16>&>(outputArray), geom, bounds, shouldCancel)();
    break;
  case DataType::uint32:
    Algorithms::CropImageGeomNeighborList<uint32>(dynamic_cast<const NeighborList<uint32>&>(inputArray), dynamic_cast<NeighborList<uint32>&>(outputArray), geom, bounds, shouldCancel)();
    break;
  case DataType::uint64:
    Algorithms::CropImageGeomNeighborList<uint64>(dynamic_cast<const NeighborList<uint64>&>(inputArray), dynamic_cast<NeighborList<uint64>&>(outputArray), geom, bounds, shouldCancel)();
    break;
  case DataType::float32:
    Algorithms::CropImageGeomNeighborList<float32>(dynamic_cast<const NeighborList<float32>&>(inputArray), dynamic_cast<NeighborList<float32>&>(outputArray), geom, bounds, shouldCancel)();
    break;
  case DataType::float64:
    Algorithms::CropImageGeomNeighborList<float64>(dynamic_cast<const NeighborList<float64>&>(inputArray), dynamic_cast<NeighborList<float64>&>(outputArray), geom, bounds, shouldCancel)();
    break;
  default:
    return MakeErrorResult(-5556, fmt::format("Failed to crop NeighborList by voxels. Invali array type '{}'", static_cast<uint8>(dataType)));
  }

  return {};
}
Result<> CropVoxelStringArray(const ImageGeom& geom, const StringArray& inputArray, StringArray& outputArray, const std::array<bool, 3>& isBound, const IntVec3& minBounds, const IntVec3& maxBounds)
{
  std::atomic_bool shouldCancel = false;
  auto bounds = Generic::getBounds(geom, isBound, minBounds, maxBounds);
  Algorithms::CropImageGeomStringArray(inputArray, outputArray, geom, bounds, shouldCancel)();
  return {};
}

} // namespace CropGeometry
} // namespace nx::core