#define ASCII_MIN 32
#define ASCII_MAX 126
#define RETURN_KEY 65293
#define BACKSPACE_KEY 65288
#define LEFT_ARROW_KEY 65361
#define RIGHT_ARROW_KEY 65363
#define DOWN_ARROW_KEY 65364
#define UP_ARROW_KEY 65362


#include "gui-textfield.h"
#include "gui-widget.h"
#include "xdg-shell-client-protocol.h" //these are only included for an enum, mb unnecessary coupling
#include "time.h"


void add_letter_length_to_cursor(struct TextField* textfield, struct Cursor* cursor, char letter);
void shift_cursor_right(struct TextField* textfield);
int get_character_width(struct TextField* textfield, char letter);
                        
void text_release_font(struct TextField* textfield)
{
    FT_Done_Face(textfield->face);
    FT_Done_FreeType(textfield->library);
}

void release_textfield(struct TextField* textfield)
{
    gb_release(&(textfield->gb));
    text_release_font(textfield);
}


static int in_widget(struct Widget* widget, int x, int y)
{
    if(x < (widget->x + widget->width) && x >= widget->x)
    {
        if(y < (widget->y + widget->height) && y >= widget->y)
            return 1;
    }

    return 0;
}

static int in_window(int w_width, int w_height, int x, int y)
{
    if(x < w_width && x >= 0)
    {
        if(y < w_height && y >= 0)
            return 1;
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

static void draw_border(struct Widget* widget, uint32_t *data, int w_width, int w_height)
{
    int hSteps = widget->width;
    int vSteps = widget->height;
    int pos = 0; 

    for(int i = 0; i < vSteps; i++)
    {
        if( ! in_window(w_width, w_height, widget->x, widget->y + i))
            continue;
        pos = widget->x + (w_width * (widget->y + i)); 
        data[pos] = 0xFFFF0000;
    }

    pos = widget->x + (w_width * widget->y);
    for(int i = 0; i < hSteps; i++)
    {
        if( ! in_window(w_width, w_height, widget->x + i ,widget->y))
            continue;
        pos = widget->x + i + (w_width * widget->y );
        data[pos] = 0xFFFF0000;
    }

    for(int i = 0; i < vSteps; i++)
    {
        if( ! in_window(w_width, w_height, widget->x + widget->width, widget->y + i))
            continue;
        pos = widget->x + widget->width + (w_width * (i + widget->y));
        data[pos] = 0xFFFF0000;
    }

    
    for(int i = 0; i < hSteps; i++)
    {
        if( ! in_window(w_width, w_height, widget->x + i ,widget->y + widget->height))
            continue;
        pos = (widget->x + i) + (w_width * (widget->height + widget->y));
        data[pos] = 0xFFFF0000;
    }
}



static int draw_letter(char letter, uint32_t* data, struct Widget* widget, FT_Face face, int xOffset,
                       int yOffset, int w_width, int w_height)
{
    if(letter == '\n') //Cases not to render
        return 0;

    long ascender = face->size->metrics.ascender >> 6;

    int base_line_offset = (ascender - face->glyph->bitmap_top); //BASELINE offset

    for (int y = 0; y < face->glyph->bitmap.rows; y++) 
    {
        if(! in_widget(widget, widget->x, y + yOffset + base_line_offset))
            break;

        for (int x = 0; x < face->glyph->bitmap.width; x++) 
        {
            if(!in_window(w_width, w_height, x+xOffset, y+yOffset + base_line_offset))
                break;

            if(face->glyph->bitmap.buffer[y * face->glyph->bitmap.width + x] >= 128)
                data[((y+yOffset + base_line_offset )*w_width)+x+xOffset] = 0xFFFFFFFF; //white
            else
                data[((y+yOffset + base_line_offset)*w_width)+x+xOffset] = 0x00000000; //Transparent
        }
    }
    
    return face->glyph->advance.x >> 6;
}

void focus_textfield(struct Widget* widget)
{    
    widget->isFocused = 1;
    struct TextField* textfield = widget->child;

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

        if(!in_window(w_width, w_height, x, y+i))
            break;

        data[w_width * (i + y) + x] = 0xFF00FF00; 
    }
}

void set_cursor_position(struct TextField* textfield, int index)
{
    struct Cursor c;
    cursor_init(&c, textfield->base->x, textfield->base->y, index, 0);

    for(int i = 0; i < index; i++)
    {
        char letter = gb_get(&(textfield->gb), i);
        add_letter_length_to_cursor(textfield, &c, letter);
    }

    textfield->cursor = c;
}

void draw_focus(struct Widget* widget, uint32_t* data, int w_width, int w_height)
{
    if(widget->isFocused)
    {
        struct TextField* t = (struct TextField*)widget->child;
        draw_border(widget,data,w_width,w_height);

        time_t timer;
        time(&timer);

        if(t->last_blink < timer)
        {
            toggle_cursor(t);
            t->last_blink = timer;
        } 
        if(t->cursor_visible) 
            draw_cursor(widget, t->cursor.x, t->cursor.y, t->font_height, data, w_width, w_height);
    }
}

void draw_text(struct Widget* widget, uint32_t *data, int w_width, int w_height)
{
    struct TextField* t = (struct TextField*)widget->child;
    
    int textLength = t->gb.size;

    int xOffset = widget->x;
    int yOffset = widget->y;
    int lines = 0;

    for (int i = 0; i < textLength; i++)
    {
        char letter = gb_get(&(t->gb), i);
        FT_Load_Char(t->face, letter, FT_LOAD_RENDER);
        FT_Render_Glyph( t->face->glyph, FT_RENDER_MODE_NORMAL );

        if((t->face->glyph->advance.x >> 6) > widget->width) //Characters are too wide for the width, then dont display anything
            break;

        if(letter == '\n' || (t->face->glyph->advance.x >> 6) + xOffset > widget->x + widget->width) //if newline or next character will go past edge
        {
            yOffset += (t->face->size->metrics.height >> 6) + LINE_SPACEING;
            xOffset = widget->x;
            lines++;
        }
        xOffset += draw_letter(letter, data, widget, t->face, xOffset, yOffset, w_width, w_height);
    }
    t->font_height = t->face->size->metrics.height >> 6;
    t->total_lines = lines;
}

void draw_textfield(struct Widget* widget, uint32_t *data, int w_width, int w_height)
{
    draw_text(widget, data, w_width, w_height);
    draw_focus(widget, data, w_width, w_height);
}

static void force_cursor_state(struct TextField* textfield, int state)
{
    textfield->cursor_visible = 1;
    time_t timer;
    time(&timer);
    textfield->last_blink = timer+1;
}



static int insert_char(struct TextField* textfield, char new_char, int position)
{
    return gb_insert(&(textfield->gb), new_char, position);
}

int remove_char(struct TextField* textfield, int position)
{
    return gb_remove(&(textfield->gb), position);
}


void key_press_up(struct TextField* textfield)
{
    //NAIVE Implementation: set_cursor_position recalculates the cursor position for each character change
    if( textfield->cursor.y > textfield->base->y) //This will break if we added padding
    {
        struct Cursor current_cursor = textfield->cursor;
        textfield->cursor.index--;
        set_cursor_position(textfield, textfield->cursor.index);

        int t = get_character_width(textfield, gb_get(&(textfield->gb), current_cursor.index)) ;
        while(textfield->cursor.x > current_cursor.x + (t/2) || 
            textfield->cursor.y >= current_cursor.y && 
            textfield->cursor.index < current_cursor.index)
        {
            textfield->cursor.index--;
            set_cursor_position(textfield, textfield->cursor.index);
            t = get_character_width(textfield, gb_get(&(textfield->gb), current_cursor.index)) ;
        }
        force_cursor_state(textfield, 1);
    }
}

int get_character_width(struct TextField* textfield, char letter)
{
    FT_Load_Char(textfield->face, letter, FT_LOAD_RENDER);
    FT_Render_Glyph( textfield->face->glyph, FT_RENDER_MODE_NORMAL ); 
    return textfield->face->glyph->advance.x >> 6;
}

void add_letter_length_to_cursor(struct TextField* textfield, struct Cursor* cursor, char letter)
{
    FT_Load_Char(textfield->face, letter, FT_LOAD_RENDER);
    FT_Render_Glyph( textfield->face->glyph, FT_RENDER_MODE_NORMAL ); 
    if((textfield->face->glyph->advance.x >> 6) > textfield->base->width) //Characters are too wide for the width, then dont display anything
        return;

    if(letter == '\n' || (textfield->face->glyph->advance.x >> 6) + (cursor->x) > textfield->base->x + textfield->base->width) //if newline or next character will go past edge
    {
        cursor->y += (textfield->face->size->metrics.height >> 6) + LINE_SPACEING;
        (cursor->line)++;
        cursor->x = textfield->base->x;
    }
    if(letter != '\n')
        cursor->x += textfield->face->glyph->advance.x >> 6;
}


void key_press_down(struct TextField* textfield)
{

    if(textfield->cursor.line >= textfield->total_lines) // do nothing if on last line
        return;

    struct Cursor current_cursor = textfield->cursor;

    int t = get_character_width(textfield, gb_get(&(textfield->gb), current_cursor.index)) ;

    while(current_cursor.x + (t/2) < textfield->cursor.x   || 
        current_cursor.y <= textfield->cursor.y )
    {

        if(current_cursor.index > textfield->gb.size-1) // End when cursor reaches the last position
            break;

        char letter = gb_get(&(textfield->gb), current_cursor.index);

        struct Cursor backtrack_cursor = current_cursor;
    
        add_letter_length_to_cursor(textfield, &current_cursor, letter);
        current_cursor.index++;

        if(current_cursor.line > textfield->cursor.line+1) //track backwards if we jumped multiple liens 
        {
            current_cursor = backtrack_cursor;
            break;
        }
        t = get_character_width(textfield, gb_get(&(textfield->gb), current_cursor.index)) ;

    }
    textfield->cursor = current_cursor;
    force_cursor_state(textfield, 1);
}

void key_press_return_key(struct TextField* textfield)
{
    if( insert_char(textfield, '\n', textfield->cursor.index) )
        shift_cursor_right(textfield);
    force_cursor_state(textfield, 1);
}

void key_press_backspace_key(struct TextField* textfield)
{

    if( remove_char(textfield, textfield->cursor.index-1) )
        set_cursor_position(textfield, --textfield->cursor.index);
    force_cursor_state(textfield, 1);
}

void key_press_left_key(struct TextField* textfield)
{
    if(textfield->cursor.index > 0)
        set_cursor_position(textfield, textfield->cursor.index-1);
    force_cursor_state(textfield, 1);
}


void shift_cursor_right(struct TextField* textfield)
{
        add_letter_length_to_cursor(textfield, 
                                    &(textfield->cursor),
                                    gb_get(&(textfield->gb), textfield->cursor.index));
    textfield->cursor.index += 1; 
}

void key_press_right_key(struct TextField* textfield)
{
    if(textfield->cursor.index <= textfield->gb.size-1)
        shift_cursor_right(textfield);
    force_cursor_state(textfield, 1);
}

void key_press_ascii_key(struct TextField* textfield, int sym)
{
    if(insert_char(textfield, sym, textfield->cursor.index))
        shift_cursor_right(textfield);
    force_cursor_state(textfield, 1);
}


void key_press_textfield(struct Widget* widget, uint32_t state, int sym)
{
    struct TextField* textfield = widget->child;
    if(state != WL_KEYBOARD_KEY_STATE_PRESSED)
        return;

    switch(sym) {
        case RETURN_KEY:
            key_press_return_key(textfield);
            break;
        case BACKSPACE_KEY:
            key_press_backspace_key(textfield);
            break;
        case LEFT_ARROW_KEY:
            key_press_left_key(textfield);
            break;
        case RIGHT_ARROW_KEY:
            key_press_right_key(textfield);
            break;
        case DOWN_ARROW_KEY:
            key_press_down(textfield);
            break;
        case UP_ARROW_KEY:
            key_press_up(textfield);
            break;
    default:
        if(sym >= ASCII_MIN && sym <= ASCII_MAX)
                key_press_ascii_key(textfield, sym);
    }
} 



void init_default_textfield(struct TextField* textfield)
{
    textfield->cursor.x = 0;
    textfield->cursor.y = 0;
    textfield->cursor.index = 0;
    textfield->cursor.line = 0;
    
    textfield->cursor_visible = 0;

    textfield->last_blink = 0;
    textfield->base = NULL;

    strcpy(textfield->font, "");
    gb_gap_buffer_default(&(textfield->gb));

}

void init_font(struct TextField* textfield, char* font)
{
    if(strlen(font) >= MAX_FONT)
    {
        printf("Error: Font filename too long");
        exit(1);
    }
    strncpy(textfield->font, font, MAX_FONT-1);


    FT_Init_FreeType(&(textfield->library));
    FT_New_Face(textfield->library, textfield->font, 0, &(textfield->face));
    FT_Set_Char_Size(textfield->face, 0, 16 * 128, 100, 100);
}

void init_textfield(struct TextField* textfield,
                    char* font,
                    char* text,
                    int text_length,
                    int x,
                    int y,
                    int width,
                    int height,
                    int max_length,
                    void (*draw)(struct Widget*, uint32_t*, int, int),
                    void (*key_press)(struct Widget*, uint32_t state, int),
                    void (*focus)(struct Widget*)
                    )
{
    init_default_textfield(textfield);
    init_font(textfield, font);
    gb_gap_buffer_init(&(textfield->gb), max_length);

    gb_set_text( &(textfield->gb), text, text_length);
    textfield->cursor.index = textfield->gb.size;

    textfield->base = (struct Widget*)malloc(sizeof(struct Widget));
    if (textfield->base == NULL) {
        perror("Memory allocation failed");
        free(textfield);
        exit(1);
    }
    init_widget(textfield->base, x, y, height, width, textfield, draw, key_press, focus);
    set_cursor_position(textfield, textfield->cursor.index);
}


struct TextField* create_textfield(int x, int y, char font[], int width, int height, char text[], int max_length) {
    struct TextField* textField = (struct TextField*)malloc(sizeof(struct TextField));
    if (textField == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }
    init_textfield(textField, font, text, strlen(text), x, y, width, height, max_length, draw_textfield, key_press_textfield, focus_textfield);
    return textField;
}

