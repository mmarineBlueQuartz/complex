#pragma once

#include "simplnx/Common/Array.hpp"
#include "simplnx/Common/Result.hpp"
#include "simplnx/Common/Types.hpp"
#include "simplnx/DataStructure/DataPath.hpp"
#include "simplnx/DataStructure/DataStructure.hpp"
#include "simplnx/DataStructure/Geometry/ImageGeom.hpp"
#include "simplnx/DataStructure/IDataArray.hpp"
#include "simplnx/DataStructure/INeighborList.hpp"
#include "simplnx/DataStructure/StringArray.hpp"
#include "simplnx/Parameters/CropGeometryParameter.hpp"

#include "simplnx/simplnx_export.hpp"

namespace nx::core
{
namespace CropGeometry
{
SIMPLNX_EXPORT Result<> CropArray(DataStructure& dataStructure, const DataPath& geomPath, const DataPath& inputArrayPath, const DataPath& outputArrayPath,
                                  const CropGeometryParameter::ValueType& cropValues);

SIMPLNX_EXPORT IntVec3 ConvertPhysicalToVoxel(const ImageGeom& geom, const FloatVec3& physical);
SIMPLNX_EXPORT usize ConvertVoxelToTupleIndex(const SizeVec3& dimensions, const IntVec3& voxel);
SIMPLNX_EXPORT usize ConvertPhysicalToTupleIndex(const ImageGeom& geom, const FloatVec3& dimensions);

SIMPLNX_EXPORT Result<> CropVoxelDataArray(const ImageGeom& geom, const IDataArray& inputArray, IDataArray& outputArray, const std::array<bool, 3>& isBound, const IntVec3& minBounds,
                                       const IntVec3& maxBounds);
SIMPLNX_EXPORT Result<> CropVoxelNeighborList(const ImageGeom& geom, const INeighborList& inputArray, INeighborList& outputArray, const std::array<bool, 3>& isBound, const IntVec3& minBounds,
                                       const IntVec3& maxBounds);
SIMPLNX_EXPORT Result<> CropVoxelStringArray(const ImageGeom& geom, const StringArray& inputArray, StringArray& outputArray, const std::array<bool, 3>& isBound, const IntVec3& minBounds,
                                       const IntVec3& maxBounds);

namespace Generic
{
SIMPLNX_EXPORT Result<> CropVoxelArray(const ImageGeom& geom, const IArray& inputArray, IArray& outputArray, const std::array<bool, 3>& isBound, const IntVec3& minBounds, const IntVec3& maxBounds);
SIMPLNX_EXPORT Result<> CropPhysicalArray(const ImageGeom& geom, const IArray& inputArray, IArray& outputArray, const std::array<bool, 3>& isBound, const FloatVec3& minBounds, const FloatVec3& maxBounds);
} // namespace Generic
} // namespace CropGeometry
} // namespace nx::core
