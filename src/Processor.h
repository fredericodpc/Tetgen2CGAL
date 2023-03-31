// --------------------------------------------------------- //
// ----------- PROCESSOR - PREVENTING REDEFINITION --------- //
// --------------------------------------------------------- //
#ifndef __PROCESSOR__
#define __PROCESSOR__

	// ----------------------------------------------------- //
	// ------------------ INCLUDE HEADERS ------------------ //
	// ----------------------------------------------------- //
	// STD library
	#include <iostream>
	#include <string>
	#include <stdlib.h>
	#include <fstream>
	#include <string>
	#include <chrono>
	// CGAL
	#include "CGALDef.h"
	// Boost
	#include "boost\filesystem.hpp"
	#include "boost\accumulators\accumulators.hpp"
	#include "boost\accumulators\statistics\stats.hpp"
	#include "boost\accumulators\statistics\mean.hpp"
	#include "boost\accumulators\statistics\variance.hpp"
	#include "boost\archive\text_oarchive.hpp"
	#include "boost\archive\text_iarchive.hpp"
	// ----------------------------------------------------- //
	// ----------------------- STRUCTS --------------------- //
	// ----------------------------------------------------- //
	struct meshStr{
		Triangulation3D						tri;
		intTriPointMap						nodeMap;
		intIntsMap							eleMap;
		intIntsMap							neighMap;
		pointDoubleIntMap					scalarFieldMap;
	};
	typedef struct meshStr meshStr;
	// ----------------------------------------------------- //
	// ---------------------- CLASS ------------------------ //
	// ----------------------------------------------------- //

	// ----------------------------------------------------- //
	// ------------------ CLASS DEFINITION ----------------- //
	// ----------------------------------------------------- //
	class Processor{
		public:
			// Default Constructor
			Processor(void);
			// Virtual Destructor
			virtual ~Processor(void);

		public:
			virtual void	read_mesh(const std::string testCaseName);
			virtual void	convert_mesh(void);
			
		public:
			meshStr				mesh;
	};

	// ----------------------------------------------------- //
	// -------- INLINE MEMBER FUNCTIONS DEFINITION --------- //
	// ----------------------------------------------------- //
	
#endif