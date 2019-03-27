#include "boost_predefine.h"



#include "boost_thread_msgque_condi.h"
#include "boost_thread_msgque_sem.h"


//boost::mutex io_mutex;

//zen_message_queue_condi<int, std::list<int> >   message_queue_(100);
//zen_message_queue_condi <int,boost::circular_buffer<int> > message_queue_(100);

//
//void producer(size_t number_prc)
//{
//    for(size_t i=0;i<number_prc;++i)
//    {
//        message_queue_.enqueue(i);
//
//        {
//            boost::mutex::scoped_lock lock(io_mutex);
//            std::cout<<"queue size:"<< message_queue_.size() <<" input data "<<i<<std::endl;
//        }
//    }
//
//}
//
//void consumer(size_t number_prc)
//{
//    int x;
//    ::Sleep(1000);
//    for(size_t i=0;i<number_prc;++i)
//    {
//        message_queue_.dequeue(x);
//
//        {
//            boost::mutex::scoped_lock lock(io_mutex);
//            std::cout<<"queue size:"<< message_queue_.size() <<" output data "<<x<<std::endl;
//        }
//    }
//
//}
//
//int test_smem_main3 (int /*argc*/, char * /*argv*/ [])
//{
//    boost::thread t1(producer,2000);
//    boost::thread t2(consumer,1000);
//    boost::thread t3(consumer,1000);
//
//    t1.join();
//    t2.join();
//    t3.join();
//    return 0;
//}
