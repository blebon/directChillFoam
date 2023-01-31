/*---------------------------------------------------------------------------*\
Application
    test_soluteModel

Description
    Unit testing for soluteModel.

Author
    Bruno Lebon

\*---------------------------------------------------------------------------*/

#define BOOST_TEST_MODULE Check_soluteModel
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

BOOST_FIXTURE_TEST_SUITE(ChecksoluteModel, F);

    BOOST_AUTO_TEST_CASE(CheckIfsoluteModelHasBeenRead)
    {       
        #include "setRootCaseLists.H"
        #include "createTime.H"
        #include "createDynamicFvMesh.H"
        #include "createFields.H"
        
        BOOST_TEST_MESSAGE("-- Checking if solute model has been created");
        BOOST_REQUIRE_EQUAL(alloy.name(), "alloy");

        BOOST_TEST_MESSAGE("-- Checking if C.Cu solute properties were correctly read");
        forAllIter(PtrDictionary<soluteModel>, alloy.solutes(), solute)
        {
            BOOST_REQUIRE_EQUAL(solute().name(), "Cu");
            BOOST_REQUIRE_EQUAL(solute().keyword(), "Cu");
            if (solute().name() == "Cu")
            {
                BOOST_REQUIRE_CLOSE_FRACTION(solute().D_l().value(), 5.66e-09, 1e-9);
                BOOST_REQUIRE_CLOSE_FRACTION(solute().kp().value(), 0.171, 0.001);
                BOOST_REQUIRE_CLOSE_FRACTION(solute().C0().value(), 0.06, 0.01);
                BOOST_REQUIRE_CLOSE_FRACTION(solute().Ceut().value(), 0.32, 0.01);
                BOOST_REQUIRE_CLOSE_FRACTION(solute().beta().value(), -7.3e-3, 0.001);
            }
        }
    }

BOOST_AUTO_TEST_SUITE_END();

// ************************************************************************* //
