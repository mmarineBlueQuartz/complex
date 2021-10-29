#include "InsertTransformationPhases.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/DataGroupSelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string InsertTransformationPhases::name() const
{
  return FilterTraits<InsertTransformationPhases>::name.str();
}

//------------------------------------------------------------------------------
std::string InsertTransformationPhases::className() const
{
  return FilterTraits<InsertTransformationPhases>::className;
}

//------------------------------------------------------------------------------
Uuid InsertTransformationPhases::uuid() const
{
  return FilterTraits<InsertTransformationPhases>::uuid;
}

//------------------------------------------------------------------------------
std::string InsertTransformationPhases::humanName() const
{
  return "Insert Transformation Phases";
}

//------------------------------------------------------------------------------
std::vector<std::string> InsertTransformationPhases::defaultTags() const
{
  return {"#Unsupported", "#Packing"};
}

//------------------------------------------------------------------------------
Parameters InsertTransformationPhases::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<Int32Parameter>(k_ParentPhase_Key, "Parent Phase", "", 1234356));
  params.insert(std::make_unique<ChoicesParameter>(k_TransCrystalStruct_Key, "Transformation Phase Crystal Structure", "", 0, ChoicesParameter::Choices{"Option 1", "Option 2", "Option 3"}));
  params.insert(std::make_unique<Float32Parameter>(k_TransformationPhaseMisorientation_Key, "Transformation Phase Misorientation", "", 1.23345f));
  params.insertLinkableParameter(std::make_unique<BoolParameter>(k_DefineHabitPlane_Key, "Define Habit Plane", "", false));
  params.insert(std::make_unique<VectorFloat32Parameter>(k_TransformationPhaseHabitPlane_Key, "Transformation Phase Habit Plane", "", std::vector<float32>(3), std::vector<std::string>(3)));
  params.insertLinkableParameter(std::make_unique<BoolParameter>(k_UseAllVariants_Key, "Use All Variants", "", false));
  params.insert(std::make_unique<Float32Parameter>(k_CoherentFrac_Key, "Coherent Fraction", "", 1.23345f));
  params.insert(std::make_unique<Float32Parameter>(k_TransformationPhaseThickness_Key, "Transformation Phase Thickness", "", 1.23345f));
  params.insert(std::make_unique<Int32Parameter>(k_NumTransformationPhasesPerFeature_Key, "Average Number Of Transformation Phases Per Feature", "", 1234356));
  params.insert(std::make_unique<Float32Parameter>(k_PeninsulaFrac_Key, "Peninsula Transformation Phase Fraction", "", 1.23345f));
  params.insertSeparator(Parameters::Separator{"Cell Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeatureIdsArrayPath_Key, "Feature Ids", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_CellEulerAnglesArrayPath_Key, "Euler Angles", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_CellPhasesArrayPath_Key, "Phases", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Cell Feature Data"});
  params.insert(std::make_unique<DataGroupSelectionParameter>(k_CellFeatureAttributeMatrixName_Key, "Cell Feature Attribute Matrix", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeatureEulerAnglesArrayPath_Key, "Average Euler Angles", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_AvgQuatsArrayPath_Key, "Average Quaternions", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_CentroidsArrayPath_Key, "Centroids", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_EquivalentDiametersArrayPath_Key, "Equivalent Diameters", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeaturePhasesArrayPath_Key, "Phases", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Cell Ensemble Data"});
  params.insert(std::make_unique<DataGroupSelectionParameter>(k_StatsGenCellEnsembleAttributeMatrixPath_Key, "Cell Ensemble Attribute Matrix", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_CrystalStructuresArrayPath_Key, "Crystal Structures", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_PhaseTypesArrayPath_Key, "Phase Types", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_ShapeTypesArrayPath_Key, "Shape Types", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_NumFeaturesArrayPath_Key, "Number of Features", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Cell Feature Data"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_FeatureParentIdsArrayName_Key, "Parent Ids", "", DataPath{}));
  params.insert(std::make_unique<ArrayCreationParameter>(k_NumFeaturesPerParentArrayPath_Key, "Number of Features Per Parent", "", DataPath{}));
  // Associate the Linkable Parameter(s) to the children parameters that they control
  params.linkParameters(k_DefineHabitPlane_Key, k_TransformationPhaseHabitPlane_Key, true);
  params.linkParameters(k_DefineHabitPlane_Key, k_UseAllVariants_Key, true);
  params.linkParameters(k_UseAllVariants_Key, k_CoherentFrac_Key, true);

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer InsertTransformationPhases::clone() const
{
  return std::make_unique<InsertTransformationPhases>();
}

//------------------------------------------------------------------------------
Result<OutputActions> InsertTransformationPhases::preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Write any preflight sanity checking codes in this function
   ***************************************************************************/
  auto pParentPhaseValue = filterArgs.value<int32>(k_ParentPhase_Key);
  auto pTransCrystalStructValue = filterArgs.value<ChoicesParameter::ValueType>(k_TransCrystalStruct_Key);
  auto pTransformationPhaseMisorientationValue = filterArgs.value<float32>(k_TransformationPhaseMisorientation_Key);
  auto pDefineHabitPlaneValue = filterArgs.value<bool>(k_DefineHabitPlane_Key);
  auto pTransformationPhaseHabitPlaneValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_TransformationPhaseHabitPlane_Key);
  auto pUseAllVariantsValue = filterArgs.value<bool>(k_UseAllVariants_Key);
  auto pCoherentFracValue = filterArgs.value<float32>(k_CoherentFrac_Key);
  auto pTransformationPhaseThicknessValue = filterArgs.value<float32>(k_TransformationPhaseThickness_Key);
  auto pNumTransformationPhasesPerFeatureValue = filterArgs.value<int32>(k_NumTransformationPhasesPerFeature_Key);
  auto pPeninsulaFracValue = filterArgs.value<float32>(k_PeninsulaFrac_Key);
  auto pFeatureIdsArrayPathValue = filterArgs.value<DataPath>(k_FeatureIdsArrayPath_Key);
  auto pCellEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_CellEulerAnglesArrayPath_Key);
  auto pCellPhasesArrayPathValue = filterArgs.value<DataPath>(k_CellPhasesArrayPath_Key);
  auto pCellFeatureAttributeMatrixNameValue = filterArgs.value<DataPath>(k_CellFeatureAttributeMatrixName_Key);
  auto pFeatureEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_FeatureEulerAnglesArrayPath_Key);
  auto pAvgQuatsArrayPathValue = filterArgs.value<DataPath>(k_AvgQuatsArrayPath_Key);
  auto pCentroidsArrayPathValue = filterArgs.value<DataPath>(k_CentroidsArrayPath_Key);
  auto pEquivalentDiametersArrayPathValue = filterArgs.value<DataPath>(k_EquivalentDiametersArrayPath_Key);
  auto pFeaturePhasesArrayPathValue = filterArgs.value<DataPath>(k_FeaturePhasesArrayPath_Key);
  auto pStatsGenCellEnsembleAttributeMatrixPathValue = filterArgs.value<DataPath>(k_StatsGenCellEnsembleAttributeMatrixPath_Key);
  auto pCrystalStructuresArrayPathValue = filterArgs.value<DataPath>(k_CrystalStructuresArrayPath_Key);
  auto pPhaseTypesArrayPathValue = filterArgs.value<DataPath>(k_PhaseTypesArrayPath_Key);
  auto pShapeTypesArrayPathValue = filterArgs.value<DataPath>(k_ShapeTypesArrayPath_Key);
  auto pNumFeaturesArrayPathValue = filterArgs.value<DataPath>(k_NumFeaturesArrayPath_Key);
  auto pFeatureParentIdsArrayNameValue = filterArgs.value<DataPath>(k_FeatureParentIdsArrayName_Key);
  auto pNumFeaturesPerParentArrayPathValue = filterArgs.value<DataPath>(k_NumFeaturesPerParentArrayPath_Key);

  OutputActions actions;
#if 0
  // Define a custom class that generates the changes to the DataStructure.
  auto action = std::make_unique<InsertTransformationPhasesAction>();
  actions.actions.push_back(std::move(action));
#endif
  return {std::move(actions)};
}

