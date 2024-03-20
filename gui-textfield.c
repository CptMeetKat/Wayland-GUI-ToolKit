
#include "gui-textfield.h"

#include "gui-widget.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "xdg-shell-client-protocol.h" //these are only included for an enum, mb unnecessary coupling

static void addBorder(struct Widget* widget, uint32_t *data, int stride, int w_width, int w_height)
{
//NOTE: need to stop textboxes overflowing there borders
    int hSteps = widget->width;
    int vSteps = widget->height;
    int cursor = 0; 

    for(int i = 0; i < vSteps; i++)
    {
        cursor = widget->x + ((stride/4)*widget->y) + ((stride/4)*i);
        data[cursor] = 0xFFFF0000;
    }

    cursor = widget->x + ((stride/4) * widget->y);
    for(int i = 0; i < hSteps; i++)
    {
        cursor = cursor+1;
        data[cursor] = 0xFFFF0000;
    }

    int cursorH = cursor;
    for(int i = 0; i < vSteps; i++)
    {
        cursor = cursorH + ((stride/4)*i);
        data[cursor] = 0xFFFF0000;
    }

    
    for(int i = 0; i < hSteps; i++)
    {
        cursor = cursor-1;
        data[cursor] = 0xFFFF0000;
    }
}

static void draw_letter(char letter, uint32_t* data, struct Widget* widget, FT_Face face, int xOffset, int yOffset, int w_width, int w_height)
{
        for (int y = 0; y < face->glyph->bitmap.rows; ++y)
    {
        for (int x = 0; x < face->glyph->bitmap.width; ++x) 
        {
            if(x+xOffset > widget->x+widget->width) //Prevent overflowing right
            {
                break;
            }

            if(face->glyph->bitmap.buffer[y * face->glyph->bitmap.width + x] >= 128)
                data[((y+yOffset)*w_width)+x+xOffset] = 0xFFFFFFFF; //white
            else
                data[((y+yOffset)*w_width)+x+xOffset] = 0x00000000; //Transparent
        }
    }
}

void draw_textfield(struct Widget* widget, uint32_t *data, int stride, int w_width, int w_height)
{
    struct TextField* t = (struct TextField*)widget->child;
    
    char* text = t->text;
    int textLength = t->text_length;

    FT_Library library;
    FT_Face face;
    FT_Init_FreeType(&library);
    FT_New_Face(library, "DejaVuSansMono.ttf", 0, &face);
    FT_Set_Char_Size(face, 0, 16 * 128, 100, 100);

    int xOffset = widget->x;
    int yOffset = widget->y;

    const int MAX_LINE_CHARS = 8; //Note: need to calculate this
    const int LINE_SPACEING = 10;
        for (int i = 0; i < textLength; i++)
    {

        FT_Load_Char(face, text[i], FT_LOAD_RENDER);
        FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
        if(i % MAX_LINE_CHARS == 0 && i > 0)
        {
            yOffset += face->glyph->bitmap.rows + LINE_SPACEING;
            xOffset = widget->x;
        }

        draw_letter(text[i], data, widget, face, xOffset, yOffset, w_width, w_height);
        xOffset += face->glyph->advance.x >> 6;
    }

    if(widget->isFocused)
        addBorder(widget,data,stride,w_width,w_height);

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
    else if (sym == 65288 && state == WL_KEYBOARD_KEY_STATE_PRESSED)
       removeChar(textfield);
}


struct TextField* create_test_textfield(int x, int y) {
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
    textField->base->height = 100;
    textField->base->width = 200;
    textField->base->order = 0;
    textField->base->draw = draw;

    textField->key_press = key_press_textfield;
    textField->base->key_press = key_press;

    textField->draw = draw_textfield;
    textField->base->isFocused = 0;
    strcpy(textField->text, "Hello"); //Change to static memory l8r

    textField->text_length = 5; //Question this a bit cause im tired

    return textField;
}
