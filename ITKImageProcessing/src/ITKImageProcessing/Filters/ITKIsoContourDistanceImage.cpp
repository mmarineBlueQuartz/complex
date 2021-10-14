#include "ITKIsoContourDistanceImage.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"

using namespace complex;

namespace complex
{
std::string ITKIsoContourDistanceImage::name() const
{
  return FilterTraits<ITKIsoContourDistanceImage>::name.str();
}

Uuid ITKIsoContourDistanceImage::uuid() const
{
  return FilterTraits<ITKIsoContourDistanceImage>::uuid;
}

std::string ITKIsoContourDistanceImage::humanName() const
{
  return "ITK::Iso Contour Distance Image Filter";
}

Parameters ITKIsoContourDistanceImage::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<Float64Parameter>(k_LevelSetValue_Key, "LevelSetValue", "", 2.3456789));
  params.insert(std::make_unique<Float64Parameter>(k_FarValue_Key, "FarValue", "", 2.3456789));
  params.insertSeparator(Parameters::Separator{"Cell Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_SelectedCellArrayPath_Key, "Attribute Array to filter", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Cell Data"});
  params.insert(std::make_unique<StringParameter>(k_NewCellArrayName_Key, "Filtered Array", "", "SomeString"));

  return params;
}

IFilter::UniquePointer ITKIsoContourDistanceImage::clone() const
{
  return std::make_unique<ITKIsoContourDistanceImage>();
}

Result<OutputActions> ITKIsoContourDistanceImage::preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Write any preflight sanity checking codes in this function
   ***************************************************************************/
  auto pLevelSetValueValue = filterArgs.value<float64>(k_LevelSetValue_Key);
  auto pFarValueValue = filterArgs.value<float64>(k_FarValue_Key);
  auto pSelectedCellArrayPathValue = filterArgs.value<DataPath>(k_SelectedCellArrayPath_Key);
  auto pNewCellArrayNameValue = filterArgs.value<StringParameter::ValueType>(k_NewCellArrayName_Key);

  OutputActions actions;
#if 0
  // Define a custom class that generates the changes to the DataStructure.
  auto action = std::make_unique<ITKIsoContourDistanceImageAction>();
  actions.actions.push_back(std::move(action));
#endif
  return {std::move(actions)};
}

Result<> ITKIsoContourDistanceImage::executeImpl(DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pLevelSetValueValue = filterArgs.value<float64>(k_LevelSetValue_Key);
  auto pFarValueValue = filterArgs.value<float64>(k_FarValue_Key);
  auto pSelectedCellArrayPathValue = filterArgs.value<DataPath>(k_SelectedCellArrayPath_Key);
  auto pNewCellArrayNameValue = filterArgs.value<StringParameter::ValueType>(k_NewCellArrayName_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
