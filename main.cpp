#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>

using namespace std::chrono;

#include <vector>

class SumArray
{
private:
    int size;                   //size of vector
    int num_threads;            //number of threads to calculate sum
    std::vector<int>values;     //vector of values
    int result;                 //result is a sum of all values of vector
    void calc_sum_part(int l_index, int r_index, int index);    //calculate a sum of values of the interval [l_index, r_index]
public:
    SumArray(int _size, int _num_threads);          //_size - size of vector, _num_threads - nummber of threads
    ~SumArray();
    int calculate_using_threads();                  //calculate sum of values using threads
    int calculate_without_threads();                //calculate sum of values in main thread
    void printArray();                              //print values of vector
};


SumArray::SumArray(int _size, int _num_threads): size(_size), num_threads(_num_threads)
{
    const int min_value = 0;
    const int max_value = 10;
    srand(std::time(0));
    for (int i = 0; i < size; i++)
    {
        values.push_back(min_value + rand() % (max_value - min_value + 1));
    }
}

SumArray::~SumArray()
{

}

void SumArray::printArray()
{
    std::cout << "Array:" << std::endl;
    for (int i = 0; i < size; i++)
        std::cout << values[i] << " ";
    std::cout << "\n" << std::endl;
}

void SumArray::calc_sum_part(int l_index, int r_index, int index)
{
    int s = 0;
    while (l_index <= r_index)
    {
        s += values[l_index++];
    }
    result += s;
    //std::cout << "Thread id = " << std::this_thread::get_id() << "  index = " << index << " sum = " << s << std::endl;
}

int SumArray::calculate_without_threads()
{
    int s = 0;
    for (int i = 0; i < values.size(); i++)
    {
        s += values[i];
    }
    return s;
}

int SumArray::calculate_using_threads()
{
    std::vector<std::thread> threads;
    int l_index = 0;
    int r_index = 0;
    int elements_count = size/num_threads;
    result = 0;

    for (int i = 0; i < num_threads; i++)
    {
        if (i == num_threads - 1)
            r_index = values.size();    //if we have left elements, we consider them
        else
            r_index += elements_count;

        //create a thread
        std::thread t(&SumArray::calc_sum_part, this, l_index, r_index - 1, i);
        //we cannot copy a thread object so we move it
        threads.push_back(std::move(t));
        l_index += elements_count;
    }

    for (auto& t: threads)
    {
        if (t.joinable())
            t.join();
    }

    return result;
}



int main()
{
    const int array_size = 10000000;
    const int number_threads = 5;
    SumArray sum(array_size, number_threads);
    //sum.printArray();

    //calsulate sum using threads
    auto t_start = system_clock::now();
    int s = sum.calculate_using_threads();
    auto t_end = system_clock::now();
    long diff = duration_cast<milliseconds>(t_end - t_start).count();
    std::cout << "SUM (using threads: " << number_threads << ") is " << s << "  Time (ms): " << diff << std::endl;

    //calculate sum in main thread
    t_start = system_clock::now();
    s = sum.calculate_without_threads();
    t_end = system_clock::now();
    diff = duration_cast<milliseconds>(t_end - t_start).count();
    std::cout << "SUM (main thread) is " << s << "  Time (ms): " << diff << std::endl;

    return 0;
}