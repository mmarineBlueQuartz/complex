#include "ITKErodeObjectMorphologyImage.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

using namespace complex;

namespace complex
{
std::string ITKErodeObjectMorphologyImage::name() const
{
  return FilterTraits<ITKErodeObjectMorphologyImage>::name.str();
}

Uuid ITKErodeObjectMorphologyImage::uuid() const
{
  return FilterTraits<ITKErodeObjectMorphologyImage>::uuid;
}

std::string ITKErodeObjectMorphologyImage::humanName() const
{
  return "ITK::Erode Object Morphology Image Filter";
}

Parameters ITKErodeObjectMorphologyImage::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<ChoicesParameter>(k_KernelType_Key, "Kernel Type", "", 0, ChoicesParameter::Choices{"Option 1", "Option 2", "Option 3"}));
  params.insert(std::make_unique<Float64Parameter>(k_ObjectValue_Key, "ObjectValue", "", 2.3456789));
  params.insert(std::make_unique<Float64Parameter>(k_BackgroundValue_Key, "BackgroundValue", "", 2.3456789));
  params.insert(std::make_unique<VectorFloat32Parameter>(k_KernelRadius_Key, "KernelRadius", "", std::vector<float32>(3), std::vector<std::string>(3)));
  params.insertSeparator(Parameters::Separator{"Cell Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_SelectedCellArrayPath_Key, "Attribute Array to filter", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Cell Data"});
  params.insert(std::make_unique<StringParameter>(k_NewCellArrayName_Key, "Filtered Array", "", "SomeString"));

  return params;
}

IFilter::UniquePointer ITKErodeObjectMorphologyImage::clone() const
{
  return std::make_unique<ITKErodeObjectMorphologyImage>();
}

Result<OutputActions> ITKErodeObjectMorphologyImage::preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Write any preflight sanity checking codes in this function
   ***************************************************************************/
  auto pKernelTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_KernelType_Key);
  auto pObjectValueValue = filterArgs.value<float64>(k_ObjectValue_Key);
  auto pBackgroundValueValue = filterArgs.value<float64>(k_BackgroundValue_Key);
  auto pKernelRadiusValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_KernelRadius_Key);
  auto pSelectedCellArrayPathValue = filterArgs.value<DataPath>(k_SelectedCellArrayPath_Key);
  auto pNewCellArrayNameValue = filterArgs.value<StringParameter::ValueType>(k_NewCellArrayName_Key);

  OutputActions actions;
#if 0
  // Define a custom class that generates the changes to the DataStructure.
  auto action = std::make_unique<ITKErodeObjectMorphologyImageAction>();
  actions.actions.push_back(std::move(action));
#endif
  return {std::move(actions)};
}

Result<> ITKErodeObjectMorphologyImage::executeImpl(DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pKernelTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_KernelType_Key);
  auto pObjectValueValue = filterArgs.value<float64>(k_ObjectValue_Key);
  auto pBackgroundValueValue = filterArgs.value<float64>(k_BackgroundValue_Key);
  auto pKernelRadiusValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_KernelRadius_Key);
  auto pSelectedCellArrayPathValue = filterArgs.value<DataPath>(k_SelectedCellArrayPath_Key);
  auto pNewCellArrayNameValue = filterArgs.value<StringParameter::ValueType>(k_NewCellArrayName_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
