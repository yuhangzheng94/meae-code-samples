#include <os-scheduling.h>


// Round Robin
int RR(task tasks[], int nbOfTasks, sched_data* schedData, int currentTime) {
    int i, j;

    // Initialize single queue
    if (currentTime == 0) {
        printf("Initializing job queue\n");
        schedData->nbOfQueues = 1;
        for (i = 0; i < MAX_NB_OF_TASKS; i++) {
            schedData->queues[0][i] = -1;
        }
    }

    admitNewTasks(tasks, nbOfTasks, schedData, currentTime);
    printQueues(tasks, schedData);

	// check if there is any task in the queue
    i = schedData->queues[0][0];
    if (i == -1) {
		return -1;  // no tasks could be elected
	}

	// check if the task is running
	if (tasks[i].state == RUNNING) {

		// case 1: if the task has finished its computations, terminate the task
		if (tasks[i].executionTime == tasks[i].computationTime) {

			// update task: state, completionDate
			tasks[i].state = TERMINATED;
			tasks[i].completionDate = currentTime;

			// update queue: delete the first task
			for (j = 0; j < MAX_NB_OF_TASKS - 1; j++) {
				schedData->queues[0][j] = schedData->queues[0][j+1];
			}
			schedData->queues[0][MAX_NB_OF_TASKS - 1] = -1;

		// case 2: if the task has finished its quantum, push it back to the queue
		} else if (tasks[i].cyclesInQuantum == (schedData->quantum)){
			// update task: state, cyclesInQuantum

			tasks[i].state = READY;
			tasks[i].cyclesInQuantum = 0;

			// update queue: cut the first task and put it back to the end of queque
			for (j = 0; j < MAX_NB_OF_TASKS - 1; j++) {
				schedData->queues[0][j] = schedData->queues[0][j+1];
				if (schedData->queues[0][j] == -1) {
					schedData->queues[0][j] = i;
					break;
				}
			}

		// otherwise, reelect this task
		} else {
			tasks[i].executionTime ++;
			tasks[i].cyclesInQuantum ++;
			return i;
		}
	}

    // for cases 1 and 2, elect the next task in the queue
    i = schedData->queues[0][0];
    if (i != -1){
        tasks[i].executionTime ++;
        tasks[i].cyclesInQuantum ++;
        tasks[i].state = RUNNING;
        return i;
    }

    // no tasks could be elected
    return -1;
}

// Multilevel Feedback Queue
int MFQ(task tasks[], int nbOfTasks, sched_data* schedData, int currentTime) {
    /*
    each queue enforces FCFS, 3 priority queues, 1 being the highest
    upon submission, each task gets inserted into L = 1
    a process gets L * D units of time in CPU
    upon eviction, the process gets pushed to the lower priority queue, circular (1->2->3->1)
    */

    int i, j, l;

    // Initialize priority queues
    if (currentTime == 0) {
        printf("Initializing job queue\n");
        schedData->nbOfQueues = 3;
        for (i = 0; i < schedData->nbOfQueues; i++) {
          for (j = 0; j < MAX_NB_OF_TASKS; j++) {
              schedData->queues[i][j] = -1;
          }
        }
    }

    admitNewTasks(tasks, nbOfTasks, schedData, currentTime);
    printQueues(tasks, schedData);

    // check if there is any task in the queues
    i = schedData->queues[0][0];
    j = 0;
    while (i == -1) {
      if ( (++j) < (schedData->nbOfQueues) ) {
        i = schedData->queues[j][0];
      } else {
        return -1;
      }
    }

    l = j + 1;  // l denotes the priority, 1 being highest

    // check if the task is running
    if (tasks[i].state == RUNNING) {

		// case 1: if the task has finished its computations, terminate the task
        if (tasks[i].executionTime == tasks[i].computationTime) {

            // update task: state, completionDate
            tasks[i].state = TERMINATED;
            tasks[i].completionDate = currentTime;

            // update queue: queue.pop(0)
            for (j = 0; j < MAX_NB_OF_TASKS - 1; j++) {
                schedData->queues[l-1][j] = schedData->queues[l-1][j+1];
            }
			schedData->queues[l-1][MAX_NB_OF_TASKS - 1] = -1;
		
		// case 2: if the task has finished its quantum, push it to the queue one level lower (circular, 1->2->3->1)
        } else if (tasks[i].cyclesInQuantum == (l * (schedData->quantum))){
        
            // update task: state, cyclesInQuantum
            tasks[i].state = READY;
            tasks[i].cyclesInQuantum = 0;

            // update queue: queue.pop(0)
            for (j = 0; j < MAX_NB_OF_TASKS - 1; j++) {
                schedData->queues[l-1][j] = schedData->queues[l-1][j+1];
            }
			schedData->queues[l-1][MAX_NB_OF_TASKS - 1] = -1;
			
			// update queue: newQueue.push()
            l = l % (schedData->nbOfQueues);
            for (j = 0; j < MAX_NB_OF_TASKS; j++) {
                if (schedData->queues[l][j] == -1) {
                    schedData->queues[l][j] = i;
                    break;
                }
            }

		// otherwise, reelect this task
        } else {
            tasks[i].executionTime ++;
            tasks[i].cyclesInQuantum ++;
            return i;
        }
    }

    // for cases 1 and 2, elect the next task in the queue
    i = schedData->queues[0][0];
    j = 0;
    while (i == -1) {
      if ( (++j) < (schedData->nbOfQueues) ) {
        i = schedData->queues[j][0];
      } else {
        return -1;	// no tasks could be elected
      }
    }

    tasks[i].executionTime ++;
    tasks[i].cyclesInQuantum ++;
    tasks[i].state = RUNNING;
    return i;
}

