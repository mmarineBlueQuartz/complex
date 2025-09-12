#include "CropGeometryParameter.hpp"

#include "simplnx/Common/Any.hpp"

#include <fmt/core.h>
#include <nlohmann/json.hpp>

namespace nx::core
{
namespace
{
constexpr StringLiteral k_Type_Key = "type";
constexpr StringLiteral k_CropX_Key = "crop_x";
constexpr StringLiteral k_CropY_Key = "crop_y";
constexpr StringLiteral k_CropZ_Key = "crop_z";
constexpr StringLiteral k_XBoundVoxels_Key = "voxels_x";
constexpr StringLiteral k_YBoundVoxels_Key = "voxels_y";
constexpr StringLiteral k_ZBoundVoxels_Key = "voxels_z";
constexpr StringLiteral k_XBoundPhysical_Key = "physical_x";
constexpr StringLiteral k_YBoundPhysical_Key = "physical_y";
constexpr StringLiteral k_ZBoundPhysical_Key = "physical_z";

constexpr int64 k_InvalidType = -63900;
constexpr int64 k_InvalidBounds = -63901;
} // namespace

CropGeometryParameter::CropGeometryParameter(const std::string& name, const std::string& humanName, const std::string& helpText, const ValueType& defaultValue)
: ValueParameter(name, humanName, helpText)
, m_DefaultValue(defaultValue)
{
}

Uuid CropGeometryParameter::uuid() const
{
  return ParameterTraits<CropGeometryParameter>::uuid;
}

IParameter::AcceptedTypes CropGeometryParameter::acceptedTypes() const
{
  return {typeid(ValueType)};
}

//------------------------------------------------------------------------------
IParameter::VersionType CropGeometryParameter::getVersion() const
{
  return 1;
}

nlohmann::json CropGeometryParameter::toJsonImpl(const std::any& value) const
{
  const auto& cropValues = GetAnyRef<ValueType>(value);
  nlohmann::json json;
  json[k_Type_Key] = static_cast<uint8>(cropValues.type);
  json[k_CropX_Key] = cropValues.cropX;
  json[k_CropY_Key] = cropValues.cropY;
  json[k_CropZ_Key] = cropValues.cropZ;
  json[k_XBoundVoxels_Key] = cropValues.xBoundVoxels;
  json[k_YBoundVoxels_Key] = cropValues.yBoundVoxels;
  json[k_ZBoundVoxels_Key] = cropValues.zBoundVoxels;
  json[k_XBoundPhysical_Key] = cropValues.xBoundPhysical;
  json[k_YBoundPhysical_Key] = cropValues.yBoundPhysical;
  json[k_ZBoundPhysical_Key] = cropValues.zBoundPhysical;
  return json;
}

Result<std::any> CropGeometryParameter::fromJsonImpl(const nlohmann::json& json, VersionType version) const
{
  static constexpr StringLiteral prefix = "FilterParameter 'CropGeometryParameter' JSON Error: ";
  static const std::string nameDiv = name() + "/";

  ValueType value;
  if(!json.is_object())
  {
    return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_String, fmt::format("{}JSON value for key '{}' is not an object", prefix.view(), name()));
  }

