
#ifndef _BOOST_PREDEFINE_H_
#define _BOOST_PREDEFINE_H_

#define _STLP_DEBUG 1
//#define _STLP_USE_BOOST_SUPPORT 1  这个宏不要用，要增加的化STLPort代码还有修正一下，
//#define _STLP_USE_STATIC_LIB 1




#include <Windows.h>
#include <Winbase.h>

//STL
#include <cstdlib> 
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstddef>
#include <cstdlib>


//Boost
#pragma warning (disable :4099)
#pragma warning (disable :4819)
#pragma warning (disable :4996)
#pragma warning (disable:4244)

#include <boost/any.hpp>
#include <boost/any.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/utility/result_of.hpp>


#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/allocators/allocator.hpp>



#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/random.hpp>

//#include <boost/archive/basic_text_iprimitive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/tmpdir.hpp>


#include <boost/lockfree/queue.hpp>


#include <boost/atomic.hpp>

#include <boost/serialization/config.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>


#pragma warning (default:4099)
#pragma warning (default:4819)
#pragma warning (default:4996)
#pragma warning (default:4244)


int test_random_main1(int ,char *[]);

int test_ptree_main1(int ,char *[]);

int test_smem_main1 (int argc, char *argv[]);
int test_smem_main2 (int argc, char *argv[]);
int test_smem_main3 (int argc, char *argv[]);

int test_serialize_main(int argc, char *argv[]);

int test_result_of_main(int argc, char *argv[]);


int test_shmem_vector_main(int argc, char *argv[]);
int test_shmem_list_main(int argc, char *argv[]);

int test_lockfree(int argc, char *argv[]);


#endif //_BOOST_PREDEFINE_H_

