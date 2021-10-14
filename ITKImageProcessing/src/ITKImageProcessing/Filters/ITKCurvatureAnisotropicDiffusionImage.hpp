#pragma once

#include "complex/Filter/FilterTraits.hpp"
#include "complex/Filter/IFilter.hpp"
#include "complex/complex_export.hpp"

namespace complex
{
/**
 * @class ITKCurvatureAnisotropicDiffusionImage
 * @brief This filter will ....
 */
class COMPLEX_EXPORT ITKCurvatureAnisotropicDiffusionImage : public IFilter
{
public:
  ITKCurvatureAnisotropicDiffusionImage() = default;
  ~ITKCurvatureAnisotropicDiffusionImage() noexcept override = default;

  ITKCurvatureAnisotropicDiffusionImage(const ITKCurvatureAnisotropicDiffusionImage&) = delete;
  ITKCurvatureAnisotropicDiffusionImage(ITKCurvatureAnisotropicDiffusionImage&&) noexcept = delete;

  ITKCurvatureAnisotropicDiffusionImage& operator=(const ITKCurvatureAnisotropicDiffusionImage&) = delete;
  ITKCurvatureAnisotropicDiffusionImage& operator=(ITKCurvatureAnisotropicDiffusionImage&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_TimeStep_Key = "TimeStep";
  static inline constexpr StringLiteral k_ConductanceParameter_Key = "ConductanceParameter";
  static inline constexpr StringLiteral k_ConductanceScalingUpdateInterval_Key = "ConductanceScalingUpdateInterval";
  static inline constexpr StringLiteral k_NumberOfIterations_Key = "NumberOfIterations";
  static inline constexpr StringLiteral k_SelectedCellArrayPath_Key = "SelectedCellArrayPath";
  static inline constexpr StringLiteral k_NewCellArrayName_Key = "NewCellArrayName";

  /**
   * @brief Returns the name of the filter.
   * @return
   */
  std::string name() const override;

  /**
   * @brief Returns the uuid of the filter.
   * @return
   */
  Uuid uuid() const override;

  /**
   * @brief Returns the human readable name of the filter.
   * @return
   */
  std::string humanName() const override;

  /**
   * @brief Returns the parameters of the filter (i.e. its inputs)
   * @return
   */
  Parameters parameters() const override;

  /**
   * @brief Returns a copy of the filter.
   * @return
   */
  UniquePointer clone() const override;

protected:
  /**
   * @brief Takes in a DataStructure and checks that the filter can be run on it with the given arguments.
   * Returns any warnings/errors. Also returns the changes that would be applied to the DataStructure.
   * Some parts of the actions may not be completely filled out if all the required information is not available at preflight time.
   * @param ds The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  Result<OutputActions> preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const override;

  /**
   * @brief Applies the filter's algorithm to the DataStructure with the given arguments. Returns any warnings/errors.
   * On failure, there is no guarantee that the DataStructure is in a correct state.
   * @param ds The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  Result<> executeImpl(DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const override;
};
} // namespace complex

COMPLEX_DEF_FILTER_TRAITS(complex::ITKCurvatureAnisotropicDiffusionImage, "60387baa-1ab0-5b32-ada4-f0e99d554117");
