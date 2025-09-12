#include "simplnx/DataStructure/AbstractStringStore.hpp"
#include "simplnx/DataStructure/ListStore.hpp"
#include "simplnx/UnitTest/UnitTestCommon.hpp"
#include "simplnx/Utilities/CropGeometryUtilities.hpp"

#include <catch2/catch.hpp>

using namespace nx::core;

namespace
{
constexpr usize k_NumTuples = 15;
constexpr usize k_NumComps = 3;
const SizeVec3 k_VoxelDims = {3, 5, 1};

constexpr StringLiteral k_OutputCopyName = "Output Copy";
constexpr StringLiteral k_OutputVoxelName = "Output Voxels";
constexpr StringLiteral k_OutputPhysicalName = "Output Physical";

const DataPath k_GeomPath({Constants::k_ImageGeometry});
const DataPath k_InputMatrixPath({Constants::k_ImageGeometry, Constants::k_CellData});
} // namespace

DataStructure createTestDataSturcture()
{
  IDataStore::ShapeType tupleShape{k_NumTuples};
  auto listStore = std::make_shared<ListStore<int32>>(k_NumTuples);

  // Input Data
  DataStructure dataStructure;
  ImageGeom* imageGeom = ImageGeom::Create(dataStructure, Constants::k_ImageGeometry);
  AttributeMatrix* cellMatrix = AttributeMatrix::Create(dataStructure, Constants::k_CellData, {3, 5, 1}, imageGeom->getId());
  imageGeom->setCellData(*cellMatrix);
  imageGeom->setDimensions(k_VoxelDims);

  auto* dataArray = DataArray<int16>::CreateWithStore<DataStore<int16>>(dataStructure, "DataArray", tupleShape, {k_NumComps}, cellMatrix->getId());
  auto* neighborList = NeighborList<int32>::Create(dataStructure, "NeighborList", listStore, cellMatrix->getId());
  auto* stringArray = StringArray::Create(dataStructure, "StringArray", imageGeom->getId());

  // Fill Data
  auto& dataStoreRef = dataArray->getDataStoreRef();
  for(usize i = 0; i < k_NumTuples * k_NumComps; i++)
  {
    dataStoreRef[i] = i;
  }
  auto& neighborStoreRef = neighborList->getStoreRef();
  for(usize i = 0; i < k_NumTuples; i++)
  {
    std::vector<int32> value(1);
    value[0] = i;
    neighborStoreRef[i] = value;
  }
  auto& stringStoreRef = stringArray->getStoreRef();
  stringStoreRef.resize(k_NumTuples);
  for(usize i = 0; i < k_NumTuples; i++)
  {
    stringStoreRef[i] = std::to_string(i);
  }

  // Output Data
  DataGroup* outputCopyGroup = DataGroup::Create(dataStructure, k_OutputCopyName);
  DataGroup* outputVoxelGroup = DataGroup::Create(dataStructure, k_OutputVoxelName);
  DataGroup* outputPhysicalGroup = DataGroup::Create(dataStructure, k_OutputPhysicalName);

  auto* outputCopyArray = DataArray<int16>::CreateWithStore<DataStore<int16>>(dataStructure, "DataArray", tupleShape, {k_NumComps}, outputCopyGroup->getId());
  auto outputCopyListStore = std::make_shared<ListStore<int32>>(k_NumTuples);
  auto* outputCopyList = NeighborList<int32>::Create(dataStructure, "NeighborList", outputCopyListStore, outputCopyGroup->getId());
  auto* outputCopyStrings = StringArray::Create(dataStructure, "StringArray", outputCopyGroup->getId());

  auto* outputVoxelArray = DataArray<int16>::CreateWithStore<DataStore<int16>>(dataStructure, "DataArray", tupleShape, {k_NumComps}, outputVoxelGroup->getId());
  auto outputVoxelListStore = std::make_shared<ListStore<int32>>(k_NumTuples);
  auto* outputVoxelList = NeighborList<int32>::Create(dataStructure, "NeighborList", outputVoxelListStore, outputVoxelGroup->getId());
  auto* outputVoxelStrings = StringArray::Create(dataStructure, "StringArray", outputVoxelGroup->getId());
  outputVoxelStrings->resizeTuples({k_NumTuples});

  auto* outputPhysicalArray = DataArray<int16>::CreateWithStore<DataStore<int16>>(dataStructure, "DataArray", tupleShape, {k_NumComps}, outputPhysicalGroup->getId());
  auto outputPhysicalListStore = std::make_shared<ListStore<int32>>(k_NumTuples);
  auto* outputPhysicalList = NeighborList<int32>::Create(dataStructure, "NeighborList", outputPhysicalListStore, outputPhysicalGroup->getId());
  auto* outputPhysicalStrings = StringArray::Create(dataStructure, "StringArray", outputPhysicalGroup->getId());

  return dataStructure;
}

