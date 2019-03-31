
#ifndef _BOOST_PREDEFINE_H_
#define _BOOST_PREDEFINE_H_


//很多头文件以及数值定义本来可以放到各个OS的配置文件里面去的，但是感觉比较重要，还是放到这个地方了
// pretend it's at least Windows XP or Win2003，如果不定义这个，有时候会有一些API无法使用的问题
#if !defined (_WIN32_WINNT)
#if (defined ZCE_SUPPORT_WINSVR2008) && (ZCE_SUPPORT_WINSVR2008 == 1)
# define _WIN32_WINNT 0x0600
#else
# define _WIN32_WINNT 0x0501
#endif
#endif

#include <winsock2.h>
#include <MSWSock.h>
#include <winerror.h>
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

#include <cstdlib>
#include <iostream>
#include <boost/aligned_storage.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>


#include <boost/lockfree/queue.hpp>


#include <boost/atomic.hpp>

#include <boost/serialization/config.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>


#include <boost/aligned_storage.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>


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


int test_asio_main1(int argc, char *argv[]);

#endif //_BOOST_PREDEFINE_H_

