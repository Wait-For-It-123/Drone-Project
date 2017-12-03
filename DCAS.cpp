#include <fstream>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>

void drawmap(char a[][10]);

using namespace std;

#define NUM_THREADS 4
char board[10][10];
int x_final[4] = {2, 4, 6, 8};
int y_final[4] = {2, 4, 6, 8};

class Drone{
private:
  int currx;
  int curry;
  int x_Goal;
  int y_Goal;
  int state;
  bool requestLanding;
  char board[10][10];
  int howManytimesHaveIreachedGoal;
public:
  int x_final;
  int y_final;
  bool allowedToTakeOff;
  char DroneID;
  Drone(char threadID, char board[][10], int x, int y){
    x_final = x;
    y_final = y;
    DroneID = threadID;
    allowedToTakeOff = false;
    requestLanding = false;
    state = 0;
    stateMachine(board, x_final, y_final);
  }

  void setXCurLocation(int x){
    currx = x;
  }

  void setYCurLocation(int y){
    curry = y;
  }

  void setXGoalLocation(int x){
    x_Goal = x;
  }

  void setYGoalLocation(int y){
    y_Goal = y;
  }

  int getCurrentXLocation(){
    return currx;
  }

  int getCurrentYLocation(){
    return curry;
  }

  char getDroneID(){
    return DroneID;
  }

  bool canTakeOff(char board[][10]){
    if(board[0][0] == 'H'){
      return true;
    }
    else{
      return false;
    }
  }

  int movex(char board[][10], int x, int y, int x2){
    board[y][x] = '-';
    if(x<x2){
      return x + 1;
    }
    else{
      return x - 1;
    }
  }

  int movey(char board[][10], int x, int y, int y2){
    board[y][x] = '-';
    if(y<y2){
      return y + 1;
    }
    else{
      return y - 1;
    }
  }

  bool atGoal(int x, int y){
    if(x == x_Goal && y == y_Goal){
      return true;
    }
    else{
      return false;
    }
  }

  bool CanLand(bool requestLanding){
    if(requestLanding == true){
      return true;
    }
    else{
      return false;
    }
  }

  bool canMoveX(char board[][10], int x, int y, int x2){
    if(x<x2){
      if(board[y][x+1] == '-' || board[y][x+1] == 'G'){
        return true;
      }
      else{
        return false;
      }
    }
    else{
      if(board[y][x-1] == '-' || board[y][x-1] == 'H'){
        return true;
      }
      else{
        return false;
      }
    }
  }

  bool canMoveY(char board[][10], int x, int y, int y2){
    if(y<y2){
      if(board[y+1][x] == '-' || board[y+1][x] == 'G'){
        return true;
      }
      else{
        return false;
      }
    }
    else{
      if(board[y-1][x] == '-' || board[y-1][x] == 'H'){
        return true;
      }
      else{
        return false;
      }
    }
  }

  void drawmap(char board[][10]){
    cout << "\n" << endl;
    //cout<< "####################" << endl;
    for(int i=0; i<10; i++){
      for(int j=0; j<10; j++){
        cout << board[i][j] << " ";
      }
      cout << endl;
    }
    //cout<< "####################" << endl;
    cout << "\n" << endl;
  }