IntVec2Type getXBounds(const ImageGeom& geom, const CropGeometryParameter::ValueType& cropValues)
{
  if(!cropValues.cropX)
  {
    auto dimensions = geom.getDimensions();
    int32 value = dimensions[0];
    return {0, value};
  }

  switch(cropValues.type)
  {
  case CropGeometryParameter::ValueType::TypeEnum::VoxelSubvolume:
    return cropValues.xBoundVoxels;
  case CropGeometryParameter::ValueType::TypeEnum::PhysicalSubvolume: {
    FloatVec3 minBounds{cropValues.xBoundPhysical[0], cropValues.yBoundPhysical[0], cropValues.zBoundPhysical[0]};
    FloatVec3 maxBounds{cropValues.xBoundPhysical[1], cropValues.yBoundPhysical[1], cropValues.zBoundPhysical[1]};
    auto minVoxel = CropGeometry::ConvertPhysicalToVoxel(geom, minBounds);
    auto maxVoxel = CropGeometry::ConvertPhysicalToVoxel(geom, maxBounds);
    return {minVoxel[0], maxVoxel[0]};
  }
  default: {
    auto dimensions = geom.getDimensions();
    int32 value = dimensions[0];
    return {0, value};
  }
  }
}

IntVec2Type getYBounds(const ImageGeom& geom, const CropGeometryParameter::ValueType& cropValues)
{
  if(!cropValues.cropY)
  {
    auto dimensions = geom.getDimensions();
    int32 value = dimensions[1];
    return {0, value};
  }

  switch(cropValues.type)
  {
  case CropGeometryParameter::ValueType::TypeEnum::VoxelSubvolume:
    return cropValues.yBoundVoxels;
  case CropGeometryParameter::ValueType::TypeEnum::PhysicalSubvolume: {
    FloatVec3 minBounds{cropValues.xBoundPhysical[0], cropValues.yBoundPhysical[0], cropValues.zBoundPhysical[0]};
    FloatVec3 maxBounds{cropValues.xBoundPhysical[1], cropValues.yBoundPhysical[1], cropValues.zBoundPhysical[1]};
    auto minVoxel = CropGeometry::ConvertPhysicalToVoxel(geom, minBounds);
    auto maxVoxel = CropGeometry::ConvertPhysicalToVoxel(geom, maxBounds);
    return {minVoxel[1], maxVoxel[1]};
  }
  default: {
    auto dimensions = geom.getDimensions();
    int32 value = dimensions[1];
    return {0, value};
  }
  }
}

IntVec2Type getZBounds(const ImageGeom& geom, const CropGeometryParameter::ValueType& cropValues)
{
  if(!cropValues.cropZ)
  {
    auto dimensions = geom.getDimensions();
    int32 value = dimensions[2];
    return {0, value};
  }

  switch(cropValues.type)
  {
  case CropGeometryParameter::ValueType::TypeEnum::VoxelSubvolume:
    return cropValues.zBoundVoxels;
  case CropGeometryParameter::ValueType::TypeEnum::PhysicalSubvolume: {
    FloatVec3 minBounds{cropValues.xBoundPhysical[0], cropValues.yBoundPhysical[0], cropValues.zBoundPhysical[0]};
    FloatVec3 maxBounds{cropValues.xBoundPhysical[1], cropValues.yBoundPhysical[1], cropValues.zBoundPhysical[1]};
    auto minVoxel = CropGeometry::ConvertPhysicalToVoxel(geom, minBounds);
    auto maxVoxel = CropGeometry::ConvertPhysicalToVoxel(geom, maxBounds);
    return {minVoxel[2], maxVoxel[2]};
  }
  default: {
    auto dimensions = geom.getDimensions();
    int32 value = dimensions[2];
    return {0, value};
  }
  }
}

