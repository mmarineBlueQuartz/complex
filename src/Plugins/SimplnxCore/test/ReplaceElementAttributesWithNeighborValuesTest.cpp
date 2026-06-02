#include "SimplnxCore/SimplnxCore_test_dirs.hpp"
#include <catch2/catch.hpp>

#include "SimplnxCore/Filters/ReplaceElementAttributesWithNeighborValuesFilter.hpp"

#include "simplnx/Core/Application.hpp"
#include "simplnx/DataStructure/DataArray.hpp"
#include "simplnx/DataStructure/DataStore.hpp"
#include "simplnx/Parameters/ChoicesParameter.hpp"
#include "simplnx/Pipeline/Pipeline.hpp"
#include "simplnx/Pipeline/PipelineFilter.hpp"
#include "simplnx/UnitTest/UnitTestCommon.hpp"

#include <filesystem>
#include <fstream>
#include <memory>

namespace fs = std::filesystem;

using namespace nx::core;
using namespace nx::core::Constants;
using namespace nx::core::UnitTest;

namespace
{
const DataPath k_ConfidenceIndexPath = k_CellAttributeMatrix.createChildPath(Constants::k_Confidence_Index);
const std::string k_ExemplarDataContainer2("DataContainer");

const DataPath k_LocalConfidence({k_ImageGeometry, k_ConfidenceIndex});
constexpr StringLiteral k_Data = "Data";
const DataPath k_DataPath({k_ImageGeometry, k_CellData, k_Data});
const ShapeType k_Dimensions = {2, 3, 1};

bool CompareFloats(const float32 generated, const float32 expected)
{
  return std::abs(generated - expected) < 0.000012f;
}
} // namespace

DataStructure CreateTestData()
{
  DataStructure dataStructure;

  auto* imageGeom = ImageGeom::Create(dataStructure, k_ImageGeometry);
  imageGeom->setDimensions(::k_Dimensions);

  auto* cellData = AttributeMatrix::Create(dataStructure, k_CellData, k_Dimensions, imageGeom->getId());
  imageGeom->setCellData(cellData->getId());

  // Confidence Index
  {
    auto store = std::make_shared<DataStore<float32>>(k_Dimensions, ShapeType{1}, 0);
    auto* dataArrayPtr = DataArray<float32>::Create(dataStructure, k_ConfidenceIndex, store, imageGeom->getId());
    auto& storeRef = *store.get();

    for(usize i = 0; i < 3; i++)
    {
      storeRef[i * 2 + 0] = 0.010f * i;
      storeRef[i * 2 + 1] = 0.012f * i;
    }
  }
  // Cell Data
  {
    auto store = std::make_shared<DataStore<int32>>(k_Dimensions, ShapeType{1}, 0);
    auto* dataArrayPtr = DataArray<int32>::Create(dataStructure, k_Data, store, cellData->getId());
    auto& storeRef = *store.get();

    for(usize i = 0; i < 3; i++)
    {
      storeRef[i * 2 + 0] = 10 * i;
      storeRef[i * 2 + 1] = 12 * i;
    }
    storeRef[5] = 100;
  }

  return dataStructure;
}

void CheckTest1Output(DataStructure& dataStructure)
{
  auto& dataStore = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  REQUIRE(dataStore[0] == 0);
  REQUIRE(dataStore[1] == 0);
  REQUIRE(dataStore[2] == 10);
  REQUIRE(dataStore[3] == 12);
  REQUIRE(dataStore[4] == 20);
  REQUIRE(dataStore[5] == 100);
}

// Less than comparison
void CheckTest2Output(DataStructure& dataStructure)
{
  auto& dataStore = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  REQUIRE(dataStore[0] == 10);
  REQUIRE(dataStore[1] == 12);
  REQUIRE(dataStore[2] == 10);
  REQUIRE(dataStore[3] == 12);
  REQUIRE(dataStore[4] == 20);
  REQUIRE(dataStore[5] == 100);
}

// Greater than comparison
void CheckTest3Output(DataStructure& dataStructure)
{
  auto& dataStore = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  REQUIRE(dataStore[0] == 0);
  REQUIRE(dataStore[1] == 0);
  REQUIRE(dataStore[2] == 0);
  REQUIRE(dataStore[3] == 0);
  REQUIRE(dataStore[4] == 20);
  REQUIRE(dataStore[5] == 100);
}

// Loop Less Than: Loop
void CheckTest4Output(DataStructure& dataStructure)
{
  auto& dataStore = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  REQUIRE(dataStore[0] == 0);
  REQUIRE(dataStore[1] == 0);
  REQUIRE(dataStore[2] == 10);
  REQUIRE(dataStore[3] == 10);
  REQUIRE(dataStore[4] == 20);
  REQUIRE(dataStore[5] == 100);
}