//------------------------------------------------------------------------------
Result<> InsertTransformationPhases::executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pParentPhaseValue = filterArgs.value<int32>(k_ParentPhase_Key);
  auto pTransCrystalStructValue = filterArgs.value<ChoicesParameter::ValueType>(k_TransCrystalStruct_Key);
  auto pTransformationPhaseMisorientationValue = filterArgs.value<float32>(k_TransformationPhaseMisorientation_Key);
  auto pDefineHabitPlaneValue = filterArgs.value<bool>(k_DefineHabitPlane_Key);
  auto pTransformationPhaseHabitPlaneValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_TransformationPhaseHabitPlane_Key);
  auto pUseAllVariantsValue = filterArgs.value<bool>(k_UseAllVariants_Key);
  auto pCoherentFracValue = filterArgs.value<float32>(k_CoherentFrac_Key);
  auto pTransformationPhaseThicknessValue = filterArgs.value<float32>(k_TransformationPhaseThickness_Key);
  auto pNumTransformationPhasesPerFeatureValue = filterArgs.value<int32>(k_NumTransformationPhasesPerFeature_Key);
  auto pPeninsulaFracValue = filterArgs.value<float32>(k_PeninsulaFrac_Key);
  auto pFeatureIdsArrayPathValue = filterArgs.value<DataPath>(k_FeatureIdsArrayPath_Key);
  auto pCellEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_CellEulerAnglesArrayPath_Key);
  auto pCellPhasesArrayPathValue = filterArgs.value<DataPath>(k_CellPhasesArrayPath_Key);
  auto pCellFeatureAttributeMatrixNameValue = filterArgs.value<DataPath>(k_CellFeatureAttributeMatrixName_Key);
  auto pFeatureEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_FeatureEulerAnglesArrayPath_Key);
  auto pAvgQuatsArrayPathValue = filterArgs.value<DataPath>(k_AvgQuatsArrayPath_Key);
  auto pCentroidsArrayPathValue = filterArgs.value<DataPath>(k_CentroidsArrayPath_Key);
  auto pEquivalentDiametersArrayPathValue = filterArgs.value<DataPath>(k_EquivalentDiametersArrayPath_Key);
  auto pFeaturePhasesArrayPathValue = filterArgs.value<DataPath>(k_FeaturePhasesArrayPath_Key);
  auto pStatsGenCellEnsembleAttributeMatrixPathValue = filterArgs.value<DataPath>(k_StatsGenCellEnsembleAttributeMatrixPath_Key);
  auto pCrystalStructuresArrayPathValue = filterArgs.value<DataPath>(k_CrystalStructuresArrayPath_Key);
  auto pPhaseTypesArrayPathValue = filterArgs.value<DataPath>(k_PhaseTypesArrayPath_Key);
  auto pShapeTypesArrayPathValue = filterArgs.value<DataPath>(k_ShapeTypesArrayPath_Key);
  auto pNumFeaturesArrayPathValue = filterArgs.value<DataPath>(k_NumFeaturesArrayPath_Key);
  auto pFeatureParentIdsArrayNameValue = filterArgs.value<DataPath>(k_FeatureParentIdsArrayName_Key);
  auto pNumFeaturesPerParentArrayPathValue = filterArgs.value<DataPath>(k_NumFeaturesPerParentArrayPath_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
