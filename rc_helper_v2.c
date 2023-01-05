enum rc_layout_type {    
    RC_LAYOUT_SIMPLE = 0,
    RC_LAYOUT_VERTICAL = 1,
    RC_LAYOUT_HORIZONTAL = 2,
    RC_LAYOUT_GRID = 3,
};

enum rc_row_flags {
    RC_FLAGS_ROW_DEFAULT = 0x00,
    RC_FLAGS_ROW_RTL = 0x1,
};

enum rc_command_type {
    RC_COMMAND_BEGIN_DIALOG,
    
    RC_COMMAND_BEGIN_ROW,
    RC_COMMAND_BEGIN_COLUMN,
    RC_COMMAND_BEGIN_GRID,
    RC_COMMAND_BEGIN_GROUPBOX,
    
    RC_COMMAND_END_ROW,
    RC_COMMAND_END_COLUMN,
    RC_COMMAND_END_GRID,
    RC_COMMAND_END_GROUPBOX,

    RC_COMMAND_ADD_CONTROL,
    RC_COMMAND_VSKIP,
    RC_COMMAND_HSKIP,
};

static const int RC_RECOMMENDED = -1;
static const int RC_FILL = -2;
static const int RC_NEED_COMPUTE = -3;

static const int RC_DIALOG_PADDING = 7;
static const int RC_GROUPBOX_PADDING_Y = 11;
static const int RC_GROUPBOX_PADDING_X = 4;

static const int RC_BUTTON_WIDTH = 50;
static const int RC_BUTTON_HEIGHT = 14;
static const int RC_CHECKBOX_HEIGHT = 10;
static const int RC_STATIC_HEIGHT = 8;
static const int RC_TEXTBOX_HEIGHT = 14;
static const int RC_COMBOBOX_HEIGHT = 14;
static const int RC_RADIO_HEIGHT = 14;
static const int RC_VERTICAL_SPACING = 4;
static const int RC_HORIZONTAL_SPACING = 4;

static const WORD RC_CONTROL_BUTTON = 0x0080;
static const WORD RC_CONTROL_EDIT   = 0x0081;
static const WORD RC_CONTROL_STATIC = 0x0082;
static const WORD RC_CONTROL_COMBOBOX = 0x0085;

#define RC_MAX_COMMANDS 1024
#define RC_MAX_LAYOUT_STACK_DEPTH 16

struct rc_command {
    enum rc_command_type type;
    enum rc_row_flags flags;

    int width, height;
    int spacing;
    int ncols, nrows;
    int x, y;
    int adjust;

    DWORD helpId;
    DWORD exStyle;
    DWORD style;
    DWORD id;
    WORD className;
    WCHAR *text;
    WCHAR *typeface;
};

struct rc_rect {
    enum rc_layout_type type;
    int x, y, w, h;
    int cur_x, cur_y;
    int cellx, celly;
    int nrows, ncols;
    int spacing;
};

struct rc_dialog {
    int solved;
    struct rc_command commands[RC_MAX_COMMANDS];
    size_t ncommands;
    size_t ncontrols;
};

struct rc_writer {
    char *data;
    int size;
};

// TODO: check for overflows
// TODO: error handling

/********* Internal functions *********/
static void
_rc_pad_and_align_writer(struct rc_writer *w, SIZE_T size)
{    
	SIZE_T pointer = (SIZE_T) w->data;
	w->data += ((pointer + size - 1) & ~(size - 1)) - pointer;    

    BYTE *old_head = (BYTE *) pointer;
    BYTE *new_head = (BYTE *) w->data;

    if (old_head != new_head) {
        ZeroMemory((void *) old_head, new_head - old_head);
    }
}

static void
_rc_write_byte(struct rc_writer *w, BYTE v)
{
    CopyMemory(w->data, &v, sizeof(v));
    w->data += sizeof(v);
    w->size -= sizeof(v);
}

static void
_rc_write_short(struct rc_writer *w, short v)
{
    CopyMemory(w->data, &v, sizeof(v));
    w->data += sizeof(v);
    w->size -= sizeof(v);
}

static void
_rc_write_word(struct rc_writer *w, WORD v)
{
    CopyMemory(w->data, &v, sizeof(v));
    w->data += sizeof(v);
    w->size -= sizeof(v);
}

static void
_rc_write_dword(struct rc_writer *w, DWORD v)
{
    CopyMemory(w->data, &v, sizeof(v));
    w->data += sizeof(v);
    w->size -= sizeof(v);
}

