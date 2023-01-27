#define BOOST_TEST_MODULE CheckTemperatures
#include <boost/test/included/unit_test.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

using namespace std;
namespace fs = std::filesystem;
namespace utf = boost::unit_test;

struct TemperatureVectors
{
    TemperatureVectors(string casename, string end_time = "100", double tolerance = 3.0)
        : casename{casename},
          end_time{end_time},
          tolerance{tolerance}
    {
        string expected_file{"../" + casename + "_T.xy"}; // Expected in test directory when run in build subdir
        string numerical_file{"../../postProcessing/" + casename + "/" + end_time + "/line_T.xy"};  // Numerical result in postProcessing directory
        // Read benchmark temperatures
        expected = read_temperature(expected_file);
        // Read prediction temperatures
        numerical = read_temperature(numerical_file);
        // Check if files exist
        fs::path expected_path{expected_file};
        fs::path numerical_path{numerical_file};
        BOOST_ASSERT_MSG(fs::exists(expected_path), "-- Benchmark file not found!");
        BOOST_ASSERT_MSG(fs::exists(numerical_path), "-- Prediction file not found!");
    }

    ~TemperatureVectors()
    {

    }

    // Read temperatures column in filename
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

    // Check if predicted temperatures agree with benchmark
    void boost_check()
    {
        BOOST_TEST_MESSAGE("Checking " + casename + " temperatures");
        for (auto e = expected.begin(), n= numerical.begin();
                  e != expected.end() && n != numerical.end(); 
                  ++e, ++n)
        {
            BOOST_CHECK_CLOSE(*e, *n, tolerance);
        };
        // BOOST_CHECK_CLOSE(expected.begin(), numerical.begin(), 1e-6);
        // BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(),
        //                               numerical.begin(), numerical.end(), tolerance);
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
    //! Comparison tolerance
    /*! Double containing the tolerance for floating point comparison*/
    double tolerance;
};

struct F {
  F() { BOOST_TEST_MESSAGE( "\nSetup fixture" ); }
  ~F() { BOOST_TEST_MESSAGE( "Teardown fixture" ); }
};

BOOST_FIXTURE_TEST_SUITE(CheckIfTemperaturesMatchExpectedValues, F);

    BOOST_AUTO_TEST_CASE(CheckIfCentrelineTemperaturesMatchExpectedValues)
    {
        string casename{"x0mm"};
        TemperatureVectors temperatures{casename};
        temperatures.boost_check();
    }

    BOOST_AUTO_TEST_CASE(CheckIfMidradiusTemperaturesMatchExpectedValues)
    {
        string casename{"x325mm"};
        TemperatureVectors temperatures{casename};
        temperatures.boost_check();
    }

    BOOST_AUTO_TEST_CASE(CheckIfSubsurfaceTemperaturesMatchExpectedValues)
    {
        string casename{"x62mm"};
        TemperatureVectors temperatures{casename};
        temperatures.boost_check();
    }

BOOST_AUTO_TEST_SUITE_END();