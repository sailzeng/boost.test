

#include "boost_predefine.h"


int test_smem_main1 (int argc, char *argv[])
{
    using namespace boost::interprocess;

    if (argc == 1)
    {
        //Parent process
        //Remove shared memory on construction and destruction
        struct shm_remove
        {
            shm_remove()
            {
                shared_memory_object::remove("MySharedMemory");
            }
            ~shm_remove()
            {
                shared_memory_object::remove("MySharedMemory");
            }
        } remover;

        //Create a managed shared memory segment
        managed_shared_memory segment(create_only, "MySharedMemory", 65536);

        //Allocate a portion of the segment (raw memory)
        std::size_t free_memory = segment.get_free_memory();
        void *shptr = segment.allocate(1024/*bytes to allocate*/);

        //Check invariant
        if (free_memory <= segment.get_free_memory())
        {
            return 1;
        }

        //An handle from the base address can identify any byte of the shared
        //memory segment even if it is mapped in different base addresses
        managed_shared_memory::handle_t handle = segment.get_handle_from_address(shptr);
        std::stringstream s;
        s << argv[0] << " " << handle;
        s << std::ends;

        //Launch child process
        if (0 != std::system(s.str().c_str()))
        {
            return 1;
        }

        //Check memory has been freed
        if (free_memory != segment.get_free_memory())
        {
            return 1;
        }
    }
    else
    {
        //Open managed segment
        managed_shared_memory segment(open_only, "MySharedMemory");

        //An handle from the base address can identify any byte of the shared
        //memory segment even if it is mapped in different base addresses
        managed_shared_memory::handle_t handle = 0;

        //Obtain handle value
        std::stringstream s;
        s << argv[1];
        s >> handle;

        //Get buffer local address from handle
        void *msg = segment.get_address_from_handle(handle);

        //Deallocate previously allocated memory
        segment.deallocate(msg);
    }

    return 0;
}






int test_smem_main2(int argc, char *argv[])
{
    using namespace boost::interprocess;
    const std::size_t FileSize = 10000;

    if (argc == 1)
    {
        //Parent process executes this
        {
            //Create a file
            std::filebuf fbuf;
            fbuf.open("file.bin", std::ios_base::in | std::ios_base::out
                      | std::ios_base::trunc | std::ios_base::binary);
            //Set the size
            fbuf.pubseekoff(FileSize - 1, std::ios_base::beg);
            fbuf.sputc(0);
        }
        //Remove file on exit
        struct file_remove
        {
            ~file_remove ()
            {
                file_mapping::remove("file.bin");
            }
        } destroy_on_exit;

        //Create a file mapping
        file_mapping m_file("file.bin", read_write);

        //Map the whole file with read-write permissions in this process
        mapped_region region(m_file, read_write);

        //Get the address of the mapped region
        void *addr       = region.get_address();
        std::size_t size  = region.get_size();

        //Write all the memory to 1
        std::memset(addr, 1, size);

        //Launch child process
        std::string s(argv[0]);
        s += " child ";

        if (0 != std::system(s.c_str()))
        {
            return 1;
        }
    }
    else
    {
        //Child process executes this
        {
            //Open the file mapping and map it as read-only
            file_mapping m_file("file.bin", read_only);

            mapped_region region(m_file, read_only);

            //Get the address of the mapped region
            void *addr       = region.get_address();
            std::size_t size  = region.get_size();

            //Check that memory was initialized to 1
            const char *mem = static_cast<char *>(addr);

            for (std::size_t i = 0; i < size; ++i)
                if (*mem++ != 1)
                {
                    return 1;    //Error checking memory
                }
        }
        {
            //Now test it reading the file
            std::filebuf fbuf;
            fbuf.open("file.bin", std::ios_base::in | std::ios_base::binary);

            //Read it to memory
            std::vector<char> vect(FileSize, 0);
            fbuf.sgetn(&vect[0], std::streamsize(vect.size()));

            //Check that memory was initialized to 1
            const char *mem = static_cast<char *>(&vect[0]);

            for (std::size_t i = 0; i < FileSize; ++i)
                if (*mem++ != 1)
                {
                    return 1;    //Error checking memory
                }
        }
    }

    return 0;
}







