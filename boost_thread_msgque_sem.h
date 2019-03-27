/************************************************************************************************************
Copyright           : 2004-2011  Tencent Technology (Shenzhen) Company Limited.
FileName            : zen_thread_msgque_sem.h
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


#ifndef _ZEN_LIB_THREAD_MESSAGE_QUEUE_SEMAPHORE_H_ 
#define _ZEN_LIB_THREAD_MESSAGE_QUEUE_SEMAPHORE_H_




/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��18��
Template Param  : 
  Param1: class _value_type ��Ϣ���з������������
  Param2: class _container_type =std::deque<_value_type> ��Ϣ�����ڲ���������
Class           : zen_message_queue_sem
Inherit         : public boost::noncopyable
Description     : ���źŵ�+����ʵ�ֵ���Ϣ���У������Ҹ�����˵�������źŵƺ����һЩ
Other           : 
Modify Record   : 
************************************************************************************************************/
template <class _value_type,class _container_type > 
class zen_message_queue_sem : public boost::noncopyable
{
protected:

    //QUEUE�����ߴ�
    std::size_t                                    queue_max_size_;

    //����LIST��size()�����ȽϺ�ʱ����������������˸�������������ֱ��ʹ��_container_type.size()
    std::size_t                                    queue_cur_size_;

    //���е�LOCK,���ڶ�д������ͬ������
    boost::recursive_timed_mutex                   queue_lock_;

    //�źŵƣ������źŵ�
    boost::interprocess::interprocess_semaphore    sem_full_;

    //�źŵƣ��յ��źŵƣ�������
    boost::interprocess::interprocess_semaphore    sem_empty_;

    //�������ͣ�������list,dequeue,
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

        bool bret = false;
        bret = sem_full_.timed_wait(wait_time);
        //�����ʱ�ˣ�����false
        if (!bret)
        {
            //error = ETIME;
            return -1;
        }

        //ע����δ��������{}��������Ϊ������ȱ�֤����ȡ��
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
        //�����ʱ�ˣ�����false
        if (!bret)
        {
            //error = ETIME;
            return -1;
        }
        
        //ע����δ��������{}��������Ϊ������ȱ�֤����ȡ��
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

    //������Ϣ����
    void clear()
    {
        boost::recursive_timed_mutex::scoped_lock lock(queue_lock_);
        message_queue_.clear();
        queue_cur_size_ = 0;
    }

    //������Ϣ����ĳߴ�
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
Inherit         : public zen_message_queue_sem<_value_type,std::list>
Description     : �ڲ���LISTʵ�ֵ���Ϣ���У����ܵ�,�߽籣���õ�������������һ��ʼռ���ڴ治��
Other           : ��Ҫ����Ϊ�˸���һЩ�﷨��
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
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��17��
Template Param  : 
  Param1: class _value_type ��Ϣ���б������������
Class           : zen_condi_msgqueue_deque
Inherit         : public zen_message_queue_sem<_value_type,std::deque<_value_type> >
Description     : �ڲ���DQUEUEʵ�ֵ���Ϣ���У����ܽϺ�,�߽籣���õ�����������
Other           : ��װ����Ҫ����Ϊ�˸���һЩ�﷨��
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
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��17��
Template Param  : 
  Param1: class _value_type ��Ϣ���б������������
Class           : zen_condi_msgqueue_ring
Inherit         : public zen_message_queue_sem<_value_type,boost::circular_buffer<_value_type> >
Description     : �ڲ���circular_bufferʵ�ֵ���Ϣ���У����ܷǳ���,�߽籣�����źŵƣ�����Ϣ���У����ռ�ȽϷ�
Other           : �����װ����Ҫ��������Ϊ�˸����﷨�ǣ�������Ϊ�˼�������
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



