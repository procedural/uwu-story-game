#include "raylib.h"
#include "stdio.h"
#include "string.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 450

typedef enum CommandType {
  COMMAND_TYPE_NONE   = 0,
  COMMAND_TYPE_ECHO   = 1,
  COMMAND_TYPE_OPTION = 2,
} CommandType;

typedef struct YesOrNo {
  bool yes;
  bool no;
} YesOrNo;

typedef struct Command {
  union {
    struct {
      // In
      const char * text;
      int          textLength;
      // Working
      float        timer;
      int          textI;
    } echo;
    struct {
      // In
      const char * text;
      int          textLength;
      const char * option1;
      int          option1Length;
      const char * option2;
      int          option2Length;
      // Working
      float        timer;
      int          textI;
      int          space1;
      int          space2;
      bool         textAnimationIsDone;
      // Output
      bool         result;
      YesOrNo *    choice;
    } option;
  };
  CommandType type;
} Command;

Command globalCommand;
int     globalProgress;
int     globalCurrentProgress;

void start() {
  globalCurrentProgress = 0;
}

void echo(const char * text) {
  globalCurrentProgress += 1;
  if (globalCurrentProgress > globalProgress && globalCommand.type == COMMAND_TYPE_NONE) {
    globalCommand = (Command) {
      .type            = COMMAND_TYPE_ECHO,
      .echo.text       = text,
      .echo.textLength = strlen(text)
    };
  }
}

void option(YesOrNo * outUniqueChoice, const char * text, const char * option1, const char * option2) {
  globalCurrentProgress += 1;
  if (globalCurrentProgress > globalProgress && globalCommand.type == COMMAND_TYPE_NONE) {
    globalCommand = (Command) {
      .type                 = COMMAND_TYPE_OPTION,
      .option.text          = text,
      .option.textLength    = strlen(text),
      .option.option1       = option1,
      .option.option1Length = strlen(option1),
      .option.option2       = option2,
      .option.option2Length = strlen(option2),
      .option.choice        = outUniqueChoice,
    };
  }
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Awataw: The last Bwead bendew");
  SetTargetFPS(60);

  int  textI = 0;
  char text[1024 * 8] = {0};

  YesOrNo choice = {};

  while (!WindowShouldClose()) {
    switch (globalCommand.type) {
      case COMMAND_TYPE_ECHO: {
        globalCommand.echo.timer += GetFrameTime();
        if (globalCommand.echo.timer >= 0.01) {
          globalCommand.echo.timer = 0;
          text[textI++] = globalCommand.echo.text[globalCommand.echo.textI++];
          if (globalCommand.echo.textI == globalCommand.echo.textLength) {
            globalCommand.type = COMMAND_TYPE_NONE;
            globalProgress += 1;
          }
        }
      } break;

      case COMMAND_TYPE_OPTION: {
        globalCommand.option.textAnimationIsDone = globalCommand.option.textI == globalCommand.option.textLength;

        globalCommand.option.timer += GetFrameTime();
        if (globalCommand.option.timer >= 0.01 && !globalCommand.option.textAnimationIsDone) {
          globalCommand.option.timer = 0;
          text[textI++] = globalCommand.option.text[globalCommand.option.textI++];

          globalCommand.option.textAnimationIsDone = globalCommand.option.textI == globalCommand.option.textLength;

          if (globalCommand.option.textAnimationIsDone) {
            globalCommand.option.result = true;
            const char * option_text = TextFormat("\n>%s   %s\n\n", globalCommand.option.option1, globalCommand.option.option2);
            globalCommand.option.space1 = textI + 1;
            globalCommand.option.space2 = textI + 1 + globalCommand.option.option1Length + 3;
            memcpy(&text[textI], option_text, strlen(option_text) + 1);
            textI += strlen(option_text);
          }
        }

        if (globalCommand.option.textAnimationIsDone) {
          if (IsKeyPressed(KEY_LEFT)) {
            globalCommand.option.result = true;
            text[globalCommand.option.space1] = '>';
            text[globalCommand.option.space2] = ' ';
          }

          if (IsKeyPressed(KEY_RIGHT)) {
            globalCommand.option.result = false;
            text[globalCommand.option.space1] = ' ';
            text[globalCommand.option.space2] = '>';
          }

          if (IsKeyPressed(KEY_ENTER)) {
            YesOrNo choiceYes = {};
            choiceYes.yes     = true;
            YesOrNo choiceNo  = {};
            choiceNo.no       = true;
            globalCommand.option.choice[0] = globalCommand.option.result == true ? choiceYes : choiceNo;
            globalCommand.type = COMMAND_TYPE_NONE;
            globalProgress += 1;
          }
        }
      } break;

      default: {
      } break;
    }

    start();

    echo("Hi, my name is Sussie from the Southern Beans Tribe and I am the high lord of UwU.\n\n");
    echo("I have developed the UwU movement during the Second Bread War in order to mobilize and win all the BROD.\n\n");
    option(&choice, "Will you join me in the cause of UwU or will you disgrace me by taking the path of -_- ?\n", "YES", "NO");
    if (choice.yes) {
      echo("Amazin'");
    } else if (choice.no) {
      echo("SHAME!");
    }

    BeginDrawing();
    ClearBackground(BLACK);
    const int       padding  = 30;
    const Rectangle textArea = {padding, padding, GetScreenWidth() - padding * 2, GetScreenHeight() - padding * 2};
    DrawTextRec(GetFontDefault(), text, textArea, 20, 2, true, RAYWHITE);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}