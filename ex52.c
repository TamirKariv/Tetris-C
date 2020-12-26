
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>

#define SCREENS_LIMIT 20
#define HORIZONTAL 0
#define VERTICAL 1
#define CHANGE_DIRECTION 1
#define DONT_CHANGE_DIRECTION 0

//location of the shape
typedef struct Location {
    int curX;
    int curY;
} Location;

//game's shape
typedef struct Shape {
    Location location;
    bool direction;
} Shape;
Shape shape;

//game's screen
char screen[SCREENS_LIMIT][SCREENS_LIMIT];


void InitScreen();

void ResetScreenAndShape();

void DrawScreen();

void RemoveShape();

void SetShape();

void Move(int newX, int newY, int changeDirection);

void Rotate();

void user_handle();

void alarm_handle();

void Run();


//initialize the screen
void InitScreen() {
    int i, j;
    for (i = 0; i < SCREENS_LIMIT - 1; i++) {
        for (j = 0; j < SCREENS_LIMIT; j++) {
            if (j == 0 || j == SCREENS_LIMIT - 1) {
                screen[i][j] = '*';
                continue;
            }
            screen[i][j] = ' ';
        }
    }
    for (j = 0; j < SCREENS_LIMIT; j++) {
        screen[SCREENS_LIMIT - 1][j] = '*';
    }
}

//reset the screen and shape
void ResetScreenAndShape() {
    InitScreen();
    shape.location.curX = 0;
    shape.location.curY = SCREENS_LIMIT / 2 - 1;
    shape.direction = HORIZONTAL;
    Move(0, SCREENS_LIMIT / 2 - 1, DONT_CHANGE_DIRECTION);
}

//draw the screen
void DrawScreen() {
    system("clear");
    int i, j;
    for (i = 0; i < SCREENS_LIMIT; i++) {
        for (j = 0; j < SCREENS_LIMIT; j++) {
            printf("%c", screen[i][j]);
        }
        printf("\n");
    }
}

//remove the shape from the screen
void RemoveShape() {
    if (shape.direction == VERTICAL) {
        screen[shape.location.curX][shape.location.curY] = ' ';
        screen[shape.location.curX + 1][shape.location.curY] = ' ';
        screen[shape.location.curX + 2][shape.location.curY] = ' ';
    } else {
        screen[shape.location.curX][shape.location.curY] = ' ';
        screen[shape.location.curX][shape.location.curY + 1] = ' ';
        screen[shape.location.curX][shape.location.curY + 2] = ' ';
    }
}

//set the shape on the screen
void SetShape() {
    if (shape.direction == HORIZONTAL) {
        screen[shape.location.curX][shape.location.curY] = '-';
        screen[shape.location.curX][shape.location.curY + 1] = '-';
        screen[shape.location.curX][shape.location.curY + 2] = '-';
        if (shape.location.curX >= SCREENS_LIMIT - 1) {
            ResetScreenAndShape();
        }
    } else if (shape.direction == VERTICAL) {
        screen[shape.location.curX][shape.location.curY] = '-';
        screen[shape.location.curX + 1][shape.location.curY] = '-';
        screen[shape.location.curX + 2][shape.location.curY] = '-';
        if (shape.location.curX + 2 >= SCREENS_LIMIT - 1) {
            ResetScreenAndShape();
        }
    }

}

//move the shape
void Move(int newX, int newY, int changeDirection) {
    //remove old shape if not changing direction
    if (!changeDirection) {
        RemoveShape();
    }
    //check limits
    if (shape.direction == VERTICAL && (newY >= SCREENS_LIMIT - 1 || newY <= 0)) {
        return;
    }
    if (shape.direction == HORIZONTAL && (newY + 2 >= SCREENS_LIMIT - 1 || newY <= 0)) {
        return;
    }
    //update new location of the shape
    shape.location.curX = newX;
    shape.location.curY = newY;
    //set the new shape on screen
    SetShape();
}


//rotate the shape
void Rotate() {
    //remove old shape
    RemoveShape();
    if (shape.direction == VERTICAL) {
        shape.direction = HORIZONTAL;
        shape.location.curX += 1;
        shape.location.curY -= 1;
        //check limits
        if (shape.location.curY + 2 >= SCREENS_LIMIT - 1) {
            shape.location.curY = SCREENS_LIMIT - 4;
        } else if (shape.location.curY < 1) {
            shape.location.curY = 1;
        }
        //move in rotation
        Move(shape.location.curX, shape.location.curY, CHANGE_DIRECTION);
    } else {
        shape.direction = VERTICAL;
        //move in rotation
        Move(shape.location.curX - 1, shape.location.curY + 1, CHANGE_DIRECTION);
    }
}


//handle the signal SIGUSR2 read the input from the pipe
void user_handle() {
    signal(SIGUSR2, user_handle);
    char input = getchar();
    switch (input) {
        //right
        case 'd':
            Move(shape.location.curX, shape.location.curY + 1, DONT_CHANGE_DIRECTION);
            DrawScreen();
            break;
            //left
        case 'a':
            Move(shape.location.curX, shape.location.curY - 1, DONT_CHANGE_DIRECTION);
            DrawScreen();
            break;
            //down
        case 's':
            Move(shape.location.curX + 1, shape.location.curY, DONT_CHANGE_DIRECTION);
            DrawScreen();
            break;
            //rotate
        case 'w':
            Rotate();
            DrawScreen();
            break;
            //quit
        case 'q':
            exit(1);
        default:
            DrawScreen();
            break;
    }
}

//handle the signal SIGALRM move the shape down
void alarm_handle() {
    signal(SIGALRM, alarm_handle);
    alarm(1);
    Move(shape.location.curX + 1, shape.location.curY, DONT_CHANGE_DIRECTION);
    DrawScreen();
}


//run the programme
void Run() {
    //reset the screen and shape
    ResetScreenAndShape();
    //show the game screen
    DrawScreen();
    //set up handler for alarm signal(move down the shape)
    signal(SIGALRM, alarm_handle);
    alarm(1);
    //set up handler for user signal(read input from the pipe)
    signal(SIGUSR2, user_handle);
    //wait for signal
    while (1) {
        pause();
    }
}

int main() {
    Run();
}




