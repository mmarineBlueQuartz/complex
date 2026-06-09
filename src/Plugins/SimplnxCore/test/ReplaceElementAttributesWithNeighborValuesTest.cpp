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
const ShapeType k_Dimensions = {3, 3, 3};

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

    storeRef[0] = 0.1f;
    storeRef[1] = 0.25f;
    storeRef[2] = 0.1f;
    storeRef[3] = 0.35f;
    storeRef[4] = 0.6f;
    storeRef[5] = 0.5f;
    storeRef[6] = 0.1f;
    storeRef[7] = 0.35f;
    storeRef[8] = 0.1f;
    //
    storeRef[9] = 0.1f;
    storeRef[10] = 0.35f;
    storeRef[11] = 0.1f;
    storeRef[12] = 0.5f;
    storeRef[13] = 0.9f;
    storeRef[14] = 0.25f;
    storeRef[15] = 0.1f;
    storeRef[16] = 0.4f;
    storeRef[17] = 0.1f;
    //
    storeRef[18] = 0.1f;
    storeRef[19] = 0.2f;
    storeRef[20] = 0.1f;
    storeRef[21] = 0.3f;
    storeRef[22] = 0.5f;
    storeRef[23] = 0.15f;
    storeRef[24] = 0.1f;
    storeRef[25] = 0.2f;
    storeRef[26] = 0.1f;
  }
  // Cell Data
  {
    auto store = std::make_shared<DataStore<int32>>(k_Dimensions, ShapeType{1}, 0);
    auto* dataArrayPtr = DataArray<int32>::Create(dataStructure, k_Data, store, cellData->getId());
    auto& storeRef = *store.get();

    storeRef[0] = 1;
    storeRef[1] = 2;
    storeRef[2] = 1;
    storeRef[3] = 2;
    storeRef[4] = 3;
    storeRef[5] = 2;
    storeRef[6] = 1;
    storeRef[7] = 2;
    storeRef[8] = 1;
    //
    storeRef[9] = 1;
    storeRef[10] = 2;
    storeRef[11] = 1;
    storeRef[12] = 3;
    storeRef[13] = 3;
    storeRef[14] = 2;
    storeRef[15] = 1;
    storeRef[16] = 2;
    storeRef[17] = 1;
    //
    storeRef[18] = 1;
    storeRef[19] = 1;
    storeRef[20] = 1;
    storeRef[21] = 2;
    storeRef[22] = 3;
    storeRef[23] = 1;
    storeRef[24] = 1;
    storeRef[25] = 1;
    storeRef[26] = 1;
  }

  return dataStructure;
}

// Values not changed
void CheckTest1Output(DataStructure& dataStructure)
{
  auto& dataStore = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();

  REQUIRE(dataStore[0] == 1);
  REQUIRE(dataStore[1] == 2);
  REQUIRE(dataStore[2] == 1);
  REQUIRE(dataStore[3] == 2);
  REQUIRE(dataStore[4] == 3);
  REQUIRE(dataStore[5] == 2);
  REQUIRE(dataStore[6] == 1);
  REQUIRE(dataStore[7] == 2);
  REQUIRE(dataStore[8] == 1);
  //
  REQUIRE(dataStore[9] == 1);
  REQUIRE(dataStore[10] == 2);
  REQUIRE(dataStore[11] == 1);
  REQUIRE(dataStore[12] == 3);
  REQUIRE(dataStore[13] == 3);
  REQUIRE(dataStore[14] == 2);
  REQUIRE(dataStore[15] == 1);
  REQUIRE(dataStore[16] == 2);
  REQUIRE(dataStore[17] == 1);
  //
  REQUIRE(dataStore[18] == 1);
  REQUIRE(dataStore[19] == 1);
  REQUIRE(dataStore[20] == 1);
  REQUIRE(dataStore[21] == 2);
  REQUIRE(dataStore[22] == 3);
  REQUIRE(dataStore[23] == 1);
  REQUIRE(dataStore[24] == 1);
  REQUIRE(dataStore[25] == 1);
  REQUIRE(dataStore[26] == 1);
}