  auto keyJson = json[k_Type_Key];
  if(!keyJson.is_number_unsigned())
  {
    return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_String,
                                     fmt::format("{}JSON value for key '{}' is not an unsigned integer", prefix.view(), nameDiv + k_Type_Key.str()));
  }
  value.type = static_cast<ValueType::TypeEnum>(keyJson.get<uint8>());

  {
    auto cropXJson = json[k_CropX_Key];
    if(!cropXJson.is_boolean())
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Value_Type, fmt::format("{}JSON value for key '{}' is not a boolean", prefix.view(), nameDiv + k_CropX_Key.str()));
    }
    value.cropX = cropXJson.get<bool>();
  }

  {
    auto cropYJson = json[k_CropY_Key];
    if(!cropYJson.is_boolean())
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Value_Type, fmt::format("{}JSON value for key '{}' is not a boolean", prefix.view(), nameDiv + k_CropY_Key.str()));
    }
    value.cropY = cropYJson.get<bool>();
  }

  {
    auto cropZJson = json[k_CropZ_Key];
    if(!cropZJson.is_boolean())
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Value_Type, fmt::format("{}JSON value for key '{}' is not a boolean", prefix.view(), nameDiv + k_CropZ_Key.str()));
    }
    value.cropZ = cropZJson.get<bool>();
  }

  {
    auto xBoundsJson = json[k_XBoundVoxels_Key];
    if(!xBoundsJson.is_array())
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Value_Type,
                                       fmt::format("{}JSON value for key '{}' is not an array", prefix.view(), nameDiv + k_XBoundVoxels_Key.str()));
    }
    value.xBoundVoxels = xBoundsJson.get<std::array<int32, 2>>();
  }

  {
    auto yBoundsJson = json[k_YBoundVoxels_Key];
    if(!yBoundsJson.is_array())
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Value_Type,
                                       fmt::format("{}JSON value for key '{}' is not an array", prefix.view(), nameDiv + k_YBoundVoxels_Key.str()));
    }
    value.yBoundVoxels = yBoundsJson.get<std::array<int32, 2>>();
  }

  {
    auto zBoundsJson = json[k_ZBoundVoxels_Key];
    if(!zBoundsJson.is_array())
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Value_Type,
                                       fmt::format("{}JSON value for key '{}' is not an array", prefix.view(), nameDiv + k_ZBoundVoxels_Key.str()));
    }
    value.zBoundVoxels = zBoundsJson.get<std::array<int32, 2>>();
  }

  {
    auto xBoundsJson = json[k_XBoundPhysical_Key];
    if(!xBoundsJson.is_array())
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Value_Type, fmt::format("{}JSON value for key '{}' is not an array", prefix.view(), nameDiv + k_XBoundPhysical_Key.str()));
    }
    value.xBoundPhysical = xBoundsJson.get<std::array<float32, 2>>();
  }

  {
    auto yBoundsJson = json[k_YBoundPhysical_Key];
    if(!yBoundsJson.is_array())
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Value_Type,
                                       fmt::format("{}JSON value for key '{}' is not an array", prefix.view(), nameDiv + k_YBoundPhysical_Key.str()));
    }
    value.yBoundPhysical = yBoundsJson.get<std::array<float32, 2>>();
  }

  {
    auto zBoundsJson = json[k_ZBoundPhysical_Key];
    if(!zBoundsJson.is_array())
    {
      return MakeErrorResult<std::any>(FilterParameter::Constants::k_Json_Value_Not_Value_Type,
                                       fmt::format("{}JSON value for key '{}' is not an array", prefix.view(), nameDiv + k_ZBoundPhysical_Key.str()));
    }
    value.zBoundPhysical = zBoundsJson.get<std::array<float32, 2>>();
  }

  return {value};
}

IParameter::UniquePointer CropGeometryParameter::clone() const
{
  return std::make_unique<CropGeometryParameter>(name(), humanName(), helpText(), m_DefaultValue);
}

std::any CropGeometryParameter::defaultValue() const
{
  return defaultPath();
}

typename CropGeometryParameter::ValueType CropGeometryParameter::defaultPath() const
{
  return m_DefaultValue;
}

Result<> CropGeometryParameter::validate(const std::any& value) const
{
  const auto& cropValues = GetAnyRef<ValueType>(value);

  const auto cropTypeInt = static_cast<uint8>(cropValues.type);
  if(cropTypeInt >= 3)
  {
    return MakeErrorResult(k_InvalidType, fmt::format("Invalid CropGeometry type: '{}'", cropTypeInt));
  }

  if(cropValues.xBoundVoxels[0] >= cropValues.xBoundVoxels[1])
  {
    return MakeErrorResult(k_InvalidBounds, "Invalid X bounds [Voxels]");
  }
  if(cropValues.yBoundVoxels[0] >= cropValues.yBoundVoxels[1])
  {
    return MakeErrorResult(k_InvalidBounds, "Invalid Y bounds [Voxels]");
  }
  if(cropValues.zBoundVoxels[0] >= cropValues.zBoundVoxels[1])
  {
    return MakeErrorResult(k_InvalidBounds, "Invalid Z bounds [Voxels]");
  }

  if(cropValues.xBoundPhysical[0] >= cropValues.xBoundPhysical[1])
  {
    return MakeErrorResult(k_InvalidBounds, "Invalid X bounds [Physical]");
  }
  if(cropValues.yBoundPhysical[0] >= cropValues.yBoundPhysical[1])
  {
    return MakeErrorResult(k_InvalidBounds, "Invalid Y bounds [Physical]");
  }
  if(cropValues.zBoundPhysical[0] >= cropValues.zBoundPhysical[1])
  {
    return MakeErrorResult(k_InvalidBounds, "Invalid Z bounds [Physical]");
  }

  return {};
}
} // namespace nx::core
