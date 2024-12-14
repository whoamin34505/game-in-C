#include <stdio.h>

void locations(int n);

int main(void){
    printf("Welcome to the game! Now, you are in the room. Choose a place: ");
    int n;
    scanf("%d",&n);
    while (1){
        locations(n);
    }
    return 0;
}

void locations(int n){
    
}