// Less than comparison
void CheckTest2Output(DataStructure& dataStructure)
{
  auto& storeRef = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  // 
  REQUIRE(storeRef[0] == 1);
  REQUIRE(storeRef[1] == 3);
  REQUIRE(storeRef[2] == 2);
  REQUIRE(storeRef[3] == 3);
  REQUIRE(storeRef[4] == 3);
  REQUIRE(storeRef[5] == 2);
  REQUIRE(storeRef[6] == 1);
  REQUIRE(storeRef[7] == 3);
  REQUIRE(storeRef[8] == 2);
  //
  REQUIRE(storeRef[9] == 3);
  REQUIRE(storeRef[10] == 3);
  REQUIRE(storeRef[11] == 1);
  REQUIRE(storeRef[12] == 3);
  REQUIRE(storeRef[13] == 3);
  REQUIRE(storeRef[14] == 3);
  REQUIRE(storeRef[15] == 3);
  REQUIRE(storeRef[16] == 3);
  REQUIRE(storeRef[17] == 1);
  //
  REQUIRE(storeRef[18] == 1);
  REQUIRE(storeRef[19] == 3);
  REQUIRE(storeRef[20] == 1);
  REQUIRE(storeRef[21] == 3);
  REQUIRE(storeRef[22] == 3);
  REQUIRE(storeRef[23] == 3);
  REQUIRE(storeRef[24] == 1);
  REQUIRE(storeRef[25] == 3);
  REQUIRE(storeRef[26] == 1);
}

// Greater than comparison
void CheckTest3Output(DataStructure& dataStructure)
{
  auto& storeRef = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  //
  REQUIRE(storeRef[0] == 1);
  REQUIRE(storeRef[1] == 2);
  REQUIRE(storeRef[2] == 1);
  REQUIRE(storeRef[3] == 2);
  REQUIRE(storeRef[4] == 2);
  REQUIRE(storeRef[5] == 2);
  REQUIRE(storeRef[6] == 1);
  REQUIRE(storeRef[7] == 2);
  REQUIRE(storeRef[8] == 1);
  //
  REQUIRE(storeRef[9] == 1);
  REQUIRE(storeRef[10] == 2);
  REQUIRE(storeRef[11] == 1);
  REQUIRE(storeRef[12] == 3);
  REQUIRE(storeRef[13] == 2);
  REQUIRE(storeRef[14] == 2);
  REQUIRE(storeRef[15] == 1);
  REQUIRE(storeRef[16] == 2);
  REQUIRE(storeRef[17] == 1);
  //
  REQUIRE(storeRef[18] == 1);
  REQUIRE(storeRef[19] == 1);
  REQUIRE(storeRef[20] == 1);
  REQUIRE(storeRef[21] == 2);
  REQUIRE(storeRef[22] == 3);
  REQUIRE(storeRef[23] == 1);
  REQUIRE(storeRef[24] == 1);
  REQUIRE(storeRef[25] == 1);
  REQUIRE(storeRef[26] == 1);
}