template <typename T>
void checkDataArrayOutput(const ImageGeom& geom, const AbstractDataStore<T>& inputStore, AbstractDataStore<T>& outputStore, const CropGeometryParameter::ValueType& cropValues)
{
  size_t numComps = inputStore.getNumberOfComponents();
  auto srcDims = geom.getDimensions();
  auto xBounds = getXBounds(geom, cropValues);
  auto yBounds = getYBounds(geom, cropValues);
  auto zBounds = getZBounds(geom, cropValues);

  uint64 destTupleIndex = 0;
  for(uint64 zIndex = zBounds[0]; zIndex < zBounds[1]; zIndex++)
  {
    for(uint64 yIndex = yBounds[0]; yIndex < yBounds[1]; yIndex++)
    {
      for(uint64 xIndex = xBounds[0]; xIndex < xBounds[1]; xIndex++)
      {
        uint64 srcIndex = (srcDims[0] * srcDims[1] * zIndex) + (srcDims[0] * yIndex) + xIndex;
        for(size_t compIndex = 0; compIndex < numComps; compIndex++)
        {
          REQUIRE(inputStore.at(destTupleIndex * numComps + compIndex) == outputStore.at(srcIndex * numComps + compIndex));
        }
        destTupleIndex++;
      }
    }
  }
}

template <typename T>
void checkNeighborListOutput(const ImageGeom& geom, const AbstractListStore<T>& inputStore, AbstractListStore<T>& outputStore, const CropGeometryParameter::ValueType& cropValues)
{
  size_t numComps = 1;
  auto srcDims = geom.getDimensions();
  auto xBounds = getXBounds(geom, cropValues);
  auto yBounds = getYBounds(geom, cropValues);
  auto zBounds = getZBounds(geom, cropValues);

  uint64 destTupleIndex = 0;
  for(uint64 zIndex = zBounds[0]; zIndex < zBounds[1]; zIndex++)
  {
    for(uint64 yIndex = yBounds[0]; yIndex < yBounds[1]; yIndex++)
    {
      for(uint64 xIndex = xBounds[0]; xIndex < xBounds[1]; xIndex++)
      {
        uint64 srcIndex = (srcDims[0] * srcDims[1] * zIndex) + (srcDims[0] * yIndex) + xIndex;
        for(size_t compIndex = 0; compIndex < numComps; compIndex++)
        {
          REQUIRE(inputStore.at(destTupleIndex * numComps + compIndex) == outputStore.at(srcIndex * numComps + compIndex));
        }
        destTupleIndex++;
      }
    }
  }
}

void checkStringArrayOutput(const ImageGeom& geom, const AbstractStringStore& inputStore, AbstractStringStore& outputStore, const CropGeometryParameter::ValueType& cropValues)
{
  size_t numComps = 1;
  auto srcDims = geom.getDimensions();
  auto xBounds = getXBounds(geom, cropValues);
  auto yBounds = getYBounds(geom, cropValues);
  auto zBounds = getZBounds(geom, cropValues);

  uint64 destTupleIndex = 0;
  for(uint64 zIndex = zBounds[0]; zIndex < zBounds[1]; zIndex++)
  {
    for(uint64 yIndex = yBounds[0]; yIndex < yBounds[1]; yIndex++)
    {
      for(uint64 xIndex = xBounds[0]; xIndex < xBounds[1]; xIndex++)
      {
        uint64 srcIndex = (srcDims[0] * srcDims[1] * zIndex) + (srcDims[0] * yIndex) + xIndex;
        for(size_t compIndex = 0; compIndex < numComps; compIndex++)
        {
          //REQUIRE(inputStore.at(destTupleIndex * numComps + compIndex) == outputStore.at(srcIndex * numComps + compIndex));
        }
        destTupleIndex++;
      }
    }
  }
}

