/*---------------------------------------------------------------------------*\
Application
    test_waterFilmHTC

Description
    Unit testing for waterFilmHTC.

Author
    Bruno Lebon

\*---------------------------------------------------------------------------*/

#define BOOST_TEST_MODULE Check_waterFilmHTC
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
        BOOST_TEST_MESSAGE("\nStarting waterFilmHTC tests\n");
    }

    ~F()
    {
        Info << "\nEnd\n" << endl;
    }

    public:
    int argc;
    char **argv;
};

BOOST_FIXTURE_TEST_SUITE(CheckWaterFilmHTC, F);

    BOOST_AUTO_TEST_CASE(CheckIfWaterFilmHTCFvModelHasBeenRead)
    {       
        #include "setRootCaseLists.H"
        #include "createTime.H"
        #include "createDynamicFvMesh.H"
        #include "createFields.H"
        
        // Get patches list and T
        const volScalarField& T = mesh.lookupObject<volScalarField>("T");
        const polyPatchList& patches = mesh.boundaryMesh();

        BOOST_TEST_MESSAGE("-- Checking if a waterFilmHTC boundary condition entry has been applied");
        forAll(T.boundaryField(), patchI)
        {
            if (patches[patchI].name() == "water-film")
            {
                BOOST_REQUIRE_EQUAL(T.boundaryField()[patchI].type(), "waterFilmHTC");
            }
        }
    }

BOOST_AUTO_TEST_SUITE_END();

// ************************************************************************* //
