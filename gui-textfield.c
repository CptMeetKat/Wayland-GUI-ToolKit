
#include "gui-textfield.h"
#include "gui-widget.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "xdg-shell-client-protocol.h" //these are only included for an enum, mb unnecessary coupling
#include "time.h"

static int in_widget(struct Widget* widget, int x, int y)
{
    if(x < (widget->x + widget->width) && x >= widget->x)
    {
        if(y < (widget->y + widget->height) && y >= widget->y)
        {
            return 1;
        }
    }

    return 0;
}

static int in_window(int w_width, int w_height, int x, int y)
{
    if(x < w_width && x >= 0)
    {
        if(y < w_height && y >= 0)
        {
            return 1;
        }
    }

    return 0;
}

void toggle_cursor(struct TextField* t)
{
    if(t->cursor_visible)
        t->cursor_visible = 0;
    else
        t->cursor_visible = 1;
}

static void addBorder(struct Widget* widget, uint32_t *data, int w_width, int w_height)
{
    int hSteps = widget->width;
    int vSteps = widget->height;
    int cursor = 0; 

    for(int i = 0; i < vSteps; i++)
    {
        printf("%d %d\n", i, vSteps);
        if( ! in_window(w_width, w_height, widget->x, widget->y + i))
            break;
        cursor = widget->x + (w_width * (widget->y + i)); 
        data[cursor] = 0xFFFF0000;
    }

    cursor = widget->x + (w_width * widget->y);
    for(int i = 0; i < hSteps; i++)
    {
        if( ! in_window(w_width, w_height, widget->x + i ,widget->y))
            break;
        cursor = widget->x + i + (w_width * widget->y );
        data[cursor] = 0xFFFF0000;
    }

   //This kinda broken, need to calculate pixel not go by relatively 
    for(int i = 0; i < vSteps; i++)
    {
        if( ! in_window(w_width, w_height, widget->x + widget->width, widget->y + i))
            break;

        //cursor = cursorH + (w_width*i);
        cursor = widget->x + widget->width + (w_width * (i + widget->y));
        data[cursor] = 0xFFFF0000;
    }

    
    for(int i = 0; i < hSteps; i++)
    {
        if( ! in_window(w_width, w_height, widget->x + i ,widget->y + widget->height))
            break;
        cursor = (widget->x + i) + (w_width * (widget->height + widget->y));
        data[cursor] = 0xFFFF0000;
    }
}



