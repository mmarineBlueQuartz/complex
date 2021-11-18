/**
 * This file is auto generated from the original Reconstruction/EBSDSegmentFeatures
 * runtime information. These are the steps that need to be taken to utilize this
 * unit test in the proper way.
 *
 * 1: Validate each of the default parameters that gets created.
 * 2: Inspect the actual filter to determine if the filter in its default state
 * would pass or fail BOTH the preflight() and execute() methods
 * 3: UPDATE the ```REQUIRE(result.result.valid());``` code to have the proper
 *
 * 4: Add additional unit tests to actually test each code path within the filter
 *
 * There are some example Catch2 ```TEST_CASE``` sections for your inspiration.
 *
 * NOTE the format of the ```TEST_CASE``` macro. Please stick to this format to
 * allow easier parsing of the unit tests.
 *
 * When you start working on this unit test remove "[EBSDSegmentFeatures][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"

#include "Reconstruction/Filters/EBSDSegmentFeatures.hpp"
#include "Reconstruction/Reconstruction_test_dirs.hpp"

using namespace complex;

TEST_CASE("Reconstruction::EBSDSegmentFeatures: Instantiation and Parameter Check", "[Reconstruction][EBSDSegmentFeatures][.][UNIMPLEMENTED][!mayfail]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  EBSDSegmentFeatures filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(EBSDSegmentFeatures::k_MisorientationTolerance_Key, std::make_any<float32>(1.23345f));
  args.insertOrAssign(EBSDSegmentFeatures::k_UseGoodVoxels_Key, std::make_any<bool>(false));
  args.insertOrAssign(EBSDSegmentFeatures::k_QuatsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(EBSDSegmentFeatures::k_CellPhasesArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(EBSDSegmentFeatures::k_GoodVoxelsArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(EBSDSegmentFeatures::k_CrystalStructuresArrayPath_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(EBSDSegmentFeatures::k_FeatureIdsArrayName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(EBSDSegmentFeatures::k_CellFeatureAttributeMatrixName_Key, std::make_any<DataPath>(DataPath{}));
  args.insertOrAssign(EBSDSegmentFeatures::k_ActiveArrayName_Key, std::make_any<DataPath>(DataPath{}));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("Reconstruction::EBSDSegmentFeatures: Valid filter execution")
//{
//
//}

// TEST_CASE("Reconstruction::EBSDSegmentFeatures: InValid filter execution")
//{
//
//}
