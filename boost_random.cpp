#include "boost_predefine.h"


int test_random_main1(int argc,char *argv[])
{
    // produces randomness out of thin air
    uint16_t data_16 = 0;
    uint32_t data_32 = 0;
    uint64_t data_64 = 0;

    double  data_double = 0.0;


    // distribution that maps to 1..6
    boost::uniform_int<> six(-11,6);      

    boost::lagged_fibonacci19937 generator_lf19937;
    generator_lf19937.seed((uint32_t)time(NULL));
    data_32 = generator_lf19937();
    data_32 = six(generator_lf19937);

    boost::taus88  generator_taus88;
    generator_taus88.seed((uint32_t)time(NULL));
    data_32 = generator_taus88();
    data_double = generator_taus88();

    boost::mt19937 gen_mt19937;  
    gen_mt19937.seed((uint32_t)time(NULL));
    data_32 = gen_mt19937();
    data_64 = gen_mt19937();
    data_double = gen_mt19937();

    int int_32 = six(gen_mt19937);

    boost::uniform_int<uint64_t> haodayigeshu(0,0xFFFFFFFFFF); 
    uint64_t uint64_data =haodayigeshu(gen_mt19937);

    // see pseudo-random number generators

    // see random number distributions
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
        die(gen_mt19937, six);             // glues randomness with mapping
    int x = die();                      // simulate rolling a die

    //boost::uniform_01<> abc;

    return 0;
}

