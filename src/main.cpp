#include "Processor.h"

#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <vector>
#include <math.h>


int main(int argc, char * argv[]){
	
	// ************************************************************************************************************** //
	// ************************************************************************************************************** //
	// ************************************************************************************************************** //
	// ************************************************************************************************************** //
		
	// ******************************************************** //

	Processor * proc		= new Processor;

	// ******************************************************** //
	
	std::string meshFile	= "transonicNACA0012";

	// ******************************************************** //
	
	proc->read_mesh(meshFile);
	proc->convert_Mesh();
	system("pause");
	// ******************************************************** //

}
























