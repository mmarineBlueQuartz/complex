#include "QuadGeom.hpp"

#include <stdexcept>

#include "complex/DataStructure/DataStore.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Utilities/GeometryHelpers.hpp"

using namespace complex;

QuadGeom::QuadGeom(DataStructure& ds, std::string name)
: INodeGeometry2D(ds, std::move(name))
{
}

QuadGeom::QuadGeom(DataStructure& ds, std::string name, IdType importId)
: INodeGeometry2D(ds, std::move(name), importId)
{
}

IGeometry::Type QuadGeom::getGeomType() const
{
  return IGeometry::Type::Quad;
}

DataObject::Type QuadGeom::getDataObjectType() const
{
  return DataObject::Type::QuadGeom;
}

QuadGeom* QuadGeom::Create(DataStructure& ds, std::string name, const std::optional<IdType>& parentId)
{
  auto data = std::shared_ptr<QuadGeom>(new QuadGeom(ds, std::move(name)));
  if(!AttemptToAddObject(ds, data, parentId))
  {
    return nullptr;
  }
  return data.get();
}

QuadGeom* QuadGeom::Import(DataStructure& ds, std::string name, IdType importId, const std::optional<IdType>& parentId)
{
  auto data = std::shared_ptr<QuadGeom>(new QuadGeom(ds, std::move(name), importId));
  if(!AttemptToAddObject(ds, data, parentId))
  {
    return nullptr;
  }
  return data.get();
}

std::string QuadGeom::getTypeName() const
{
  return GetTypeName();
}

std::string QuadGeom::GetTypeName()
{
  return "QuadGeom";
}

DataObject* QuadGeom::shallowCopy()
{
  return new QuadGeom(*this);
}

DataObject* QuadGeom::deepCopy()
{
  return new QuadGeom(*this);
}

usize QuadGeom::getNumberOfCells() const
{
  auto& quads = getFacesRef();
  return quads.getNumberOfTuples();
}

usize QuadGeom::getNumberOfVerticesPerFace() const
{
  return k_NumFaceVerts;
}

IGeometry::StatusCode QuadGeom::findElementSizes()
{
  auto dataStore = std::make_unique<DataStore<float32>>(getNumberOfQuads(), 0.0f);
  Float32Array* quadSizes = DataArray<float32>::Create(*getDataStructure(), "Quad Areas", std::move(dataStore), getId());
  GeometryHelpers::Topology::Find2DElementAreas(getFaces(), getVertices(), quadSizes);
  if(quadSizes == nullptr)
  {
    m_ElementSizesId.reset();
    return -1;
  }
  m_ElementSizesId = quadSizes->getId();
  return 1;
}

IGeometry::StatusCode QuadGeom::findElementsContainingVert()
{
  auto quadsContainingVert = DynamicListArray<uint16, MeshIndexType>::Create(*getDataStructure(), "Quads Containing Vert", getId());
  GeometryHelpers::Connectivity::FindElementsContainingVert<uint16, MeshIndexType>(getFaces(), quadsContainingVert, getNumberOfVertices());
  if(quadsContainingVert == nullptr)
  {
    m_CellContainingVertId.reset();
    return -1;
  }
  m_CellContainingVertId = quadsContainingVert->getId();
  return 1;
}

IGeometry::StatusCode QuadGeom::findElementNeighbors()
{
  if(getElementsContainingVert() == nullptr)
  {
    StatusCode err = findElementsContainingVert();
    if(err < 0)
    {
      return err;
    }
  }
  auto quadNeighbors = DynamicListArray<uint16, MeshIndexType>::Create(*getDataStructure(), "Quad Neighbors", getId());
  StatusCode err = GeometryHelpers::Connectivity::FindElementNeighbors<uint16, MeshIndexType>(getFaces(), getElementsContainingVert(), quadNeighbors, IGeometry::Type::Quad);
  if(quadNeighbors == nullptr)
  {
    m_CellNeighborsId.reset();
    return -1;
  }
  m_CellNeighborsId = quadNeighbors->getId();
  return err;
}

