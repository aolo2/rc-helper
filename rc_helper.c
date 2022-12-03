enum layout_kind {    
    SIMPLE = 0,
    VERTICAL = 1,
    HORIZONTAL = 2,
    GRID = 3,
};

struct pos {
    SHORT x;
    SHORT y;
};

struct layout_nested {
    enum layout_kind kind;
    int x, y;    
    int element_x, element_y;
    int width, height;
    int ncols, nrows;
    int col_width, row_height;

    int max_height;
    int max_width;
    int spacing;

    WCHAR *title;
};

#define MAX_LAYOUT_STACK_DEPTH 16

struct layout {
    struct layout_nested stack[MAX_LAYOUT_STACK_DEPTH];
    
    char *data;
    char *head;
    size_t size;

    int depth;
    WORD nitems;
};

static void
write_byte(struct layout *l, BYTE v)
{
    Assert(l->size - (l->head - l->data) >= sizeof(v));
    CopyMemory(l->head, &v, sizeof(v));
    l->head += sizeof(v);
}

static void
write_short(struct layout *l, short v)
{
    Assert(l->size - (l->head - l->data) >= sizeof(v));
    CopyMemory(l->head, &v, sizeof(v));
    l->head += sizeof(v);
}

static void
write_word(struct layout *l, WORD v)
{
    Assert(l->size - (l->head - l->data) >= sizeof(v));
    CopyMemory(l->head, &v, sizeof(v));
    l->head += sizeof(v);
}

static void
write_dword(struct layout *l, DWORD v)
{
    Assert(l->size - (l->head - l->data) >= sizeof(v));
    CopyMemory(l->head, &v, sizeof(v));
    l->head += sizeof(v);
}

static void
write_buffer(struct layout *l, void *data, SIZE_T size)
{
    data = (BYTE *) data;
    Assert(l->size - (l->head - l->data) >= size);
    CopyMemory(l->head, data, size);
    l->head += size;
}


// Recommended sizes and spacing. Everything in dialog units, taken from
// https://learn.microsoft.com/en-us/windows/win32/uxguide/vis-layout#recommended-sizing-and-spacing

// Control sizes
static const int RECOMMENDED_CHECKBOX_HEIGHT = 10;
static const int RECOMMENDED_COMBOBOX_HEIGHT = 14;
static const int RECOMMENDED_BUTTON_WIDTH = 50;
static const int RECOMMENDED_BUTTON_HEIGHT = 14;
static const int RECOMMENDED_COMMAND_LINK_HEIGHT_1L = 25;
static const int RECOMMENDED_COMMAND_LINK_HEIGHT_2L = 35;
static const int RECOMMENDED_DROPDOWN_LIST_HEIGHT = 14;
static const int RECOMMENDED_PROGRESSBAR_HEIGHT = 8;
static const int RECOMMENDED_PROGRESSBAR_WIDTH1 = 107;
static const int RECOMMENDED_PROGRESSBAR_WIDTH2 = 237;
static const int RECOMMENDED_RADIO_BUTTON_HEIGHT = 10;
static const int RECOMMENDED_SLIDER_HEIGHT = 15;
static const int RECOMMENDED_STATIC_HEIGHT = 8;
static const int RECOMMENDED_TEXTBOX_HEIGHT = 14;

// Spacing
static const int RECOMMENDED_SPACING_DIALOG_MARGIN = 7;
static const int RECOMMENDED_VSPACING_TEXT_LABEL_ABOVE_CONTROL = 3;
static const int RECOMMENDED_VSPACING_RELATED_CONTROLS = 4;
static const int RECOMMENDED_VSPACING_UNRELATED_CONTROLS = 7;
static const int RECOMMENDED_VSPACING_FIRST_CONTROL_IN_GROUPBOX = 11;
static const int RECOMMENDED_VSPACING_GROUPBOX_CONTROLS = 4;
static const int RECOMMENDED_SPACING_BUTTONS = 4;
static const int RECOMMENDED_VSPACING_GROUPBOX_BOTTOM = 7;
static const int RECOMMENDED_HSPACING_GROUPBOX_LEFT = 6;
static const int RECOMMENDED_VSPACING_LABEL_BESIDES_CONTROL_TOP = 3;
static const int RECOMMENDED_VSPACING_PARAGRAPHS = 7;
static const int RECOMMENDED_SPACINH_SMALLEST_BETNWEEN_INTERACTIVE_CONTROLS = 3;
static const int RECOMMENDED_SPACINH_SMALLEST_BETNWEEN_NONINTERACTIVE_CONTROLS = 2;