using namespace boost::interprocess;

//Define an STL compatible allocator of ints that allocates from the managed_shared_memory.
//This allocator will allow placing containers in the segment
typedef allocator<int, managed_shared_memory::segment_manager>  ShmemAllocator;

//Alias a vector that uses the previous STL-like allocator so that allocates
//its values from the segment
typedef vector<int, ShmemAllocator> MyVector;

//Main function. For parent process argc == 1, for child process argc == 2
int test_shmem_vector_main(int argc, char *argv[])
{
    if (argc == 1) //Parent process
    {
        //Remove shared memory on construction and destruction
        struct shm_remove
        {
            shm_remove()
            {
                shared_memory_object::remove("MySharedMemory");
            }
            ~shm_remove()
            {
                shared_memory_object::remove("MySharedMemory");
            }
        } remover;

        //Create a new segment with given name and size
        managed_shared_memory segment(create_only, "MySharedMemory", 65536);

        //Initialize shared memory STL-compatible allocator
        const ShmemAllocator alloc_inst (segment.get_segment_manager());

        //Construct a vector named "MyVector" in shared memory with argument alloc_inst
        MyVector *myvector = segment.construct<MyVector>("MyVector")(alloc_inst);

        for (int i = 0; i < 100; ++i) //Insert data in the vector
        {
            myvector->push_back(i);
        }

        //Launch child process
        std::string s(argv[0]);
        s += " child ";

        if (0 != std::system(s.c_str()))
        {
            return 1;
        }

        //Check child has destroyed the vector
        if (segment.find<MyVector>("MyVector").first)
        {
            return 1;
        }
    }
    else  //Child process
    {
        //Open the managed segment
        managed_shared_memory segment(open_only, "MySharedMemory");

        //Find the vector using the c-string name
        MyVector *myvector = segment.find<MyVector>("MyVector").first;

        //Use vector in reverse order
        std::sort(myvector->rbegin(), myvector->rend());

        //When done, destroy the vector from the segment
        segment.destroy<MyVector>("MyVector");
    }

    return 0;
};





//Alias a vector that uses the previous STL-like allocator so that allocates
//its values from the segment
typedef list<int, ShmemAllocator> MyList;

//Main function. For parent process argc == 1, for child process argc == 2
int test_shmem_list_main(int argc, char *argv[])
{
    if (argc == 1) //Parent process
    {
        //Remove shared memory on construction and destruction
        struct shm_remove
        {
            shm_remove()
            {
                shared_memory_object::remove("MySharedMemory");
            }
            ~shm_remove()
            {
                shared_memory_object::remove("MySharedMemory");
            }
        } remover;

        //Create a new segment with given name and size
        managed_shared_memory segment(create_only, "MySharedMemory", 65536);

        //Initialize shared memory STL-compatible allocator
        const ShmemAllocator alloc_inst (segment.get_segment_manager());

        //Construct a vector named "MyVector" in shared memory with argument alloc_inst
        MyList *myvector = segment.construct<MyList>("MyList")(alloc_inst);

        for (int i = 0; i < 100; ++i) //Insert data in the vector
        {
            myvector->push_back(i);
        }

        //Launch child process
        std::string s(argv[0]);
        s += " child ";

        if (0 != std::system(s.c_str()))
        {
            return 1;
        }

        //Check child has destroyed the vector
        if (segment.find<MyList>("MyList").first)
        {
            return 1;
        }
    }
    else  //Child process
    {
        //Open the managed segment
        managed_shared_memory segment(open_only, "MySharedMemory");

        //Find the vector using the c-string name
        MyList *myvector = segment.find<MyList>("MyList").first;

        //Use vector in reverse order
        //std::sort(myvector->rbegin(), myvector->rend());

        //When done, destroy the vector from the segment
        segment.destroy<MyList>("MyList");
    }

    return 0;
};



