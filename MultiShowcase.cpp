// MultiShowcase.cpp : This file contains the 'main' function. Program execution begins and ends there.
/*Multi-Threading examples and simple guide
By Hesiquio Ballines

This was created to be used alongside a guide that will be provided on the github link and a video tutorial going and running through the program
will be put up at some point, when that time comes I will provide the link here: youtube.com

Below are some concepts and ideas that the examples will hopefully introduce you to and will get you thinking about the complications of concurrent programming


//1. Dataraces: Why it is important to create threadsafe implementations?
//2. Perfomance: Why does multithreading worsen performance in a lot of cases
//3. How can we use ___ law to improve performance?



*/
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include <unordered_map>
//#include "threadTime.h"

using namespace std;
using namespace std::chrono;

class Node {
public:
    mutex nodeLock;
    int value;
};

//Creating objects
mutex mtxLock;

//Performance monitoring
high_resolution_clock::time_point mainStart;
high_resolution_clock::time_point mainStop;

//Storing individual thread times
//unordered_map<std::thread::id, threadTime> threadTimes;
//unordered_map<std::thread::id, int> progress;

//Used for iterating
list<int> dataRaceList;
list<int> threadSafe;

//Function 1 and Function2
int globalCounter;
int incorrectCases;
int numTests = 0;

//Function 3
int currentIndex;
int const COMMON_SIZE = 3;
int commonArray[COMMON_SIZE];

//Function 4
Node nodeArray[COMMON_SIZE];

//Originally used to monitor progress on threads to visualize the process but this hurt performance and led to some unintended results
/*void monitorThreads()
{
    double total = 0.0;
    bool checkFinished = true;
    this_thread::sleep_for(std::chrono::milliseconds(100));
    while (1)
    {
        checkFinished = true;

        //Foreach loop going through all of the threads and displaying their progress
        for (pair<thread::id, int> element : progress)
        {
            cout << "Thread #" << element.first << " :" <<((double) element.second / (1000.0 * 10000.0)) * 100.0 << "%"<<endl;

            if (element.second < (1000 * 10000) - 1)
            {
                checkFinished = false;
            }
        }

        if (checkFinished)
        {
            for (pair<thread::id, threadTime> element : threadTimes)
            {
                cout << "Thread #" << element.first << " :" << element.second.getDuration() << endl;
            }
            threadTimes.erase(threadTimes.begin(), threadTimes.end());
            progress.erase(progress.begin(), progress.end());
            return;
        }
        
        this_thread::sleep_for(std::chrono::milliseconds(50));
        system("CLS");
    }
    
}*/

/*void countConcurrently()
{
    //Grabbing the id then setting the progress to zero
    auto id = this_thread::get_id();
    progress[id] = 0;
    int count = 0;
    
    //Creating a threadTime object and setting the start time
    threadTime timeObj = threadTime();
    timeObj.setStart();
    threadTimes.emplace( id, timeObj );

    for (int i = 0; i < 1000; i++)
    { 
        for (int j = 0; j < 10000; j++)
        {

            progress[id] = count;
            count++;
        }
        
    }

    //Updating every value
    threadTimes[id].setStop();
    int time = chrono::duration_cast<chrono::milliseconds>( threadTimes[id].getStop() - threadTimes[id].getStart() ).count();
    cout << "Finished: " << endl;
    cout << time << endl;
    
}*/

//Iterating a variable with no lock/mutex to make it thread-safe
void iterateSingleVariableUnsafe()
{
    
    while (1)
    {
        
        if (globalCounter < 10000 * 1000)
        {
            globalCounter++;
        }
        else
        {
            return;
        }
    }
    
}

//Iterating a single variable but making it thread safe
//Performance should be a lot worse in this case
void threadSafeIteration()
{
            while (1)
            {
                mtxLock.lock();
                if (globalCounter < 10000 * 1000)
                {
                    globalCounter++;
                    mtxLock.unlock();
                }
                else
                {
                    mtxLock.unlock();
                    return;
                }
                
            }
        }