// Pass this instead of width/height where applicable to autocompute instead
static const DIALOG_AUTO = -1;

static const WORD CONTROL_BUTTON = 0x0080;
static const WORD CONTROL_EDIT   = 0x0081;
static const WORD CONTROL_STATIC = 0x0082;
static const WORD CONTROL_COMBOBOX = 0x0085;

static int
_dialog_push_nested(struct layout *l, struct layout_nested ln)
{
    if (l->depth >= MAX_LAYOUT_STACK_DEPTH) {
        return(1);
    }

    l->stack[l->depth++] = ln;

    return(0);
}

static int
_dialog_push_simple(struct layout *l, int x, int y, int width, int height)
{
    struct layout_nested ln = {
        .kind = SIMPLE,
        .x = x,
        .y = y,
        .element_x = x,
        .element_y = y,
        .width = width,
        .height = height
    };

    return(_dialog_push_nested(l, ln));
}    
    
static void
pad_and_align_writer(struct layout *l, SIZE_T size)
{    
	SIZE_T pointer = (SIZE_T) l->head;
	l->head += ((pointer + size - 1) & ~(size - 1)) - pointer;    

    BYTE *old_head = (BYTE *) pointer;
    BYTE *new_head = (BYTE *) l->head;

    if (old_head != new_head) {
        ZeroMemory((void *) old_head, new_head - old_head);
    }
}

static struct pos
element(struct layout *l, int *width, int *height)
{
    struct layout_nested *top = l->stack + l->depth - 1;
    struct pos result = { 0 };

    // TODO: place element at element_x, element_y
    
    switch (top->kind) {
        case VERTICAL: {
            if (*width == DIALOG_AUTO) {
                *width = top->width;
            }

            if (*height == DIALOG_AUTO) {
                DebugBreak();
            }                      

            result.x = top->element_x;
            result.y = top->element_y;

            top->element_y += *height + top->spacing;            

            break;
        }

        case HORIZONTAL: {
            if (*height == DIALOG_AUTO) {
                *height = top->height;
            }

            if (*width == DIALOG_AUTO) {
                DebugBreak();
            }

            result.x = top->element_x;
            result.y = top->element_y;
            
            // printf("at (%d, %d) | size (%d, %d)\n", x, y, *width, *height);

            top->element_x += *width + top->spacing;
            break;
        }

        case GRID: {
            if (*width == DIALOG_AUTO) {
                *width = top->col_width;
            }

            if (*height == DIALOG_AUTO) {
                *height = top->row_height;
            }

            result.x = top->x + top->element_x * top->col_width;
            result.y = top->y + top->element_y * top->row_height;

            // printf("at (%d, %d) | size (%d, %d)\n", x, y, *width, *height);

            top->element_x++;
            if (top->element_x == top->ncols) {
                top->element_y++;
                top->element_x = 0;
            }
            break;
        }

        case SIMPLE: {
            // DebugBreak();
            break;
        }
    }

    if (*width > top->max_width) {
        top->max_width = *width;
    }

    if (*height> top->max_height) {
        top->max_height= *height;
    }

    l->nitems++;

    return(result);
}