// I/O sensitive round-robin
int IORR(task tasks[], int nbOfTasks, sched_data* schedData, int currentTime) {
    int i, j;

    // Initialize single queue
    if (currentTime == 0) {
        printf("Initializing job queue\n");
        schedData->nbOfQueues = 1;
        for (j = 0; j < MAX_NB_OF_TASKS; j++) {
            schedData->queues[0][j] = -1;
        }
    }

    // Admit new tasks
    admitNewTasks(tasks, nbOfTasks, schedData, currentTime);
    printQueues(tasks, schedData);

    // check SLEEPING processes and wake them up if it's time
    for (j = 0; j < MAX_NB_OF_TASKS; j++) {
        i = schedData->queues[0][j];
        if (tasks[i].state == SLEEPING && tasks[i].nextReadyDate == currentTime) {
			if (tasks[i].executionTime == tasks[i].computationTime) {
				tasks[i].state = TERMINATED;
				tasks[i].completionDate = currentTime;
				for (j = 0; j < MAX_NB_OF_TASKS - 1; j++) {
					schedData->queues[0][j] = schedData->queues[0][j+1];
				}
				schedData->queues[0][MAX_NB_OF_TASKS - 1] = -1;
			} else {
				tasks[i].state = READY;
			}
        }
    }

    // if there is a running ask, has it finished its computations?
    // YES --> put it to termination
    // NO --> continue the task

    for (j = 0; j < MAX_NB_OF_TASKS; j++) {
        i = schedData->queues[0][j];
        if (i == -1) {
            break;
        } 
        
        if (tasks[i].state == RUNNING) {
            // case 1: 1 more I/O --> put the task to sleep
            if ( (tasks[i].ioPeriod != 0) && (tasks[i].cyclesInPeriod == tasks[i].ioPeriod) ) {
				// update task
				tasks[i].state = SLEEPING;
				tasks[i].nextReadyDate = currentTime + tasks[i].ioDuration;
				tasks[i].cyclesInPeriod = 0;
				tasks[i].cyclesInQuantum = 0;

				// update queue
				for (int k = j; k < MAX_NB_OF_TASKS - 1; k++) {
					schedData->queues[0][k] = schedData->queues[0][k+1];
					if (schedData->queues[0][k] == -1) {
						schedData->queues[0][k] = i;
						break;
					}
				}

            // case 2: no more I/O --> terminate the task
            } else if (tasks[i].executionTime == tasks[i].computationTime) {
				// update task
				tasks[i].state = TERMINATED;
				tasks[i].completionDate = currentTime;

				// update queue
				for (int k = j; k < MAX_NB_OF_TASKS - 1; k++) {
					schedData->queues[0][k] = schedData->queues[0][k+1];
				}

            // case 3: normall scheduling without I/O
            } else if (tasks[i].cyclesInQuantum == (schedData->quantum)){
              	// update task
                tasks[i].state = READY;
                tasks[i].cyclesInQuantum = 0;

                // update queue
                for (int k = j; k < MAX_NB_OF_TASKS - 1; k++) {
                    schedData->queues[0][k] = schedData->queues[0][k+1];
                    if (schedData->queues[0][k] == -1) {
                        schedData->queues[0][k] = i;
                        break;
                    }
                }
            
            // otherwise, reelect this task
            } else {
                tasks[i].executionTime ++;
                tasks[i].cyclesInQuantum ++;
                tasks[i].cyclesInPeriod ++;
                return i;
            }
            break;
        }

    }  // end of for loop

    // otherwise, elect the next task in the queue
    for (j = 0; j < MAX_NB_OF_TASKS; j++) {
        i = schedData->queues[0][j];
        if (i == -1) {
            break;
        } else if (tasks[i].state == READY) {
            tasks[i].executionTime ++;
            tasks[i].cyclesInQuantum ++;
            tasks[i].cyclesInPeriod ++;
            tasks[i].state = RUNNING;
            return i;
        }
    }

    // no tasks could be elected
    return -1;
}
