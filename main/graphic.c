#include "graphic.h"
#include "config.h"

// This function is responsible for drawing the walls
void graphic_print_wall()
{
    // Draws the background, or the carpet
    lcd_fillRect2(0, 0, LCD_W, LCD_H, GRAPHIC_WALL_COLOR);
    lcd_fillRect2(10, 10, 310, 230, GRAPHIC_CARPET_COLOR);

    lcd_fillRect2(100, 10, 105, 70, GRAPHIC_WALL_COLOR);
    lcd_fillRect2(215, 10, 220, 70, GRAPHIC_WALL_COLOR);

    lcd_fillRect2(100, 180, 105, 240, GRAPHIC_WALL_COLOR);
    lcd_fillRect2(215, 180, 220, 240, GRAPHIC_WALL_COLOR);

    lcd_fillRect2(0, 160, 80, 165, GRAPHIC_WALL_COLOR);
    lcd_fillRect2(0, 90, 80, 95, GRAPHIC_WALL_COLOR);

    lcd_fillRect2(240, 160, 320, 165, GRAPHIC_WALL_COLOR);
    lcd_fillRect2(240, 90, 320, 95, GRAPHIC_WALL_COLOR);

    lcd_fillRect2(190, 40, 195, 200, GRAPHIC_WALL_COLOR);
    lcd_fillRect2(125, 40, 130, 200, GRAPHIC_WALL_COLOR);
}

//draws ghost player
void graphic_draw_ghost(coord_t x, coord_t y)
{
    lcd_fillCircle(x, y, CHARACTER_RAD, GRAPHIC_GHOST_COLOR);
}

//draws hunter player
void graphic_draw_player(coord_t x, coord_t y)
{
    lcd_fillCircle(x, y, CHARACTER_RAD, GRAPHIC_PLAYER_COLOR);
}

//draws start menu
void graphic_start_menu()
{
    char Game_title[] = "Luigi's Mansion";
    char Hunter[] = "Hunter";
    char Ghost[] = "Ghost";
    char Button_Inst[] = "Press A for Hunter, B for Ghost";
    lcd_setFontSize(3);
    lcd_fillScreen(BLACK);
    lcd_drawString(LCD_W / 10, LCD_H / 8, Game_title, WHITE);

    lcd_setFontSize(2);

    lcd_drawRect2(60, 60, 240, 120, WHITE);
    lcd_drawCircle(LCD_W * 2 / 3, 90, 20, GRAPHIC_PLAYER_COLOR);
    lcd_drawString(80, 80, Hunter, WHITE);

    lcd_drawRect2(60, 150, 240, 210, WHITE);
    lcd_drawCircle(LCD_W * 2 / 3, 180, 20, GRAPHIC_GHOST_COLOR);
    lcd_drawString(80, 170, Ghost, WHITE);

    lcd_setFontSize(1);
    lcd_drawString(60, 220, Button_Inst, WHITE);
}

//draws timer
void graphic_draw_timer(int16_t t)
{
    char buffer[8]; // enough to hold the timer string
    int8_t display = t / 25;
    sprintf(buffer, "%d", display);         // convert int to string
    lcd_drawString(300, 10, buffer, WHITE); // display the timer
}

//draws scores
void graphic_draw_score(int8_t hunter_win, int8_t ghost_win)
{
    char str[STR_BUF_LEN];
    lcd_setFontSize(1);
    lcd_drawString(LCD_W / 2, LCD_H - 8, "Spooky W: ", WHITE);
    sprintf(str, "%3d", ghost_win);
    lcd_drawString(LCD_W / 2 + 50, LCD_H - 8, str, WHITE);
    lcd_drawString(10, LCD_H - 8, "Hunter W: ", WHITE);
    sprintf(str, "%3d", hunter_win);
    lcd_drawString(60, LCD_H - 8, str, WHITE);
}
