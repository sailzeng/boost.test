#include "boost_predefine.h"



using namespace boost::property_tree;

int test_ptree_main1(int ,char *[])
{
    ptree pt;
    read_xml("navigation-1.xml",pt);

    int abc = pt.get<int>("NAVIGATIONTREE.ABC");

    return 0;
}