static void
_rc_write_buffer(struct rc_writer *w, void *data, SIZE_T size)
{
    CopyMemory(w->data, data, size);
    w->data += size;
    w->size -= size;
}

static void
_rc_advance_parent(struct rc_rect *parent, struct rc_rect *child)
{
    switch (parent->type) {
        case RC_LAYOUT_VERTICAL: {
            parent->cur_y += child->h + parent->spacing;
            parent->h += child->h + parent->spacing;

            // TODO: do not add spacing after last element

            break;
        }

        case RC_LAYOUT_HORIZONTAL: {
            parent->cur_x += child->w + parent->spacing;
            parent->w += child->w + parent->spacing;

            // TODO: do not add spacing after last element

            break;
        }

        case RC_LAYOUT_GRID: {
            ++parent->cellx;
            
            if (parent->cellx == parent->ncols) { 
                ++parent->celly;
                parent->cellx = 0;
            }

            int spacing_total_x = 0;
            int spacing_total_y = 0;

            if (parent->cellx > 0) {
                spacing_total_x = (parent->cellx - 1) * parent->spacing;
            }

            if (parent->celly > 0) {
                spacing_total_y = (parent->celly - 1) * parent->spacing;
            }

            parent->cur_x = parent->x + spacing_total_x + parent->cellx * parent->w;
            parent->cur_y = parent->y + spacing_total_y + parent->celly * parent->h;

            break;
        }

        case RC_LAYOUT_SIMPLE: {
            // These are used for containers that do not have any direct children except
            // for one single child, which itself is a valid container. These do not
            // need to be advanced by anything when the child is completed.
            // Currently the whole dialog and groupbox have kind RC_LAYOUT_SIMPLE
            break;
        }
    }
}

static void
_rc_command(struct rc_dialog *d, struct rc_command command)
{
    if (command.type == RC_COMMAND_ADD_CONTROL || command.type == RC_COMMAND_BEGIN_GROUPBOX) {
        ++d->ncontrols;
    }

    d->commands[d->ncommands++] = command;
}

static void
_rc_control(struct rc_dialog *d, 
            DWORD helpId, DWORD exStyle, DWORD style,
            SHORT width, SHORT height,
            DWORD id, WORD className, WCHAR *text, int adjust)
{
    struct rc_command command = {
        .type = RC_COMMAND_ADD_CONTROL,
        .helpId = helpId,
        .exStyle = exStyle,
        .style = style,
        .width = width,
        .height = height,
        .id = id,
        .className = className,
        .text = text,
        .adjust = adjust,
    };

    _rc_command(d, command);
}

static void
_rc_serialize_control(struct rc_command *command, struct rc_writer *w)
{
    _rc_pad_and_align_writer(w, sizeof(DWORD));

    _rc_write_dword(w, command->helpId);
    _rc_write_dword(w, command->exStyle);
    _rc_write_dword(w, command->style | WS_CHILD | WS_VISIBLE);
    _rc_write_short(w, command->x);
    _rc_write_short(w, command->y);
    _rc_write_short(w, command->width);
    _rc_write_short(w, command->height);
    _rc_write_dword(w, command->id);
    _rc_write_word(w, 0xFFFF);
    _rc_write_word(w, command->className);
    
    if (command->text) {
        _rc_write_buffer(w, command->text, wcslen(command->text) * sizeof(WCHAR));
        _rc_write_word(w, 0x0000);
    } else {
        _rc_write_word(w, 0x0000);
    }

    _rc_write_word(w, 0);
}

static void
_rc_serialize_header(struct rc_dialog *d, struct rc_writer *w, struct rc_command dialog_command)
{
    DWORD style = 0; // TODO

    _rc_write_word(w, 1);                             // dlgVer
    _rc_write_word(w, 0xFFFF);                        // signature
    _rc_write_dword(w, 0);                            // helpID
    _rc_write_dword(w, 0);                            // exStyle
    _rc_write_dword(w, style | (WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_SETFONT)); // style
    _rc_write_word(w, d->ncontrols);                  // cDlgItems
    _rc_write_short(w, 0);                            // x
    _rc_write_short(w, 0);                            // y
    _rc_write_short(w, dialog_command.width);         // cx
    _rc_write_short(w, dialog_command.height);        // cy
    _rc_write_word(w, 0);                             // menu
    _rc_write_word(w, 0);                             // windowClass
    
    _rc_write_buffer(w, dialog_command.text, wcslen(dialog_command.text) * sizeof(WCHAR)); // title[titleLen]
    _rc_write_word(w, 0x0000);

    _rc_write_word(w, 9);                             // TODO pointsize
    _rc_write_word(w, FW_NORMAL);                     // weight
    _rc_write_byte(w, 0);                             // italic
    _rc_write_byte(w, DEFAULT_CHARSET);               // charset
    
    _rc_write_buffer(w, dialog_command.typeface, wcslen(dialog_command.typeface) * sizeof(WCHAR));  // typeface[stringLen]
    _rc_write_word(w, 0x0000);
}

