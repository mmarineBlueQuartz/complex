/**
 * This file is auto generated from the original ITKImageProcessing/ITKPCMTileRegistration
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
 * When you start working on this unit test remove "[ITKPCMTileRegistration][.][UNIMPLEMENTED]"
 * from the TEST_CASE macro. This will enable this unit test to be run by default
 * and report errors.
 */

#include <catch2/catch.hpp>

#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

#include "ITKImageProcessing/Filters/ITKPCMTileRegistration.hpp"
#include "ITKImageProcessing/ITKImageProcessing_test_dirs.hpp"

using namespace complex;

TEST_CASE("ITKImageProcessing::ITKPCMTileRegistration: Instantiation and Parameter Check", "[ITKImageProcessing][ITKPCMTileRegistration][.][UNIMPLEMENTED][!mayfail]")
{
  // Instantiate the filter, a DataStructure object and an Arguments Object
  ITKPCMTileRegistration filter;
  DataStructure ds;
  Arguments args;

  // Create default Parameters for the filter.
  args.insertOrAssign(ITKPCMTileRegistration::k_ColumnMontageLimits_Key, std::make_any<VectorInt32Parameter::ValueType>(std::vector<int32>(2)));
  args.insertOrAssign(ITKPCMTileRegistration::k_RowMontageLimits_Key, std::make_any<VectorInt32Parameter::ValueType>(std::vector<int32>(2)));
  args.insertOrAssign(ITKPCMTileRegistration::k_DataContainerPaddingDigits_Key, std::make_any<int32>(1234356));
  args.insertOrAssign(ITKPCMTileRegistration::k_DataContainerPrefix_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ITKPCMTileRegistration::k_CommonAttributeMatrixName_Key, std::make_any<StringParameter::ValueType>("SomeString"));
  args.insertOrAssign(ITKPCMTileRegistration::k_CommonDataArrayName_Key, std::make_any<StringParameter::ValueType>("SomeString"));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(ds, args);
  REQUIRE(preflightResult.outputActions.valid());

  // Execute the filter and check the result
  auto executeResult = filter.execute(ds, args);
  REQUIRE(executeResult.result.valid());
}

// TEST_CASE("ITKImageProcessing::ITKPCMTileRegistration: Valid filter execution")
//{
//
//}

// TEST_CASE("ITKImageProcessing::ITKPCMTileRegistration: InValid filter execution")
//{
//
//}
