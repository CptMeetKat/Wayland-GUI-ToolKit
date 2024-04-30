#include "command_insert.h"
#include "command_remove.h"
#include "history.h"
#define ASCII_MIN 32
#define ASCII_MAX 126
#define RETURN_KEY 65293
#define BACKSPACE_KEY 65288
#define LEFT_ARROW_KEY 65361
#define RIGHT_ARROW_KEY 65363
#define DOWN_ARROW_KEY 65364
#define UP_ARROW_KEY 65362
#define L_CONTROL 65507
#define CURSOR_WIDTH 1

#include "gui-textfield.h"
#include "gui-widget.h"
#include "xdg-shell-client-protocol.h" //these are only included for an enum, mb unnecessary coupling
#include "time.h"


void add_letter_to_cursor(struct TextField* textfield, struct Cursor* cursor, char letter);
void shift_cursor_right(struct TextField* textfield);
int get_character_width(struct TextField* textfield, char letter);
char get_char(struct TextField* textfield, int position);
int is_word_wrap_position(struct TextField* textfield, int position);
static void generate_wrap_format_array(struct TextField* textfield);

void text_release_font(struct TextField* textfield)
{
    FT_Done_Face(textfield->face);
    FT_Done_FreeType(textfield->library);
}

void release_textfield(struct TextField* textfield)
{
    gb_release(&(textfield->gb));
    history_release(&(textfield->history));
    text_release_font(textfield);

    free(textfield->base);
    free(textfield);
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
        if( ! in_window(w_width, w_height, widget->x + widget->width-1, widget->y + i))
            continue;
        pos = widget->x + widget->width-1 + (w_width * (i + widget->y));
        data[pos] = 0xFFFF0000;
    }

    
    for(int i = 0; i < hSteps; i++)
    {
        if( ! in_window(w_width, w_height, widget->x + i ,widget->y + widget->height-1))
            continue;
        pos = (widget->x + i) + (w_width * (widget->height + widget->y-1));
        data[pos] = 0xFFFF0000;
    }
}



static int draw_letter(char letter, uint32_t* data, struct Widget* widget, FT_Face face, int xOffset,
                       int yOffset, int w_width, int w_height, int text_color, int background_color)
{
    if(letter == '\n') //Cases not to render
        return 0;

    long ascender = face->size->metrics.ascender >> 6;

    int base_line_offset = (ascender - face->glyph->bitmap_top); //BASELINE offset

    for (int y = 0; y < face->glyph->bitmap.rows; y++) 
    {

        for (int x = 0; x < face->glyph->bitmap.width; x++) 
        {
            if(! in_widget(widget, xOffset, y + yOffset + base_line_offset)) //need to reassess the need of placeing this here
                continue;
            if(!in_window(w_width, w_height, x+xOffset, y+yOffset + base_line_offset)) //need to reassess the need of placeing this here
                continue;

            if(face->glyph->bitmap.buffer[y * face->glyph->bitmap.width + x] >= 128)
                data[((y+yOffset + base_line_offset )*w_width)+x+xOffset] = text_color; //white
            else
                data[((y+yOffset + base_line_offset)*w_width)+x+xOffset] = background_color; //Transparent
        }
    }
    
    return face->glyph->advance.x >> 6;
}

void focus_textfield(struct Widget* widget)
{    
    widget->isFocused = 1;
    struct TextField* textfield = widget->child;
    cursor_set_last_blink((&textfield->cursor) );
}


void draw_cursor(struct Widget* widget, int x, int y, int height, uint32_t *data, int w_width, int w_height)
{
    struct TextField* t = (struct TextField*)widget->child;
    time_t timer;
    time(&timer);

    if(t->cursor.last_blink < timer)
    {
        toggle_cursor(&(t->cursor));
        t->cursor.last_blink = timer;
    } 

    if(t->cursor.cursor_visible) 
    {
        for( int i = 0; i < height; i++)
        {
            if(! in_widget(widget, x, y + i))
                break;

            if(!in_window(w_width, w_height, x, y+i))
                break;

            data[w_width * (i + y) + x] = 0xFF00FF00; 
        }
    }
}

void cursor_to_start_of_line(struct TextField* textfield, struct Cursor* c)
{
    c->x = textfield->base->x;
}

void move_cursor_down_a_line(struct TextField* textfield, struct Cursor* c)
{
    c->y += (textfield->face->size->metrics.height >> 6) + LINE_SPACEING;
    c->line++;
}