// Loop Less Than: Loop
void CheckTest4Output(DataStructure& dataStructure)
{
  auto& storeRef = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  //
  REQUIRE(storeRef[0] == 3);
  REQUIRE(storeRef[1] == 3);
  REQUIRE(storeRef[2] == 2);
  REQUIRE(storeRef[3] == 3);
  REQUIRE(storeRef[4] == 3);
  REQUIRE(storeRef[5] == 2);
  REQUIRE(storeRef[6] == 3);
  REQUIRE(storeRef[7] == 3);
  REQUIRE(storeRef[8] == 2);
  //
  REQUIRE(storeRef[9] == 3);
  REQUIRE(storeRef[10] == 3);
  REQUIRE(storeRef[11] == 3);
  REQUIRE(storeRef[12] == 3);
  REQUIRE(storeRef[13] == 3);
  REQUIRE(storeRef[14] == 3);
  REQUIRE(storeRef[15] == 3);
  REQUIRE(storeRef[16] == 3);
  REQUIRE(storeRef[17] == 3);
  //
  REQUIRE(storeRef[18] == 3);
  REQUIRE(storeRef[19] == 3);
  REQUIRE(storeRef[20] == 3);
  REQUIRE(storeRef[21] == 3);
  REQUIRE(storeRef[22] == 3);
  REQUIRE(storeRef[23] == 3);
  REQUIRE(storeRef[24] == 3);
  REQUIRE(storeRef[25] == 3);
  REQUIRE(storeRef[26] == 3);
}

// Loop Greater Than: Loop
void CheckTest5Output(DataStructure& dataStructure)
{
  auto& storeRef = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  //
  REQUIRE(storeRef[0] == 1);
  REQUIRE(storeRef[1] == 2);
  REQUIRE(storeRef[2] == 1);
  REQUIRE(storeRef[3] == 2);
  REQUIRE(storeRef[4] == 2);
  REQUIRE(storeRef[5] == 2);
  REQUIRE(storeRef[6] == 1);
  REQUIRE(storeRef[7] == 2);
  REQUIRE(storeRef[8] == 1);
  //
  REQUIRE(storeRef[9] == 1);
  REQUIRE(storeRef[10] == 2);
  REQUIRE(storeRef[11] == 1);
  REQUIRE(storeRef[12] == 3);
  REQUIRE(storeRef[13] == 2);
  REQUIRE(storeRef[14] == 2);
  REQUIRE(storeRef[15] == 1);
  REQUIRE(storeRef[16] == 2);
  REQUIRE(storeRef[17] == 1);
  //
  REQUIRE(storeRef[18] == 1);
  REQUIRE(storeRef[19] == 1);
  REQUIRE(storeRef[20] == 1);
  REQUIRE(storeRef[21] == 2);
  REQUIRE(storeRef[22] == 3);
  REQUIRE(storeRef[23] == 1);
  REQUIRE(storeRef[24] == 1);
  REQUIRE(storeRef[25] == 1);
  REQUIRE(storeRef[26] == 1);
}

// Loop Less Than: Loop: Value 2
void CheckTest6Output(DataStructure& dataStructure)
{
  auto& storeRef = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  //
  REQUIRE(storeRef[0] == 2);
  REQUIRE(storeRef[1] == 3);
  REQUIRE(storeRef[2] == 2);
  REQUIRE(storeRef[3] == 2);
  REQUIRE(storeRef[4] == 3);
  REQUIRE(storeRef[5] == 2);
  REQUIRE(storeRef[6] == 2);
  REQUIRE(storeRef[7] == 2);
  REQUIRE(storeRef[8] == 2);
  //
  REQUIRE(storeRef[9] == 3);
  REQUIRE(storeRef[10] == 2);
  REQUIRE(storeRef[11] == 2);
  REQUIRE(storeRef[12] == 3);
  REQUIRE(storeRef[13] == 3);
  REQUIRE(storeRef[14] == 3);
  REQUIRE(storeRef[15] == 3);
  REQUIRE(storeRef[16] == 2);
  REQUIRE(storeRef[17] == 2);
  //
  REQUIRE(storeRef[18] == 2);
  REQUIRE(storeRef[19] == 3);
  REQUIRE(storeRef[20] == 3);
  REQUIRE(storeRef[21] == 2);
  REQUIRE(storeRef[22] == 3);
  REQUIRE(storeRef[23] == 3);
  REQUIRE(storeRef[24] == 2);
  REQUIRE(storeRef[25] == 3);
  REQUIRE(storeRef[26] == 3);
}

