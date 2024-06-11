#include "SimplnxCore/Filters/InterpolatePointCloudToRegularGridFilter.hpp"
#include "SimplnxCore/SimplnxCore_test_dirs.hpp"

#include "simplnx/UnitTest/UnitTestCommon.hpp"

#include <catch2/catch.hpp>

#include <xtensor/xio.hpp>

#include <string>

namespace fs = std::filesystem;
using namespace nx::core;
using namespace nx::core::Constants;

namespace
{
const std::string k_UniformInterpolatedData = "UniformInterpolatedData";
const std::string k_GaussianInterpolatedData = "GaussianInterpolatedData";
const std::string k_Computed = "[Computed]";
const std::string k_KernalDistances = "KernelDistances";

const DataPath k_ImageGeomPath({k_ImageGeometry});
const DataPath k_VertexGeometryPath({k_PointCloudContainerName});
const DataPath k_VertexDataPath = k_VertexGeometryPath.createChildPath(k_VertexData);
const DataPath k_MaskPath = k_VertexDataPath.createChildPath(k_Mask);
const DataPath k_FaceAreasPath = k_VertexDataPath.createChildPath(k_FaceAreas);
const DataPath k_VoxelIndicesPath = k_VertexDataPath.createChildPath(k_VoxelIndices);

const DataPath k_UniformInterpolatedDataExemplar = k_ImageGeomPath.createChildPath(k_UniformInterpolatedData);
const DataPath k_UniformInterpolatedDataComputed = k_ImageGeomPath.createChildPath(k_UniformInterpolatedData + k_Computed);
const DataPath k_GaussianInterpolatedDataExemplar = k_ImageGeomPath.createChildPath(k_GaussianInterpolatedData);
const DataPath k_GaussianInterpolatedDataComputed = k_ImageGeomPath.createChildPath(k_GaussianInterpolatedData + k_Computed);

const DataPath k_UniformFaceAreasExemplar = k_UniformInterpolatedDataExemplar.createChildPath(k_FaceAreas);
const DataPath k_UniformVoxelIndicesExemplar = k_UniformInterpolatedDataExemplar.createChildPath(k_VoxelIndices);
const DataPath k_UniformKernalDistancesExemplar = k_UniformInterpolatedDataExemplar.createChildPath(k_KernalDistances);
const DataPath k_UniformFaceAreasComputed = k_UniformInterpolatedDataComputed.createChildPath(k_FaceAreas);
const DataPath k_UniformVoxelIndicesComputed = k_UniformInterpolatedDataComputed.createChildPath(k_VoxelIndices);
const DataPath k_UniformKernalDistancesComputed = k_UniformInterpolatedDataComputed.createChildPath(k_KernalDistances);

const DataPath k_GaussianFaceAreasExemplar = k_GaussianInterpolatedDataExemplar.createChildPath(k_FaceAreas);
const DataPath k_GaussianVoxelIndicesExemplar = k_GaussianInterpolatedDataExemplar.createChildPath(k_VoxelIndices);
const DataPath k_GaussianKernalDistancesExemplar = k_GaussianInterpolatedDataExemplar.createChildPath(k_KernalDistances);
const DataPath k_GaussianFaceAreasComputed = k_GaussianInterpolatedDataComputed.createChildPath(k_FaceAreas);
const DataPath k_GaussianVoxelIndicesComputed = k_GaussianInterpolatedDataComputed.createChildPath(k_VoxelIndices);
const DataPath k_GaussianKernalDistancesComputed = k_GaussianInterpolatedDataComputed.createChildPath(k_KernalDistances);
} // namespace