// Loop Greater Than: Loop
void CheckTest5Output(DataStructure& dataStructure)
{
  auto& dataStore = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  REQUIRE(dataStore[0] == 0);
  REQUIRE(dataStore[1] == 0);
  REQUIRE(dataStore[2] == 12);
  REQUIRE(dataStore[3] == 12);
  REQUIRE(dataStore[4] == 20);
  REQUIRE(dataStore[5] == 100);
}

// Loop Less Than: Loop: Value 2
void CheckTest6Output(DataStructure& dataStructure)
{
  auto& dataStore = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  REQUIRE(dataStore[0] == 0);
  REQUIRE(dataStore[1] == 0);
  REQUIRE(dataStore[2] == 0);
  REQUIRE(dataStore[3] == 0);
  REQUIRE(dataStore[4] == 0);
  REQUIRE(dataStore[5] == 100);
}

// Loop Less Than: Loop: Value 2
void CheckTest7Output(DataStructure& dataStructure)
{
  auto& dataStore = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  REQUIRE(dataStore[0] == 24);
  REQUIRE(dataStore[1] == 24);
  REQUIRE(dataStore[2] == 24);
  REQUIRE(dataStore[3] == 24);
  REQUIRE(dataStore[4] == 24);
  REQUIRE(dataStore[5] == 100);
}

