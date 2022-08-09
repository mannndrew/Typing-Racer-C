#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

typedef struct game_info
{
    COORD typing;
    clock_t start, timer;
    int letter_counter;
    int completed_sentences;
    bool used_sentences[10];
    float correct_letters;
    float total_words;
    float total_letters;
    float time;
    float time_buffer;
    char *str;
    char answer[100];
    char ch;
} game_info;

clock_t start_game(char *str);
void initialize();
void green_print(char ch);
void red_print(char ch);
void end_game();
char get_input();
char *sentence_generator(bool used_sentences[]);
int check_letter(char ch);
int word_check(char *str);
int right_letter(game_info vars);
int right_sentence(game_info vars);
int backspace(game_info vars);
int wrong_letter(game_info vars);
game_info every_second(game_info vars);
game_info every_loop(game_info vars);
game_info next_sentence(game_info vars);
game_info true_increment(game_info vars);
game_info false_increment(game_info vars);
game_info decrement(game_info vars);

CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
HANDLE _output_handle;
WORD saved_attributes;

int main()
{
    game_info vars = {};
    initialize();

    vars.str = sentence_generator(vars.used_sentences);
    vars.total_words = word_check(vars.str);
    vars.total_letters = strlen(vars.str);
    vars.start = start_game(vars.str);

    while(vars.completed_sentences<10)
    {
        vars = every_loop(vars);
        vars = every_second(vars);

        if(right_letter(vars)) //if letter is right
        {
            vars = true_increment(vars);

            if(right_sentence(vars))
            {
                vars = next_sentence(vars);
            }
        }

        else if(backspace(vars)) //if backspace
        {
            vars = decrement(vars);
        }

        else if(wrong_letter(vars)) //if letter is wrong
        {
            vars = false_increment(vars);
        }
    }
    end_game();
}

clock_t start_game(char *str)
{
    printf("Press any button to begin the game!");
    if(getch())
    {
        system("cls");
    }
    printf("%s", str);

    return clock();
}

void initialize()
{
    _output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(_output_handle, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
}

void green_print(char ch)
{
    SetConsoleTextAttribute(_output_handle, FOREGROUND_GREEN);
    printf("%c", ch);
    SetConsoleTextAttribute(_output_handle, saved_attributes);
}

void red_print(char ch)
{
    SetConsoleTextAttribute(_output_handle, FOREGROUND_RED);
    printf("%c", ch);
    SetConsoleTextAttribute(_output_handle, saved_attributes);
}

void end_game(game_info vars)
{
    system("cls");
    printf("All sentences have been completed! Now exiting...");
    sleep(5);
    exit(0);
}

char get_input()
{
    char ch = 0;

    if(kbhit())
    {
        ch = getch();
    }
}

char *sentence_generator(bool used_sentences[])
{
    char *sentence_bank[] =
    {
        "The quick brown fox jumped over the high hill.",
        "What a beautiful day it is on the beach, here in beautiful and sunny Hawaii.",
        "Do you know why all those chemicals are so hazardous to the environment?",
        "You never did tell me how many copper pennies where in that jar; how come?",
        "The two boys collected twigs outside, for over an hour, in the freezing cold!",
        "When do you think they will get back from their adventure in Cairo, Egypt?",
        "Trixie and Veronica, our two cats, just love to play with their pink ball of yarn.",
        "We climbed to the top of the mountain in just under two hours; isn't that great?",
        "I have three things to do today: wash my car, call my mother, and feed my dog.",
        "The Reckson family decided to go to an amusement park on Wednesday."
    };

    int random_num = 0;
    int counter = 0;

    srand(time(0));

    while(1)
    {
        random_num = (rand() % 10);

        if(used_sentences[random_num]==false)
        {
            used_sentences[random_num]=true;
            return sentence_bank[random_num];
        }
    }
}

int check_letter(char ch)
{
    if((ch>=65 && ch<=90) || (ch>=97 && ch<=122))
        return 1;
}

int word_check(char *str)
{
    int words = 0;
    for(int i=0; i<strlen(str); i++)
    {
        if(str[i]==32)
        {
            words++;
        }
    }
    return words+1;
}

int right_letter(game_info vars)
{
    if(vars.ch==vars.str[vars.letter_counter] && vars.letter_counter<strlen(vars.str) && vars.ch!=8 && vars.ch!=13 && check_letter(vars.ch))
        return 1;
    else
        return 0;
}

int right_sentence(game_info vars)
{
    if(strcmp(vars.answer, vars.str)==0)
        return 1;
    else
        return 0;
}

int backspace(game_info vars)
{
    if(vars.ch==8 && vars.letter_counter!=0 && vars.ch!=13)
        return 1;
    else
        return 0;
}

int wrong_letter(game_info vars)
{
    if(vars.ch!=8 && vars.ch!=vars.str[vars.letter_counter] && vars.letter_counter<strlen(vars.str) && vars.ch!=13 && check_letter(vars.ch))
        return 1;
    else
        return 0;
}

game_info every_second(game_info vars)
{
    if(((int)(vars.time-vars.time_buffer))==1)
    {
        vars.time_buffer = vars.time;
        printf("\nWordsPM: %.0f  ", vars.total_words*(vars.correct_letters/vars.total_letters)*(60/vars.time));
        SetConsoleCursorPosition(_output_handle, vars.typing);
    }
    return vars;
}

game_info every_loop(game_info vars)
{
    SetConsoleCursorPosition(_output_handle, vars.typing);
    vars.timer = clock();
    vars.time = (float)(vars.timer - vars.start) / CLOCKS_PER_SEC;
    vars.ch = get_input();

    return vars;
}

game_info next_sentence(game_info vars)
{
    vars.completed_sentences++;
    printf("\nWordsPM: %.0f  ", vars.total_words*(vars.correct_letters/vars.total_letters)*(60/vars.time));
    printf("\nCompleted Sentences: %d", vars.completed_sentences);
    printf("\n\nWell done! Press any letter to continue!");

    if(getch() && vars.completed_sentences<10)
    {
        vars.start = clock();
        vars.time_buffer = 0;
        vars.typing.X = 0;
        vars.typing.Y = 0;
        vars.letter_counter = 0;
        vars.correct_letters = 0;
        memset(vars.answer, 0, 100);
        vars.str = sentence_generator(vars.used_sentences);
        vars.total_words = word_check(vars.str);
        vars.total_letters = strlen(vars.str);

        system("cls");
        printf("%s", vars.str);
        printf("\n\nCompleted Sentences: %d", vars.completed_sentences);
    }
    return vars;
}

game_info true_increment(game_info vars)
{
    green_print(vars.ch);
    vars.answer[vars.letter_counter] = vars.ch;
    vars.correct_letters++;
    vars.typing.X++;
    vars.letter_counter++;

    return vars;
}

game_info false_increment(game_info vars)
{
    if(vars.str[vars.letter_counter]==32)
    {
        red_print('_');
    }
    vars.answer[vars.letter_counter] = vars.ch;
    vars.typing.X++;
    vars.letter_counter++;

    return vars;
}

game_info decrement(game_info vars)
{
    vars.typing.X--;
    vars.letter_counter--;

    if(vars.str[vars.letter_counter]==vars.answer[vars.letter_counter])
        vars.correct_letters--;

    vars.answer[vars.letter_counter] = '-';
    SetConsoleCursorPosition(_output_handle, vars.typing);
    printf("%c", vars.str[vars.letter_counter]);

    return vars;
}