static int draw_letter(char letter, uint32_t* data, struct Widget* widget, FT_Face face, int xOffset,
                       int yOffset, int w_width, int w_height)
{
    if(letter == '\n') //Cases not to render
        return 0;
    for (int y = 0; y < face->glyph->bitmap.rows; ++y) //Why is this ++y?, same thing?
    {
        if(! in_widget(widget, widget->x, y + yOffset  ))
           break;

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

void focus_textfield(struct TextField* textfield)
{
    time_t timer;
    time(&timer);
    textfield->last_blink = timer;
}


void draw_cursor(struct Widget* widget, int x, int y, int height, uint32_t *data, int w_width, int w_height)
{
    for( int i = 0; i < height; i++)
    {
        if(! in_widget(widget, widget->x, y + i))
           break;

        data[w_width * (i + y) + x] = 0xFF00FF00; //Need to add window safety to this
    }
}

void set_cursor_position(struct TextField* textfield, int index)
{
    FT_Library library;
    FT_Face face;
    FT_Init_FreeType(&library);
    FT_New_Face(library, textfield->font, 0, &face);
    FT_Set_Char_Size(face, 0, 16 * 128, 100, 100);

    int x = textfield->base->x;
    int y = textfield->base->y;

    const int LINE_SPACEING = 0;
    for(int i = 0; /*i < textfield->text_length && */ i < index; i++)
    {
        FT_Load_Char(face, textfield->text[i], FT_LOAD_RENDER);
        FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );

        if((face->glyph->advance.x >> 6) > textfield->base->width) //Characters are too wide for the width, then dont display anything
            break;

        if(textfield->text[i] == '\n' || (face->glyph->advance.x >> 6) + x > textfield->base->x + textfield->base->width) //if newline or next character will go past edge
        {
            y += (face->size->metrics.height >> 6) + LINE_SPACEING;
            x = textfield->base->x;
        }
        if(textfield->text[i] != '\n')
            x += face->glyph->advance.x >> 6;
    }

    textfield->cursor_x = x;
    textfield->cursor_y = y;
   
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

void draw_textfield(struct Widget* widget, uint32_t *data, int w_width, int w_height)
{
    // I feel like this should be TextField parametre for consistency
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
    int fontHeight = face->size->metrics.height >> 6;
    //// Cleanup
    FT_Done_Face(face);
    FT_Done_FreeType(library);

    if(widget->isFocused)
    {
        addBorder(widget,data,w_width,w_height);

        time_t timer;
        time(&timer);

        if(t->last_blink < timer)
        {
            toggle_cursor(t);
            t->last_blink = timer;
        } 
        if(t->cursor_visible) 
            draw_cursor(widget, t->cursor_x, t->cursor_y, fontHeight  , data, w_width, w_height);
    }

}



static int appendChar(struct TextField* textfield, char c)
{

    if(textfield->text_length >= GUI_TEXTFIELD_MAX_TEXT-1)
        return 0;

    textfield->text[textfield->text_length] = c;
    textfield->text[textfield->text_length+1] = '\0';
    textfield->text_length = textfield->text_length+1; 
    
    return 1;
}


static void removeChar(struct TextField* textfield)
{
    if(textfield->text_length > 0)
    {
        textfield->text[textfield->text_length-1] = '\0';
        textfield->text_length = textfield->text_length - 1;
        set_cursor_position(textfield, textfield->text_length);
        textfield->cursor_index = textfield->text_length;
    }
}

static void force_cursor_state(struct TextField* textfield, int state)
{
    textfield->cursor_visible = 1;
    time_t timer;
    time(&timer);
    textfield->last_blink = timer+1;
}



static int insertChar(struct TextField* textfield, int max_length, char new_char, int position)
{
    int text_length = textfield->text_length;
    char* text = textfield->text; 

    if(text_length+1 >= max_length-1)
        return 0;

    if(position < 0) 
        position = 0;

    if(position > text_length)
        position = text_length;

    for(int i = text_length; position <= i; i--)
    {
        text[i] = text[i-1];
    }

    text_length+=1;
    text[text_length] = '\0';
    text[position] = new_char;

    textfield->text_length += 1;

    return 1;
}

int remove_char(struct TextField* textfield, int position)
{
    if(position < 0 || position >= textfield->text_length)
        return 0;

    for(int i = position; i < textfield->text_length-1; i++)
    {
        textfield->text[i] = textfield->text[i+1]; 
    }
    textfield->text_length -=1;
    textfield->text[textfield->text_length] = '\0';
    return 1;
}

void key_press_textfield(struct TextField* textfield, uint32_t state, int sym)
{
    if(sym >= 32 && sym <= 126 && state == WL_KEYBOARD_KEY_STATE_PRESSED) //ASCII char
    {
        if( insertChar(textfield, GUI_TEXTFIELD_MAX_TEXT, sym, textfield->cursor_index) )
        {
            textfield->cursor_index += 1;
            set_cursor_position(textfield, textfield->cursor_index);
        }
    }
    else if (sym == 65293 && state == WL_KEYBOARD_KEY_STATE_PRESSED) //RETURN
    {
       if( insertChar(textfield, GUI_TEXTFIELD_MAX_TEXT, '\n', textfield->cursor_index) )
       {
           textfield->cursor_index += 1;
           set_cursor_position(textfield, textfield->cursor_index);
       }
    }
    else if (sym == 65288 && state == WL_KEYBOARD_KEY_STATE_PRESSED) //Backspace
    {
        if( remove_char(textfield, textfield->cursor_index-1) )
        {   
            textfield->cursor_index -= 1;
            set_cursor_position(textfield, textfield->cursor_index);
        } 
    }
    else if (sym == 65361 && state == WL_KEYBOARD_KEY_STATE_PRESSED) //Left
    {
        if(textfield->cursor_index > 0)
        {
            textfield->cursor_index -= 1;
            set_cursor_position(textfield, textfield->cursor_index);
            force_cursor_state(textfield, 1);
        }
    }
    else if (sym == 65363 && state == WL_KEYBOARD_KEY_STATE_PRESSED) //Right
    {
        if(textfield->cursor_index <= textfield->text_length-1)
        {
            textfield->cursor_index += 1;
            set_cursor_position(textfield, textfield->cursor_index);
            force_cursor_state(textfield, 1);
        }
    }
    //else if (sym == 65364 && state == WL_KEYBOARD_KEY_STATE_PRESSED) //down
    //{
    //    force_cursor_state(textfield, 1);
    //}
    //else if (sym == 65362 && state == WL_KEYBOARD_KEY_STATE_PRESSED) //down
    //{
    //    force_cursor_state(textfield, 1);
    //}
} 


struct TextField* create_test_textfield(int x, int y, char font[], int width, int height, char text[]) {
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
    textField->base->height = height;
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
    
    textField->base->focus = focus_widget;
    textField->focus = focus_textfield;

    textField->cursor_index = textField->text_length;
    set_cursor_position(textField, textField->cursor_index);

    //This function should do an equivalent of super()

    return textField;
}
