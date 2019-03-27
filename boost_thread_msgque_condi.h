/************************************************************************************************************
Copyright           : 2004-2011  Tencent Technology (Shenzhen) Company Limited.
FileName            : zen_thread_msg_queue.h
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


#ifndef _ZEN_LIB_THREAD_MESSAGE_QUEUE_CONDITION_H_ 
#define _ZEN_LIB_THREAD_MESSAGE_QUEUE_CONDITION_H_



/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011年6月17日
Template Param  : 
  Param1: class _value_type 消息队列放入的数据类型
  Param2: class _container_type =std::deque<_value_type> 消息队列内部容器类型
Class           : zen_message_queue_condi
Inherit         : public boost::noncopyable 不能复制拷贝
Description     : 用条件变量+容器实现的消息队列，对于我个人来说，条件变量有点怪，装B？请问condi传入Mutex的目的是？
Other           : 
Modify Record   : 
************************************************************************************************************/
template <class _value_type,class _container_type > 
class zen_message_queue_condi : public boost::noncopyable
{
protected:

    //QUEUE的最大尺寸
    std::size_t                       queue_max_size_;

    //由于LIST的size()函数比较耗时，所以这儿还是用了几个计数器
    std::size_t                       queue_cur_size_;

    //队列的LOCK,用于读写操作的同步控制
    boost::recursive_timed_mutex      queue_lock_;

    //插入保护的条件变量
    boost::condition_variable_any     cond_enqueue_;

    //取出进行保护的条件变量
    boost::condition_variable_any     cond_dequeue_;

    //容器类型，可以是list,dequeue,
    _container_type                   message_queue_;

public:

    //构造函数和析构函数
    zen_message_queue_condi(size_t queue_max_size):
        queue_max_size_(queue_max_size),  
        queue_cur_size_(0)
    {
    }
    
    ~zen_message_queue_condi()
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
        //注意这段代码必须用{}保护，因为你必须先保证数据放入，再触发条件，
        //而条件触发其实内部是解开了保护的
        {
            boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
            bool bret = false;

            //cond的语意是非常含混的，讨厌的，这个地方必须用while，必须重入检查
            //详细见pthread_condi的说明，
            while (queue_cur_size_ == queue_max_size_)
            {
                //timed_wait里面放入锁的目的是为了解开（退出的时候加上），不是加锁，
                //所以含义很含混,WINDOWS下的实现应该是用信号灯模拟的
                bret = cond_enqueue_.timed_wait(queue_lock_,wait_time);
                //如果超时了，返回false
                if (!bret)
                {
                    //error = ETIME;
                    return -1;
                }

            }
            message_queue_.push_back(value_data);
            ++queue_cur_size_;

        }

        //通知所有等待的人
        cond_dequeue_.notify_all();

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
        //注意这段代码必须用{}保护，因为你必须先保证数据取出
        {
            boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
            bool bret = false;

            //cond的语意是非常含混的，讨厌的，这个地方必须用while，
            //详细见pthread_condi的说明，
            while (queue_cur_size_ == 0)
            {
                //timed_wait里面放入锁的目的是为了解开（退出的时候加上），不是加锁，
                //所以含义很含混
                bret = cond_dequeue_.timed_wait(queue_lock_,wait_time);
                //如果超时了，返回false
                if (!bret)
                {
                    //error = ETIME;
                    return -1;
                }
            }
            //
            value_data = *message_queue_.begin();
            message_queue_.pop_front();
            --queue_cur_size_;
        }

        //通知所有等待的人
        cond_enqueue_.notify_all();

        return 0;
    }

    int dequeue(_value_type& value_data)
    {
        boost::posix_time::time_duration wait_time = boost::posix_time::hours(1024);
        return dequeue(value_data,
            wait_time);
    }

    void clear()
    {
        boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
        message_queue_.clear();
        queue_cur_size_ = 0;
    }

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
Inherit         : public zen_message_queue_condi<_value_type,std::list>
Description     : 内部用LIST实现的消息队列，性能低,边界保护用的条件变量。但一开始占用内存不多
Other           : 主要就是为了给你一些语法糖
Modify Record   : 
************************************************************************************************************/
template <class _value_type >
class zen_condi_msgqueue_list : public zen_message_queue_condi<_value_type,std::list<_value_type> >
{
    //
    zen_condi_msgqueue_list(size_t queue_max_size):
        queue_max_size_(queue_max_size),  
        queue_cur_size_(0)
    {
    }

    ~zen_condi_msgqueue_list()
    {
    }
};

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011年6月17日
Template Param  : 
  Param1: class _value_type 消息队列保存的数据类型
Class           : zen_condi_msgqueue_deque
Inherit         : public zen_message_queue_condi<_value_type,std::deque<_value_type> >
Description     : 内部用DQUEUE实现的消息队列，性能较好,边界保护用的条件变量。
Other           : 封装的主要就是为了给你一些语法糖
Modify Record   : 
************************************************************************************************************/
template <class _value_type >
class zen_condi_msgqueue_deque : public zen_message_queue_condi<_value_type,std::deque<_value_type> >
{
    //
    zen_condi_msgqueue_deque(size_t queue_max_size):
        queue_max_size_(queue_max_size),  
        queue_cur_size_(0)
    {
    }

    ~zen_condi_msgqueue_deque()
    {
    }
};

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011年6月17日
Template Param  : 
  Param1: class _value_type 消息队列保存的数据类型
Class           : zen_condi_msgqueue_ring
Inherit         : public zen_message_queue_condi<_value_type,boost::circular_buffer<_value_type> >
Description     : 内部用circular_buffer实现的消息队列，性能非常好,边界保护用的条件变量。
Other           : 封装的主要不光是了为了给你语法糖，而且是为了极限性能
Modify Record   : 
************************************************************************************************************/
template <class _value_type >
class zen_condi_msgqueue_ring : public zen_message_queue_condi<_value_type,boost::circular_buffer<_value_type> >
{
    //
    zen_condi_msgqueue_ring(size_t queue_max_size):
        queue_max_size_(queue_max_size),  
        queue_cur_size_(0)
    {
        message_queue_.resize(queue_max_size);
    }

    ~zen_condi_msgqueue_ring()
    {
    }
};



#endif //#ifndef _ZEN_LIB_THREAD_MESSAGE_QUEUE_CONDITION_H_



