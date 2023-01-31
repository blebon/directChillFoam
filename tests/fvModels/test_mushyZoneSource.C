/*---------------------------------------------------------------------------*\
Application
    test_mushyZoneSource

Description
    Unit testing for mushyZoneSource.

Author
    Bruno Lebon

\*---------------------------------------------------------------------------*/

#define BOOST_TEST_MODULE Check_mushyZoneSource
#include <boost/test/included/unit_test.hpp>

#include "fvCFD.H"
#include "dynamicFvMesh.H"
#include "multicomponentAlloy.H"
#include "fluidThermo.H"
#include "fvModels.H"

namespace utf = boost::unit_test;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

struct F
{
    F()
        : argc(utf::framework::master_test_suite().argc),
          argv(utf::framework::master_test_suite().argv)
    {
        BOOST_TEST_MESSAGE("\nStarting mushyZoneSource tests\n");
    }

    ~F()
    {
        Info << "\nEnd\n" << endl;
    }

    public:
    int argc;
    char **argv;
};

BOOST_FIXTURE_TEST_SUITE(CheckmushyZoneSourceFvModel, F);

    BOOST_AUTO_TEST_CASE(CheckIfmushyZoneSourceFvModelHasBeenRead)
    {       
        #include "setRootCaseLists.H"
        #include "createTime.H"
        #include "createDynamicFvMesh.H"
        #include "createFields.H"
        
        BOOST_TEST_MESSAGE("-- Checking if a mushyZoneSource dictionary entry has been read");
        BOOST_WARN_EQUAL(fvModels.PtrListDictionary<fvModel>::size(), 1);

        PtrListDictionary<fvModel>& modelsList(fvModels);
        auto melt1 = modelsList[0];

        BOOST_TEST_MESSAGE("-- Checking if melt1 mushyZoneSource dictionary has been read");
        BOOST_REQUIRE_EQUAL(melt1.name(), "melt1");

        BOOST_REQUIRE_CLOSE(melt1.coeffs().lookup<scalar>("Tsol"), 820.98, 0.1);
        BOOST_REQUIRE_CLOSE(melt1.coeffs().lookup<scalar>("Tliq"), 913.13, 0.1);
        BOOST_REQUIRE_CLOSE(melt1.coeffs().lookup<scalar>("L"), 392000.0, 0.1);        
        BOOST_REQUIRE_CLOSE(melt1.coeffs().lookup<scalar>("rhoRef"), 2573., 0.1);
        BOOST_REQUIRE_CLOSE(melt1.coeffs().lookup<scalar>("Cu"), 1e5, 0.1);
        BOOST_REQUIRE_CLOSE(melt1.coeffs().lookup<scalar>("beta"), 2.25e-5, 1e-7);

        BOOST_REQUIRE_CLOSE_FRACTION(melt1.coeffs().lookup<scalar>("g_env"), 0.7, 0.01);
        BOOST_REQUIRE_CLOSE_FRACTION(melt1.coeffs().lookup<scalar>("relax"), 0.2, 0.01);
        BOOST_REQUIRE_CLOSE_FRACTION(melt1.coeffs().lookup<scalar>("q"), 1e-6, 1e-7);
    }

BOOST_AUTO_TEST_SUITE_END();

// ************************************************************************* //
