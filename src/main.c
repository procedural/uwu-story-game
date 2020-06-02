#include "libco.h"
#include "raylib.h"
#include "stdio.h"
#include "string.h"

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)
#define ASSET_PATH "../assets/"

cothread_t main_cothread;
cothread_t story_cothread;

typedef enum CommandType
{
    NONE = 0,
    ECHO,
    OPTION,
} CommandType;

typedef struct Command
{
    CommandType type;

    union
    {
        struct
        {
            // In
            const char* text;
            int text_len;

            // Working
            float timer;
            int text_i;
        } echo;

        struct
        {
            // In
            const char* text;
            int text_len;

            const char* option1;
            int option1_len;

            const char* option2;
            int option2_len;

            // Working
            float timer;
            int text_i;
            int space1;
            int space2;
            bool text_anim_done;

            // Output
            bool result;
        } option;
    };
} Command;

Command cmd;

void echo(const char* text)
{
    // Set some data to tell the main thread what to do
    cmd = (Command) {
        .type = ECHO,
        .echo.text = text,
        .echo.text_len = strlen(text)
    };

    // Switch to main thread
    co_switch(main_cothread);
}

bool option(const char* text, const char* option1, const char* option2)
{
    cmd = (Command) {
        .type = OPTION,
        .option.text = text,
        .option.text_len = strlen(text),

        .option.option1 = option1,
        .option.option1_len = strlen(option1),

        .option.option2 = option2,
        .option.option2_len = strlen(option2),
    };

    co_switch(main_cothread);

    return cmd.option.result;
}

void pause()
{
    cmd = (Command) {0};
    co_switch(main_cothread);
}

void story(void)
{
    echo("Hi, my name is Sussie from the Southern Beans Tribe and I am the high lord of UwU.\n\n");
    echo("I have developed the UwU movement during the Second Bread War in order to mobilize and win all the BROD.\n\n");

    if (option("Will you join me in the cause of UwU or will you disgrace me by taking the path of -_- ?\n", "YES", "NO"))
    {
        echo("Amazin'");
        pause();
    }
    else
    {
        echo("SHAME!");
        pause();
    }
}

int main(void)
{
    main_cothread = co_active();
    story_cothread = co_create(1024 * 8, story, NULL);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Awataw: The last Bwead bendew");
    SetTargetFPS(60);

    bool command_executed = true;

    char text[1024 * 8] = {0};
    int text_i = 0;

    while (!WindowShouldClose())
    {
        if (command_executed)
        {
            co_switch(story_cothread);
            command_executed = false;
        }

        switch (cmd.type)
        {
            case ECHO:
            {
                cmd.echo.timer += GetFrameTime();
                if (cmd.echo.timer >= 0.01)
                {
                    cmd.echo.timer = 0;
                    text[text_i++] = cmd.echo.text[cmd.echo.text_i++];
                    if (cmd.echo.text_i == cmd.echo.text_len)
                        command_executed = true;
                }
            } break;

            case OPTION:
            {
                cmd.option.text_anim_done = cmd.option.text_i == cmd.option.text_len;

                cmd.option.timer += GetFrameTime();
                if (cmd.option.timer >= 0.01 && !cmd.option.text_anim_done)
                {
                    cmd.option.timer = 0;
                    text[text_i++] = cmd.option.text[cmd.option.text_i++];

                    cmd.option.text_anim_done = cmd.option.text_i == cmd.option.text_len;

                    if (cmd.option.text_anim_done)
                    {
                        cmd.option.result = true;
                        const char* option_text = TextFormat("\n>%s   %s\n\n", cmd.option.option1, cmd.option.option2);
                        cmd.option.space1 = text_i + 1;
                        cmd.option.space2 = text_i + 1 + cmd.option.option1_len + 3;
                        memcpy(&text[text_i], option_text, strlen(option_text) + 1);
                        text_i += strlen(option_text);
                    }
                }

                if (cmd.option.text_anim_done)
                {
                    if (IsKeyPressed(KEY_LEFT))
                    {
                        cmd.option.result = true;
                        text[cmd.option.space1] = '>';
                        text[cmd.option.space2] = ' ';
                    }

                    if (IsKeyPressed(KEY_RIGHT))
                    {
                        cmd.option.result = false;
                        text[cmd.option.space1] = ' ';
                        text[cmd.option.space2] = '>';
                    }

                    if (IsKeyPressed(KEY_ENTER))
                    {
                        command_executed = true;
                    }
                }
            } break;
            default: break;
        }

        BeginDrawing();

        ClearBackground(BLACK);

        const int padd = 30;
        const Rectangle text_area = { padd, padd, GetScreenWidth() - padd * 2, GetScreenHeight() - padd * 2 };
        DrawTextRec(GetFontDefault(), text, text_area, 20, 2, true, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}