void RunFilter(DataStructure& dataStructure, uint64 comparisonType, float32 minConfidence, bool loop)
{
  {
    // Instantiate the filter, a DataStructure object and an Arguments Object
    ReplaceElementAttributesWithNeighborValuesFilter filter;
    Arguments args;

    // Create default Parameters for the filter.
    args.insertOrAssign(ReplaceElementAttributesWithNeighborValuesFilter::k_MinConfidence_Key, std::make_any<float32>(minConfidence));
    args.insertOrAssign(ReplaceElementAttributesWithNeighborValuesFilter::k_SelectedComparison_Key, std::make_any<ChoicesParameter::ValueType>(comparisonType));
    args.insertOrAssign(ReplaceElementAttributesWithNeighborValuesFilter::k_Loop_Key, std::make_any<bool>(loop));
    args.insertOrAssign(ReplaceElementAttributesWithNeighborValuesFilter::k_ComparisonDataPath, std::make_any<DataPath>(k_LocalConfidence));
    args.insertOrAssign(ReplaceElementAttributesWithNeighborValuesFilter::k_SelectedImageGeometryPath_Key, std::make_any<DataPath>(DataPath({k_ImageGeometry})));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    SIMPLNX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    SIMPLNX_RESULT_REQUIRE_VALID(executeResult.result)
  }
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: No Confidence", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  UnitTest::LoadPlugins();

  const float32 confidence = 0.9f;
  const bool loop = false;
  const uint64 comparison = 0; // Less than
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest1Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: No Loop: Less Than", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  UnitTest::LoadPlugins();

  const float32 confidence = 0.005f;
  const bool loop = false;
  const uint64 comparison = 0;
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest2Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: No Loop: Greater Than", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  UnitTest::LoadPlugins();

  const float32 confidence = 0.005f;
  const bool loop = false;
  const uint64 comparison = 1; // Greater than
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest3Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: Loop: Less Than", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  UnitTest::LoadPlugins();

  const float32 confidence = 0.005f;
  const bool loop = true;
  const uint64 comparison = 0;
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest4Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: Loop: Greater Than", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  UnitTest::LoadPlugins();

  const float32 confidence = 0.005f;
  const bool loop = true;
  const uint64 comparison = 1;
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest5Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: Loop: Less Than v2", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  UnitTest::LoadPlugins();

  const float32 confidence = 0.01f;
  const bool loop = true;
  const uint64 comparison = 0;
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest6Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: Loop: Greater Than v2", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  UnitTest::LoadPlugins();

  const float32 confidence = 0.01f;
  const bool loop = true;
  const uint64 comparison = 1;
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest7Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  UnitTest::LoadPlugins();

  const nx::core::UnitTest::TestFileSentinel testDataSentinel(nx::core::unit_test::k_TestFilesDir, "6_6_replace_element_attributes_with_neighbor.tar.gz",
                                                              "6_6_replace_element_attributes_with_neighbor");

  // Read Exemplar DREAM3D File Filter
  auto exemplarFilePath = fs::path(fmt::format("{}/TestFiles/6_6_replace_element_attributes_with_neighbor/6_6_replace_element_attributes_with_neighbor.dream3d", unit_test::k_DREAM3DDataDir));
  DataStructure exemplarDataStructure = nx::core::UnitTest::LoadDataStructure(exemplarFilePath);

  // Read the Test Data set
  auto baseDataFilePath = fs::path(fmt::format("{}/TestFiles/6_6_replace_element_attributes_with_neighbor/6_6_replace_element_attributes_with_neighbor.dream3d", unit_test::k_DREAM3DDataDir));
  DataStructure dataStructure = UnitTest::LoadDataStructure(baseDataFilePath);

  {
    // Instantiate the filter, a DataStructure object and an Arguments Object
    ReplaceElementAttributesWithNeighborValuesFilter filter;
    Arguments args;

    // Create default Parameters for the filter.
    args.insertOrAssign(ReplaceElementAttributesWithNeighborValuesFilter::k_MinConfidence_Key, std::make_any<float32>(0.1F));
    args.insertOrAssign(ReplaceElementAttributesWithNeighborValuesFilter::k_SelectedComparison_Key, std::make_any<ChoicesParameter::ValueType>(0));
    args.insertOrAssign(ReplaceElementAttributesWithNeighborValuesFilter::k_Loop_Key, std::make_any<bool>(true));
    args.insertOrAssign(ReplaceElementAttributesWithNeighborValuesFilter::k_ComparisonDataPath, std::make_any<DataPath>(k_ConfidenceIndexPath));
    args.insertOrAssign(ReplaceElementAttributesWithNeighborValuesFilter::k_SelectedImageGeometryPath_Key, std::make_any<DataPath>(k_DataContainerPath));

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    SIMPLNX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    SIMPLNX_RESULT_REQUIRE_VALID(executeResult.result)
  }

  UnitTest::CompareExemplarToGeneratedData(dataStructure, exemplarDataStructure, k_CellAttributeMatrix, k_ExemplarDataContainer2);

#ifdef SIMPLNX_WRITE_TEST_OUTPUT
  WriteTestDataStructure(dataStructure, fmt::format("{}/7_0_replace_element_attributes_with_neighbor.dream3d", unit_test::k_BinaryTestOutputDir));
#endif

  UnitTest::CheckArraysInheritTupleDims(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: SIMPL Backwards Compatibility", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter][BackwardsCompatibility]")
{
  auto app = Application::GetOrCreateInstance();
  UnitTest::LoadPlugins();
  auto filterList = app->getFilterList();

  const fs::path conversionDir = fs::path(nx::core::unit_test::k_SourceDir.view()) / "test" / "simpl_conversion";

  const std::vector<std::pair<std::string, fs::path>> fixtures = {
      {"SIMPL 6.5 (UUID)", conversionDir / "6_5" / "ReplaceElementAttributesWithNeighborValuesFilter.json"},
      {"SIMPL 6.4 (Filter_Name)", conversionDir / "6_4" / "ReplaceElementAttributesWithNeighborValuesFilter.json"},
  };

  for(const auto& [label, fixturePath] : fixtures)
  {
    DYNAMIC_SECTION(label)
    {
      auto pipelineResult = Pipeline::FromSIMPLFile(fixturePath, filterList);
      REQUIRE(pipelineResult.valid());

      auto& pipeline = pipelineResult.value();
      REQUIRE(pipeline.size() == 1);

      auto* pipelineFilter = dynamic_cast<PipelineFilter*>(pipeline.at(0));
      REQUIRE(pipelineFilter != nullptr);

      const IFilter* filter = pipelineFilter->getFilter();
      REQUIRE(filter != nullptr);
      REQUIRE(filter->uuid() == FilterTraits<ReplaceElementAttributesWithNeighborValuesFilter>::uuid);

      CHECK(pipelineFilter->getComments().empty());

      const Arguments args = pipelineFilter->getArguments();
      CHECK(args.value<float32>(ReplaceElementAttributesWithNeighborValuesFilter::k_MinConfidence_Key) == 2.5f);
      CHECK(args.value<ChoicesParameter::ValueType>(ReplaceElementAttributesWithNeighborValuesFilter::k_SelectedComparison_Key) == 0);
      CHECK(args.value<bool>(ReplaceElementAttributesWithNeighborValuesFilter::k_Loop_Key) == true);
      CHECK(args.value<DataPath>(ReplaceElementAttributesWithNeighborValuesFilter::k_ComparisonDataPath) == DataPath({"DataContainer", "CellData", "TestArray"}));
      CHECK(args.value<DataPath>(ReplaceElementAttributesWithNeighborValuesFilter::k_SelectedImageGeometryPath_Key) == DataPath({"DataContainer"}));
    }
  }
}
