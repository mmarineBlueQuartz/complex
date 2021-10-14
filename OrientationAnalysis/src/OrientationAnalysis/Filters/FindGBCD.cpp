#include "FindGBCD.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"

using namespace complex;

namespace complex
{
std::string FindGBCD::name() const
{
  return FilterTraits<FindGBCD>::name.str();
}

Uuid FindGBCD::uuid() const
{
  return FilterTraits<FindGBCD>::uuid;
}

std::string FindGBCD::humanName() const
{
  return "Find GBCD";
}

Parameters FindGBCD::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<Float32Parameter>(k_GBCDRes_Key, "GBCD Spacing (Degrees)", "", 1.23345f));
  params.insertSeparator(Parameters::Separator{"Face Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_SurfaceMeshFaceLabelsArrayPath_Key, "Face Labels", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_SurfaceMeshFaceNormalsArrayPath_Key, "Face Normals", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_SurfaceMeshFaceAreasArrayPath_Key, "Face Areas", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Cell Feature Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeatureEulerAnglesArrayPath_Key, "Average Euler Angles", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeaturePhasesArrayPath_Key, "Phases", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Cell Ensemble Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_CrystalStructuresArrayPath_Key, "Crystal Structures", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Face Ensemble Data"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_FaceEnsembleAttributeMatrixName_Key, "Face Ensemble Attribute Matrix", "", DataPath{}));
  params.insert(std::make_unique<ArrayCreationParameter>(k_GBCDArrayName_Key, "GBCD", "", DataPath{}));

  return params;
}

IFilter::UniquePointer FindGBCD::clone() const
{
  return std::make_unique<FindGBCD>();
}

Result<OutputActions> FindGBCD::preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Write any preflight sanity checking codes in this function
   ***************************************************************************/
  auto pGBCDResValue = filterArgs.value<float32>(k_GBCDRes_Key);
  auto pSurfaceMeshFaceLabelsArrayPathValue = filterArgs.value<DataPath>(k_SurfaceMeshFaceLabelsArrayPath_Key);
  auto pSurfaceMeshFaceNormalsArrayPathValue = filterArgs.value<DataPath>(k_SurfaceMeshFaceNormalsArrayPath_Key);
  auto pSurfaceMeshFaceAreasArrayPathValue = filterArgs.value<DataPath>(k_SurfaceMeshFaceAreasArrayPath_Key);
  auto pFeatureEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_FeatureEulerAnglesArrayPath_Key);
  auto pFeaturePhasesArrayPathValue = filterArgs.value<DataPath>(k_FeaturePhasesArrayPath_Key);
  auto pCrystalStructuresArrayPathValue = filterArgs.value<DataPath>(k_CrystalStructuresArrayPath_Key);
  auto pFaceEnsembleAttributeMatrixNameValue = filterArgs.value<DataPath>(k_FaceEnsembleAttributeMatrixName_Key);
  auto pGBCDArrayNameValue = filterArgs.value<DataPath>(k_GBCDArrayName_Key);

  OutputActions actions;
#if 0
  // Define a custom class that generates the changes to the DataStructure.
  auto action = std::make_unique<FindGBCDAction>();
  actions.actions.push_back(std::move(action));
#endif
  return {std::move(actions)};
}

Result<> FindGBCD::executeImpl(DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pGBCDResValue = filterArgs.value<float32>(k_GBCDRes_Key);
  auto pSurfaceMeshFaceLabelsArrayPathValue = filterArgs.value<DataPath>(k_SurfaceMeshFaceLabelsArrayPath_Key);
  auto pSurfaceMeshFaceNormalsArrayPathValue = filterArgs.value<DataPath>(k_SurfaceMeshFaceNormalsArrayPath_Key);
  auto pSurfaceMeshFaceAreasArrayPathValue = filterArgs.value<DataPath>(k_SurfaceMeshFaceAreasArrayPath_Key);
  auto pFeatureEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_FeatureEulerAnglesArrayPath_Key);
  auto pFeaturePhasesArrayPathValue = filterArgs.value<DataPath>(k_FeaturePhasesArrayPath_Key);
  auto pCrystalStructuresArrayPathValue = filterArgs.value<DataPath>(k_CrystalStructuresArrayPath_Key);
  auto pFaceEnsembleAttributeMatrixNameValue = filterArgs.value<DataPath>(k_FaceEnsembleAttributeMatrixName_Key);
  auto pGBCDArrayNameValue = filterArgs.value<DataPath>(k_GBCDArrayName_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