static void
write_control(struct layout *l,
               DWORD helpId, DWORD exStyle, DWORD style, 
               SHORT x, SHORT y, SHORT cx, SHORT cy, 
               DWORD id, WORD className, WCHAR *text)
{
    pad_and_align_writer(l, sizeof(DWORD));

    write_dword(l, helpId);
    write_dword(l, exStyle);
    write_dword(l, style | WS_CHILD | WS_VISIBLE);
    write_short(l, x);
    write_short(l, y);
    write_short(l, cx);
    write_short(l, cy);
    write_dword(l, id);
    write_word(l, 0xFFFF);
    write_word(l, className);
    
    if (text) {
        write_buffer(l, text, wcslen(text) * sizeof(WCHAR));
        write_word(l, 0x0000);
    } else {
        write_word(l, 0x0000);
    }

    write_word(l, 0);
}

static void
button(struct layout *l, DWORD style, int width, int height, DWORD id, WCHAR *text)
{
    if (height == DIALOG_AUTO) {
        height = RECOMMENDED_BUTTON_HEIGHT;
    }

    struct pos at = element(l, &width, &height);    

    write_control(l,
        0,
        0,
        style | (WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON),
        at.x,
        at.y,
        width,
        height,
        id,
        CONTROL_BUTTON,
        text
    );
}

static void
text(struct layout *l, DWORD style, int width, int height, WCHAR *text)
{
    if (height == DIALOG_AUTO) {
        height = RECOMMENDED_STATIC_HEIGHT;
    }

    struct pos at = element(l, &width, &height);    

    write_control(l,
        0,
        0,
        style | (WS_CHILD | WS_VISIBLE),
        at.x,
        at.y,
        width,
        height,
        -1,
        CONTROL_STATIC,
        text
    );
}

static void
label(struct layout *l, DWORD style, int width, int height, WCHAR *text)
{
    if (height == DIALOG_AUTO) {
        height = RECOMMENDED_STATIC_HEIGHT;
    }

    struct pos at = element(l, &width, &height);    

    write_control(l,
        0,
        0,
        style | (WS_CHILD | WS_VISIBLE),
        at.x,
        at.y + RECOMMENDED_VSPACING_LABEL_BESIDES_CONTROL_TOP,
        width,
        height,
        -1,
        CONTROL_STATIC,
        text
    );
}

static void
edit(struct layout *l, DWORD style, int width, int height)
{
    if (height == DIALOG_AUTO) {
        height = RECOMMENDED_TEXTBOX_HEIGHT;
    }

    struct pos at = element(l, &width, &height);    
   
    write_control(l,
        0,
        0,
        style | (WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL),
        at.x,
        at.y,
        width,
        height,
        -1,
        CONTROL_EDIT,
        0
    );
}

static void
checkbox(struct layout *l, DWORD style, int width, int height, WCHAR *text)
{
    if (height == DIALOG_AUTO) {
        height = RECOMMENDED_CHECKBOX_HEIGHT;
    }

    struct pos at = element(l, &width, &height);     

    write_control(l,
        0,
        0,
        style | (WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX),
        at.x,
        at.y,
        width,
        height,
        -1,
        CONTROL_BUTTON,
        text
    );
}

static void
combobox(struct layout *l, DWORD style, int width, int height)
{
    if (height == DIALOG_AUTO) {
        height = RECOMMENDED_COMBOBOX_HEIGHT;
    }

    struct pos at = element(l, &width, &height);         

    write_control(l,
        0,
        0,
        style | (WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST),
        at.x,
        at.y,
        width,
        height,
        -1,
        CONTROL_COMBOBOX,
        0
    );
}

static void
groupbox(struct layout *l, int width, int height, WCHAR *title)
{
    struct pos at = element(l, &width, &height);         

    write_control(l,
        0,
        0,
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        at.x,
        at.y,
        width,
        height,
        -1,
        CONTROL_BUTTON,
        title
    );
}

static void
radio(struct layout *l, DWORD style, int width, int height, WCHAR *title)
{
    struct pos at = element(l, &width, &height);         

    write_control(l,
        0,
        0,
        style | (WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON),
        at.x,
        at.y,
        width,
        height,
        -1,
        CONTROL_BUTTON,
        title
    );
}

static void
vskip(struct layout *l, int size)
{
    struct layout_nested *top = l->stack + l->depth - 1;
    
    if (top->kind != VERTICAL) {
        DebugBreak();
    }

    top->element_y += size;
}

