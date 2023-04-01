# Tetgen2CGAL

Object to read a Constrained Delaunay Tetrahedralization constructed from `Tetgen` and convert it to `CGAL` data structure. 


## Instructions
`Tetgen mesh` is given in the following ASCII files format located in /mesh_data/:
- .node: list of mesh 3D nodes

index | z | y | z | attribute (e.g. scalar field) | marker (e.g. 0 for interior and 1 for boundary)

- .ele: list of mesh tetrahedra

index | idx vertex 1 | idx vertex 2 | idx vertex 3 | idx vertex 4

- .neigh: list of tetrahedra neighbors

index | idx tetra 1 | idx tetra 2 | idx tetra 3 | idx tetra 4

`CGAL data structure`: 
CGAL::Triangulation_3<CGAL::Exact_predicates_inexact_consutrusctions_kernel, CGAL::Triangulation_data_structure_3<CGAL::Triangulation_vertex_base_3<CGAL::Exact_predicates_inexact_consutrusctions_kernel>,CGAL::Triangulation_cell_base_3<CGAL::Exact_predicates_inexact_consutrusctions_kernel>>>

## Requirements
+ CGAL
+ Boost

## Resources
H. Si. Tetgen, a delaunay-based quality tetrahedral mesh generator. ACM Transactions on Mathematical Software (TOMS), 41(2):1–36, 2015.

WIAS-Software. Tetgen A Quality Tetrahedral Mesh Generator and a 3D Delaunay Triangulator. https://wias-berlin.de/software/index.jsp?id=TetGen&lang=1#Documentation, 2023. Accessed: 2023-04-01.

The CGAL Project. CGAL User and Reference Manual. CGAL Editorial Board, 5.5.2 edition, 2023.

The CGAL Project. Cgal the computational geometry algorithms library. https://www.cgal.org/, 2023. Accessed: 2023-04-01.
 
## Author
Dr Frederico Dias Paulino da Costa









