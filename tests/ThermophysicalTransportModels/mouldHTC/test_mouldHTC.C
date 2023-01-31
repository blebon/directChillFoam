/*---------------------------------------------------------------------------*\
Application
    test_mouldHTC

Description
    Unit testing for mouldHTC.

Author
    Bruno Lebon

\*---------------------------------------------------------------------------*/

#define BOOST_TEST_MODULE Check_mouldHTC
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
        BOOST_TEST_MESSAGE("\nStarting mouldHTC tests\n");
    }

    ~F()
    {
        Info << "\nEnd\n" << endl;
    }

    public:
    int argc;
    char **argv;
};

BOOST_FIXTURE_TEST_SUITE(CheckMouldHTC, F);

    BOOST_AUTO_TEST_CASE(CheckIfMouldHTCFvModelHasBeenRead)
    {       
        #include "setRootCaseLists.H"
        #include "createTime.H"
        #include "createDynamicFvMesh.H"
        #include "createFields.H"
        
        // Get patches list and T
        const volScalarField& T = mesh.lookupObject<volScalarField>("T");
        const polyPatchList& patches = mesh.boundaryMesh();

        BOOST_TEST_MESSAGE("-- Checking if a mouldHTC boundary condition entry has been applied");
        forAll(T.boundaryField(), patchI)
        {
            if (patches[patchI].name() == "air-gap")
            {
                BOOST_REQUIRE_EQUAL(T.boundaryField()[patchI].type(), "mouldHTC");
            }
            else if (patches[patchI].name() == "mould")
            {
                BOOST_REQUIRE_EQUAL(T.boundaryField()[patchI].type(), "mouldHTC");
            }
        }
    }

BOOST_AUTO_TEST_SUITE_END();

// ************************************************************************* //