  void stateMachine(char board[][10], int x_final, int y_final){
    while(state != 10){
      usleep(125000);
      switch(state){
        //Case 0 is to set the drones current location to (0,0) and set its given goal location
        case 0:
        //cout << "Drone is currently in State: " << state << endl;
        setYGoalLocation(x_final);
        setXGoalLocation(y_final);
        howManytimesHaveIreachedGoal = 0;
        //cout << "I Just set the x goal location and goal x = " << x_Goal << endl;
        //cout << "I Just set the y goal location and goal y = " << y_Goal << endl;
        state = 1;
        break;

        //Case 1 is to request take off
        case 1:
        //cout << "Drone is currently in State: " << state << endl;
        //cout << "can I take off? " << canTakeOff(board) << endl;
        if(canTakeOff(board) == 1){
          state = 2;
        }
        else{
          state = 3;
        }
        break;

        //Case 2 is to take off
        case 2:
        //cout << "Drone is currently in State: " << state << endl;
        setXCurLocation(0);
        setYCurLocation(0);
        //cout << "I Just set the current x location and currx = " << getCurrentXLocation() << endl;
        //cout << "I Just set the current y location and curry = " << getCurrentYLocation() << endl;
        board[0][0] = getDroneID();
        drawmap(board);
        state = 4;
        break;

        //case 3 is wait to take off
        case 3:
        //cout << "Drone is currently in State: " << state << endl;
        cout << "Drone is not allowed to take off yet so I'm going to go back and request to take off again"<< endl;
        state = 1;
        break;

        //Case 4 is to check to see if drone can move.
        case 4:
        //cout << "Drone is currently in State: " << state << endl;
        //cout << "X is currently equal to : " << getCurrentXLocation() << endl;
        //cout << "Y is currently equal to : " << getCurrentYLocation() << endl;
        //cout << "X goal is equal to : " << x_Goal << endl;
        //cout << "Y goal is equal to : " << y_Goal << endl;
        if(canMoveX(board, getCurrentXLocation(), getCurrentYLocation(), x_Goal) == 1 && getCurrentXLocation() < x_Goal){
          state = 5;
        }
        else if(canMoveX(board, getCurrentXLocation(), getCurrentYLocation(), x_Goal) == 1 && getCurrentXLocation() > x_Goal){
          state = 5;
        }
        else if(canMoveY(board, getCurrentXLocation(), getCurrentYLocation(), y_Goal) == 1 && getCurrentYLocation() < y_Goal){
          state = 6;
        }
        else if(canMoveY(board, getCurrentXLocation(), getCurrentYLocation(), y_Goal) == 1 && getCurrentYLocation() > y_Goal){
          state = 6;
        }
        else if(atGoal(getCurrentXLocation(), getCurrentYLocation()) == 1){
          state = 8;
        }
        else{
          state = 7;
        }
        break;

        //Case 5 is to actually move the drone in the x direction
        case 5:
        //cout << "Drone is currently in State: " << state << endl;
        //cout << "X currently = " << getCurrentXLocation() << " and the x goal = " << x_Goal << endl;
        if(getCurrentXLocation() < x_Goal){
          setXCurLocation(movex(board, getCurrentXLocation(), getCurrentYLocation(), x_Goal));
          board[getCurrentYLocation()][getCurrentXLocation()] = getDroneID();
          if((getCurrentXLocation() - 1) == 0){
            //cout << "I made it to the print statement where i should change (0,0) back to H" << endl;
            board[0][0] = 'H';
          }
          drawmap(board);
          state = 4;
        }
        if(getCurrentXLocation() > x_Goal){
          setXCurLocation(movex(board, getCurrentXLocation(), getCurrentYLocation(), x_Goal));
          board[getCurrentYLocation()][getCurrentXLocation()] = getDroneID();
          drawmap(board);
          state = 4;
        }
        break;

        //Case 6 is to move in the y direction
        case 6:
        //cout << "Drone is currently in State: " << state << endl;
        if(getCurrentYLocation() < y_Goal){
          setYCurLocation(movey(board, getCurrentXLocation(), getCurrentYLocation(), y_Goal));
          board[getCurrentYLocation()][getCurrentXLocation()] = getDroneID();
          drawmap(board);
          state = 4;
        }
        if(getCurrentYLocation() > y_Goal){
          setYCurLocation(movey(board, getCurrentXLocation(), getCurrentYLocation(), y_Goal));
          board[getCurrentYLocation()][getCurrentXLocation()] = getDroneID();
          drawmap(board);
          state = 4;
        }
        break;

        //Case 7 is to wait to move
        case 7:
        //cout << "Drone is currently in State: " << state << endl;
        cout << "Drone is not allowed to move, try again" << endl;
        state = 4;
        break;

        //Case 8, the drone has now made it to the goal
        case 8:
        if(howManytimesHaveIreachedGoal == 0){
          //cout << "Drone is currently in State: " << state << endl;
          cout << "Drone "<< getDroneID() << " made it to the Goal!!!" << endl;
          setXGoalLocation(0);
          setYGoalLocation(0);
          howManytimesHaveIreachedGoal++;
          state = 4;
        }
        else{
          state = 9;
        }
        break;

        //Case 9, the drone has completed its entire mission
        case 9:
        //cout << "Drone is currently in State: " << state << endl;
        cout << "Drone "<< getDroneID() <<" has now completed its entire mission!" << endl;
        board[0][0] = 'H';
        drawmap(board);
        state = 10;
        break;
      }
    }
  }
};


//The void pointer that pthread requires, the void pointer actually creates a drone object. The drone object then takes care of the rest of the program by
//moving through the map and printing each time.
void *drone0SM(void *threadID){
  Drone drone0('0', board, x_final[0], y_final[0]);
}

void *drone1SM(void *threadID){
  Drone drone1('1', board, x_final[1], y_final[1]);
}

void *drone2SM(void *threadID){
  Drone drone2('2', board, x_final[2], y_final[2]);
}

void *drone3SM(void *threadID){
  Drone drone3('3', board, x_final[3], y_final[3]);
}


int main ()
{
      pthread_t threads[NUM_THREADS];
      int drone;
      int drone1;
      int drone2;
      int drone3;

      //Initialize the drone map, H stands for home base, - means nothing there
      for(int i=0; i<10; i++){
        for(int j=0; j<10; j++){
          if(i==0 && j ==0){
            board[i][j] = 'H';
          }
          else{
            board[i][j] = '-';
          }
        }
      }

      //Place the goal location(s) on the map with a G
      for(int y = 0; y<4; y++){
        board[x_final[y]][y_final[y]] = 'G';
      }


      drawmap(board);

      //Creation of the pthreads with a slight delay so that the threads don't get locked up upon initiation of the program
      drone = pthread_create(&threads[0], NULL, drone0SM, (void *)0);
      usleep(125000);
      drone1 = pthread_create(&threads[1], NULL, drone1SM, (void *)1);
      usleep(125000);
      drone2 = pthread_create(&threads[2], NULL, drone2SM, (void *)2);
      usleep(125000);
      drone3 = pthread_create(&threads[3], NULL, drone3SM, (void *)3);

      //This allows the drones to all come to and end very nicely so that way the program doesn't just end as soon as the threads are created
      pthread_join(threads[0], NULL);
      pthread_join(threads[1], NULL);
      pthread_join(threads[2], NULL);
      pthread_join(threads[3], NULL);
      return 0;
}



//This function is used to print the drone map, this will get called everytime a drone moves
void drawmap(char board[][10]){
  cout << "\n" << endl;
  //cout<< "####################" << endl;
  for(int i=0; i<10; i++){
    for(int j=0; j<10; j++){
      cout << board[i][j] << " ";
    }
    cout << endl;
  }
  //cout<< "####################" << endl;
  cout<< "\n"<<endl;
}
