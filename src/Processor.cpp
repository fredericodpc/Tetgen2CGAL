// --------------------------------------------------------- //
// -------------------- INCLUDE HEADERS -------------------- //
// --------------------------------------------------------- //
#include "Processor.h"

// --------------------------------------------------------- //
// -------------- MEMBER FUNCTIONS DEFINITION -------------- //
// --------------------------------------------------------- //
Processor::Processor(void){};
Processor::~Processor(void){};

// ************************************************************************************* //
// ************************************************************************************* //
// ************************************************************************************* //
// ************************************************************************************* //
void Processor::read_mesh(const std::string testCaseName){
	
	// -------------------------------------------------------------------- //
	// ACCESS THE DIRECTORY CONTAINING THE CONSTRAINED TETRAHEDRALIZATION
	// -------------------------------------------------------------------- //
	boost::filesystem::path currentPath = boost::filesystem::current_path();
	boost::filesystem::current_path(currentPath / "data_files");


	// ----------------------------------------------------------------------------- //
	// ".NODE", ".ELE" AND ".NEIGH" FILE NAMES
	// ----------------------------------------------------------------------------- //
	std::string nodeFile 	= testCaseName + ".1.node";
	std::string eleFile 	= testCaseName + ".1.ele";
	std::string neighFile 	= testCaseName + ".1.neigh";


	// ----------------------------------------------------------------------------- //
	// INITIALIZE STD::MAPS
	// ----------------------------------------------------------------------------- //
	vertexHandleMap				vertexMap;
	cellHandleMap				cellMap;


	// ----------------------------------------------------------------------------- //
	// READ ".NODE" FILE -> LIST OF VERTICES
	// IDX VERTEX - X-COORD - Y-COORD - Z-COORD - QUANTITY - BOUNDARY MARKER
	// ----------------------------------------------------------------------------- //
	std::string		line;
	std::fstream	nodeFileReader(nodeFile.c_str());

	if (!nodeFileReader){
		std::cout << "Error reading .NODE file!" << std::endl;
		system("pause");
	}
	else{
		while (std::getline(nodeFileReader, line)){

			int     idxV, bm;
			double  x, y, z, rho;

			std::stringstream lineStream;
			lineStream << line;

			lineStream >> idxV >> x >> y >> z >> rho >> bm;

			mesh.nodeMap.insert(std::make_pair(idxV, Triangulation3D::Point(x, y, z)));
			mesh.scalarFieldMap.insert(std::make_pair(Point3D(x, y, z), std::make_pair(rho, bm)));

		}
		nodeFileReader.close();
	}

	// ----------------------------------------------------------------------------- //
	// READ ".ELE" FILE -> LIST OF TETRAHEDRON
	// IDX TETRAHEDRON - IDX VERTEX 1 - IDX VERTEX 2 - IDX VERTEX 3 - IDX VERTEX 4
	// ----------------------------------------------------------------------------- //
	std::fstream 		eleFileReader(eleFile.c_str());

	if (!eleFileReader){
		std::cout << "Error reading .ELE file!" << std::endl;
		system("pause");
	}
	else{
		while (std::getline(eleFileReader, line)){
			int     idxC, idxV0, idxV1, idxV2, idxV3;

			std::stringstream lineStream;
			lineStream << line;

			lineStream >> idxC >> idxV0 >> idxV1 >> idxV2 >> idxV3;
			std::vector<int> tmp{ idxV0, idxV1, idxV2, idxV3 };
			// Ele std::map so be used later
			mesh.eleMap.insert(std::make_pair(idxC, tmp));

		}
		eleFileReader.close();
	}

	// ----------------------------------------------------------------------------- //
	// READ ".NEIGH" FILE -> LIST OF NEIGHBOURS
	// IDX TETRAHEDRON - IDX NEIGH 1 - IDX NEIGH 2 - IDX NEIGH 3 - IDX NEIGH 4
	// ----------------------------------------------------------------------------- //
	std::fstream						neighFileReader(neighFile.c_str());

	if (!neighFileReader){
		std::cout << "Error reading .NEIGH file!" << std::endl;
		system("pause");
	}
	else{
		while (std::getline(neighFileReader, line)){
			int     idxC, idxC0, idxC1, idxC2, idxC3;

			std::stringstream lineStream;
			lineStream << line;

			lineStream >> idxC >> idxC0 >> idxC1 >> idxC2 >> idxC3;
			std::vector<int> tmp{ idxC0, idxC1, idxC2, idxC3 };

			// Neigh std::map to be used later
			mesh.neighMap.insert(std::make_pair(idxC, tmp));

		}
		neighFileReader.close();
	}
}
// ************************************************************************************* //
// ************************************************************************************* //
// ************************************************************************************* //
// ************************************************************************************* //
void Processor::convert_mesh(void){
	// ----------------------------------------------------------------------------- //
	// ----------------------------------------------------------------------------- //
	// ----------------------------------------------------------------------------- //
	// ----------------------------------------------------------------------------- //

	// ----------------------------------------------------------------------------- //
	// INITIALIZE TETRAHEDRALIZATION
	// ----------------------------------------------------------------------------- //
	/*Triangulation3D				tri; -> mesh.tri*/


	// ----------------------------------------------------------------------------- //
	// CLEAR ANY TETRAHEDRALIZATION PARAMETERS SET DURING CONSTRUCTION
	// ----------------------------------------------------------------------------- //
	mesh.tri.tds().clear();

	// ----------------------------------------------------------------------------- //
	// SET TETRAHEDRALIZATION DIMENSION
	// ----------------------------------------------------------------------------- //
	mesh.tri.tds().set_dimension(3);

	// ----------------------------------------------------------------------------- //
	// CREATE FINITE VERTICES
	// ----------------------------------------------------------------------------- //
	// Vertex handle initialization
	VertexHandle	vH;

	// Iterate over finite vertices
	for (auto iterV = mesh.nodeMap.begin(); iterV != mesh.nodeMap.end(); iterV++){

		vH = mesh.tri.tds().create_vertex();

		// Set its point geometric coordinates
		vH->set_point(iterV->second);

		// Vertex std::map to be used later
		vertexMap.insert(std::make_pair(iterV->first, vH));
	}

	// ----------------------------------------------------------------------------- //
	// CREATE FINITE CELLS
	// ----------------------------------------------------------------------------- //
	// Cell handle initialization
	CellHandle		cH;

	// Iterate over finite cells
	for (auto iterC = mesh.eleMap.begin(); iterC != mesh.eleMap.end(); iterC++){

		cH = mesh.tri.tds().create_cell();
		// Tetgen already orients the cells' vertices
		// positively (clockwise) in the ".ele" file
		cH->set_vertices(	vertexMap.at(iterC->second[0]),
							vertexMap.at(iterC->second[1]),
							vertexMap.at(iterC->second[2]),
							vertexMap.at(iterC->second[3]));


		// Set incident cell to each one of the vertices
		vertexMap.at(iterC->second[0])->set_cell(cH);
		vertexMap.at(iterC->second[1])->set_cell(cH);
		vertexMap.at(iterC->second[2])->set_cell(cH);
		vertexMap.at(iterC->second[3])->set_cell(cH);

		// Cell std::map to be used later
		cellMap.insert(std::make_pair(iterC->first, cH));
	}

	// ----------------------------------------------------------------------------- //
	// CREATE INFINITE VERTEX
	// ----------------------------------------------------------------------------- //
	VertexHandle infV = mesh.tri.tds().create_vertex();
	mesh.tri.set_infinite_vertex(infV);

	// ----------------------------------------------------------------------------- //
	// ASSIGN FINITE NEIGHBOURS TO FINITE AND INFINITE CELLS
	// ----------------------------------------------------------------------------- //
	// Iterate over finite cells
	for (auto iterC = mesh.neighMap.begin(); iterC != mesh.neighMap.end(); iterC++){
		// Initialize cell handles
		CellHandle c, c00, c11, c22, c33, cNN, c0, c1, c2, c3;
		// Initialize finite/infinite neighbours identifier and neighbour indexing identifier
		std::vector<int> tmp1{ 1, 1, 1, 1 };
		std::vector<int> tmp2{ 0, 0, 0, 0 };

		// Finite cell
		c = cellMap.at(iterC->first);

		// Convex hull cell
		if ((iterC->second[0] == -1) | (iterC->second[1] == -1) | (iterC->second[2] == -1) | (iterC->second[3] == -1)){
			// *********************************************************** //
			// tmp1[i] == 1 -> Finite Cell | tmp1[i] == 0 -> Infinite Cell //
			// *********************************************************** //
			auto iter = iterC->second.begin();
			while ((iter = std::find(iter, iterC->second.end(), -1)) != iterC->second.end())
			{
				tmp1[iter - iterC->second.begin()] = 0;
				iter++;
			}

			// Iterate over neighbours
			for (int i = 0; i < 4; i++){
				// Finite neighbours
				if (tmp1[i] == 1){
					// Finite neighbour
					cNN = cellMap.at(iterC->second[i]);

					// **************************************** //
					// tmp2[i] == 1 -> Neighbour index assigned //
					// **************************************** //
					// Determine which vertices are shared between the two finite neighbours
					// and assign correspondent neighbour index
					// Ex.:
					// vertices shared: 0, 1, 2
					// neighbour index: 3
					if ((cNN->has_vertex(c->vertex(1)) == true) & (cNN->has_vertex(c->vertex(2)) == true) & (cNN->has_vertex(c->vertex(3)) == true)){
						c0 = cNN;
						tmp2[0] = 1;
					}
					else if ((cNN->has_vertex(c->vertex(0)) == true) & (cNN->has_vertex(c->vertex(2)) == true) & (cNN->has_vertex(c->vertex(3)) == true)){
						c1 = cNN;
						tmp2[1] = 1;
					}
					else if ((cNN->has_vertex(c->vertex(0)) == true) & (cNN->has_vertex(c->vertex(1)) == true) & (cNN->has_vertex(c->vertex(3)) == true)){
						c2 = cNN;
						tmp2[2] = 1;
					}
					else if ((cNN->has_vertex(c->vertex(0)) == true) & (cNN->has_vertex(c->vertex(1)) == true) & (cNN->has_vertex(c->vertex(2)) == true)){
						c3 = cNN;
						tmp2[3] = 1;
					}
				}
			}

			// Check if finite neighbours were assigned
			if (std::accumulate(tmp2.begin(), tmp2.end(), 0) != std::accumulate(tmp1.begin(), tmp1.end(), 0)){
				std::cout << "Issue assigning finite neighbor cells" << std::endl;
				system("pause");
			}


			// Create infinite cells = Infinite neighbours of finite convex hull cells
			// *************************************************************** //
			// ATTENTION: IF CGAL THROWS "PB ORIENTATION WITH NEIGHBOR" ERROR,
			// SIMPLY CHANGE THE VERTICES INDEXES/ORDERING ON CH->SET_VERTEX
			// *************************************************************** //
			if (tmp2[0] == 0){

				cH = mesh.tri.tds().create_cell();
				// The neighbour index of the finite cell not yet assigned determines the vertices indexes that comprise the convex hull facet
				// The auxiliary infinite vertex completes the infinite cell

				cH->set_vertex(0, mesh.tri.infinite_vertex());
				cH->set_vertex(1, c->vertex(3));
				cH->set_vertex(2, c->vertex(2));
				cH->set_vertex(3, c->vertex(1));

				c0 = cH;
				tmp2[0] = 1;

				mesh.tri.infinite_vertex()->set_cell(cH);

				// The index of the finite neighbour of the infinite cell is exactly the index of the infinite vertex
				cH->set_neighbor(cH->index(mesh.tri.infinite_vertex()), c);
			}
			if (tmp2[1] == 0){
				cH = mesh.tri.tds().create_cell();

				cH->set_vertex(0, c->vertex(2));
				cH->set_vertex(1, mesh.tri.infinite_vertex());
				cH->set_vertex(2, c->vertex(0));
				cH->set_vertex(3, c->vertex(3));

				c1 = cH;
				tmp2[1] = 1;

				mesh.tri.infinite_vertex()->set_cell(cH);
				cH->set_neighbor(cH->index(mesh.tri.infinite_vertex()), c);
			}
			if (tmp2[2] == 0){
				cH = mesh.tri.tds().create_cell();

				cH->set_vertex(0, c->vertex(1));
				cH->set_vertex(1, c->vertex(0));
				cH->set_vertex(2, mesh.tri.infinite_vertex());
				cH->set_vertex(3, c->vertex(3));

				c2 = cH;
				tmp2[2] = 1;


				mesh.tri.infinite_vertex()->set_cell(cH);
				cH->set_neighbor(cH->index(mesh.tri.infinite_vertex()), c);
			}
			if (tmp2[3] == 0){
				cH = mesh.tri.tds().create_cell();


				cH->set_vertex(0, c->vertex(2));
				cH->set_vertex(1, c->vertex(1));
				cH->set_vertex(2, c->vertex(0));
				cH->set_vertex(3, mesh.tri.infinite_vertex());

				c3 = cH;
				tmp2[3] = 1;

				mesh.tri.infinite_vertex()->set_cell(cH);
				cH->set_neighbor(cH->index(mesh.tri.infinite_vertex()), c);

			}

			// Set neighbours of finite convex hull cell
			if (std::accumulate(tmp2.begin(), tmp2.end(), 0) == 4){
				c->set_neighbors(c0, c1, c2, c3);
			}
			else{
				std::cout << "INFINITE NEIGHBOR MISSING" << std::endl;
				system("pause");
			}

		}
		// Interior cells
		else{
			// Interior cell
			c = cellMap.at(iterC->first);

			// Finite neighbours
			c00 = cellMap.at(iterC->second[0]);
			c11 = cellMap.at(iterC->second[1]);
			c22 = cellMap.at(iterC->second[2]);
			c33 = cellMap.at(iterC->second[3]);

			// Determine which vertices are shared between the two finite neighbours
			// and assign correspondent neighbour index
			// Ex.:
			// vertices shared: 0, 1, 2
			// neighbour index: 3
			if ((c00->has_vertex(c->vertex(1)) == true) & (c00->has_vertex(c->vertex(2)) == true) & (c00->has_vertex(c->vertex(3)) == true)){
				c0 = c00;
				tmp2[0] = 1;
			}
			else if ((c00->has_vertex(c->vertex(0)) == true) & (c00->has_vertex(c->vertex(2)) == true) & (c00->has_vertex(c->vertex(3)) == true)){
				c1 = c00;
				tmp2[1] = 1;
			}
			else if ((c00->has_vertex(c->vertex(0)) == true) & (c00->has_vertex(c->vertex(1)) == true) & (c00->has_vertex(c->vertex(3)) == true)){
				c2 = c00;
				tmp2[2] = 1;
			}
			else if ((c00->has_vertex(c->vertex(0)) == true) & (c00->has_vertex(c->vertex(1)) == true) & (c00->has_vertex(c->vertex(2)) == true)){
				c3 = c00;
				tmp2[3] = 1;
			}


			if ((c11->has_vertex(c->vertex(1)) == true) & (c11->has_vertex(c->vertex(2)) == true) & (c11->has_vertex(c->vertex(3)) == true)){
				c0 = c11;
				tmp2[0] = 1;
			}
			else if ((c11->has_vertex(c->vertex(0)) == true) & (c11->has_vertex(c->vertex(2)) == true) & (c11->has_vertex(c->vertex(3)) == true)){
				c1 = c11;
				tmp2[1] = 1;
			}
			else if ((c11->has_vertex(c->vertex(0)) == true) & (c11->has_vertex(c->vertex(1)) == true) & (c11->has_vertex(c->vertex(3)) == true)){
				c2 = c11;
				tmp2[2] = 1;
			}
			else if ((c11->has_vertex(c->vertex(0)) == true) & (c11->has_vertex(c->vertex(1)) == true) & (c11->has_vertex(c->vertex(2)) == true)){
				c3 = c11;
				tmp2[3] = 1;
			}


			if ((c22->has_vertex(c->vertex(1)) == true) & (c22->has_vertex(c->vertex(2)) == true) & (c22->has_vertex(c->vertex(3)) == true)){
				c0 = c22;
				tmp2[0] = 1;
			}
			else if ((c22->has_vertex(c->vertex(0)) == true) & (c22->has_vertex(c->vertex(2)) == true) & (c22->has_vertex(c->vertex(3)) == true)){
				c1 = c22;
				tmp2[1] = 1;
			}
			else if ((c22->has_vertex(c->vertex(0)) == true) & (c22->has_vertex(c->vertex(1)) == true) & (c22->has_vertex(c->vertex(3)) == true)){
				c2 = c22;
				tmp2[2] = 1;
			}
			else if ((c22->has_vertex(c->vertex(0)) == true) & (c22->has_vertex(c->vertex(1)) == true) & (c22->has_vertex(c->vertex(2)) == true)){
				c3 = c22;
				tmp2[3] = 1;
			}


			if ((c33->has_vertex(c->vertex(1)) == true) & (c33->has_vertex(c->vertex(2)) == true) & (c33->has_vertex(c->vertex(3)) == true)){
				c0 = c33;
				tmp2[0] = 1;
			}
			else if ((c33->has_vertex(c->vertex(0)) == true) & (c33->has_vertex(c->vertex(2)) == true) & (c33->has_vertex(c->vertex(3)) == true)){
				c1 = c33;
				tmp2[1] = 1;
			}
			else if ((c33->has_vertex(c->vertex(0)) == true) & (c33->has_vertex(c->vertex(1)) == true) & (c33->has_vertex(c->vertex(3)) == true)){
				c2 = c33;
				tmp2[2] = 1;
			}
			else if ((c33->has_vertex(c->vertex(0)) == true) & (c33->has_vertex(c->vertex(1)) == true) & (c33->has_vertex(c->vertex(2)) == true)){
				c3 = c33;
				tmp2[3] = 1;
			}

			// Set neighbours of finite interior cell
			if (std::accumulate(tmp2.begin(), tmp2.end(), 0) == 4){
				c->set_neighbors(c0, c1, c2, c3);
			}
			else{
				std::cout << " NEIGHBOR MISSING" << std::endl;
				system("pause");
			}


		}

	}

	// ----------------------------------------------------------------------------- //
	// ASSIGN INFINITE NEIGHBOURS TO INFINITE CELLS
	// ----------------------------------------------------------------------------- //
	int countNeigh = 0;
	// Iterate over all finite/infinite cells
	for (auto iterCell = mesh.tri.all_cells_begin(); iterCell != mesh.tri.all_cells_end(); iterCell++){

		// Find infinite cells - main infinite cell
		if (mesh.tri.is_infinite(iterCell) == true){

			// Get index of infinite vertex/finite neighbour
			int idxInf = iterCell->index(mesh.tri.infinite_vertex());

			// Iterate again over all finite/infinite cells
			for (auto iterN = mesh.tri.all_cells_begin(); iterN != mesh.tri.all_cells_end(); iterN++){

				// Find infinite cells different from main infinite cell - infinite neighbours
				if ((mesh.tri.is_infinite(iterN) == true) & (iterN != iterCell)){
					// Determine which of the finite edges is shared between main infinite cell and infinite neighbours
					// and assign correspondent neighbour
					// Ex.:
					// index of finite neighbour						= 0 
					// finite edges shared between infinite neighbours	= (v(1), v(2)), (v(1),v(3)), (v(2),v(3)) 
					// indexes of infinite neighbours					= 3,            2,            1
					if ((iterN->has_vertex(iterCell->vertex(((idxInf + 1) & 3))) == true) & (iterN->has_vertex(iterCell->vertex(((idxInf + 2) & 3))) == true)){
						iterCell->set_neighbor(((idxInf + 3) & 3), iterN);
					}
					else if ((iterN->has_vertex(iterCell->vertex(((idxInf + 1) & 3))) == true) & (iterN->has_vertex(iterCell->vertex(((idxInf + 3) & 3))) == true)){
						iterCell->set_neighbor(((idxInf + 2) & 3), iterN);
					}
					else if ((iterN->has_vertex(iterCell->vertex(((idxInf + 2) & 3))) == true) & (iterN->has_vertex(iterCell->vertex(((idxInf + 3) & 3))) == true)){
						iterCell->set_neighbor(((idxInf + 1) & 3), iterN);
					}
				}
			}
		}
	}


	// ----------------------------------------------------------------------------- //
	// FINAL CHECKS
	// ----------------------------------------------------------------------------- //		
	std::cout << "--------------------------------------------" << std::endl;
	std::cout << "Constrained Tetrahedralization:" << std::endl;
	mesh.tri.is_valid(true);
	std::cout << "Number of finite cells: " << mesh.tri.number_of_finite_cells() << std::endl;
	std::cout << "Number of finite facets: " << mesh.tri.number_of_finite_facets() << std::endl;
	std::cout << "Number of finite vertices: " << mesh.tri.number_of_vertices() << std::endl;
	std::cout << "--------------------------------------------" << std::endl;
	// -------------------------------------------------------------------- //
	
}
// ************************************************************************************* //
// ************************************************************************************* //
// ************************************************************************************* //
// ************************************************************************************* //