static void
hskip(struct layout *l, int size)
{
    struct layout_nested *top = l->stack + l->depth - 1;
    
    if (top->kind != HORIZONTAL) {
        DebugBreak();
    }

    top->element_x += size;
}

static struct layout
dialog_start(int width, int height, char *data, int size, DWORD style, WCHAR *title, WORD pointSize,
             WCHAR *typeface)
{
    struct layout l = { 
        .data = data,
        .head = data,
        .size = size
    };

    if (!title) {
        title = L"unnamed dialog";
    }

    if (!typeface) {
        typeface = L"MS Shell Dlg";
    }

    write_word(&l, 1);                             // dlgVer
    write_word(&l, 0xFFFF);                        // signature
    write_dword(&l, 0);                            // helpID
    write_dword(&l, 0);                            // exStyle
    write_dword(&l, style | (WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_SETFONT)); // style
    write_word(&l, 0);                             // cDlgItems
    write_short(&l, 0);                            // x
    write_short(&l, 0);                            // y
    write_short(&l, width + RECOMMENDED_SPACING_DIALOG_MARGIN * 2);                        // cx
    write_short(&l, height + RECOMMENDED_SPACING_DIALOG_MARGIN * 2);                       // cy
    write_word(&l, 0);                             // menu
    write_word(&l, 0);                             // windowClass
    
    write_buffer(&l, title, wcslen(title) * sizeof(WCHAR)); // title[titleLen]
    write_word(&l, 0x0000);

    write_word(&l, pointSize);                     // pointsize
    write_word(&l, FW_NORMAL);                     // weight
    write_byte(&l, 0);                             // italic
    write_byte(&l, DEFAULT_CHARSET);               // charset
    
    write_buffer(&l, typeface, wcslen(typeface) * sizeof(WCHAR));  // typeface[stringLen]
    write_word(&l, 0x0000);

    _dialog_push_simple(&l, 
        RECOMMENDED_SPACING_DIALOG_MARGIN,
        RECOMMENDED_SPACING_DIALOG_MARGIN,
        width,
        height
    );

    return(l);
}

static void
groupbox_start(struct layout *l, int width, int height, WCHAR *title, int spacing)
{    
    if (spacing == DIALOG_AUTO) {
        spacing = RECOMMENDED_VSPACING_GROUPBOX_CONTROLS;
    }
    
    struct layout_nested top = l->stack[l->depth - 1];    

    if (width == DIALOG_AUTO) {
        width = top.width; 
    }
    
    struct layout_nested row = {
        .kind = VERTICAL,
        .x = top.element_x,
        .y = top.element_y,
        .element_x = top.element_x + RECOMMENDED_HSPACING_GROUPBOX_LEFT,
        .element_y = top.element_y + RECOMMENDED_VSPACING_FIRST_CONTROL_IN_GROUPBOX,
        .width = width,
        .height = height,
        .spacing = spacing,
        .title = title
    };

     _dialog_push_nested(l, row);
}

static void
groupbox_end(struct layout *l)
{
    struct layout_nested box = l->stack[l->depth - 1];

    int width = box.width;  
    int height = box.height;

    if (height == DIALOG_AUTO) {
        height = box.element_y - box.y;
    }

    l->depth--;

    groupbox(l, width, height, box.title);
}

static void
row_start(struct layout *l, int height, int spacing)
{
    if (spacing == DIALOG_AUTO) {
        spacing = RECOMMENDED_VSPACING_RELATED_CONTROLS;
    }

    struct layout_nested top = l->stack[l->depth - 1];

    if (height == DIALOG_AUTO) {
        height = top.height;
    }
    
    struct layout_nested row = {
        .kind = HORIZONTAL,
        .x = top.element_x,
        .y = top.element_y,
        .element_x = top.element_x,
        .element_y = top.element_y,
        .width = DIALOG_AUTO,
        .height = height,
        .spacing = spacing,
    };

    _dialog_push_nested(l, row);
}

