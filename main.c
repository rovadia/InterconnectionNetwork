//Submitter Roei Ovadia
//ID 40885696

#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define PROC_SIZE 2 //here decide number of processes
static int new_total_requests;

struct proc {
    //index represents processor number
    int wait_counter; //number of cycles until processor gets module
    int preference; //mean representing its own preference for which memory module it wants to access
    bool got_access; //got access to memory in previous run
    int num_accessed; //number of times processor has successfully accessed a memory module
    double wait_average; //wait counter sum/num_accessed, updates at end of each cycle
    int wait_counter_sum; //total number of cycles
};

struct memory_model {
    //index represent memory module
    bool connected; //true if already connected to processor
};

struct output {
    int number_memory_model;
    double average;
};

void generate_new_uniform_random (struct proc *processor, int number_memory_modules) {
    int j;
    for (j = 0; j < PROC_SIZE; j++) {
        if (processor[j].got_access == true) { //at beginning of next cycle, all processing elements that were granted access
            processor[j].preference = rand() % number_memory_modules; //generate a new request
        }
        //those who weren't granted access keep the same request
        new_total_requests++;
    }
}

//void generate_gaussian (struct proc *processor, int number_memory_modules, int j) { //gaussian from piazza post
//    double mu=rand()%number_memory_modules;
//    double sigma=number_memory_modules/6.0;
//    double U1, U2, W, mult;
//    static double X1, X2;
//    static int call = 0;
//    if (call == 1) {
//        call = !call;
//        processor[j].preference=(mu + sigma * (double) X2);
//        return;
//    }
//
//    do {
//        U1 = -1 + ((double) rand() / number_memory_modules) * 2;
//        U2 = -1 + ((double) rand() / number_memory_modules) * 2;
//        W = pow(U1, 2) + pow(U2, 2);
//    } while (W >= 1 || W == 0);
//
//    mult = sqrt((-2 * log(W)) / W);
//    X1 = U1 * mult;
//    X2 = U2 * mult;
//
//    call = !call;
//    processor[j].preference=(mu + sigma * (double) X1);
//    new_total_requests++;
//}

void reset_access (struct proc *processor, struct memory_model *memory, int number_memory_modules) {
    int j;
    for (j = 0; j < PROC_SIZE; j++) { //reset so all processors haven't been accessed
        processor[j].got_access = false;
    }
    for (j = 0; j < number_memory_modules; j++) { //reset so all memory modules haven't been accessed
        memory[j].connected = false;
    }
}
void make_connections (struct proc *processor, struct memory_model *memory, int number_memory_modules) {
    int j;
    for (j = 0; j < PROC_SIZE; j++) { //for all processors
        int k;
        for (k = 0; k < number_memory_modules; k++) { //for all memory modules
            if (processor[j].preference == k && memory[k].connected == false) { //if the processor's preference is memory's index, and memory hasn't been accessed yet
                processor[j].got_access = true; //then connect them
                processor[j].num_accessed++; //number accessed goes up, used to calculate average wait time
                memory[k].connected = true;
                processor[j].wait_counter = 0; //processor wait resets
            }
            //else do nothing, chill until next cycle
        }
    }
}

void calculate_wait_times (struct proc *processor) {
    int j;
    for (j = 0; j < PROC_SIZE; j++) { //for all processors
        if (processor[j].got_access == false) { //if processor couldn't get access
            processor[j].wait_counter++; //increase that processor's wait time
        }
        //calculate wait average
        processor[j].wait_counter_sum += processor[j].wait_counter;
        processor[j].wait_average = (double) processor[j].wait_counter_sum / processor[j].num_accessed;
    }
}

void sort_by_wait_time (struct proc *processor) {
    int j;
    for (j=0;j<PROC_SIZE;j++) {
        int k;
        for (k=0;k<PROC_SIZE;k++) {
            if (processor[j].wait_counter<processor[k].wait_counter) { //bubble sort
                int temp1=processor[j].wait_counter; //swapping of all the variables in the struct
                bool temp2=processor[j].got_access;
                int temp3=processor[j].preference;
                int temp4=processor[j].wait_counter_sum;
                double temp5=processor[j].wait_average;
                int temp6=processor[j].num_accessed;

                processor[j].wait_counter=processor[k].wait_counter;
                processor[j].got_access=processor[k].got_access;
                processor[j].preference=processor[k].preference;
                processor[j].wait_counter_sum=processor[k].wait_counter_sum;
                processor[j].wait_average=processor[k].wait_average;
                processor[j].num_accessed=processor[k].num_accessed;

                processor[k].wait_counter=temp1;
                processor[k].got_access=temp2;
                processor[k].preference=temp3;
                processor[k].wait_counter_sum=temp4;
                processor[k].wait_average=temp5;
                processor[k].num_accessed=temp6;
            }
        }
    }
}