/********* Dialog creation helpers *********/
struct rc_dialog
rc_window(int width, int height, WCHAR *title, WCHAR *typeface, int font_size)
{
    struct rc_dialog dialog = { 0 };

    if (!typeface) {
        typeface = L"MS Shell Dlg";
    }

    struct rc_command command = {
        .type = RC_COMMAND_BEGIN_DIALOG,
        .width = width,
        .height = height,
        .text = title,
        .typeface = typeface,
    };

    // TODO: save font_size

    _rc_command(&dialog, command);

    return(dialog);
}

// TODO: autosized dialog

/********* Layout beginning helpers *********/
void
rc_row(struct rc_dialog *d, int height, int spacing, enum rc_row_flags flags)
{
    if (spacing == RC_RECOMMENDED) {
        spacing = RC_VERTICAL_SPACING;
    }

    struct rc_command command = {
        .type = RC_COMMAND_BEGIN_ROW,
        .flags = flags,
        .height = height,
        .spacing = spacing,
    };

    _rc_command(d, command);
}

void
rc_column(struct rc_dialog *d, int width, int spacing)
{
    if (spacing == RC_RECOMMENDED) {
        spacing = RC_HORIZONTAL_SPACING;
    }

    struct rc_command command = {
        .type = RC_COMMAND_BEGIN_COLUMN,
        .width = width,
        .spacing = spacing,
    };

    _rc_command(d, command);
}

// TODO: another grid function, where user specifies the total width/height instead of 
// cell width/height
void
rc_grid(struct rc_dialog *d, int ncols, int nrows, int col_width, int row_height, int spacing)
{
    struct rc_command command = {
        .type = RC_COMMAND_BEGIN_GRID,
        .ncols = ncols,
        .nrows = nrows,
        .width = col_width,
        .height = row_height,
        .spacing = spacing,
    };

    _rc_command(d, command);
}

void
rc_groupbox(struct rc_dialog *d, int width, int height, int spacing, WCHAR *text)
{
    struct rc_command box_command = {
        .type = RC_COMMAND_BEGIN_GROUPBOX,
        .width = width,
        .height = height,
        .text = text,
    };

    if (spacing == RC_RECOMMENDED) {
        spacing = RC_VERTICAL_SPACING;
    }

    struct rc_command col_command = {
        .type = RC_COMMAND_BEGIN_COLUMN,
        .width = width,
        .spacing = spacing,
    };

    _rc_command(d, box_command);
    _rc_command(d, col_command);
}

/********* Layout ending helpers *********/
void
rc_row_end(struct rc_dialog *d)
{
    struct rc_command command = { .type = RC_COMMAND_END_ROW };
    _rc_command(d, command);
}

void
rc_column_end(struct rc_dialog *d)
{
    struct rc_command command = { .type = RC_COMMAND_END_COLUMN };
    _rc_command(d, command);
}

void
rc_grid_end(struct rc_dialog *d)
{
    struct rc_command command = { .type = RC_COMMAND_END_GRID };
    _rc_command(d, command);
}

void
rc_groupbox_end(struct rc_dialog *d)
{
    struct rc_command col_command = { .type = RC_COMMAND_END_COLUMN };
    struct rc_command box_command = { .type = RC_COMMAND_END_GROUPBOX };

    _rc_command(d, col_command);
    _rc_command(d, box_command);
}

/********* Control helpers *********/
void
rc_button(struct rc_dialog *d, int width, int height, WCHAR *text)
{
    if (height == RC_RECOMMENDED) {
        height = RC_BUTTON_HEIGHT;
    }

    if (width == RC_RECOMMENDED) {
        width = RC_BUTTON_WIDTH;
    }

    _rc_control(d, 0, 0, 0, width, height, -1, RC_CONTROL_BUTTON, text, 0);
}

void
rc_text(struct rc_dialog *d, int width, int height, WCHAR *text)
{
    if (height = RC_RECOMMENDED) {
        height = RC_STATIC_HEIGHT;
    }

    _rc_control(d, 0, 0, 0, width, height, -1, RC_CONTROL_STATIC, text, 0);
}

