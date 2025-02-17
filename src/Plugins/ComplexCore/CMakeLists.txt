include("${complex_SOURCE_DIR}/cmake/Plugin.cmake")

set(PLUGIN_NAME "ComplexCore")
set(${PLUGIN_NAME}_SOURCE_DIR ${complex_SOURCE_DIR}/src/Plugins/${PLUGIN_NAME})

# ------------------------------------------------------------------------------
# These are all the filters in the plugin. All filters should be kept in the
# PLUGIN_NAME/src/PLUGIN_NAME/Filters/ directory.
set(FilterList
  AlignGeometries
  ApproximatePointCloudHull
  ApplyTransformationToGeometryFilter
  CalculateFeatureSizesFilter
  CalculateTriangleAreasFilter
  CreateFeatureArrayFromElementArray
  ChangeAngleRepresentation
  ConditionalSetValue
  CopyDataGroup
  CopyFeatureArrayToElementArray
  CreateDataArray
  CreateDataGroup
  CreateImageGeometry
  CropImageGeometry
  CropVertexGeometry
  DeleteData
  ExtractInternalSurfacesFromTriangleGeometry
  ExportDREAM3DFilter
  FindDifferencesMap
  FindFeaturePhasesFilter
  FindNeighborListStatistics
  FindNeighbors
  FindSurfaceFeatures
  IdentifySample
  ImportDREAM3DFilter
  ImportHDF5Dataset
  ImportTextFilter
  InitializeData
  InterpolatePointCloudToRegularGridFilter
  IterativeClosestPointFilter
  LaplacianSmoothingFilter
  LinkGeometryDataFilter
  MapPointCloudToRegularGridFilter
  MinNeighbors
  MoveData
  MultiThresholdObjects
  PointSampleTriangleGeometryFilter
  QuickSurfaceMeshFilter
  RawBinaryReaderFilter
  ImportCSVDataFilter
  RemoveFlaggedVertices
  RemoveMinimumSizeFeaturesFilter
  RenameDataObject
  RobustAutomaticThreshold
  ScalarSegmentFeaturesFilter
  SetImageGeomOriginScalingFilter
  StlFileReaderFilter
  FindArrayStatisticsFilter
  TriangleNormalFilter
  TriangleDihedralAngleFilter
)

set(ActionList
)

set(AlgorithmList
  ApplyTransformationToGeometry
  StlFileReader
  LaplacianSmoothing
  QuickSurfaceMesh
  PointSampleTriangleGeometry
  TupleTransfer
  RawBinaryReader
  ScalarSegmentFeatures
  FindArrayStatistics
)

create_complex_plugin(NAME ${PLUGIN_NAME}
                      FILTER_LIST ${FilterList}
                      ACTION_LIST ${ActionList}
                      ALGORITHM_LIST ${AlgorithmList}
                      DESCRIPTION "Plugin for ${PLUGIN_NAME}"
                      VERSION "0.1.0"
)

#------------------------------------------------------------------------------
# Add in the unit tests for the filters included in this plugin
enable_testing()
add_subdirectory(${${PLUGIN_NAME}_SOURCE_DIR}/test)

#------------------------------------------------------------------------------
# If there are additional library packages that need to be found, there is where
# to do that using the usual find_package(....) call
# find_package(....)


#------------------------------------------------------------------------------
# If there are additional libraries that this plugin needs to link against you
# can use the target_link_libraries() cmake call
# target_link_libraries(${PLUGIN_NAME} PUBLIC [name of library])

#------------------------------------------------------------------------------
# If there are additional source files that need to be compiled for this plugin
# can use the target_sources(...) cmake call

set(PLUGIN_EXTRA_SOURCES
  ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/utils/CSVDataParser.hpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/utils/nanoflann.hpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/utils/StlUtilities.hpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}/utils/StlUtilities.cpp
)

target_sources(${PLUGIN_NAME}
  PRIVATE
    ${PLUGIN_EXTRA_SOURCES}
)

source_group(TREE "${${PLUGIN_NAME}_SOURCE_DIR}/src/${PLUGIN_NAME}" PREFIX ${PLUGIN_NAME}
  FILES
    ${PLUGIN_EXTRA_SOURCES}
)

#------------------------------------------------------------------------------
# If there are additional include directories that are needed for this plugin
# you can use the target_include_directories(.....) cmake call
# target_include_directories(${PLUGIN_NAME}
#     PUBLIC
#     additional include directories here
# )
