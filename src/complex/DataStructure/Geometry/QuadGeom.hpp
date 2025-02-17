#pragma once

#include "complex/Common/Point3D.hpp"
#include "complex/DataStructure/Geometry/INodeGeometry2D.hpp"

#include "complex/complex_export.hpp"

namespace complex
{
/**
 * @class QuadGeom
 * @brief
 */
class COMPLEX_EXPORT QuadGeom : public INodeGeometry2D
{
public:
  friend class DataStructure;

  static inline constexpr usize k_NumVerts = 4;

  /**
   * @brief
   * @param ds
   * @param name
   * @param parentId = {}
   * @return QuadGeom*
   */
  static QuadGeom* Create(DataStructure& ds, std::string name, const std::optional<IdType>& parentId = {});

  /**
   * @brief
   * @param ds
   * @param name
   * @param importId
   * @param parentId = {}
   * @return QuadGeom*
   */
  static QuadGeom* Import(DataStructure& ds, std::string name, IdType importId, const std::optional<IdType>& parentId = {});

  /**
   * @brief
   * @param other
   */
  QuadGeom(const QuadGeom& other);

  /**
   * @brief
   * @param other
   */
  QuadGeom(QuadGeom&& other) noexcept;

  ~QuadGeom() noexcept override;

  QuadGeom& operator=(const QuadGeom&) = delete;
  QuadGeom& operator=(QuadGeom&&) noexcept = delete;

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
   * @param quadId
   * @param verts
   */
  void setVertexIdsForFace(usize quadId, usize verts[4]);

  /**
   * @brief
   * @param faceId
   * @param verts
   */
  void getVertexIdsForFace(usize faceId, usize verts[4]) const;

  /**
   * @brief
   * @param faceId
   * @param vert1
   * @param vert2
   * @param vert3
   * @param vert4
   */
  void getVertexCoordsForFace(usize faceId, Point3D<float32>& vert1, Point3D<float32>& vert2, Point3D<float32>& vert3, Point3D<float32>& vert4) const;

  /**
   * @brief Returns the number of quads in the geometry. If the quad list has
   * not been set, this method returns 0.
   * @return usize
   */
  usize getNumberOfQuads() const;

  /**
   * @brief
   * @return usize
   */
  usize getNumberOfElements() const override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementSizes() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementsContainingVert() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementNeighbors() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementCentroids() override;

  /**
   * @brief
   * @return Point3D<float64>
   */
  Point3D<float64> getParametricCenter() const override;

  /**
   * @brief
   * @param pCoords
   * @param shape
   */
  void getShapeFunctions(const Point3D<float64>& pCoords, float64* shape) const override;

  /**
   * @brief
   * @param vertId
   * @param Point3D<float32>
   */
  void setCoords(usize vertId, const Point3D<float32>& coord) override;

  /**
   * @brief
   * @param vertId
   * @return Point3D<float32>
   */
  Point3D<float32> getCoords(usize vertId) const override;

  /**
   * @brief
   * @param edgeId
   * @param vert1
   * @param vert2
   */
  void getVertCoordsAtEdge(usize edgeId, Point3D<float32>& vert1, Point3D<float32>& vert2) const override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findEdges() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findUnsharedEdges() override;

protected:
  /**
   * @brief
   * @param ds
   * @param name
   */
  QuadGeom(DataStructure& ds, std::string name);

  /**
   * @brief
   * @param ds
   * @param name
   * @param importId
   */
  QuadGeom(DataStructure& ds, std::string name, IdType importId);
};
} // namespace complex