void
rc_label(struct rc_dialog *d, int width, int height, WCHAR *text)
{
    // TODO: adjust 3 px down

    if (height = RC_RECOMMENDED) {
        height = RC_STATIC_HEIGHT;
    }

    _rc_control(d, 0, 0, 0, width, height, -1, RC_CONTROL_STATIC, text, 3);
}

void
rc_edit(struct rc_dialog *d, int width, int height)
{
    if (height = RC_RECOMMENDED) {
        height = RC_TEXTBOX_HEIGHT;
    }

    _rc_control(d, 0, 0, WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, width, height, -1, RC_CONTROL_EDIT, 0, 0);
}

void
rc_checkbox(struct rc_dialog *d, int width, int height, WCHAR *text)
{
    if (height = RC_RECOMMENDED) {
        height = RC_CHECKBOX_HEIGHT;
    }

    _rc_control(d, 0, 0, BS_AUTOCHECKBOX, width, height, -1, RC_CONTROL_BUTTON, text, 0);
}

void
rc_combobox(struct rc_dialog *d, int width, int height)
{
    if (height == RC_RECOMMENDED) {
        height = RC_COMBOBOX_HEIGHT;
    }

    _rc_control(d, 0, 0, CBS_DROPDOWNLIST, width, height, -1, RC_CONTROL_COMBOBOX, 0, 0);
}

void
rc_radio(struct rc_dialog *d, int width, int height, WCHAR *title)
{
    if (height == RC_RECOMMENDED) {
        height = RC_RADIO_HEIGHT;
    }

    _rc_control(d, 0, 0, WS_TABSTOP | BS_AUTORADIOBUTTON, width, height, -1, RC_CONTROL_BUTTON, title, 0);
}

void
rc_vskip(struct rc_dialog *d, int units)
{
    if (units == RC_RECOMMENDED) {
        units = RC_VERTICAL_SPACING;
    }

    struct rc_command command = {
        .type = RC_COMMAND_VSKIP,
        .height = units
    };

    _rc_command(d, command);
}

void
rc_hskip(struct rc_dialog *d, int units)
{
    if (units == RC_RECOMMENDED) {
        units = RC_HORIZONTAL_SPACING;
    }

    struct rc_command command = {
        .type = RC_COMMAND_HSKIP,
        .width = units
    };

    _rc_command(d, command);
}

