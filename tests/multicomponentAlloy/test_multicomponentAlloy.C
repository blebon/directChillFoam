/*---------------------------------------------------------------------------*\
Application
    test_multicomponentAlloy

Description
    Unit testing for multicomponentAlloy.

Author
    Bruno Lebon

\*---------------------------------------------------------------------------*/

#define BOOST_TEST_MODULE Check_multicomponentAlloy
#include <boost/test/included/unit_test.hpp>

#include "fvCFD.H"
#include "dynamicFvMesh.H"
#include "multicomponentAlloy.H"
#include "fluidThermo.H"

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

BOOST_FIXTURE_TEST_SUITE(CheckMulticomponentAlloy, F);

    BOOST_AUTO_TEST_CASE(CheckIfMulticomponentAlloyHasBeenCreated)
    {       
        #include "setRootCaseLists.H"
        #include "createTime.H"
        #include "createDynamicFvMesh.H"
        #include "createFields.H"
        
        BOOST_TEST_MESSAGE("-- Checking if multicomponent alloy has been created");
        BOOST_WARN_EQUAL(alloy.name(), "alloy");
        BOOST_WARN_EQUAL(alloy.keyword(), "alloy");
    }

BOOST_AUTO_TEST_SUITE_END();

// ************************************************************************* //
