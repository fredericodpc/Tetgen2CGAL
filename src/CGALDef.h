#include <CGAL\Exact_predicates_inexact_constructions_kernel.h>

// ------------------------------------------------------------------------------------------------------ //

#include <CGAL\Linear_algebraCd.h>

// ------------------------------------------------------------------------------------------------------ //

#include <CGAL\Triangulation_3.h>
#include <CGAL\Triangulation_cell_base_3.h>
#include <CGAL\Triangulation_vertex_base_3.h>
#include <CGAL\Triangulation_vertex_base_with_info_3.h>
#include <CGAL\Triangulation_data_structure_3.h>

// ------------------------------------------------------------------------------------------------------ //

#include <CGAL\Polyhedron_3.h>
#include <CGAL\AABB_tree.h>
#include <CGAL\AABB_traits.h>
#include <CGAL\AABB_face_graph_triangle_primitive.h>
#include <CGAL\AABB_primitive.h>

// ------------------------------------------------------------------------------------------------------ //

typedef CGAL::Exact_predicates_inexact_constructions_kernel							K;
typedef K::FT																		doubleType;
typedef CGAL::Linear_algebraCd<doubleType>											LA;
typedef K::Point_3																	Point3D;
typedef K::Vector_3																	Vector3D;
typedef K::Segment_3																Segment3D;
typedef CGAL::Triangulation_cell_base_3<K>											CB3D;
typedef CGAL::Triangulation_vertex_base_3<K>										VB3D;
typedef CGAL::Triangulation_data_structure_3<VB3D, CB3D>							TDS3D;
typedef CGAL::Triangulation_3<K, TDS3D>												Triangulation3D;

// ------------------------------------------------------------------------------------------------------ //

typedef Triangulation3D::Vertex														Vertex;
typedef Triangulation3D::Vertex_handle												VertexHandle;
typedef Triangulation3D::Cell														Cell;
typedef Triangulation3D::Cell_handle												CellHandle;
typedef Triangulation3D::Facet														Facet;
typedef Triangulation3D::Edge														Edge;
typedef Triangulation3D::Finite_vertices_iterator									VertexIterator;
typedef Triangulation3D::Finite_cells_iterator										CellIterator;
typedef Triangulation3D::Finite_edges_iterator										EdgeIterator;
typedef Triangulation3D::Edge														Edge;

// ------------------------------------------------------------------------------------------------------ //

typedef LA::Matrix																	Matrix;
typedef LA::Vector																	Vector;

// ------------------------------------------------------------------------------------------------------ //

typedef std::map<int, Triangulation3D::Point >										intTriPointMap;
typedef std::map<int, std::vector<int> >											intIntsMap;
typedef std::map<Point3D, std::pair<double, int> >									pointDoubleIntMap;
typedef std::map<int, VertexHandle >												vertexHandleMap;
typedef std::map<int, CellHandle >													cellHandleMap;


