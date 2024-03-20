
#include "gui-textfield.h"
#include "gui-widget.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "xdg-shell-client-protocol.h" //these are only included for an enum, mb unnecessary coupling

static void addBorder(struct Widget* widget, uint32_t *data, int w_width, int w_height)
{
    int hSteps = widget->width;
    int vSteps = widget->height;
    int cursor = 0; 

    for(int i = 0; i < vSteps; i++)
    {
        cursor = widget->x + (w_width * (widget->y + i)); 
        data[cursor] = 0xFFFF0000;
    }

    cursor = widget->x + (w_width * widget->y);
    for(int i = 0; i < hSteps; i++)
    {
        data[cursor] = 0xFFFFFF00;
        cursor = cursor+1;
    }

    int cursorH = cursor;
    for(int i = 0; i < vSteps; i++)
    {
        data[cursor] = 0xFFFF00FF;
        cursor = cursorH + (w_width*i);
    }

    
    for(int i = 0; i < hSteps; i++)
    {
        data[cursor] = 0xFF00FF00;
        cursor = cursor-1;
    }
}

static int draw_letter(char letter, uint32_t* data, struct Widget* widget, FT_Face face, int xOffset, int yOffset, int w_width, int w_height)
{
    if(letter == '\n') //Cases not to render
        return 0;
    for (int y = 0; y < face->glyph->bitmap.rows; ++y)
    {
        for (int x = 0; x < face->glyph->bitmap.width; ++x) 
        {
            if(face->glyph->bitmap.buffer[y * face->glyph->bitmap.width + x] >= 128)
                data[((y+yOffset)*w_width)+x+xOffset] = 0xFFFFFFFF; //white
            else
                data[((y+yOffset)*w_width)+x+xOffset] = 0x00000000; //Transparent
        }
    }
    return face->glyph->advance.x >> 6;
}


void draw_cursor(int x, int y, int height, uint32_t *data, int w_width, int w_height)
{
    for( int i = 0; i < height; i++)
    {
        data[w_width * (i + y) + x] = 0xFF00FF00; //Need to add window safety to this
    }
}

void draw_textfield(struct Widget* widget, uint32_t *data, int w_width, int w_height)
{
    struct TextField* t = (struct TextField*)widget->child;
    
    char* text = t->text;
    int textLength = t->text_length;

    FT_Library library;
    FT_Face face;
    FT_Init_FreeType(&library);
    FT_New_Face(library, t->font, 0, &face);
    FT_Set_Char_Size(face, 0, 16 * 128, 100, 100);

    int xOffset = widget->x;
    int yOffset = widget->y;

    const int LINE_SPACEING = 0;
    for (int i = 0; i < textLength; i++)
    {
        FT_Load_Char(face, text[i], FT_LOAD_RENDER);
        FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );

        if((face->glyph->advance.x >> 6) > widget->width) //Characters are too wide for the width, then dont display anything
            break;

        if(text[i] == '\n' || (face->glyph->advance.x >> 6) + xOffset > widget->x + widget->width) //if newline or next character will go past edge
        {
            yOffset += (face->size->metrics.height >> 6) + LINE_SPACEING;
            xOffset = widget->x;
        }
        xOffset += draw_letter(text[i], data, widget, face, xOffset, yOffset, w_width, w_height);
    }

    if(widget->isFocused)
    {
        addBorder(widget,data,w_width,w_height);
        draw_cursor( 200  , 300  , 100 ,data,w_width,w_height);
    }

    //// Cleanup
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    
}



static void appendChar(struct TextField* textfield, char c)
{
    textfield->text[textfield->text_length] = c;
    textfield->text[textfield->text_length+1] = '\0';
    textfield->text_length = textfield->text_length+1; 
}


static void removeChar(struct TextField* textfield)
{
    if(textfield->text_length > 0)
    {
        textfield->text[textfield->text_length-1] = '\0';
        textfield->text_length = textfield->text_length - 1;
    }
}

void key_press_textfield(struct TextField* textfield, uint32_t state, int sym)
{
    if(sym >= 32 && sym <= 126 && state == WL_KEYBOARD_KEY_STATE_PRESSED)
        appendChar(textfield, sym);
    else if (sym == 65293 && state == WL_KEYBOARD_KEY_STATE_PRESSED) //RETURN
        appendChar(textfield, '\n');
    else if (sym == 65288 && state == WL_KEYBOARD_KEY_STATE_PRESSED)
       removeChar(textfield);
}


struct TextField* create_test_textfield(int x, int y, char font[], int width, char text[]) {
    // Allocate memory for the TextField struct
    struct TextField* textField = (struct TextField*)malloc(sizeof(struct TextField));
    if (textField == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Allocate memory for the Widget struct
    textField->base = (struct Widget*)malloc(sizeof(struct Widget));
    if (textField->base == NULL) {
        perror("Memory allocation failed");
        free(textField);
        return NULL;
    }

    // Initialize the Widget
    textField->base->type = TEXTBOX;
    textField->base->child = textField;  
    textField->base->x = x;
    textField->base->y = y;
    textField->base->height = 200;
    textField->base->width = width;
    textField->base->order = 0;
    textField->base->draw = draw;

    textField->key_press = key_press_textfield;
    textField->base->key_press = key_press;
    strcpy(textField->font, font);

    textField->draw = draw_textfield;
    textField->base->isFocused = 0;
    strcpy(textField->text, text); //Change to static memory l8r

    textField->text_length = strlen(textField->text); //Question this a bit cause im tired

    return textField;
}