IGeometry::StatusCode QuadGeom::findElementCentroids()
{
  auto dataStore = std::make_unique<DataStore<float32>>(std::vector<usize>{getNumberOfQuads()}, std::vector<usize>{3}, 0.0f);
  auto quadCentroids = DataArray<float32>::Create(*getDataStructure(), "Quad Centroids", std::move(dataStore), getId());
  GeometryHelpers::Topology::FindElementCentroids(getFaces(), getVertices(), quadCentroids);
  if(quadCentroids == nullptr)
  {
    m_CellCentroidsId.reset();
    return -1;
  }
  m_CellCentroidsId = quadCentroids->getId();
  return 1;
}

Point3D<float64> QuadGeom::getParametricCenter() const
{
  return {0.5, 0.5, 0.0};
}

void QuadGeom::getShapeFunctions(const Point3D<float64>& pCoords, float64* shape) const
{
  float64 rm = 1.0 - pCoords[0];
  float64 sm = 1.0 - pCoords[1];

  shape[0] = -sm;
  shape[1] = sm;
  shape[2] = pCoords[1];
  shape[3] = -pCoords[1];
  shape[4] = -rm;
  shape[5] = -pCoords[0];
  shape[6] = pCoords[0];
  shape[7] = rm;
}

IGeometry::StatusCode QuadGeom::findEdges()
{
  auto* edgeList = createSharedEdgeList(0);
  GeometryHelpers::Connectivity::Find2DElementEdges(getFaces(), edgeList);
  if(edgeList == nullptr)
  {
    m_EdgeListId.reset();
    return -1;
  }
  m_EdgeListId = edgeList->getId();
  return 1;
}

IGeometry::StatusCode QuadGeom::findUnsharedEdges()
{
  auto dataStore = std::make_unique<DataStore<MeshIndexType>>(std::vector<usize>{0}, std::vector<usize>{2}, 0);
  auto unsharedEdgeList = DataArray<MeshIndexType>::Create(*getDataStructure(), "Unshared Edge List", std::move(dataStore), getId());
  GeometryHelpers::Connectivity::Find2DUnsharedEdges(getFaces(), unsharedEdgeList);
  if(unsharedEdgeList == nullptr)
  {
    m_UnsharedEdgeListId.reset();
    return -1;
  }
  m_UnsharedEdgeListId = unsharedEdgeList->getId();
  return 1;
}

#if 0
Zarr::ErrorType QuadGeom::readZarr(Zarr::DataStructureReader& dataStructureReader, const FileVec::Group& collection, bool preflight)
{
  m_QuadListId = ReadZarrDataId(collection, H5Constants::k_QuadListTag);
  m_QuadsContainingVertId = ReadZarrDataId(collection, H5Constants::k_QuadsContainingVertTag);
  m_QuadNeighborsId = ReadZarrDataId(collection, H5Constants::k_QuadNeighborsTag);
  m_QuadCentroidsId = ReadZarrDataId(collection, H5Constants::k_QuadCentroidsTag);
  m_QuadSizesId = ReadZarrDataId(collection, H5Constants::k_QuadSizesTag);

  return AbstractGeometry2D::readZarr(dataStructureReader, collection, preflight);
}

Zarr::ErrorType QuadGeom::writeZarr(Zarr::DataStructureWriter& dataStructureWriter, FileVec::IGroup& parentGroupWriter, bool importable) const
{
  auto& groupWriter = *parentGroupWriter.createOrFindGroup(getName()).get();
  writeZarrObjectAttributes(dataStructureWriter, groupWriter, importable);

  // Write DataObject IDs
  WriteZarrDataId(groupWriter, m_QuadListId, H5Constants::k_QuadListTag);
  WriteZarrDataId(groupWriter, m_QuadsContainingVertId, H5Constants::k_QuadsContainingVertTag);
  WriteZarrDataId(groupWriter, m_QuadNeighborsId, H5Constants::k_QuadNeighborsTag);
  WriteZarrDataId(groupWriter, m_QuadCentroidsId, H5Constants::k_QuadCentroidsTag);
  WriteZarrDataId(groupWriter, m_QuadSizesId, H5Constants::k_QuadSizesTag);

  return getDataMap().writeZarrGroup(dataStructureWriter, groupWriter);
}
#endif