static void
row_end(struct layout *l)
{
    int row_height = l->stack[l->depth - 1].height;

    if (row_height == DIALOG_AUTO) {
        // If not specified, row height is computed as the maximum height of all the direct row children
        row_height = l->stack[l->depth - 1].max_height;
    }

    l->depth--;

    struct layout_nested *top = l->stack + l->depth - 1;
    
    switch (top->kind) {
        case VERTICAL: {
            top->element_y += row_height + top->spacing;
            break;
        }

        case GRID: {
            top->element_x++;
            if (top->element_x == top->ncols) {
                top->element_y++;
                top->element_x = 0;
            }
            break;
        }
    
        case SIMPLE: {
            break;
        }

        case HORIZONTAL: {
            DebugBreak();
        }
    }    
}

static void
column_start(struct layout *l, int width, int spacing)
{
    if (spacing == DIALOG_AUTO) {
        spacing = RECOMMENDED_VSPACING_RELATED_CONTROLS;
    }
    
    struct layout_nested top = l->stack[l->depth - 1];    

    if (width == DIALOG_AUTO) {
        width = top.width;
    }
    
    struct layout_nested row = {
        .kind = VERTICAL,
        .x = top.element_x,
        .y = top.element_y,
        .element_x = top.element_x,
        .element_y = top.element_y,
        .width = width,
        .height = DIALOG_AUTO,
        .spacing = spacing,
    };

    _dialog_push_nested(l, row);
}

static void
column_end(struct layout *l)
{
    int col_width = l->stack[l->depth - 1].width;

    if (col_width == DIALOG_AUTO) {
        // If not specified, column width is computed as the maximum width of all the direct row children
        col_width = l->stack[l->depth - 1].max_width;
    }

    l->depth--;

    struct layout_nested *top = l->stack + l->depth - 1;
    
    switch (top->kind) {
        case HORIZONTAL: {
            top->element_x += col_width + top->spacing;
            break;
        }

        case GRID: {
            top->element_x++;
            if (top->element_x == top->ncols) {
                top->element_y++;
                top->element_x = 0;
            }
            break;
        }
    
        case SIMPLE: {
            break;
        }

        case VERTICAL: {
            DebugBreak();
        }
    }    
}

static void
grid_start(struct layout *l, int nrows, int ncols, int row_height, int col_width)
{
    // TODO: unevenly sized columns/rows, accept an array of sizes

    struct layout_nested top = l->stack[l->depth - 1];            
    if (col_width == DIALOG_AUTO) {
        col_width = top.width / ncols;
    }

    struct layout_nested row = {
        .kind = GRID,
        .x = top.element_x,
        .y = top.element_y,
        .element_x = 0,
        .element_y = 0,
        .width = top.height,
        .height = top.width,
        .col_width = col_width,
        .row_height = row_height,
        .ncols = ncols,
        .nrows = nrows
    };

    _dialog_push_nested(l, row);
}

static void
grid_end(struct layout *l)
{
    struct layout_nested grid = l->stack[l->depth - 1];

    int width = grid.width;
    int height = grid.nrows * grid.row_height;
    
    l->depth--;

    struct layout_nested *top = l->stack + l->depth - 1;
    
    switch (top->kind) {
        case HORIZONTAL: {
            top->element_x += width + top->spacing;
            break;
        }

        case GRID: {
            top->element_x++;
            if (top->element_x == top->ncols) {
                top->element_y++;
                top->element_x = 0;
            }
            break;
        }
    
        case SIMPLE: {
            break;
        }

        case VERTICAL: {
            top->element_y += height + top->spacing;
        }
    }    
}

static void
set_spacing(struct layout *l, int spacing)
{
    if (spacing == DIALOG_AUTO) {
        spacing = RECOMMENDED_VSPACING_RELATED_CONTROLS;
    }

    struct layout_nested *top = l->stack + l->depth - 1;
    top->spacing = spacing;
}

static void
dialog_end(struct layout *l)
{
    CopyMemory(l->data + 16, &l->nitems, sizeof(WORD));
}