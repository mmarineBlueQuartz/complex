#pragma once

#include "ComplexCore/ComplexCore_export.hpp"

#include "complex/Filter/FilterTraits.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{
/**
 * @class LaplacianSmoothingFilter
 * @brief This filter will ....
 */
class COMPLEXCORE_EXPORT LaplacianSmoothingFilter : public IFilter
{
public:
  LaplacianSmoothingFilter() = default;
  ~LaplacianSmoothingFilter() noexcept override = default;

  LaplacianSmoothingFilter(const LaplacianSmoothingFilter&) = delete;
  LaplacianSmoothingFilter(LaplacianSmoothingFilter&&) noexcept = delete;

  LaplacianSmoothingFilter& operator=(const LaplacianSmoothingFilter&) = delete;
  LaplacianSmoothingFilter& operator=(LaplacianSmoothingFilter&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_TriangleGeometryDataPath_Key = "TriangleGeometryDataPath";
  static inline constexpr StringLiteral k_IterationSteps_Key = "IterationSteps";
  static inline constexpr StringLiteral k_Lambda_Key = "Lambda";
  static inline constexpr StringLiteral k_UseTaubinSmoothing_Key = "UseTaubinSmoothing";
  static inline constexpr StringLiteral k_MuFactor_Key = "MuFactor";
  static inline constexpr StringLiteral k_TripleLineLambda_Key = "TripleLineLambda";
  static inline constexpr StringLiteral k_QuadPointLambda_Key = "QuadPointLambda";
  static inline constexpr StringLiteral k_SurfacePointLambda_Key = "SurfacePointLambda";
  static inline constexpr StringLiteral k_SurfaceTripleLineLambda_Key = "SurfaceTripleLineLambda";
  static inline constexpr StringLiteral k_SurfaceQuadPointLambda_Key = "SurfaceQuadPointLambda";
  static inline constexpr StringLiteral k_SurfaceMeshNodeTypeArrayPath_Key = "SurfaceMeshNodeTypeArrayPath";
  static inline constexpr StringLiteral k_SurfaceMeshFaceLabelsArrayPath_Key = "SurfaceMeshFaceLabelsArrayPath";

  /**
   * @brief Returns the name of the filter.
   * @return
   */
  std::string name() const override;

  /**
   * @brief Returns the C++ classname of this filter.
   * @return
   */
  std::string className() const override;

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
   * @brief Returns the default tags for this filter.
   * @return
   */
  std::vector<std::string> defaultTags() const override;

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
  PreflightResult preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;

  /**
   * @brief Applies the filter's algorithm to the DataStructure with the given arguments. Returns any warnings/errors.
   * On failure, there is no guarantee that the DataStructure is in a correct state.
   * @param ds The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  Result<> executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;
};
} // namespace complex

COMPLEX_DEF_FILTER_TRAITS(complex, LaplacianSmoothingFilter, "0dd0916e-9305-4a7b-98cf-a6cfb97cb501");
