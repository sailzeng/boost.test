#include "boost_predefine.h"


int fun_abc(int a,int b,int c)
{
    return a+b+c;
}

typedef int (*fp_fun_abc)(int ,int ,int );

int test_result_of_main(int argc, char *argv[])
{
    

    boost::result_of<fp_fun_abc(int,int,int)>::type i=0;

    //BOOST_TYPEOF(fp_fun_abc(int,int,int)) j = 0;

    BOOST_AUTO(v,fun_abc(1,2,3));

    BOOST_TYPEOF(fun_abc(1,2,3)) j = 0;

    return 0;
}