void testCropOutput(const CropGeometryParameter::ValueType& cropValues, const DataPath& outputGroupPath)
{
  using CropValuesType = CropGeometryParameter::ValueType;

  DataStructure dataStruct = createTestDataSturcture();

  SIMPLNX_RESULT_REQUIRE_VALID(CropGeometry::CropArray(dataStruct, k_GeomPath, k_InputMatrixPath.createChildPath("DataArray"), outputGroupPath.createChildPath("DataArray"), cropValues));
  SIMPLNX_RESULT_REQUIRE_VALID(CropGeometry::CropArray(dataStruct, k_GeomPath, k_InputMatrixPath.createChildPath("NeighborList"), outputGroupPath.createChildPath("NeighborList"), cropValues));
  SIMPLNX_RESULT_REQUIRE_VALID(CropGeometry::CropArray(dataStruct, k_GeomPath, k_GeomPath.createChildPath("StringArray"), outputGroupPath.createChildPath("StringArray"), cropValues));

  const ImageGeom& geom = dataStruct.getDataRefAs<ImageGeom>(k_GeomPath);

  auto& inputDataStore = dataStruct.getDataRefAs<DataArray<int16>>(k_InputMatrixPath.createChildPath("DataArray")).getDataStoreRef();
  auto& inputListStore = dataStruct.getDataRefAs<NeighborList<int32>>(k_InputMatrixPath.createChildPath("NeighborList")).getStoreRef();
  auto& inputStringStore = dataStruct.getDataRefAs<StringArray>(k_GeomPath.createChildPath("StringArray")).getStoreRef();

  auto& outputDataStore = dataStruct.getDataRefAs<DataArray<int16>>(outputGroupPath.createChildPath("DataArray")).getDataStoreRef();
  auto& outputListStore = dataStruct.getDataRefAs<NeighborList<int32>>(outputGroupPath.createChildPath("NeighborList")).getStoreRef();
  auto& outputStringStore = dataStruct.getDataRefAs<StringArray>(outputGroupPath.createChildPath("StringArray")).getStoreRef();

  checkDataArrayOutput<int16>(geom, inputDataStore, outputDataStore, cropValues);
  checkNeighborListOutput<int32>(geom, inputListStore, outputListStore, cropValues);
  checkStringArrayOutput(geom, inputStringStore, outputStringStore, cropValues);
}

TEST_CASE("Utilities::CropImageGeom::Copy")
{
  UnitTest::LoadPlugins();

  using CropValuesType = CropGeometryParameter::ValueType;

  CropValuesType cropValues;
  cropValues.type = CropValuesType::TypeEnum::EntireVolume;

  DataPath outputGroupPath({k_OutputCopyName});

  testCropOutput(cropValues, outputGroupPath);
}

TEST_CASE("Utilities::CropImageGeom::Voxel Dimensions")
{
  UnitTest::LoadPlugins();

  using CropValuesType = CropGeometryParameter::ValueType;

  CropValuesType cropValues;
  cropValues.xBoundVoxels = {1, 2};
  cropValues.yBoundVoxels = {2, 4};
  cropValues.zBoundVoxels = {0, 0};
  cropValues.type = CropValuesType::TypeEnum::VoxelSubvolume;

  DataPath outputGroupPath({k_OutputVoxelName});

  SECTION("All Dimensions")
  {
    cropValues.cropX = true;
    cropValues.cropY = true;
    cropValues.cropZ = true;
    testCropOutput(cropValues, outputGroupPath);
  }

  SECTION("Disable X")
  {
    cropValues.cropX = false;
    cropValues.cropY = true;
    cropValues.cropZ = true;
    testCropOutput(cropValues, outputGroupPath);
  }

  SECTION("Disable Y")
  {
    cropValues.cropX = true;
    cropValues.cropY = false;
    cropValues.cropZ = true;
    testCropOutput(cropValues, outputGroupPath);
  }

  SECTION("Disable Z")
  {
    cropValues.cropX = true;
    cropValues.cropY = true;
    cropValues.cropZ = false;
    testCropOutput(cropValues, outputGroupPath);
  }
}

TEST_CASE("Utilities::CropImageGeom::Physical Dimensions")
{
  UnitTest::LoadPlugins();

  using CropValuesType = CropGeometryParameter::ValueType;

  CropValuesType cropValues;
  cropValues.xBoundPhysical = {1, 2};
  cropValues.yBoundPhysical = {2, 4};
  cropValues.zBoundPhysical = {0, 0};
  cropValues.type = CropValuesType::TypeEnum::VoxelSubvolume;

  DataPath outputGroupPath({k_OutputPhysicalName});

  SECTION("All Dimensions")
  {
    cropValues.cropX = true;
    cropValues.cropY = true;
    cropValues.cropZ = true;
    testCropOutput(cropValues, outputGroupPath);
  }

  SECTION("Disable X")
  {
    cropValues.cropX = false;
    cropValues.cropY = true;
    cropValues.cropZ = true;
    testCropOutput(cropValues, outputGroupPath);
  }

  SECTION("Disable Y")
  {
    cropValues.cropX = true;
    cropValues.cropY = false;
    cropValues.cropZ = true;
    testCropOutput(cropValues, outputGroupPath);
  }

  SECTION("Disable Z")
  {
    cropValues.cropX = true;
    cropValues.cropY = true;
    cropValues.cropZ = false;
    testCropOutput(cropValues, outputGroupPath);
  }
}
