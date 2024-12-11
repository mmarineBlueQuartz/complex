/*
# Test Plan

Input Files:
DREAM3D_Data/TestFiles/ASCIIData/EulerAngles.csv
DREAM3D_Data/TestFiles/ASCIIData/Phases.csv

Output DataArrays:
IPFColors (3 component UInt8 Array)

Comparison Files:
DREAM3D_Data/TestFiles/ASCIIData/IPFColor.csv

You will need to create a UInt32 DataArray with 2 values in it: [ 999, 1 ]. This will
be the input 'k_CrystalStructuresArrayPath_Key' path and data.


Compare the data sets. The values should be exactly the same.

*/
#include "OrientationAnalysis/Filters/ComputeIPFColorsFilter.hpp"
#include "OrientationAnalysis/OrientationAnalysis_test_dirs.hpp"

#include "simplnx/Parameters/VectorParameter.hpp"
#include "simplnx/UnitTest/UnitTestCommon.hpp"
#include "simplnx/Utilities/Parsing/DREAM3D/Dream3dIO.hpp"
#include "simplnx/Utilities/Parsing/HDF5/IO/FileIO.hpp"

#include <catch2/catch.hpp>

#include <cstdio>
#include <filesystem>

namespace fs = std::filesystem;
using namespace nx::core;
using namespace nx::core::UnitTest;
using namespace nx::core::Constants;

namespace nx::core::Constants
{
constexpr StringLiteral k_ImageDataContainer("ImageDataContainer");
constexpr StringLiteral k_OutputIPFColors("IPF Colors_Test_Output");
} // namespace nx::core::Constants

TEST_CASE("OrientationAnalysis::ComputeIPFColors", "[OrientationAnalysis][ComputeIPFColors]")
{
  UnitTest::LoadPlugins();

  const nx::core::UnitTest::TestFileSentinel testDataSentinel(nx::core::unit_test::k_CMakeExecutable, nx::core::unit_test::k_TestFilesDir, "so3_cubic_high_ipf_001.tar.gz",
                                                              "so3_cubic_high_ipf_001.dream3d");

  DataStructure dataStructure;
  {

    // This test file was produced by SIMPL/DREAM3D. our results should match theirs
    auto exemplarFilePath = fs::path(fmt::format("{}/so3_cubic_high_ipf_001.dream3d", unit_test::k_TestFilesDir));
    REQUIRE(fs::exists(exemplarFilePath));
    auto result = DREAM3D::ImportDataStructureFromFile(exemplarFilePath);
    REQUIRE(result.valid());
    dataStructure = result.value();
  }

  // Instantiate the filter, a DataStructure object and an Arguments Object
  {
    ComputeIPFColorsFilter filter;
    Arguments args;

    DataPath cellEulerAnglesPath({Constants::k_ImageDataContainer, Constants::k_CellData, Constants::k_EulerAngles});
    DataPath cellPhasesArrayPath({Constants::k_ImageDataContainer, Constants::k_CellData, Constants::k_Phases});
    DataPath goodVoxelsPath({Constants::k_ImageDataContainer, Constants::k_CellData, Constants::k_Mask});
    DataPath crystalStructuresArrayPath({Constants::k_ImageDataContainer, Constants::k_CellEnsembleData, Constants::k_CrystalStructures});
    DataPath cellIPFColorsArrayName({Constants::k_ImageDataContainer, Constants::k_CellData, Constants::k_OutputIPFColors});

    // Create default Parameters for the filter.
    args.insertOrAssign(ComputeIPFColorsFilter::k_ReferenceDir_Key, std::make_any<VectorFloat32Parameter::ValueType>({0.0F, 0.0F, 1.0F}));
    args.insertOrAssign(ComputeIPFColorsFilter::k_UseMask_Key, std::make_any<bool>(true));
    args.insertOrAssign(ComputeIPFColorsFilter::k_CellEulerAnglesArrayPath_Key, std::make_any<DataPath>(cellEulerAnglesPath));
    args.insertOrAssign(ComputeIPFColorsFilter::k_CellPhasesArrayPath_Key, std::make_any<DataPath>(cellPhasesArrayPath));
    args.insertOrAssign(ComputeIPFColorsFilter::k_MaskArrayPath_Key, std::make_any<DataPath>(goodVoxelsPath));
    args.insertOrAssign(ComputeIPFColorsFilter::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(crystalStructuresArrayPath));
    args.insertOrAssign(ComputeIPFColorsFilter::k_CellIPFColorsArrayName_Key, std::make_any<std::string>(Constants::k_OutputIPFColors));

    REQUIRE(dataStructure.getData(goodVoxelsPath) != nullptr);
    REQUIRE(dataStructure.getData(cellEulerAnglesPath) != nullptr);
    REQUIRE(dataStructure.getData(cellPhasesArrayPath) != nullptr);

    // Preflight the filter and check result
    auto preflightResult = filter.preflight(dataStructure, args);
    SIMPLNX_RESULT_REQUIRE_VALID(preflightResult.outputActions);

    // Execute the filter and check the result
    auto executeResult = filter.execute(dataStructure, args);
    SIMPLNX_RESULT_REQUIRE_VALID(executeResult.result);
    {
      // Write out the DataStructure for later viewing/debugging
      auto fileWriter = nx::core::HDF5::FileIO::WriteFile(std::filesystem::path(fmt::format("{}/ComputeIPFColors_Test.dream3d", unit_test::k_BinaryTestOutputDir)));
      auto resultH5 = HDF5::DataStructureWriter::WriteFile(dataStructure, fileWriter);
      SIMPLNX_RESULT_REQUIRE_VALID(resultH5);
    }

    DataPath ipfColors({Constants::k_ImageDataContainer, Constants::k_CellData, Constants::k_Ipf_Colors});

    UInt8Array& exemplar = dataStructure.getDataRefAs<UInt8Array>(ipfColors);
    UInt8Array& output = dataStructure.getDataRefAs<UInt8Array>(cellIPFColorsArrayName);

    size_t totalElements = exemplar.getSize();
    bool valid = true;
    for(size_t i = 0; i < totalElements; i++)
    {
      if(exemplar[i] != output[i])
      {
        valid = false;
        break;
      }
    }
    REQUIRE(valid == true);
  }
}
