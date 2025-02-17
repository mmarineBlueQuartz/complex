# Calculate Triangle Normals #


## Group (Subgroup) ##

Surface Meshing (Misc)

## Description ##

This **Filter** computes the normal of each **Triangle** in a **Triangle Geometry** by utilizing matrix subtraction, cross product, and normalization to implement the following theory:
    For a triangle with point1, point2, point3, if the vector U = point2 - point1 and the vector V = point3 - point1 

    Nx = UyVz - UzVy
    Ny = UzVx - UxVz
    Nz = UxVy - UyVx

    Where "point#" represents a 3x1 Matrix of coordinates x, y, and z belonging to one of a Triangles vertexes and N represents the normal of the corresponding axis value


## Parameters ##

None

## Required Geometry ##

Triangle

## Required Objects ##

None

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array**  | FaceNormals | double array | (1) | Specifies the normal of each **Face** |


## Example Pipelines ##

+ (03) SmallIN100 Mesh Statistics

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)