void set_cursor_position(struct TextField* textfield, int index)
{
    cursor_init(&(textfield->cursor), textfield->base->x, textfield->base->y, 0, 0);

    for(int i = 0; i < index; i++)
    {
        char letter = get_char(textfield, i);
        add_letter_to_cursor(textfield, &(textfield->cursor), letter);
    }
}

void draw_focus(struct Widget* widget, uint32_t* data, int w_width, int w_height)
{
    if(widget->isFocused)
    {
        struct TextField* t = (struct TextField*)widget->child;
        draw_border(widget,data,w_width,w_height);
        draw_cursor(widget, t->cursor.x, t->cursor.y, t->font_height, data, w_width, w_height);
    }
}

int is_word_wrap_position(struct TextField* textfield, int position)
{
    for(int i = 0; i < textfield->total_wraps; i++)
    {
        if(position == textfield->wrap_positions[i])
            return 1;
    }
    return 0;
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
        char letter = get_char(t, i);
        FT_Load_Char(t->face, letter, FT_LOAD_RENDER);
        FT_Render_Glyph( t->face->glyph, FT_RENDER_MODE_NORMAL );

        if((t->face->glyph->advance.x >> 6) > widget->width) //Characters are too wide for the width, then dont display anything
            break;

        if(letter == '\n' || is_word_wrap_position(t, i)) //if newline or next character will go past edge
        {
            yOffset += (t->face->size->metrics.height >> 6) + LINE_SPACEING;
            xOffset = widget->x;
            lines++;
        }
        xOffset += draw_letter(letter, data, widget, t->face, xOffset, yOffset, w_width, w_height, t->text_color, t->background_color);
    }
    t->font_height = t->face->size->metrics.height >> 6;
    t->last_line = lines;
}

void draw_textfield(struct Widget* widget, uint32_t *data, int w_width, int w_height)
{
    draw_text(widget, data, w_width, w_height);
    draw_focus(widget, data, w_width, w_height);
}


static void generate_wrap_format_array(struct TextField* textfield)
{
    int total_wraps = 0;
    
    int currentX = 0;
    for(int i = 0; i < textfield->gb.size; i++)
    {
        char letter = get_char(textfield, i);
        currentX += get_character_width(textfield, letter);
        if(letter == '\n')
        {
            currentX = 0;
        }
        else if(  ! in_widget(textfield->base, currentX + textfield->base->x + CURSOR_WIDTH, textfield->base->y) ) 
        {
            textfield->wrap_positions[total_wraps++] = i;
            currentX = get_character_width(textfield, letter);
        }
    }

    textfield->total_wraps = total_wraps;
}

int insert_char(struct TextField* textfield, char new_char, int position, int save_history)
{
    int result = gb_insert(&(textfield->gb), new_char, position);

    if(result)
    {
        generate_wrap_format_array(textfield);

        if(save_history)
        {
            struct Command_Remove* cmd = cmd_remove_create(textfield, new_char, position);
            history_add(&(textfield->history), cmd->base);
        }
    }
    return result;
}

int remove_char(struct TextField* textfield, int position, int save_history)
{
    char removed_char = 0;
    int result = gb_remove(&(textfield->gb), position, &removed_char);
    if(result)
    {
        generate_wrap_format_array(textfield);
    
        if(save_history)
        {
            struct Command_Insert* cmd = cmd_insert_create(textfield, removed_char, position);
            history_add(&(textfield->history), cmd->base);
        }

    }
    return result;
}

char get_char(struct TextField* textfield, int position)
{
    return gb_get(&(textfield->gb), position);
}

void move_cursor_up(struct TextField* textfield)
{
    //NAIVE Implementation: set_cursor_position recalculates the cursor position for each character change
    struct Cursor current_cursor = textfield->cursor;
    textfield->cursor.index--;
    set_cursor_position(textfield, textfield->cursor.index);

    int tolerance = get_character_width(textfield, get_char(textfield, current_cursor.index)) / 2;
    while(textfield->cursor.x > current_cursor.x + tolerance || 
        textfield->cursor.y >= current_cursor.y && 
        textfield->cursor.index < current_cursor.index)
    {
        textfield->cursor.index--;
        set_cursor_position(textfield, textfield->cursor.index);
        tolerance = get_character_width(textfield, get_char(textfield, current_cursor.index)) / 2;
    }
    
}