void
rc_dialog_solve(struct rc_dialog *d)
{
    struct rc_rect layout_stack[RC_MAX_LAYOUT_STACK_DEPTH] = { 0 };
    size_t stack_depth = 0;
    unsigned layout_flags = 0; // TODO

    for (size_t i = 0; i < d->ncommands; ++i) {
        struct rc_command *command = d->commands + i;
        struct rc_rect *parent = layout_stack + stack_depth - 1;
        
        switch (command->type) {
            case RC_COMMAND_BEGIN_DIALOG: {
                 // Fixed size
                 Assert(command->width > 0 && command->height > 0);

                 struct rc_rect dialog_rect = {
                    .type = RC_LAYOUT_SIMPLE,
                    .x = RC_DIALOG_PADDING,
                    .y = RC_DIALOG_PADDING,
                    .cur_x = RC_DIALOG_PADDING,
                    .cur_y = RC_DIALOG_PADDING,
                    .w = command->width - RC_DIALOG_PADDING * 2,
                    .h = command->height - RC_DIALOG_PADDING * 2,
                };

                layout_stack[stack_depth++] = dialog_rect;

                break;
            }
    
            case RC_COMMAND_BEGIN_GRID: {
                // Fixed size
                Assert(command->width > 0 && command->height > 0);

                struct rc_rect grid_rect = {
                    .type = RC_LAYOUT_GRID,
                    .x = parent->cur_x,
                    .y = parent->cur_y,
                    .cur_x = parent->cur_x,
                    .cur_y = parent->cur_y,
                    .w = command->width,
                    .h = command->height,
                    .ncols = command->ncols,
                    .nrows = command->nrows,
                    .spacing = command->spacing,
                };

                layout_stack[stack_depth++] = grid_rect;

                break;
            }

            case RC_COMMAND_BEGIN_GROUPBOX: {
                // Fixed size
                Assert(command->width > 0 && command->height > 0);

                if (command->height== RC_FILL) {
                    command->height = parent->h;
                }

                struct rc_rect groupbox_rect = {
                    .type = RC_LAYOUT_SIMPLE,
                    .x = parent->cur_x,
                    .y = parent->cur_y,
                    .cur_x = parent->cur_x + RC_GROUPBOX_PADDING_X,
                    .cur_y = parent->cur_y + RC_GROUPBOX_PADDING_Y,
                    .w = command->width,
                    .h = command->height,
                };

                command->x = parent->cur_x; // These coords being set
                command->y = parent->cur_y; // is why the purpose of the whole library

                layout_stack[stack_depth++] = groupbox_rect;

                break;
            }
            
            case RC_COMMAND_BEGIN_ROW: {
                Assert(command->height > 0);
                
                struct rc_rect row_rect = { 
                    .type = RC_LAYOUT_HORIZONTAL,
                    .x = parent->cur_x,
                    .y = parent->cur_y,
                    .cur_x = parent->cur_x,
                    .cur_y = parent->cur_y,
                    .w = 0, // This will get computed by direct children calling _rc_advance_parent
                    .h = command->height,
                    .spacing = command->spacing,
                };

                if (command->flags & RC_FLAGS_ROW_RTL) {
                    // TODO
                }

                layout_stack[stack_depth++] = row_rect;

                break;
            }

            case RC_COMMAND_BEGIN_COLUMN: {
                Assert(command->width > 0);

                if (command->width == RC_FILL && stack_depth == 1) {
                    command->width = parent->w;
                }

                struct rc_rect column_rect = {
                    .type = RC_LAYOUT_VERTICAL,
                    .x = parent->cur_x,
                    .y = parent->cur_y,
                    .cur_x = parent->cur_x,
                    .cur_y = parent->cur_y,
                    .w = command->width,
                    .h = 0,
                    .spacing = command->spacing,
                };

                layout_stack[stack_depth++] = column_rect;

                break;
            }

            case RC_COMMAND_END_ROW:
            case RC_COMMAND_END_COLUMN:
            case RC_COMMAND_END_GRID:
            case RC_COMMAND_END_GROUPBOX: {
                // At this point child has its width/height already computed

                struct rc_rect *child = parent;

                --stack_depth;
                parent = layout_stack + stack_depth - 1;

                _rc_advance_parent(parent, child);

                break;
            }

            case RC_COMMAND_ADD_CONTROL: {
                if (command->width == RC_RECOMMENDED && parent->type == RC_LAYOUT_GRID) {
                    command->width = parent->w;
                }

                if (command->height == RC_RECOMMENDED && parent->type == RC_LAYOUT_GRID) {
                    command->height = parent->h;
                }

                struct rc_rect control_rect = { 
                    .x = parent->cur_x,
                    .y = parent->cur_y,
                    
                    .w = command->width,  // These two must be set
                    .h = command->height, // to a positive value at this point
                };

                command->x = parent->cur_x;                   // These coords being set
                command->y = parent->cur_y + command->adjust; // is why the purpose of the whole library

                _rc_advance_parent(parent, &control_rect);

                break;
            }

            case RC_COMMAND_VSKIP: {
                Assert(parent->type == RC_LAYOUT_VERTICAL);
                parent->cur_y += command->height;
            }

            case RC_COMMAND_HSKIP: {
                Assert(parent->type == RC_LAYOUT_HORIZONTAL);
                parent->cur_x += command->width;
            }
        }
    }
}

void
rc_dialog_serialize(struct rc_dialog *d, char *data, size_t size)
{
    struct rc_writer w = { .data = data, .size = size };

    _rc_serialize_header(d, &w, d->commands[0]);

    for (size_t i = 0; i < d->ncommands; ++i) {
        struct rc_command *command = d->commands + i;
        
        switch (command->type) {
            case RC_COMMAND_BEGIN_GROUPBOX: {
                struct rc_command control_command = {
                    .type = RC_COMMAND_ADD_CONTROL,
                    .style = command->style | BS_GROUPBOX,
                    .width = command->width,
                    .height = command->height,
                    .x = command->x,
                    .y = command->y,
                    .id = -1,
                    .className = RC_CONTROL_BUTTON,
                    .text = command->text,
                };

                _rc_serialize_control(&control_command, &w);

                break;
            }

            case RC_COMMAND_ADD_CONTROL: {
                _rc_serialize_control(command, &w);
                break;
            }
    
            default: {
                // All other commands are only neeeded for layout
                // and do not result in any additional data being
                // written to the final buffer
            }
        }
    }
}