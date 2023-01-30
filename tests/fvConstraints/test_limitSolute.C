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

namespace utf = boost::unit_test;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

struct F
{
    F()
        : argc(utf::framework::master_test_suite().argc),
          argv(utf::framework::master_test_suite().argv)
    {
        BOOST_TEST_MESSAGE("\nStarting limitSolute tests\n");
    }

    ~F()
    {
        Info << "\nEnd\n" << endl;
    }

    public:
    int argc;
    char **argv;
};

BOOST_FIXTURE_TEST_SUITE(CheckLimitSoluteFvConstraint, F);

    BOOST_AUTO_TEST_CASE(CheckIfLimitSoluteFvConstraintHasBeenRead)
    {       
        #include "setRootCaseLists.H"
        #include "createTime.H"
        #include "createDynamicFvMesh.H"
        #include "createFields.H"
        
        BOOST_TEST_MESSAGE("-- Checking if a limitSolute dictionary entry has been read");
        BOOST_WARN_EQUAL(fvConstraints.PtrListDictionary<fvConstraint>::size(), 1);

        BOOST_TEST_MESSAGE("-- Checking if C.Cu limitSolute dictionary has been read");
        BOOST_REQUIRE_EQUAL(fvConstraints.constrainsField("C.Cu"), 1);
        BOOST_REQUIRE_EQUAL(fvConstraints.constrainsField("C.Si"), 0);
    }

BOOST_AUTO_TEST_SUITE_END();

// ************************************************************************* //
