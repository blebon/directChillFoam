#define BOOST_TEST_MODULE CheckTemperatures
#include <boost/test/included/unit_test.hpp>

#include <fstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

using namespace std;
namespace utf = boost::unit_test;

struct TemperatureVectors
{
    TemperatureVectors(string casename, string end_time = "800")
        : casename{casename},
          end_time{end_time}
    {
        expected = read_temperature("../" + casename + "_T.xy"); // Expected in test directory when run in build subdir
        numerical = read_temperature("../../postProcessing/" + casename + "/" + end_time + "/line_T.xy"); // Numerical result in postProcessing directory
    }

    ~TemperatureVectors()
    {

    }

    vector<double> read_temperature(string filename)
    {
        ifstream datafile(filename);
        vector<double> expected;
        double number;
        vector<string> line_array;
        string line;

        while(getline(datafile, line))
        {
            line_array.clear();
            boost::split(line_array, line, boost::is_any_of("\t "), boost::token_compress_on);
            if ( strcmp(line_array[0].c_str(), "#") == 0 ) continue; // Skip headers and comments
            number = stod(line_array[1]); // Temperatures are in the second column
            expected.push_back(number);
        }

        return expected;
    }

    public:
    //! Expected temperatures
    /*! Vector holding the expected temperatures. */
    vector<double> expected;
    //! Predicted temperatures
    /*! Vector holding the predicted temperatures. */
    vector<double> numerical;

    private:
    //! Case name
    /*! String holding the line along which temperatures are compared*/
    string casename;
    //! Case end time
    /*! String holding the time at which the simulation ends*/
    string end_time;
};

struct F {
  F() { BOOST_TEST_MESSAGE( "Setup fixture" ); }
  ~F() { BOOST_TEST_MESSAGE( "Teardown fixture" ); }
};

BOOST_FIXTURE_TEST_SUITE(CheckIfTemperaturesMatchExpectedValues, F);

    BOOST_AUTO_TEST_CASE(CheckIfCentrelineTemperaturesMatchExpectedValues)
    {
        BOOST_TEST_MESSAGE("Checking centreline temperatures");
        TemperatureVectors centreline{ "centreline" };
        BOOST_CHECK_EQUAL_COLLECTIONS(centreline.expected.begin(), centreline.expected.end(),
                                      centreline.numerical.begin(), centreline.numerical.end());
    }

    BOOST_AUTO_TEST_CASE(CheckIfMidradiusTemperaturesMatchExpectedValues)
    {
        BOOST_TEST_MESSAGE("Checking midradius temperatures");
        TemperatureVectors midradius{ "midradius" };
        BOOST_CHECK_EQUAL_COLLECTIONS(midradius.expected.begin(), midradius.expected.end(),
                                      midradius.numerical.begin(), midradius.numerical.end());
    }

    BOOST_AUTO_TEST_CASE(CheckIfSubsurfaceTemperaturesMatchExpectedValues)
    {
        BOOST_TEST_MESSAGE("Checking subsurface temperatures");
        TemperatureVectors subsurface{ "subsurface" };
        BOOST_CHECK_EQUAL_COLLECTIONS(subsurface.expected.begin(), subsurface.expected.end(),
                                      subsurface.numerical.begin(), subsurface.numerical.end());
    }

BOOST_AUTO_TEST_SUITE_END();