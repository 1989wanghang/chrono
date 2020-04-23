// Ensure that generated C# code does not use 'override' for various virtual methods.
// This is because solvers use multiple inheritance and SWIG ignores all but the first base class.

%csmethodmodifiers chrono::ChSolverAPGD::GetType "public"
%csmethodmodifiers chrono::ChSolverBB::GetType "public"
%csmethodmodifiers chrono::ChSolverPJacobi::GetType "public"
%csmethodmodifiers chrono::ChSolverPSOR::GetType "public"
////%csmethodmodifiers chrono::ChSolverBiCGSTAB::GetType "public"
////%csmethodmodifiers chrono::ChSolverGMRES::GetType "public"
////%csmethodmodifiers chrono::ChSolverMINRES::GetType "public"

%csmethodmodifiers chrono::ChSolverAPGD::GetError "public"
%csmethodmodifiers chrono::ChSolverBB::GetError "public"
%csmethodmodifiers chrono::ChSolverPJacobi::GetError "public"
%csmethodmodifiers chrono::ChSolverPSOR::GetError "public"
%csmethodmodifiers chrono::ChSolverBiCGSTAB::GetError "public"
%csmethodmodifiers chrono::ChSolverGMRES::GetError "public"
%csmethodmodifiers chrono::ChSolverMINRES::GetError "public"

%csmethodmodifiers chrono::ChIterativeSolverVI::GetIterations "public"
%csmethodmodifiers chrono::ChSolverBiCGSTAB::GetIterations "public"
%csmethodmodifiers chrono::ChSolverGMRES::GetIterations "public"
%csmethodmodifiers chrono::ChSolverMINRES::GetIterations "public"


%{

/* Includes the header in the wrapper code */
#include <cstdlib>
#include <cmath>

#include "chrono/solver/ChSolver.h"
#include "chrono/solver/ChSolverVI.h"
#include "chrono/solver/ChSolverLS.h"
#include "chrono/solver/ChDirectSolverLS.h"
#include "chrono/solver/ChIterativeSolver.h"
#include "chrono/solver/ChIterativeSolverLS.h"
#include "chrono/solver/ChIterativeSolverVI.h"

//#include "chrono/solver/ChSolverPMINRES.h"
#include "chrono/solver/ChSolverBB.h"
#include "chrono/solver/ChSolverAPGD.h"
#include "chrono/solver/ChSolverPSOR.h"
#include "chrono/solver/ChSolverPJacobi.h"

using namespace chrono;
%}

// Tell SWIG about parent class in Python

/* Parse the header file to generate wrappers */

%shared_ptr(chrono::ChSolver)
%shared_ptr(chrono::ChSolverVI)
%shared_ptr(chrono::ChSolverLS)
%shared_ptr(chrono::ChDirectSolverLS)
%shared_ptr(chrono::ChIterativeSolver)
%shared_ptr(chrono::ChIterativeSolverLS)
%shared_ptr(chrono::ChIterativeSolverVI)

//%shared_ptr(chrono::ChSolverPMINRES)
%shared_ptr(chrono::ChSolverGMRES)
%shared_ptr(chrono::ChSolverBiCGSTAB)
%shared_ptr(chrono::ChSolverMINRES)
%shared_ptr(chrono::ChSolverBB)
%shared_ptr(chrono::ChSolverAPGD)
%shared_ptr(chrono::ChSolverPSOR)
%shared_ptr(chrono::ChSolverPJacobi)
%shared_ptr(chrono::ChSolverSparseLU)
%shared_ptr(chrono::ChSolverSparseQR)

%include "../chrono/solver/ChSolver.h"
%include "../chrono/solver/ChSolverVI.h"
%include "../chrono/solver/ChSolverLS.h"
%include "../chrono/solver/ChDirectSolverLS.h"
%include "../chrono/solver/ChIterativeSolver.h"
%include "../chrono/solver/ChIterativeSolverLS.h"
%include "../chrono/solver/ChIterativeSolverVI.h"


//%ignore chrono::ChSolverMINRES::GetType;
//%include "../chrono/solver/ChSolverPMINRES.h"
%include "../chrono/solver/ChSolverBB.h"
%include "../chrono/solver/ChSolverAPGD.h"
%include "../chrono/solver/ChSolverPSOR.h"
%include "../chrono/solver/ChSolverPJacobi.h"