TEST_CASE("SimplnxCore::InterpolatePointCloudToRegularGridFilter: Valid Filter Execution - Uniform Inpterpolation with Mask", "[SimplnxCore][InterpolatePointCloudToRegularGridFilter]")
{
  const nx::core::UnitTest::TestFileSentinel testDataSentinel(nx::core::unit_test::k_CMakeExecutable, nx::core::unit_test::k_TestFilesDir, "6_6_interpolate_point_cloud_to_regular_grid.tar.gz",
                                                              "6_6_interpolate_point_cloud_to_regular_grid");

  // Read Exemplar DREAM3D File Filter
  auto exemplarFilePath = fs::path(fmt::format("{}/6_6_interpolate_point_cloud_to_regular_grid/6_6_interpolate_point_cloud_to_regular_grid.dream3d", unit_test::k_TestFilesDir));
  DataStructure dataStructure = UnitTest::LoadDataStructure(exemplarFilePath);

  InterpolatePointCloudToRegularGridFilter filter;
  Arguments args;

  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_UseMask_Key, std::make_any<bool>(true));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_StoreKernelDistances_Key, std::make_any<bool>(true));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InterpolationTechnique_Key, std::make_any<uint64>(InterpolatePointCloudToRegularGridFilter::k_Uniform));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_KernelSize_Key, std::make_any<std::vector<float32>>(std::vector<float32>{1, 1, 1}));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_SelectedVertexGeometryPath_Key, std::make_any<DataPath>(k_VertexGeometryPath));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_SelectedImageGeometryPath_Key, std::make_any<DataPath>(k_ImageGeomPath));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_VoxelIndicesPath_Key, std::make_any<DataPath>(k_VoxelIndicesPath));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InputMaskPath_Key, std::make_any<DataPath>(k_MaskPath));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InterpolateArrays_Key, std::make_any<std::vector<DataPath>>(std::vector<DataPath>{k_FaceAreasPath}));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_CopyArrays_Key, std::make_any<std::vector<DataPath>>(std::vector<DataPath>{k_VoxelIndicesPath}));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InterpolatedGroupName_Key, std::make_any<std::string>(k_UniformInterpolatedDataComputed.getTargetName()));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_KernelDistancesArrayName_Key, std::make_any<std::string>(k_UniformKernalDistancesComputed.getTargetName()));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(dataStructure, args);
  SIMPLNX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

  // Execute the filter and check the result
  auto executeResult = filter.execute(dataStructure, args);
  SIMPLNX_RESULT_REQUIRE_VALID(executeResult.result)

  UnitTest::CompareNeighborLists<float64>(dataStructure, k_UniformFaceAreasExemplar, k_UniformFaceAreasComputed);
  UnitTest::CompareNeighborLists<uint64>(dataStructure, k_UniformVoxelIndicesExemplar, k_UniformVoxelIndicesComputed);
  UnitTest::CompareNeighborLists<float32>(dataStructure, k_UniformKernalDistancesExemplar, k_UniformKernalDistancesComputed);
}

TEST_CASE("SimplnxCore::InterpolatePointCloudToRegularGridFilter: Valid Filter Execution - Gaussian Inpterpolation", "[SimplnxCore][InterpolatePointCloudToRegularGridFilter]")
{
  const nx::core::UnitTest::TestFileSentinel testDataSentinel(nx::core::unit_test::k_CMakeExecutable, nx::core::unit_test::k_TestFilesDir, "6_6_interpolate_point_cloud_to_regular_grid.tar.gz",
                                                              "6_6_interpolate_point_cloud_to_regular_grid");

  // Read Exemplar DREAM3D File Filter
  auto exemplarFilePath = fs::path(fmt::format("{}/6_6_interpolate_point_cloud_to_regular_grid/6_6_interpolate_point_cloud_to_regular_grid.dream3d", unit_test::k_TestFilesDir));
  DataStructure dataStructure = UnitTest::LoadDataStructure(exemplarFilePath);

  InterpolatePointCloudToRegularGridFilter filter;
  Arguments args;

  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_UseMask_Key, std::make_any<bool>(false));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_StoreKernelDistances_Key, std::make_any<bool>(true));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InterpolationTechnique_Key, std::make_any<uint64>(InterpolatePointCloudToRegularGridFilter::k_Gaussian));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_KernelSize_Key, std::make_any<std::vector<float32>>(std::vector<float32>{1, 1, 1}));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_GaussianSigmas_Key, std::make_any<std::vector<float32>>(std::vector<float32>{1, 1, 1}));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_SelectedVertexGeometryPath_Key, std::make_any<DataPath>(k_VertexGeometryPath));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_SelectedImageGeometryPath_Key, std::make_any<DataPath>(k_ImageGeomPath));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_VoxelIndicesPath_Key, std::make_any<DataPath>(k_VoxelIndicesPath));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InterpolateArrays_Key, std::make_any<std::vector<DataPath>>(std::vector<DataPath>{k_FaceAreasPath}));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_CopyArrays_Key, std::make_any<std::vector<DataPath>>(std::vector<DataPath>{k_VoxelIndicesPath}));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InterpolatedGroupName_Key, std::make_any<std::string>(k_GaussianInterpolatedDataComputed.getTargetName()));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_KernelDistancesArrayName_Key, std::make_any<std::string>(k_GaussianKernalDistancesComputed.getTargetName()));

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(dataStructure, args);
  SIMPLNX_RESULT_REQUIRE_VALID(preflightResult.outputActions)

  // Execute the filter and check the result
  auto executeResult = filter.execute(dataStructure, args);
  SIMPLNX_RESULT_REQUIRE_VALID(executeResult.result)

    const auto& neighborList = dataStructure.getDataRefAs<NeighborList<float64>>(k_GaussianFaceAreasComputed);
  std::cout << "InterpolatePointCloudToRegularGridFilter: " << neighborList.getStore()->xarray() << std::endl;

  UnitTest::CompareNeighborLists<float64>(dataStructure, k_GaussianFaceAreasExemplar, k_GaussianFaceAreasComputed);
  UnitTest::CompareNeighborLists<uint64>(dataStructure, k_GaussianVoxelIndicesExemplar, k_GaussianVoxelIndicesComputed);
  UnitTest::CompareNeighborLists<float32>(dataStructure, k_GaussianKernalDistancesExemplar, k_GaussianKernalDistancesComputed);
}

