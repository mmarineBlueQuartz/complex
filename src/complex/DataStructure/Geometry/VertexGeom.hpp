#pragma once

#include "complex/DataStructure/Geometry/INodeGeometry0D.hpp"

#include "complex/complex_export.hpp"

namespace complex
{
/**
 * @class VertexGeom
 * @brief
 */
class COMPLEX_EXPORT VertexGeom : public INodeGeometry0D
{
public:
  friend class DataStructure;

  static inline constexpr usize k_NumVerts = 1;

  /**
   * @brief
   * @param ds
   * @param name
   * @param parentId = {}
   * @return VertexGeom*
   */
  static VertexGeom* Create(DataStructure& ds, std::string name, const std::optional<IdType>& parentId = {});

  /**
   * @brief
   * @param ds
   * @param name
   * @param importId
   * @param parentId = {}
   * @return VertexGeom*
   */
  static VertexGeom* Import(DataStructure& ds, std::string name, IdType importId, const std::optional<IdType>& parentId = {});

  /**
   * @brief
   * @param other
   */
  VertexGeom(const VertexGeom& other);

  /**
   * @brief
   * @param other
   */
  VertexGeom(VertexGeom&& other);

  ~VertexGeom() noexcept override;

  VertexGeom& operator=(const VertexGeom&) = delete;
  VertexGeom& operator=(VertexGeom&&) noexcept = delete;

  /**
   * @brief Returns the type of geometry.
   * @return
   */
  IGeometry::Type getGeomType() const override;

  /**
   * @brief Returns an enumeration of the class or subclass. Used for quick comparison or type deduction
   * @return
   */
  DataObject::Type getDataObjectType() const override;

  /**
   * @brief Returns typename of the DataObject as a std::string.
   * @return std::string
   */
  std::string getTypeName() const override;

  /**
   * @brief Returns typename of the DataObject as a std::string.
   * @return std::string
   */
  static std::string GetTypeName();

  /**
   * @brief
   * @return DataObject*
   */
  DataObject* shallowCopy() override;

  /**
   * @brief
   * @return DataObject*
   */
  DataObject* deepCopy() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementSizes() override;

  /**
   * @brief
   * @return complex::Point3D<float64>
   */
  Point3D<float64> getParametricCenter() const override;

  /**
   * @brief
   * @param pCoords
   * @param shape
   */
  void getShapeFunctions(const Point3D<float64>& pCoords, float64* shape) const override;

#if 0
  /**
   * @brief Reads the DataStructure group from a target Zarr data.
   * @param dataStructureReader
   * @param collection
   * @return Zarr::Error
   */
  Zarr::ErrorType readZarr(Zarr::DataStructureReader& dataStructureReader, const FileVec::IGroup& collection, bool preflight = false) override;
  
  /**
   * @brief Writes the geometry to Zarr using the provided parent group ID.
   * @param dataStructureWriter
   * @param parentGroupWriter
   * @param importable
   * @return Zarr::ErrorType
   */
  Zarr::ErrorType writeZarr(Zarr::DataStructureWriter& dataStructureWriter, FileVec::IGroup& parentGroupWriter, bool importable) const override;
#endif

protected:
  /**
   * @brief
   * @param ds
   * @param name
   */
  VertexGeom(DataStructure& ds, std::string name);

  /**
   * @brief
   * @param ds
   * @param name
   * @param importId
   */
  VertexGeom(DataStructure& ds, std::string name, IdType importId);
};
} // namespace complex
