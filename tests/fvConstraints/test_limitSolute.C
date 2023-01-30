/*---------------------------------------------------------------------------*\
Application
    test_limitSolute

Description
    Unit testing for limitSolute.

Author
    Bruno Lebon

\*---------------------------------------------------------------------------*/

#define BOOST_TEST_MODULE Check_limitSolute
#include <boost/test/included/unit_test.hpp>

#include "fvCFD.H"
#include "dynamicFvMesh.H"
#include "multicomponentAlloy.H"
#include "fluidThermo.H"
#include "fvConstraints.H"
// #include "localEulerDdtScheme.H"

namespace utf = boost::unit_test;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

struct F
{
    F()
        : argc(utf::framework::master_test_suite().argc),
          argv(utf::framework::master_test_suite().argv)
    {
        #include "setRootCaseLists.H"
        #include "createTime.H"
        #include "createDynamicFvMesh.H"
        #include "createFields.H"

        Info << "\nStarting limitSolute tests\n" << endl;
    }

    ~F()
    {
        Info << "\nEnd\n" << endl;
    }

    int argc;
    char **argv;
};

BOOST_FIXTURE_TEST_SUITE(CheckLimitSoluteFvConstraint, F);

    BOOST_AUTO_TEST_CASE(CheckIfLimitSoluteFvConstraintHasBeenRead)
    {       
        Info << "CheckIfLimitSoluteFvConstraintHasBeenRead" << endl;
    }

BOOST_AUTO_TEST_SUITE_END();

// ************************************************************************* //
