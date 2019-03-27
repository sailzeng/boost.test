/************************************************************************************************************
Copyright           : 2004-2011  Tencent Technology (Shenzhen) Company Limited.
FileName            : zen_thread_msg_queue.h
Author              : Sailzeng ZENGXING
Version             : 
Date Of Creation    : 2011��6��17��
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
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��17��
Template Param  : 
  Param1: class _value_type ��Ϣ���з������������
  Param2: class _container_type =std::deque<_value_type> ��Ϣ�����ڲ���������
Class           : zen_message_queue_condi
Inherit         : public boost::noncopyable ���ܸ��ƿ���
Description     : ����������+����ʵ�ֵ���Ϣ���У������Ҹ�����˵�����������е�֣�װB������condi����Mutex��Ŀ���ǣ�
Other           : 
Modify Record   : 
************************************************************************************************************/
template <class _value_type,class _container_type > 
class zen_message_queue_condi : public boost::noncopyable
{
protected:

    //QUEUE�����ߴ�
    std::size_t                       queue_max_size_;

    //����LIST��size()�����ȽϺ�ʱ����������������˼���������
    std::size_t                       queue_cur_size_;

    //���е�LOCK,���ڶ�д������ͬ������
    boost::recursive_timed_mutex      queue_lock_;

    //���뱣������������
    boost::condition_variable_any     cond_enqueue_;

    //ȡ�����б�������������
    boost::condition_variable_any     cond_dequeue_;

    //�������ͣ�������list,dequeue,
    _container_type                   message_queue_;

public:

    //���캯������������
    zen_message_queue_condi(size_t queue_max_size):
        queue_max_size_(queue_max_size),  
        queue_cur_size_(0)
    {
    }
    
    ~zen_message_queue_condi()
    {
    }



    //QUEUE�Ƿ�ΪNULL
    inline bool empty()
    {
        boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
        if (queue_cur_size_ == 0)
        {
            return true;
        }
        return false;
    }

    //QUEUE�Ƿ�Ϊ��
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
        //ע����δ��������{}��������Ϊ������ȱ�֤���ݷ��룬�ٴ���������
        //������������ʵ�ڲ��ǽ⿪�˱�����
        {
            boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
            bool bret = false;

            //cond�������Ƿǳ�����ģ�����ģ�����ط�������while������������
            //��ϸ��pthread_condi��˵����
            while (queue_cur_size_ == queue_max_size_)
            {
                //timed_wait�����������Ŀ����Ϊ�˽⿪���˳���ʱ����ϣ������Ǽ�����
                //���Ժ���ܺ���,WINDOWS�µ�ʵ��Ӧ�������źŵ�ģ���
                bret = cond_enqueue_.timed_wait(queue_lock_,wait_time);
                //�����ʱ�ˣ�����false
                if (!bret)
                {
                    //error = ETIME;
                    return -1;
                }

            }
            message_queue_.push_back(value_data);
            ++queue_cur_size_;

        }

        //֪ͨ���еȴ�����
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
        //ע����δ��������{}��������Ϊ������ȱ�֤����ȡ��
        {
            boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
            bool bret = false;

            //cond�������Ƿǳ�����ģ�����ģ�����ط�������while��
            //��ϸ��pthread_condi��˵����
            while (queue_cur_size_ == 0)
            {
                //timed_wait�����������Ŀ����Ϊ�˽⿪���˳���ʱ����ϣ������Ǽ�����
                //���Ժ���ܺ���
                bret = cond_dequeue_.timed_wait(queue_lock_,wait_time);
                //�����ʱ�ˣ�����false
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

        //֪ͨ���еȴ�����
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
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��17��
Template Param  : 
  Param1: class _value_type ��Ϣ���б������������
Class           : zen_condi_msgqueue_list
Inherit         : public zen_message_queue_condi<_value_type,std::list>
Description     : �ڲ���LISTʵ�ֵ���Ϣ���У����ܵ�,�߽籣���õ�������������һ��ʼռ���ڴ治��
Other           : ��Ҫ����Ϊ�˸���һЩ�﷨��
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
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��17��
Template Param  : 
  Param1: class _value_type ��Ϣ���б������������
Class           : zen_condi_msgqueue_deque
Inherit         : public zen_message_queue_condi<_value_type,std::deque<_value_type> >
Description     : �ڲ���DQUEUEʵ�ֵ���Ϣ���У����ܽϺ�,�߽籣���õ�����������
Other           : ��װ����Ҫ����Ϊ�˸���һЩ�﷨��
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
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��17��
Template Param  : 
  Param1: class _value_type ��Ϣ���б������������
Class           : zen_condi_msgqueue_ring
Inherit         : public zen_message_queue_condi<_value_type,boost::circular_buffer<_value_type> >
Description     : �ڲ���circular_bufferʵ�ֵ���Ϣ���У����ܷǳ���,�߽籣���õ�����������
Other           : ��װ����Ҫ��������Ϊ�˸����﷨�ǣ�������Ϊ�˼�������
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