void key_press_up(struct TextField* textfield)
{
    if( textfield->cursor.y > textfield->base->y) //This will break if we added padding
    {
        if(  textfield->cursor_jump_direction == DOWN && textfield->cursor_jumps.size > 0  )
        {
            struct Cursor* prev_cursor = stack_pop(&(textfield->cursor_jumps));
            textfield->cursor = *prev_cursor;
            free(prev_cursor);
            set_cursor_position(textfield, textfield->cursor.index);
        }
        else
        {
            textfield->cursor_jump_direction = UP;
            struct Cursor* prev_cursor = malloc(sizeof(struct Cursor));
            *prev_cursor = textfield->cursor;
            stack_push(&(textfield->cursor_jumps), prev_cursor);
            move_cursor_up(textfield);
        }
        cursor_force_show(&(textfield->cursor));
    }


}

int get_character_width(struct TextField* textfield, char letter)
{
    FT_Load_Char(textfield->face, letter, FT_LOAD_RENDER);
    FT_Render_Glyph( textfield->face->glyph, FT_RENDER_MODE_NORMAL ); 
    return textfield->face->glyph->advance.x >> 6;
}

void add_letter_to_cursor(struct TextField* textfield, struct Cursor* cursor, char letter)
{
    int char_width = get_character_width(textfield,letter);
    //Investigate - bad seperation of concerns here, which means this section above can go in if statement
    if(char_width > textfield->base->width) //Characters are too wide for the width, then dont display anything
        return;

    if(letter == '\n' || 
        is_word_wrap_position(textfield, cursor->index+1))
    {
        cursor_to_start_of_line(textfield, cursor);
        move_cursor_down_a_line(textfield, cursor);
    }
    else if(cursor->x+char_width + CURSOR_WIDTH > textfield->base->x + textfield->base->width)
    {
        cursor_to_start_of_line(textfield, cursor);
        move_cursor_down_a_line(textfield, cursor);
        cursor->x += textfield->face->glyph->advance.x >> 6;
    }
    else
        cursor->x += textfield->face->glyph->advance.x >> 6;
    cursor->index += 1; 
}

void move_cursor_down(struct TextField* textfield)
{
    struct Cursor current_cursor = textfield->cursor;

    int tolerance = get_character_width(textfield, get_char(textfield, current_cursor.index)) / 2;

    while(current_cursor.x + tolerance < textfield->cursor.x   || 
        current_cursor.y <= textfield->cursor.y )
    {
        if(current_cursor.index > textfield->gb.size-1) // End when cursor reaches the last position
            break;

        struct Cursor backtrack_cursor = current_cursor;

        char letter = get_char(textfield, current_cursor.index);
        add_letter_to_cursor(textfield, &current_cursor, letter);

        if(current_cursor.line > textfield->cursor.line+1) //track backwards if we jumped multiple liens 
        {
            current_cursor = backtrack_cursor;
            break;
        }
        tolerance = get_character_width(textfield, get_char(textfield, current_cursor.index)) / 2;

    }
    textfield->cursor = current_cursor;
    cursor_force_show(&(textfield->cursor));
}



void key_press_down(struct TextField* textfield)
{
    if(textfield->cursor.line >= textfield->last_line) // do nothing if on last line
        return;

    if(  textfield->cursor_jump_direction == UP && textfield->cursor_jumps.size > 0  )
    {
        struct Cursor* prev_cursor = stack_pop(&(textfield->cursor_jumps));
        textfield->cursor = *prev_cursor;
        free(prev_cursor);
        set_cursor_position(textfield, textfield->cursor.index);
    }
    else
    {
        textfield->cursor_jump_direction = DOWN;
        struct Cursor* prev_cursor = malloc(sizeof(struct Cursor));
        *prev_cursor = textfield->cursor;
        stack_push(&(textfield->cursor_jumps), prev_cursor);
        move_cursor_down(textfield);
    }
    cursor_force_show(&(textfield->cursor));

}

void key_press_return_key(struct TextField* textfield)
{
    if( insert_char(textfield, '\n', textfield->cursor.index, 1) )
    {
        clear_cursor_jumps(textfield);
        set_cursor_position(textfield, ++textfield->cursor.index);
    }
    cursor_force_show(&(textfield->cursor));
}

void key_press_backspace_key(struct TextField* textfield)
{
    if( remove_char(textfield, textfield->cursor.index-1, 1) )
    {
        clear_cursor_jumps(textfield);
        set_cursor_position(textfield, --textfield->cursor.index);
    }
    cursor_force_show(&(textfield->cursor));
}