int calculate_total_num_requests (int i) {
    int total_num_requests=i*PROC_SIZE; //calculate total number of requests, which is number of cycles (i) multiplied by number of processors
    return total_num_requests;
}

int calculate_total_waiting_time (struct proc *processor) {
    int total_waiting_time=0;
    int m;
    for (m=0;m<PROC_SIZE;m++) { //calculate total waiting time
        total_waiting_time+=processor[m].wait_counter_sum; //which is sum of all the processor's overall wait times
    }
    //  printf("wait time %d\n", total_waiting_time);
    return total_waiting_time;
}
int calculate_total_still_waiting_time (struct proc *processor) {
    int total_still_waiting_time=0;
    int j;
    for (j=0; j<PROC_SIZE; j++) { //calculate total still waiting time
        if (!processor[j].got_access) { //which is sum of processors that are at this point in time not connected
            total_still_waiting_time++;
        }
    }
    return total_still_waiting_time;
//    return 0;
}

double calculate_average_access_time (int total_waiting_time, int total_still_waiting_time, int total_num_requests) {
    double average_access_time=0;
    average_access_time=total_waiting_time+total_still_waiting_time; //calculate average access time
    average_access_time=average_access_time/total_num_requests;
    return average_access_time;
}

bool termination_check(struct proc *processor, int i) {

}

double percent_difference(double previous, double current) {
    //difference of two values divided by average of two values
    double percent_difference=(previous+current)/2;
    percent_difference=fabs(previous-current)/percent_difference;
//    if (percent_difference==0) { //don't count processors that haven't even waited yet, so ignore 0
//        return 100;
//    }
    return percent_difference;
}




int main() {
    int number_memory_modules;
    double average_access_time_array [2048]={0};
    struct proc processor [PROC_SIZE];

    for (number_memory_modules=1;number_memory_modules<2048;number_memory_modules++) {
        struct memory_model memory [2048/*number_memory_modules*/]={NULL}; //default all memory models are not connected
        int i;
        for (i = 0; i < PROC_SIZE; i++) { //give all processors
            processor[i].preference = rand() % number_memory_modules; //a default uniform random preference
            // generate_gaussian(processor,number_memory_modules,i); //gaussian random preference
            processor[i].got_access = false; //not accessed
            processor[i].wait_counter = 0; //hasn't waited yet
            processor[i].wait_average = 0;
            processor[i].num_accessed = 1; //don't want to divide by 0
            processor[i].wait_counter_sum = 0;
        }

        bool stop_loop = false;
        double average_access_time = 0;
        int termination_counter = 0;
        double previous_average = 0;
        double current_average = 0;

        for (i = 0; !stop_loop; i++) { //loop through memory cycles
            generate_new_uniform_random(processor,number_memory_modules); //generate new random requests for processors who got access
//            int j; //gaussian
//            for (j=0;j<PROC_SIZE;j++) {
//                if (processor[j].got_access) {
//                    generate_gaussian(processor,number_memory_modules,j);
//                }
//            }
            reset_access(processor, memory, number_memory_modules); //reset all processors and memory to not accessed
            make_connections(processor, memory,number_memory_modules); //connect processors and memory
            calculate_wait_times(processor); //calculate wait times based on connections made
            sort_by_wait_time(processor); //sort by wait times to avoid process starvation

            if (i == 0) { //first time through loop don't check percent difference
                previous_average = calculate_average_access_time(calculate_total_waiting_time(processor),
                                                                 calculate_total_still_waiting_time(processor),
                                                                 calculate_total_num_requests(
                                                                         i)); //and automatically make previous average the current average
            } else {
                current_average = calculate_average_access_time(calculate_total_waiting_time(processor),
                                                                calculate_total_still_waiting_time(processor),
                                                                calculate_total_num_requests(
                                                                        i)); //calculate the current average
                if (percent_difference(previous_average, current_average) < 0.02) { //compare it to previous
                    termination_counter++; //if percent_difference<0.02, then increase how close we are to termination
                }
                previous_average = current_average; //set the previous average to current average so we have a previous for next cycle
                if (termination_counter >
                    2) { //if we achieved small percent difference several times, we are ready to terminate
                    stop_loop = true;
                }
            }
        }
        average_access_time = calculate_average_access_time(calculate_total_waiting_time(processor),
                                                            calculate_total_still_waiting_time(processor),
                                                            calculate_total_num_requests(
                                                                    i)); //calculate average access time
        average_access_time_array[number_memory_modules]=average_access_time;
    }

    FILE *f = fopen("C:\\Users\\rovad\\CLionProjects\\InterconnectionNetwork\\file1.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    int x;
    for (x=1;x<2048;x++) { //write averages to tab separated txt file, then use that for excel sheet
        fprintf(f, "%f\t", average_access_time_array[x]);
    }
    fclose(f);

    return 0;
}