// Loop Greater Than: Loop
void CheckTest7Output(DataStructure& dataStructure)
{
  auto& storeRef = dataStructure.getDataRefAs<DataArray<int32>>(k_DataPath).getDataStoreRef();
  //
  REQUIRE(storeRef[0] == 1);
  REQUIRE(storeRef[1] == 2);
  REQUIRE(storeRef[2] == 1);
  REQUIRE(storeRef[3] == 1);
  REQUIRE(storeRef[4] == 2);
  REQUIRE(storeRef[5] == 1);
  REQUIRE(storeRef[6] == 1);
  REQUIRE(storeRef[7] == 1);
  REQUIRE(storeRef[8] == 1);
  //
  REQUIRE(storeRef[9] == 1);
  REQUIRE(storeRef[10] == 1);
  REQUIRE(storeRef[11] == 1);
  REQUIRE(storeRef[12] == 1);
  REQUIRE(storeRef[13] == 2);
  REQUIRE(storeRef[14] == 2);
  REQUIRE(storeRef[15] == 1);
  REQUIRE(storeRef[16] == 1);
  REQUIRE(storeRef[17] == 1);
  //
  REQUIRE(storeRef[18] == 1);
  REQUIRE(storeRef[19] == 1);
  REQUIRE(storeRef[20] == 1);
  REQUIRE(storeRef[21] == 1);
  REQUIRE(storeRef[22] == 1);
  REQUIRE(storeRef[23] == 1);
  REQUIRE(storeRef[24] == 1);
  REQUIRE(storeRef[25] == 1);
  REQUIRE(storeRef[26] == 1);
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

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: No Confidence: Less Than", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  // Require all values are outside of confidence bounds

  UnitTest::LoadPlugins();

  const float32 confidence = 0.99f;
  const bool loop = false;
  const uint64 comparison = 0; // Less than
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest1Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: No Confidence: Greater Than", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  // Require all values are outside of confidence bounds

  UnitTest::LoadPlugins();

  const float32 confidence = 0.001f;
  const bool loop = false;
  const uint64 comparison = 1; // Greater than
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest1Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: No Loop: Less Than", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  // Operate on values greater than confidence
  // Sort for smaller values

  UnitTest::LoadPlugins();

  const float32 confidence = 0.5f;
  const bool loop = false;
  const uint64 comparison = 0;
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest2Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: No Loop: Greater Than", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  // Operate on values less than confidence
  // Sort for larger values

  UnitTest::LoadPlugins();

  const float32 confidence = 0.5f;
  const bool loop = false;
  const uint64 comparison = 1; // Greater than
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest3Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: Loop: Less Than", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  // Operate on values greater than confidence
  // Sort for smaller values

  UnitTest::LoadPlugins();

  const float32 confidence = 0.5f;
  const bool loop = true;
  const uint64 comparison = 0;
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest4Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: Loop: Greater Than", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  // Operate on values less than confidence
  // Sort for larger values

  UnitTest::LoadPlugins();

  const float32 confidence = 0.5f;
  const bool loop = true;
  const uint64 comparison = 1;
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest5Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: Loop: Less Than v2", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  // Operate on values greater than confidence
  // Sort for smaller value

  UnitTest::LoadPlugins();

  const float32 confidence = 0.26f;
  const bool loop = true;
  const uint64 comparison = 0;
  DataStructure dataStructure = CreateTestData();
  RunFilter(dataStructure, comparison, confidence, loop);
  CheckTest6Output(dataStructure);
}

TEST_CASE("SimplnxCore::ReplaceElementAttributesWithNeighborValuesFilter: Loop: Greater Than v2", "[SimplnxCore][ReplaceElementAttributesWithNeighborValuesFilter]")
{
  // Operate on values less than confidence
  // Sort for larger value

  UnitTest::LoadPlugins();

  const float32 confidence = 0.26f;
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
