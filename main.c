#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <windows.h>

#define MAX_STAT 100

typedef struct {
    int sleep;
    int hunger;
    int toilet;
    int food; 
} PlayerStats;

typedef struct {
    char *name;
    char *description;
    void (*action)(void);
} Location;

PlayerStats player = {100, 100, 100, 1};

pthread_mutex_t playerMutex = PTHREAD_MUTEX_INITIALIZER; 

void displayStats() {
    pthread_mutex_lock(&playerMutex); 
    printf("\nСтатистика игрока:\n");
    printf("Сон: %d\n", player.sleep);
    printf("Голод: %d\n", player.hunger);
    printf("Туалет: %d\n", player.toilet);
    printf("Еда (бутерброды): %d\n\n", player.food);
    pthread_mutex_unlock(&playerMutex); 
}

void roomAction() {
    printf("Вы в своей комнате, отдыхаете. *храп* \n");
    pthread_mutex_lock(&playerMutex);
    player.sleep = MAX_STAT;
    pthread_mutex_unlock(&playerMutex);
}

void kitchenAction() {
    printf("Вы на кухне. Готовите еду.\n");
    pthread_mutex_lock(&playerMutex);
    player.food += 1;
    pthread_mutex_unlock(&playerMutex);
}

void toiletAction() {
    printf("Вы в туалете, пук пук!\n");
    pthread_mutex_lock(&playerMutex);
    player.toilet = MAX_STAT;
    pthread_mutex_unlock(&playerMutex);
}

void streetAction() {
    printf("Гуляй Вася\n");
}

void useInventory() {
    int choice;

    printf("\nИнвентарь:\n");
    printf("1. Бутерброды: %d\n", player.food);
    printf("0. Выйти из инвентаря\n");
    printf("Выберите предмет для использования: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            pthread_mutex_lock(&playerMutex);
            if (player.food > 0) {
                printf("Ням ням\n");
                player.hunger += 20;
                if (player.hunger > MAX_STAT) player.hunger = MAX_STAT;
                player.food--;
            } else {
                printf("В инвентаре нет бутербродов.\n");
            }
            pthread_mutex_unlock(&playerMutex);
            break;
        case 0:
            printf("Выход из инвентаря.\n");
            break;
        default:
            printf("Неверный выбор.\n");
    }
}

void decrementStats() {
    pthread_mutex_lock(&playerMutex);
    player.sleep -= 1;
    player.hunger -= 2;
    player.toilet -= 2;

    if (player.sleep < 0) player.sleep = 0;
    if (player.hunger < 0) player.hunger = 0;
    if (player.toilet < 0) player.toilet = 0;
    pthread_mutex_unlock(&playerMutex); 
}

int checkGameOver() {
    pthread_mutex_lock(&playerMutex); 
    if (player.sleep == 0 || player.hunger == 0 || player.toilet == 0) {
        printf("Вы исчерпали важные параметры. Игра окончена!\n");
        pthread_mutex_unlock(&playerMutex);
        return 1;
    }
    pthread_mutex_unlock(&playerMutex);
    return 0;
}

void* backgroundDecrement(void* arg) {
    while (1) {
        decrementStats();  
        sleep(3); 
    }
    return NULL;
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    Location locations[] = {
        {"Комната", "Место для сна и восстановления энергии.", roomAction},
        {"Кухня", "Место для еды и запасов еды.", kitchenAction},
        {"Туалет", "Место для естественных нужд.", toiletAction},
        {"Улица", "Место для прогулок и отдыха.", streetAction}
    };

    int locationCount = sizeof(locations) / sizeof(locations[0]);
    int choice;

    pthread_t decrementThread;
    pthread_create(&decrementThread, NULL, backgroundDecrement, NULL); 
    while (1) {
        system("pause");
        system("cls");

        printf("Что вы хотите сделать?\n");
        printf("1. Посмотреть статистику\n");
        printf("2. Перейти в локацию\n");
        printf("3. Посмотреть инвентарь\n");
        printf("4. Выйти из игры\n");
        printf("Введите ваш выбор: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayStats();
                break;
            case 2:
                printf("Выберите локацию:\n");
                for (int i = 0; i < locationCount; i++) {
                    printf("%d. %s - %s\n", i + 1, locations[i].name, locations[i].description);
                }
                printf("Введите ваш выбор: ");
                scanf("%d", &choice);

                if (choice >= 1 && choice <= locationCount) {
                    locations[choice - 1].action();
                } else {
                    printf("Неверный выбор. Попробуйте снова.\n");
                }
                break;
            case 3:
                useInventory();
                break;
            case 4:
                printf("Выход из игры...\n");
                pthread_cancel(decrementThread);  
                return 0; 
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
        }

        if (checkGameOver()) {
            break;
        }
    }

    pthread_join(decrementThread, NULL);
    return 0;
}