TEST_CASE("SimplnxCore::InterpolatePointCloudToRegularGridFilter: Invalid Filter Execution", "[SimplnxCore][InterpolatePointCloudToRegularGridFilter]")
{
  const nx::core::UnitTest::TestFileSentinel testDataSentinel(nx::core::unit_test::k_CMakeExecutable, nx::core::unit_test::k_TestFilesDir, "6_6_interpolate_point_cloud_to_regular_grid.tar.gz",
                                                              "6_6_interpolate_point_cloud_to_regular_grid");

  // Read Exemplar DREAM3D File Filter
  auto exemplarFilePath = fs::path(fmt::format("{}/6_6_interpolate_point_cloud_to_regular_grid/6_6_interpolate_point_cloud_to_regular_grid.dream3d", unit_test::k_TestFilesDir));
  DataStructure dataStructure = UnitTest::LoadDataStructure(exemplarFilePath);

  InterpolatePointCloudToRegularGridFilter filter;
  Arguments args;

  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_UseMask_Key, std::make_any<bool>(false));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_StoreKernelDistances_Key, std::make_any<bool>(true));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InterpolationTechnique_Key, std::make_any<uint64>(InterpolatePointCloudToRegularGridFilter::k_Gaussian));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_SelectedVertexGeometryPath_Key, std::make_any<DataPath>(k_VertexGeometryPath));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_SelectedImageGeometryPath_Key, std::make_any<DataPath>(k_ImageGeomPath));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_VoxelIndicesPath_Key, std::make_any<DataPath>(k_VoxelIndicesPath));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_CopyArrays_Key, std::make_any<std::vector<DataPath>>(std::vector<DataPath>{k_VoxelIndicesPath}));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InterpolatedGroupName_Key, std::make_any<std::string>(k_GaussianInterpolatedDataComputed.getTargetName()));
  args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_KernelDistancesArrayName_Key, std::make_any<std::string>(k_GaussianKernalDistancesComputed.getTargetName()));

  SECTION("Invalid Kernel Size")
  {
    args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_KernelSize_Key, std::make_any<std::vector<float32>>(std::vector<float32>{-1, 1, 1}));
    args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_GaussianSigmas_Key, std::make_any<std::vector<float32>>(std::vector<float32>{1, 1, 1}));
    args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InterpolateArrays_Key, std::make_any<std::vector<DataPath>>(std::vector<DataPath>{k_FaceAreasPath}));
  }
  SECTION("Invalid Gaussian Sigma")
  {
    args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_KernelSize_Key, std::make_any<std::vector<float32>>(std::vector<float32>{1, 1, 1}));
    args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_GaussianSigmas_Key, std::make_any<std::vector<float32>>(std::vector<float32>{0, 0, 0}));
    args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InterpolateArrays_Key, std::make_any<std::vector<DataPath>>(std::vector<DataPath>{k_FaceAreasPath}));
  }
  SECTION("Mismatching Input Array Tuples")
  {
    args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_KernelSize_Key, std::make_any<std::vector<float32>>(std::vector<float32>{1, 1, 1}));
    args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_GaussianSigmas_Key, std::make_any<std::vector<float32>>(std::vector<float32>{1, 1, 1}));
    args.insertOrAssign(InterpolatePointCloudToRegularGridFilter::k_InterpolateArrays_Key, std::make_any<std::vector<DataPath>>(std::vector<DataPath>{k_GaussianFaceAreasExemplar}));
  }

  // Preflight the filter and check result
  auto preflightResult = filter.preflight(dataStructure, args);
  SIMPLNX_RESULT_REQUIRE_INVALID(preflightResult.outputActions)

  // Execute the filter and check the result
  auto executeResult = filter.execute(dataStructure, args);
  SIMPLNX_RESULT_REQUIRE_INVALID(executeResult.result)
}
