#include <stdint.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "xdg-shell-client-protocol.h" //these are only included for an enum, mb unnecessary coupling

struct Widget;
struct TextField;

static void draw_textfield(struct Widget* widget, uint32_t *data, int stride, int, int);
static void key_press(struct Widget*, uint32_t state, int);
static void key_press_textfield(struct TextField*, uint32_t state, int);

enum ComponentType {
    TEXTBOX,
    SQUARE
};


struct Widget {
    int x;
    int y; 
    int height;
    int width;
    int order; 
    void (*draw)(struct Widget*, uint32_t*, int, int, int);
    void (*key_press)(struct Widget*, uint32_t state, int);
    enum ComponentType type;
    void *child;
};


struct TextField
{
    struct Widget *base;
    char text[128]; //Eventually will cause run time error
    int text_length;

    void (*key_press)(struct TextField*, uint32_t state, int);
};

static void draw(struct Widget* widget, uint32_t *data, int stride, int w_width, int w_height)
{
    if(widget->type == TEXTBOX)
    {
        draw_textfield(widget, data, stride, w_width, w_height); //May as well just call w->child->draw();
    }
    //else if ( OTHER COMPONENT )
}


static void draw_textfield(struct Widget* widget, uint32_t *data, int stride, int w_width, int w_height)
{
    struct TextField* t = (struct TextField*)widget->child;
    
    int width = w_width;
    int height = w_height;

    char* text = t->text;
    int textLength = t->text_length;

    FT_Library library;
    FT_Face face;
    FT_Init_FreeType(&library);
    FT_New_Face(library, "DejaVuSansMono.ttf", 0, &face);
    FT_Set_Char_Size(face, 0, 16 * 128, 100, 100);

    int xOffset = 0 + widget->x;
    int yOffset = 0 + widget->y;

    const int MAX_LINE_CHARS = 30;
    const int LINE_SPACEING = 10;
    for (int i = 0; i < textLength; i++)
    {
        FT_Load_Char(face, text[i], FT_LOAD_RENDER);
        FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
        if(i % MAX_LINE_CHARS == 0 && i > 0)
        {
            yOffset += face->glyph->bitmap.rows + LINE_SPACEING;
            xOffset = 0;
        }

        for (int y = 0; y < face->glyph->bitmap.rows; ++y)
        {
            for (int x = 0; x < face->glyph->bitmap.width; ++x) {

                if(face->glyph->bitmap.buffer[y * face->glyph->bitmap.width + x] >= 128)
                    data[((y+yOffset)*width)+x+xOffset] = 0xFFFFFFFF;
                else
                    data[((y+yOffset)*width)+x+xOffset] = 0xFF000000;
            }
        }
        xOffset += face->glyph->bitmap.width;
    }

    //// Cleanup
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

static struct TextField* create_test_textfield(int x, int y) {
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

    strcpy(textField->text, "Hello"); //Change to static memory l8r

    textField->text_length = 5; //Question this a bit cause im tired

    return textField;
}


static void key_press(struct Widget* widget, uint32_t state, int sym)
{
    if(widget->type == TEXTBOX)
    {
        struct TextField* t = (struct TextField*)widget->child;
        t->key_press(t,state,sym);
    }
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

static void key_press_textfield(struct TextField* textfield, uint32_t state, int sym)
{
    if(sym >= 32 && sym <= 126 && state == WL_KEYBOARD_KEY_STATE_PRESSED)
        appendChar(textfield, sym);
    else if (sym == 65288 && state == WL_KEYBOARD_KEY_STATE_PRESSED)
       removeChar(textfield);
}
