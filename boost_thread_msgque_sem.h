/************************************************************************************************************
Copyright           : 2004-2011  Tencent Technology (Shenzhen) Company Limited.
FileName            : zen_thread_msgque_sem.h
Author              : Sailzeng ZENGXING
Version             : 
Date Of Creation    : 2011年6月17日
Description         : 

Others              : 
Function List       : 
    1.  ......
Modification History:
    1.Date  :
      Author  :
      Modification  :
************************************************************************************************************/


#ifndef _ZEN_LIB_THREAD_MESSAGE_QUEUE_SEMAPHORE_H_ 
#define _ZEN_LIB_THREAD_MESSAGE_QUEUE_SEMAPHORE_H_




/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011年6月18日
Template Param  : 
  Param1: class _value_type 消息队列放入的数据类型
  Param2: class _container_type =std::deque<_value_type> 消息队列内部容器类型
Class           : zen_message_queue_sem
Inherit         : public boost::noncopyable
Description     : 用信号灯+容器实现的消息队列，对于我个人来说，还是信号灯好理解一些
Other           : 
Modify Record   : 
************************************************************************************************************/
template <class _value_type,class _container_type > 
class zen_message_queue_sem : public boost::noncopyable
{
protected:

    //QUEUE的最大尺寸
    std::size_t                                    queue_max_size_;

    //由于LIST的size()函数比较耗时，所以这儿还是用了个计数器，而不直接使用_container_type.size()
    std::size_t                                    queue_cur_size_;

    //队列的LOCK,用于读写操作的同步控制
    boost::recursive_timed_mutex                   queue_lock_;

    //信号灯，满的信号灯
    boost::interprocess::interprocess_semaphore    sem_full_;

    //信号灯，空的信号灯，当数据
    boost::interprocess::interprocess_semaphore    sem_empty_;

    //容器类型，可以是list,dequeue,
    typename _container_type                       message_queue_;

public:

    //
    zen_message_queue_sem(size_t queue_max_size):
        queue_max_size_(queue_max_size),      
        queue_cur_size_(0),
        sem_full_(queue_max_size),
        sem_empty_(0)
    {
    }

    ~zen_message_queue_sem()
    {
    }

    //QUEUE是否为NULL
    inline bool empty()
    {
        boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
        if (queue_cur_size_ == 0)
        {
            return true;
        }
        return false;
    }

    //QUEUE是否为满
    inline bool full()
    {
        boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
        if (queue_cur_size_ == queue_max_size_)
        {
            return true;
        }
        return false;
    }


    //
    int enqueue(const _value_type& value_data,
        boost::posix_time::time_duration &wait_time)
    {

        bool bret = false;
        bret = sem_full_.timed_wait(wait_time);
        //如果超时了，返回false
        if (!bret)
        {
            //error = ETIME;
            return -1;
        }

        //注意这段代码必须用{}保护，因为你必须先保证数据取出
        {
            boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
            
            message_queue_.push_back(value_data);
            ++queue_cur_size_;
        }
        sem_empty_.post();

        return 0;
    }

    int enqueue(const _value_type& value_data)
    {
        boost::posix_time::time_duration wait_time = boost::posix_time::hours(1024);
        return enqueue(value_data,
            wait_time);
    }

    //
    int dequeue(_value_type& value_data,
        boost::posix_time::time_duration &wait_time)
    {

        bool bret = false;
        bret = sem_empty_.timed_wait(wait_time);
        //如果超时了，返回false
        if (!bret)
        {
            //error = ETIME;
            return -1;
        }
        
        //注意这段代码必须用{}保护，因为你必须先保证数据取出
        {
            boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
            //
            value_data = *message_queue_.begin();
            message_queue_.pop_front();
            --queue_cur_size_;
        }
        sem_full_.post();
            
        return 0;
    }

    //
    int dequeue(_value_type& value_data)
    {
        boost::posix_time::time_duration wait_time = boost::posix_time::hours(1024);
        return dequeue(value_data,
            wait_time);
    }

    //清理消息队列
    void clear()
    {
        boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
        message_queue_.clear();
        queue_cur_size_ = 0;
    }

    //返回消息对象的尺寸
    size_t size()
    {
        boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
        return queue_cur_size_;
    }
};

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011年6月17日
Template Param  : 
  Param1: class _value_type 消息队列保存的数据类型
Class           : zen_condi_msgqueue_list
Inherit         : public zen_message_queue_sem<_value_type,std::list>
Description     : 内部用LIST实现的消息队列，性能低,边界保护用的条件变量。但一开始占用内存不多
Other           : 主要就是为了给你一些语法糖
Modify Record   : 
************************************************************************************************************/
template <class _value_type >
class zen_smem_msgqueue_list : public zen_message_queue_sem<_value_type,std::list<_value_type> >
{
    //
    zen_smem_msgqueue_list(size_t queue_max_size):
        queue_max_size_(queue_max_size),  
        queue_cur_size_(0)
    {
    }

    ~zen_smem_msgqueue_list()
    {
    }
};

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011年6月17日
Template Param  : 
  Param1: class _value_type 消息队列保存的数据类型
Class           : zen_condi_msgqueue_deque
Inherit         : public zen_message_queue_sem<_value_type,std::deque<_value_type> >
Description     : 内部用DQUEUE实现的消息队列，性能较好,边界保护用的条件变量。
Other           : 封装的主要就是为了给你一些语法糖
Modify Record   : 
************************************************************************************************************/
template <class _value_type >
class zen_smem_msgqueue_deque : public zen_message_queue_sem<_value_type,std::deque<_value_type> >
{
    //
    zen_smem_msgqueue_deque(size_t queue_max_size):
        queue_max_size_(queue_max_size),  
        queue_cur_size_(0)
    {
    }

    ~zen_smem_msgqueue_deque()
    {
    }
};

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011年6月17日
Template Param  : 
  Param1: class _value_type 消息队列保存的数据类型
Class           : zen_condi_msgqueue_ring
Inherit         : public zen_message_queue_sem<_value_type,boost::circular_buffer<_value_type> >
Description     : 内部用circular_buffer实现的消息队列，性能非常好,边界保护用信号灯，的消息队列，但空间比较费
Other           : 这个封装的主要不光是了为了给你语法糖，而且是为了极限性能
Modify Record   : 
************************************************************************************************************/
template <class _value_type >
class zen_smem_msgqueue_ring : public zen_message_queue_sem<_value_type,boost::circular_buffer<_value_type> >
{
    //
    zen_smem_msgqueue_ring(size_t queue_max_size):
        queue_max_size_(queue_max_size),  
        queue_cur_size_(0)
    {
        message_queue_.resize(queue_max_size);
    }

    ~zen_smem_msgqueue_ring()
    {
    }
};


#endif //#ifndef _ZEN_LIB_THREAD_MESSAGE_QUEUE_H_