//Iterating an array using a thread safe implementation
//It iterates three different array sections using a set number of threads
void threadSafeArray(int index)
{
    
    while (1)
    {
        
            mtxLock.lock();
            if (commonArray[index] < 10000 * 1000)
            {
                commonArray[index]++;
                mtxLock.unlock();
            }
            else
            {
                mtxLock.unlock();
                return;
            }
        
       

    }
}
void optimizedThreadSafe(int index)
{
    nodeArray[index].nodeLock.lock();
    while (1)
    {

        
        if (nodeArray[index].value < 10000 * 1000)
        {
            nodeArray[index].value++;
           
        }
        else
        {
            nodeArray[index].nodeLock.unlock();
            return;
        }



    }
}
void startThreads(int functionSelected, thread::id mainID)
{
    //Declaring thread variables 
    vector <thread> threads; 
    int numThreads = 0;

    if (functionSelected == 1)
    {
        //Number of threads to run for the selected task
        cout << "Number of Threads: ";
        cin >> numThreads;

        //Starting the thread based on the selection
        cout << "Number of tests: ";
        cin >> numTests;
        
        
        mainStart = high_resolution_clock::now();
        for (int i = 0; i < numTests; i++)
        {
            globalCounter = 0;
            for (int j = 0; j < numThreads; j++)
            {
                threads.push_back(thread(iterateSingleVariableUnsafe));
            }

            //Joining all the threads, so the main thread knows when they're all done
            for (thread& t : threads)
            {
                t.join();
            }
            if (globalCounter != (10000 * 1000))
            {
                incorrectCases++;
            }
            threads.clear();
        }
        mainStop = high_resolution_clock::now();
        cout << "Number of iterations going over!: " << incorrectCases << endl;
        incorrectCases = 0;
    }
    
    else if (functionSelected == 2)
    {
        //Number of threads to run for the selected task
        cout << "Number of Threads: ";
        cin >> numThreads;

        //Starting the thread based on the selection
        cout << "Number of tests: ";
        cin >> numTests;


        mainStart = high_resolution_clock::now();
        for (int i = 0; i < numTests; i++)
        {
            globalCounter = 0;
            for (int j = 0; j < numThreads; j++)
            {
                threads.push_back(thread(threadSafeIteration));
            }

            //Joining all the threads, so the main thread knows when they're all done
            for (thread& t : threads)
            {
                t.join();
            }
            if (globalCounter != (10000 * 1000))
            {
                incorrectCases++;
            }
            threads.clear();
        }
        mainStop = high_resolution_clock::now();
        cout << "Number of iterations going over!: " << incorrectCases << endl;
        incorrectCases = 0;
    }
    
    else if (functionSelected == 3)
    {
        //Number of threads to run for the selected task
        cout << "Number of Threads: ";
        cin >> numThreads;

        //Resetting global values
        for (int i = 0; i < COMMON_SIZE; i++)
        {
            commonArray[i] = 0;
        }

   


        mainStart = high_resolution_clock::now();
        for (int i = 0; i < COMMON_SIZE; i++)
        {
         
            //Creating the threads
            for (int j = 0; j < numThreads; j++)
            {
                threads.push_back(thread(threadSafeArray, i));
                    
            }
            
            
            //Joining all the threads, so the main thread knows when they're all done
            for (thread& t : threads)
            {

                t.join();
            }
            
            //Clearing the threads
            threads.clear();
            
        }

       

        //Stopping the clock
        mainStop = high_resolution_clock::now();

        
        //Displaying the contents of the global array
        for (int i = 0; i < COMMON_SIZE; i++)
        {
            cout << "Index #" << i << ": " << commonArray[i] << endl;
        }
        incorrectCases = 0;
    }

    else if (functionSelected == 4)
    {
        //Resetting global values
        for (int i = 0; i < COMMON_SIZE; i++)
        {
            nodeArray[i].value = 0;
        }

        mainStart = high_resolution_clock::now();
        
        //Starting each thread with a separate workload
        for (int i = 0; i < COMMON_SIZE; i++)
        {
            threads.push_back(thread(optimizedThreadSafe, i));
        }

       

        //Joining all the threads, so the main thread knows when they're all done
        for (thread& t : threads)
        {
            t.join();
        }
        for (int i = 0; i < COMMON_SIZE; i++)
        {
            
            cout <<"Index #"<<i<<": "<< nodeArray[i].value << endl;;
        }

        mainStop = high_resolution_clock::now();
    }
    
}



int main()
{
    int option = 0;
    int numThreads = 3;
    auto mainThreadID = this_thread::get_id();
    vector <thread> threads;

    cout << "1.Iterating on a single variable (no locks)" << endl;
    cout << "2.Iterating on a single variable (Safe)" << endl;
    cout << "3.Iterating three different indexes on a single array iteratively (Safe)" << endl;
    cout << "4.Iterating three different indexes on a single array concurrently (Safe and Optimized)\n" << endl;

    while (1)
    {
        

        //Will have a better menu soon-ish
        cout << "Enter an option: ";
        cin >> option;
        
        
        //Starting the threads, will ask for number of threads
        startThreads( option , mainThreadID);

        //Displaying the total run time of the threads in milliseconds
        cout << "Total Run Time(ms): " << chrono::duration_cast<chrono::milliseconds>(mainStop - mainStart).count() <<endl<<endl;
           
    }
    

    return 0;
    
  
    
}
