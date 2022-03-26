#include "InterpolatePointCloudToRegularGrid.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/EmptyAction.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/DataGroupSelectionParameter.hpp"
#include "complex/Parameters/MultiArraySelectionParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string InterpolatePointCloudToRegularGrid::name() const
{
  return FilterTraits<InterpolatePointCloudToRegularGrid>::name.str();
}

//------------------------------------------------------------------------------
std::string InterpolatePointCloudToRegularGrid::className() const
{
  return FilterTraits<InterpolatePointCloudToRegularGrid>::className;
}

//------------------------------------------------------------------------------
Uuid InterpolatePointCloudToRegularGrid::uuid() const
{
  return FilterTraits<InterpolatePointCloudToRegularGrid>::uuid;
}

//------------------------------------------------------------------------------
std::string InterpolatePointCloudToRegularGrid::humanName() const
{
  return "Interpolate Point Cloud to Regular Grid";
}

//------------------------------------------------------------------------------
std::vector<std::string> InterpolatePointCloudToRegularGrid::defaultTags() const
{
  return {"#Sampling", "#InterpolationFilters"};
}

//------------------------------------------------------------------------------
Parameters InterpolatePointCloudToRegularGrid::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insertLinkableParameter(std::make_unique<BoolParameter>(k_UseMask_Key, "Use Mask", "", false));
  params.insertLinkableParameter(std::make_unique<BoolParameter>(k_StoreKernelDistances_Key, "Store Kernel Distances", "", false));
  params.insertLinkableParameter(std::make_unique<ChoicesParameter>(k_InterpolationTechnique_Key, "Interpolation Technique", "", 0, ChoicesParameter::Choices{"Uniform", "Gaussian"}));
  params.insert(std::make_unique<VectorFloat32Parameter>(k_KernelSize_Key, "Kernel Size", "", std::vector<float32>(3), std::vector<std::string>(3)));
  params.insert(std::make_unique<VectorFloat32Parameter>(k_Sigmas_Key, "Gaussian Sigmas", "", std::vector<float32>(3), std::vector<std::string>(3)));
  params.insert(std::make_unique<DataGroupSelectionParameter>(k_DataContainerName_Key, "Data Container to Interpolate", "", DataPath{}));
  params.insert(std::make_unique<DataGroupSelectionParameter>(k_InterpolatedDataContainerName_Key, "Interpolated Data Container", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Vertex Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_VoxelIndicesArrayPath_Key, "Voxel Indices", "", DataPath{}, ArraySelectionParameter::AllowedTypes{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_MaskArrayPath_Key, "Mask", "", DataPath{}, ArraySelectionParameter::AllowedTypes{}));
  params.insert(std::make_unique<MultiArraySelectionParameter>(k_ArraysToInterpolate_Key, "Attribute Arrays to Interpolate", "",
                                                               MultiArraySelectionParameter::ValueType{DataPath(), DataPath(), DataPath()}, MultiArraySelectionParameter::AllowedTypes{}));
  params.insert(std::make_unique<MultiArraySelectionParameter>(k_ArraysToCopy_Key, "Attribute Arrays to Copy", "", MultiArraySelectionParameter::ValueType{DataPath(), DataPath(), DataPath()},
                                                               MultiArraySelectionParameter::AllowedTypes{}));
  params.insertSeparator(Parameters::Separator{"Cell Data"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_InterpolatedAttributeMatrixName_Key, "Interpolated Attribute Matrix", "", DataPath{}));
  params.insert(std::make_unique<ArrayCreationParameter>(k_KernelDistancesArrayName_Key, "Kernel Distances", "", DataPath{}));
  params.insert(std::make_unique<StringParameter>(k_InterpolatedSuffix_Key, "Interpolated Array Suffix", "", "SomeString"));
  params.insert(std::make_unique<StringParameter>(k_CopySuffix_Key, "Copied Array Suffix", "", "SomeString"));
  // Associate the Linkable Parameter(s) to the children parameters that they control
  params.linkParameters(k_UseMask_Key, k_MaskArrayPath_Key, true);
  params.linkParameters(k_StoreKernelDistances_Key, k_KernelDistancesArrayName_Key, true);
  params.linkParameters(k_InterpolationTechnique_Key, k_Sigmas_Key, 1);

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer InterpolatePointCloudToRegularGrid::clone() const
{
  return std::make_unique<InterpolatePointCloudToRegularGrid>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult InterpolatePointCloudToRegularGrid::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                           const std::atomic_bool& shouldCancel) const
{
  /****************************************************************************
   * Write any preflight sanity checking codes in this function
   ***************************************************************************/

  /**
   * These are the values that were gathered from the UI or the pipeline file or
   * otherwise passed into the filter. These are here for your convenience. If you
   * do not need some of them remove them.
   */
  auto pUseMaskValue = filterArgs.value<bool>(k_UseMask_Key);
  auto pStoreKernelDistancesValue = filterArgs.value<bool>(k_StoreKernelDistances_Key);
  auto pInterpolationTechniqueValue = filterArgs.value<ChoicesParameter::ValueType>(k_InterpolationTechnique_Key);
  auto pKernelSizeValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_KernelSize_Key);
  auto pSigmasValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Sigmas_Key);
  auto pDataContainerNameValue = filterArgs.value<DataPath>(k_DataContainerName_Key);
  auto pInterpolatedDataContainerNameValue = filterArgs.value<DataPath>(k_InterpolatedDataContainerName_Key);
  auto pVoxelIndicesArrayPathValue = filterArgs.value<DataPath>(k_VoxelIndicesArrayPath_Key);
  auto pMaskArrayPathValue = filterArgs.value<DataPath>(k_MaskArrayPath_Key);
  auto pArraysToInterpolateValue = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_ArraysToInterpolate_Key);
  auto pArraysToCopyValue = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_ArraysToCopy_Key);
  auto pInterpolatedAttributeMatrixNameValue = filterArgs.value<DataPath>(k_InterpolatedAttributeMatrixName_Key);
  auto pKernelDistancesArrayNameValue = filterArgs.value<DataPath>(k_KernelDistancesArrayName_Key);
  auto pInterpolatedSuffixValue = filterArgs.value<StringParameter::ValueType>(k_InterpolatedSuffix_Key);
  auto pCopySuffixValue = filterArgs.value<StringParameter::ValueType>(k_CopySuffix_Key);

  // Declare the preflightResult variable that will be populated with the results
  // of the preflight. The PreflightResult type contains the output Actions and
  // any preflight updated values that you want to be displayed to the user, typically
  // through a user interface (UI).
  PreflightResult preflightResult;

  // If your filter is making structural changes to the DataStructure then the filter
  // is going to create OutputActions subclasses that need to be returned. This will
  // store those actions.
  complex::Result<OutputActions> resultOutputActions;

  // If your filter is going to pass back some `preflight updated values` then this is where you
  // would create the code to store those values in the appropriate object. Note that we
  // in line creating the pair (NOT a std::pair<>) of Key:Value that will get stored in
  // the std::vector<PreflightValue> object.
  std::vector<PreflightValue> preflightUpdatedValues;

  // If the filter needs to pass back some updated values via a key:value string:string set of values
  // you can declare and update that string here.
  // None found in this filter based on the filter parameters

  // If this filter makes changes to the DataStructure in the form of
  // creating/deleting/moving/renaming DataGroups, Geometries, DataArrays then you
  // will need to use one of the `*Actions` classes located in complex/Filter/Actions
  // to relay that information to the preflight and execute methods. This is done by
  // creating an instance of the Action class and then storing it in the resultOutputActions variable.
  // This is done through a `push_back()` method combined with a `std::move()`. For the
  // newly initiated to `std::move` once that code is executed what was once inside the Action class
  // instance variable is *no longer there*. The memory has been moved. If you try to access that
  // variable after this line you will probably get a crash or have subtle bugs. To ensure that this
  // does not happen we suggest using braces `{}` to scope each of the action's declaration and store
  // so that the programmer is not tempted to use the action instance past where it should be used.
  // You have to create your own Actions class if there isn't something specific for your filter's needs

  // Store the preflight updated value(s) into the preflightUpdatedValues vector using
  // the appropriate methods.
  // None found based on the filter parameters

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> InterpolatePointCloudToRegularGrid::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                         const std::atomic_bool& shouldCancel) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pUseMaskValue = filterArgs.value<bool>(k_UseMask_Key);
  auto pStoreKernelDistancesValue = filterArgs.value<bool>(k_StoreKernelDistances_Key);
  auto pInterpolationTechniqueValue = filterArgs.value<ChoicesParameter::ValueType>(k_InterpolationTechnique_Key);
  auto pKernelSizeValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_KernelSize_Key);
  auto pSigmasValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Sigmas_Key);
  auto pDataContainerNameValue = filterArgs.value<DataPath>(k_DataContainerName_Key);
  auto pInterpolatedDataContainerNameValue = filterArgs.value<DataPath>(k_InterpolatedDataContainerName_Key);
  auto pVoxelIndicesArrayPathValue = filterArgs.value<DataPath>(k_VoxelIndicesArrayPath_Key);
  auto pMaskArrayPathValue = filterArgs.value<DataPath>(k_MaskArrayPath_Key);
  auto pArraysToInterpolateValue = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_ArraysToInterpolate_Key);
  auto pArraysToCopyValue = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_ArraysToCopy_Key);
  auto pInterpolatedAttributeMatrixNameValue = filterArgs.value<DataPath>(k_InterpolatedAttributeMatrixName_Key);
  auto pKernelDistancesArrayNameValue = filterArgs.value<DataPath>(k_KernelDistancesArrayName_Key);
  auto pInterpolatedSuffixValue = filterArgs.value<StringParameter::ValueType>(k_InterpolatedSuffix_Key);
  auto pCopySuffixValue = filterArgs.value<StringParameter::ValueType>(k_CopySuffix_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