void shift_cursor_left(struct TextField* textfield)
{
    if(textfield->cursor.index <= 0)
        return;

    clear_cursor_jumps(textfield);
    set_cursor_position(textfield, textfield->cursor.index-1);
}

void key_press_left_key(struct TextField* textfield)
{
    shift_cursor_left(textfield);
    cursor_force_show(&(textfield->cursor));
}


void shift_cursor_right(struct TextField* textfield)
{
    if(textfield->cursor.index > textfield->gb.size-1)
        return;

    clear_cursor_jumps(textfield);
    add_letter_to_cursor(textfield, 
                                    &(textfield->cursor),
                                    get_char(textfield, textfield->cursor.index));
}

void key_press_right_key(struct TextField* textfield)
{
    shift_cursor_right(textfield);
    cursor_force_show(&(textfield->cursor));
}

void key_press_ascii_key(struct TextField* textfield, int sym)
{
    if(insert_char(textfield, sym, textfield->cursor.index, 1))
    {
        clear_cursor_jumps(textfield);
        set_cursor_position(textfield, ++textfield->cursor.index);
    }
    cursor_force_show(&(textfield->cursor));
}

void undo(struct Widget* widget)
{
    struct TextField* textfield = widget->child;
    history_undo(&(textfield->history)); 
    clear_cursor_jumps(textfield);
}

void redo(struct Widget* widget)
{
    struct TextField* textfield = widget->child;
    history_redo(&(textfield->history)); 
    clear_cursor_jumps(textfield);
}

void handle_modifier_key_press(struct Widget* widget, uint32_t state, int modifier, int sym)
{
    if(modifier == L_CONTROL && sym == 'z')
         undo(widget); 
    else if(modifier == L_CONTROL && sym == 'y')
         redo(widget);
    
}

static void clear_cursor_jumps(struct TextField* textfield)
{
    struct Stack* jumps = &(textfield->cursor_jumps);
    while(jumps->size > 0)
    {
       int* pos = stack_pop(jumps); 
       free(pos);
    }

}

void key_press_textfield(struct Widget* widget, uint32_t state, int modifier, int sym)
{
    struct TextField* textfield = widget->child;
    if(state != WL_KEYBOARD_KEY_STATE_PRESSED)
        return;

    if(modifier != -1)
    {
        handle_modifier_key_press(widget, state, modifier, sym);
    }
    else
    {
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
} 



void init_default_textfield(struct TextField* textfield)
{
    cursor_default(&(textfield->cursor));

    textfield->base = NULL;

    strcpy(textfield->font, "");
    gb_gap_buffer_default(&(textfield->gb));
    textfield->total_wraps = 0;

}

void init_font(struct TextField* textfield, char* font)
{
    if(strlen(font) >= MAX_FONT)
    {
        perror("Error: Font filename too long");
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
                    void (*key_press)(struct Widget*, uint32_t state, int, int),
                    void (*focus)(struct Widget*),
                    int text_color,
                    int background_color
                    )
{
    init_default_textfield(textfield);
    init_font(textfield, font);
    gb_gap_buffer_init(&(textfield->gb), max_length);
    history_init(&(textfield->history));
    stack_init(&(textfield->cursor_jumps), MAX_LINES);
    textfield->cursor_jump_direction = UNSET;
    gb_set_text( &(textfield->gb), text, text_length);
    textfield->cursor.index = textfield->gb.size;
    textfield->text_color = text_color;
    textfield->background_color = background_color;

    textfield->base = (struct Widget*)malloc(sizeof(struct Widget));
    if (textfield->base == NULL) {
        perror("Memory allocation failed");
        free(textfield);
        exit(1);
    }
    init_widget(textfield->base, x, y, height, width, textfield, draw, key_press, focus);
    generate_wrap_format_array(textfield);

    for(int i = 0; i < textfield->gb.size; i++)
    {
        if(get_char(textfield, i) == '\n')
            textfield->last_line++;
    }

    textfield->last_line += textfield->total_wraps;
    
    set_cursor_position(textfield, textfield->cursor.index);
}


struct TextField* create_textfield(int x, int y, char font[], int width, int height, char text[], int max_length, int text_color, int background_color) {
    struct TextField* textField = (struct TextField*)malloc(sizeof(struct TextField));
    if (textField == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }
    init_textfield(textField, font, text, strlen(text), x, y, width, height, max_length, draw_textfield, key_press_textfield, focus_textfield, text_color, background_color);
    return textField;
}

