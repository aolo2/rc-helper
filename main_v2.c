#include <windows.h>
#include <winuser.h>

#ifdef _DEBUG
#define Assert(Cond) do { if (!(Cond)) __debugbreak(); } while (0)
#else
#define Assert(Cond) (void)(Cond)
#endif

#if 0
#include "rc_helper.c"

static INT_PTR
find_and_replace_generated(void)
{
    char data[4096];
    int dialog_width = 215;
    int dialog_height = 80;

    struct layout l = dialog_start(dialog_width, dialog_height, data, 4096, 0, L"Replace", 9, L"Segoe UI");

    row_start(&l, DIALOG_AUTO, DIALOG_AUTO);
    {
        column_start(&l, 160, 0);
        {
            row_start(&l, RECOMMENDED_TEXTBOX_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 65, DIALOG_AUTO, L"Fi&nd what:");
                edit(&l, 0, 85, DIALOG_AUTO);
            }
            row_end(&l);

            row_start(&l, RECOMMENDED_TEXTBOX_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 65, DIALOG_AUTO, L"Re&place with:");
                edit(&l, 0, 85, DIALOG_AUTO);
            }
            row_end(&l);

            set_spacing(&l, DIALOG_AUTO);
            vskip(&l, 30); // TODO: allow adding elements from the end of the row/column
            
            checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"Match &case");
            checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"W&rap around");
        }
        column_end(&l);

        column_start(&l, dialog_width - 160, DIALOG_AUTO);
        {
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, -1, L"&Find Next");
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, -1, L"&Replace");
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, -1, L"Replace &All");
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, IDCANCEL, L"Cancel");
        }
        column_end(&l);
    }
    row_end(&l);

    dialog_end(&l);

    return DialogBoxIndirect(GetModuleHandleW(NULL), (LPCDLGTEMPLATEW) l.data, NULL, NULL);
}

static INT_PTR
paragraph_generated(void)
{
    char data[4096];

    int dialog_width = 200;
    int dialog_height = 130;

    struct layout l = dialog_start(dialog_width, dialog_height, data, 4096, 0, L"Paragraph", 9, L"Segoe UI");

    column_start(&l, DIALOG_AUTO, DIALOG_AUTO);
    {
        rc_groupbox(&l, DIALOG_AUTO, DIALOG_AUTO, L"Indentation", DIALOG_AUTO);
        {
            row_start(&l, RECOMMENDED_TEXTBOX_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 65, DIALOG_AUTO, L"&Left:");
                edit(&l, 0, 100, DIALOG_AUTO);
            }
            row_end(&l);

            row_start(&l, RECOMMENDED_TEXTBOX_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 65, DIALOG_AUTO, L"&Right:");
                edit(&l, 0, 100, DIALOG_AUTO);
            }
            row_end(&l);

            row_start(&l, RECOMMENDED_TEXTBOX_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 65, DIALOG_AUTO, L"&First line:");
                edit(&l, 0, 100, DIALOG_AUTO);
            }
            row_end(&l);
        }
        groupbox_end(&l);

        rc_groupbox(&l, DIALOG_AUTO, DIALOG_AUTO, L"Spacing", DIALOG_AUTO);
        {
            row_start(&l, RECOMMENDED_TEXTBOX_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 65, DIALOG_AUTO, L"Line &spacing:");
                combobox(&l, 0, 85, DIALOG_AUTO);
            }
            row_end(&l);

            checkbox(&l, 0, 120, RECOMMENDED_CHECKBOX_HEIGHT, L"Add 10pt space after paragraphs");
        }        
        groupbox_end(&l);

        row_start(&l, DIALOG_AUTO, DIALOG_AUTO);
        {
            hskip(&l, 40);
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, -1, L"&Tabs");
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, IDOK, L"OK");
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, IDCANCEL, L"Cancel");
        }
        row_end(&l);
    }
    column_end(&l);

    dialog_end(&l);

    return DialogBoxIndirect(GetModuleHandleW(NULL), (LPCDLGTEMPLATEW) l.data, NULL, NULL);
}

static INT_PTR
image_properties_generated(void)
{
    char data[4096];

    int dialog_width = 200;
    int dialog_height = 145;

    struct layout l = dialog_start(dialog_width, dialog_height, data, 4096, 0, L"Image Properties", 9, L"Segoe UI");

    column_start(&l, DIALOG_AUTO, DIALOG_AUTO);
    {
        rc_groupbox(&l, DIALOG_AUTO, DIALOG_AUTO, L"File attributes", DIALOG_AUTO);
        {
            rc_grid(&l, 3, 2, RECOMMENDED_TEXTBOX_HEIGHT, DIALOG_AUTO);
            {
                text(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Last saved:");
                text(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Not Available");

                text(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Size on disk:");
                text(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Not Available");

                text(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Resolution:");
                text(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"96 DPI");
            }
            grid_end(&l);
        }
        groupbox_end(&l);

        row_start(&l, 50, DIALOG_AUTO);
        {
            rc_groupbox(&l, 100, 50, L"Units", 0);
            {
                radio(&l, WS_GROUP, 65, RECOMMENDED_RADIO_BUTTON_HEIGHT, L"&Inches");
                radio(&l, 0, 65, RECOMMENDED_RADIO_BUTTON_HEIGHT, L"Centi&meteres");
                radio(&l, 0, 65, RECOMMENDED_RADIO_BUTTON_HEIGHT, L"&Pixels");
            }
            groupbox_end(&l);

            rc_groupbox(&l, 100, 50, L"Colors", 0);
            {
                radio(&l, 0, 65, RECOMMENDED_RADIO_BUTTON_HEIGHT, L"&Black and white");
                radio(&l, 0, 65, RECOMMENDED_RADIO_BUTTON_HEIGHT, L"&Co&lor");
            }
            groupbox_end(&l);
        }
        row_end(&l);
        
        row_start(&l, RECOMMENDED_TEXTBOX_HEIGHT, DIALOG_AUTO);
        {
             label(&l, 0, 20, DIALOG_AUTO, L"&Width:");
             hskip(&l, 10);
             edit(&l, 0, 35, DIALOG_AUTO);

             label(&l, 0, 20, DIALOG_AUTO, L"&Height:");
             hskip(&l, 10);
             edit(&l, 0, 35, DIALOG_AUTO);

             button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, -1, L"&Default");
        }
        row_end(&l);

        row_start(&l, DIALOG_AUTO, DIALOG_AUTO);
        {
            hskip(&l, 95);         
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, IDOK, L"OK");
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, IDCANCEL, L"Cancel");
        }
        row_end(&l);
    }
    column_end(&l);

    dialog_end(&l);

    return DialogBoxIndirect(GetModuleHandleW(NULL), (LPCDLGTEMPLATEW) l.data, NULL, NULL);
}

static INT_PTR
wcap_settings_generated(void)
{
    char data[4096];

    int dialog_width = 250;
    int dialog_height = 260;

    struct layout l = dialog_start(dialog_width, dialog_height, data, 4096, 0, L"wcap Settings", 9, L"Segoe UI");

    column_start(&l, DIALOG_AUTO, 0);
    {
        row_start(&l, 80, DIALOG_AUTO);
        {
            rc_groupbox(&l, 110, 80, L"Capture", 0);
            {
                checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"&Mouse Cursor");
                checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"Only &Client Area");
                checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"Capture Au&dio");

                row_start(&l, RECOMMENDED_CHECKBOX_HEIGHT, DIALOG_AUTO);
                {
                    checkbox(&l, 0, 60, DIALOG_AUTO, L"GPU &Encoder");
                    combobox(&l, 0, 30, DIALOG_AUTO);
                }
                row_end(&l);
            }
            groupbox_end(&l);

            rc_groupbox(&l, 135, 80, L"Colors", DIALOG_AUTO);
            {
                row_start(&l, RECOMMENDED_CHECKBOX_HEIGHT, DIALOG_AUTO);
                {
                    edit(&l, 0, 100, DIALOG_AUTO);
                    button(&l, 0, RECOMMENDED_BUTTON_HEIGHT, RECOMMENDED_BUTTON_HEIGHT, -1, L"...");
                }
                row_end(&l);

                set_spacing(&l, 2);

                checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"&Open When Finished");
                checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"Fragmented MP&4");

                row_start(&l, RECOMMENDED_CHECKBOX_HEIGHT, DIALOG_AUTO);
                {
                    checkbox(&l, 0, 90, RECOMMENDED_CHECKBOX_HEIGHT, L"Limit Length (&seconds)");
                    edit(&l, WS_DISABLED, 30, RECOMMENDED_TEXTBOX_HEIGHT - 2);
                }
                row_end(&l);

                row_start(&l, RECOMMENDED_CHECKBOX_HEIGHT, DIALOG_AUTO);
                {
                    checkbox(&l, 0, 90, RECOMMENDED_CHECKBOX_HEIGHT, L"Limit &Size (MB)");
                    edit(&l, WS_DISABLED, 30, RECOMMENDED_TEXTBOX_HEIGHT - 2);
                }
                row_end(&l);
            }
            groupbox_end(&l);
        }
        row_end(&l);

        row_start(&l, 100, DIALOG_AUTO);
        {
            rc_groupbox(&l, 130, 100, L"&Video", 0);
            {
                rc_grid(&l, 6, 2, RECOMMENDED_TEXTBOX_HEIGHT, DIALOG_AUTO);
                {
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"&Codec");
                    combobox(&l, 0, 50, DIALOG_AUTO);
    
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Size on disk");
                    combobox(&l, 0, 50, DIALOG_AUTO);
                        
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Max &Width");
                    edit(&l, 0, 50, DIALOG_AUTO);                    

                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Max &Height");
                    edit(&l, 0, 50, DIALOG_AUTO);                    

                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Max &Framerate");
                    edit(&l, 0, 50, DIALOG_AUTO);

                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Bitrate (kbit/s):");
                    edit(&l, 0, 50, DIALOG_AUTO);
                }
                grid_end(&l);
            }
            groupbox_end(&l);

            rc_groupbox(&l, 115, 100, L"&Audio", 0);
            {
                rc_grid(&l, 4, 2, RECOMMENDED_TEXTBOX_HEIGHT, DIALOG_AUTO);
                {
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"&Codec");
                    combobox(&l, 0, 45, DIALOG_AUTO);
    
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Channels");
                    combobox(&l, 0, 45, DIALOG_AUTO);

                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Samplerate");
                    combobox(&l, 0, 45, DIALOG_AUTO);
    
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Bitrate (kbit/s)");
                    combobox(&l, 0, 45, DIALOG_AUTO);
                }
                grid_end(&l);
            }
            groupbox_end(&l);
        }
        row_end(&l);

        rc_groupbox(&l, DIALOG_AUTO, 60, L"Shor&tcuts", 0);
        {
            row_start(&l, RECOMMENDED_BUTTON_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 80, DIALOG_AUTO, L"Capture Monitor");
                button(&l, 0, 150, RECOMMENDED_BUTTON_HEIGHT, -1, L"Ctrl + Sys Req");
            }
            row_end(&l);

            row_start(&l, RECOMMENDED_BUTTON_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 80, DIALOG_AUTO, L"Capture Window");
                button(&l, 0, 150, RECOMMENDED_BUTTON_HEIGHT, -1, L"Ctrl + Win + Sys Req");
            }
            row_end(&l);

            row_start(&l, RECOMMENDED_BUTTON_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 80, DIALOG_AUTO, L"Capture Rectangle");
                button(&l, 0, 150, RECOMMENDED_BUTTON_HEIGHT, -1, L"Ctrl + Shift + Sys Req");
            }
            row_end(&l);
        }
        groupbox_end(&l);

        vskip(&l, RECOMMENDED_VSPACING_UNRELATED_CONTROLS);

        row_start(&l, DIALOG_AUTO, DIALOG_AUTO);
        {
            hskip(&l, 92);
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, IDOK, L"OK");
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, IDCANCEL, L"Cancel");
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, -1, L"Defaults");
        }
        row_end(&l);
    }
    column_end(&l);

    dialog_end(&l);

    return DialogBoxIndirect(GetModuleHandleW(NULL), (LPCDLGTEMPLATEW) l.data, NULL, NULL);
}

static INT_PTR
wcap_settings_generated_new(void)
{
    char data[4096];

    int dialog_width = 250;
    int dialog_height = 260;

    struct layout l = dialog_start(dialog_width, dialog_height, data, 4096, 0, L"wcap Settings", 9, L"Segoe UI"); // width and height are mandatory

    column_start(&l);
    {
        row_start(&l);
        {
            rc_groupbox(&l, 110, 80, L"Capture", 0);
            {
                checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"&Mouse Cursor");
                checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"Only &Client Area");
                checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"Capture Au&dio");

                row_start(&l, RECOMMENDED_CHECKBOX_HEIGHT, DIALOG_AUTO);
                {
                    checkbox(&l, 0, 60, DIALOG_AUTO, L"GPU &Encoder");
                    combobox(&l, 0, 30, DIALOG_AUTO);
                }
                row_end(&l);
            }
            groupbox_end(&l);

            rc_groupbox(&l, 135, 80, L"Colors", DIALOG_AUTO);
            {
                row_start(&l, RECOMMENDED_CHECKBOX_HEIGHT, DIALOG_AUTO);
                {
                    edit(&l, 0, 100, DIALOG_AUTO);
                    button(&l, 0, RECOMMENDED_BUTTON_HEIGHT, RECOMMENDED_BUTTON_HEIGHT, -1, L"...");
                }
                row_end(&l);

                set_spacing(&l, 2);

                checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"&Open When Finished");
                checkbox(&l, 0, 65, RECOMMENDED_CHECKBOX_HEIGHT, L"Fragmented MP&4");

                row_start(&l, RECOMMENDED_CHECKBOX_HEIGHT, DIALOG_AUTO);
                {
                    checkbox(&l, 0, 90, RECOMMENDED_CHECKBOX_HEIGHT, L"Limit Length (&seconds)");
                    edit(&l, WS_DISABLED, 30, RECOMMENDED_TEXTBOX_HEIGHT - 2);
                }
                row_end(&l);

                row_start(&l, RECOMMENDED_CHECKBOX_HEIGHT, DIALOG_AUTO);
                {
                    checkbox(&l, 0, 90, RECOMMENDED_CHECKBOX_HEIGHT, L"Limit &Size (MB)");
                    edit(&l, WS_DISABLED, 30, RECOMMENDED_TEXTBOX_HEIGHT - 2);
                }
                row_end(&l);
            }
            groupbox_end(&l);
        }
        row_end(&l);

        row_start(&l);
        {
            rc_groupbox(&l, 130, 100, L"&Video", 0);
            {
                rc_grid(&l, 6, 2, RECOMMENDED_TEXTBOX_HEIGHT, 50);
                {
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"&Codec");
                    combobox(&l, 0, 50, DIALOG_AUTO);
    
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Size on disk");
                    combobox(&l, 0, 50, DIALOG_AUTO);
                        
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Max &Width");
                    edit(&l, 0, 50, DIALOG_AUTO);                    

                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Max &Height");
                    edit(&l, 0, 50, DIALOG_AUTO);                    

                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Max &Framerate");
                    edit(&l, 0, 50, DIALOG_AUTO);

                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Bitrate (kbit/s):");
                    edit(&l, 0, 50, DIALOG_AUTO);
                }
                grid_end(&l);
            }
            groupbox_end(&l);

            rc_groupbox(&l, 115, 100, L"&Audio", 0);
            {
                rc_grid(&l, 4, 2, RECOMMENDED_TEXTBOX_HEIGHT, DIALOG_AUTO);
                {
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"&Codec");
                    combobox(&l, 0, 45, DIALOG_AUTO);
    
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Channels");
                    combobox(&l, 0, 45, DIALOG_AUTO);

                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Samplerate");
                    combobox(&l, 0, 45, DIALOG_AUTO);
    
                    label(&l, 0, DIALOG_AUTO, DIALOG_AUTO, L"Bitrate (kbit/s)");
                    combobox(&l, 0, 45, DIALOG_AUTO);
                }
                grid_end(&l);
            }
            groupbox_end(&l);
        }
        row_end(&l);

        rc_groupbox(&l, DIALOG_AUTO, 60, L"Shor&tcuts", 0);
        {
            row_start(&l, RECOMMENDED_BUTTON_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 80, DIALOG_AUTO, L"Capture Monitor");
                button(&l, 0, 150, RECOMMENDED_BUTTON_HEIGHT, -1, L"Ctrl + Sys Req");
            }
            row_end(&l);

            row_start(&l, RECOMMENDED_BUTTON_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 80, DIALOG_AUTO, L"Capture Window");
                button(&l, 0, 150, RECOMMENDED_BUTTON_HEIGHT, -1, L"Ctrl + Win + Sys Req");
            }
            row_end(&l);

            row_start(&l, RECOMMENDED_BUTTON_HEIGHT, DIALOG_AUTO);
            {
                label(&l, 0, 80, DIALOG_AUTO, L"Capture Rectangle");
                button(&l, 0, 150, RECOMMENDED_BUTTON_HEIGHT, -1, L"Ctrl + Shift + Sys Req");
            }
            row_end(&l);
        }
        groupbox_end(&l);

        vskip(&l, RECOMMENDED_VSPACING_UNRELATED_CONTROLS);

        row_start(&l, DIALOG_AUTO, DIALOG_AUTO);
        {
            hskip(&l, 92);
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, IDOK, L"OK");
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, IDCANCEL, L"Cancel");
            button(&l, 0, RECOMMENDED_BUTTON_WIDTH, RECOMMENDED_BUTTON_HEIGHT, -1, L"Defaults");
        }
        row_end(&l);
    }
    column_end(&l);

    dialog_end(&l);

    return DialogBoxIndirect(GetModuleHandleW(NULL), (LPCDLGTEMPLATEW) l.data, NULL, NULL);
}

int
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{	
    // find_and_replace_generated();
    // paragraph_generated();
    // image_properties_generated();
    wcap_settings_generated();
}


#endif

#include "rc_helper_v2.c"

int
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    int dialog_width = 285;
    int dialog_height = 275;
    
    struct rc_dialog d = rc_window(dialog_width, dialog_height, L"wcap Settings", L"Segoe UI", 9);

    rc_column(&d, RC_FILL, 0);
    {
        rc_row(&d, 85, RC_RECOMMENDED, 0);
        {
            rc_groupbox(&d, 120, RC_FILL, RC_RECOMMENDED, L"Capture");
            {
                rc_checkbox(&d, 65, RC_RECOMMENDED, L"&Mouse Cursor");
                rc_checkbox(&d, 65, RC_RECOMMENDED, L"Only &Client Area");
                rc_checkbox(&d, 65, RC_RECOMMENDED, L"Capture Au&dio");

                rc_row(&d, RC_CHECKBOX_HEIGHT, 5, 0);
                {
                    rc_checkbox(&d, 60, RC_RECOMMENDED, L"GPU &Encoder");
                    rc_combobox(&d, 45, RC_RECOMMENDED);
                }
                rc_row_end(&d);
            }
            rc_groupbox_end(&d);

            rc_groupbox(&d, 145, RC_FILL, RC_RECOMMENDED, L"Output");
            {
                rc_row(&d, RC_CHECKBOX_HEIGHT, RC_RECOMMENDED, 0);
                {
                    rc_edit(&d, 100, RC_RECOMMENDED);
                    rc_button(&d, RC_BUTTON_HEIGHT, RC_BUTTON_HEIGHT, L"...");
                }
                rc_row_end(&d);

                rc_checkbox(&d, 80, RC_RECOMMENDED, L"&Open When Finished");
                rc_checkbox(&d, 80, RC_RECOMMENDED, L"Fragmented MP&4");

                rc_row(&d, RC_CHECKBOX_HEIGHT, RC_RECOMMENDED, 0);
                {
                    rc_checkbox(&d, 90, RC_RECOMMENDED, L"Limit Length (&seconds)");
                    rc_edit(&d, 40, RC_RECOMMENDED);
                }
                rc_row_end(&d);

                rc_row(&d, RC_RECOMMENDED, RC_RECOMMENDED, 0);
                {
                    rc_checkbox(&d, 90, RC_RECOMMENDED, L"Limit &Size (MB)");
                    rc_edit(&d, 40, RC_RECOMMENDED);
                }
                rc_row_end(&d);
            }
            rc_groupbox_end(&d);
        }
        rc_row_end(&d);

        rc_row(&d, 100, RC_RECOMMENDED, 0);
        {
            rc_groupbox(&d, 130, 100, 0, L"&Video");
            {
                rc_grid(&d, 2, 6, 65, RC_TEXTBOX_HEIGHT, 0);
                {
                    rc_label(&d, RC_RECOMMENDED, RC_RECOMMENDED, L"&Codec");
                    rc_combobox(&d, 55, RC_RECOMMENDED);
    
                    rc_label(&d, RC_RECOMMENDED, RC_RECOMMENDED, L"Size on disk");
                    rc_combobox(&d, 55, RC_RECOMMENDED);
                        
                    rc_label(&d, RC_RECOMMENDED, RC_RECOMMENDED,  L"Max &Width");
                    rc_edit(&d, 55, RC_RECOMMENDED);                   

                    rc_label(&d, RC_RECOMMENDED, RC_RECOMMENDED, L"Max &Height");
                    rc_edit(&d, 55, RC_RECOMMENDED);                    

                    rc_label(&d, RC_RECOMMENDED, RC_RECOMMENDED, L"Max &Framerate");
                    rc_edit(&d, 55, RC_RECOMMENDED);

                    rc_label(&d, RC_RECOMMENDED, RC_RECOMMENDED, L"Bitrate (kbit/s):");
                    rc_edit(&d, 55, RC_RECOMMENDED);
                }
                rc_grid_end(&d);
            }
            rc_groupbox_end(&d);

            rc_groupbox(&d, 135, 100, 0, L"&Audio");
            {
                rc_grid(&d, 2, 2, 65, RC_TEXTBOX_HEIGHT, 0);
                {
                    rc_label(&d, RC_RECOMMENDED, RC_RECOMMENDED, L"&Codec");
                    rc_combobox(&d, 60, RC_RECOMMENDED);
    
                    rc_label(&d, RC_RECOMMENDED, RC_RECOMMENDED, L"Channels");
                    rc_combobox(&d, 60, RC_RECOMMENDED);

                    rc_label(&d, RC_RECOMMENDED, RC_RECOMMENDED, L"Samplerate");
                    rc_combobox(&d, 60, RC_RECOMMENDED);
    
                    rc_label(&d, RC_RECOMMENDED, RC_RECOMMENDED, L"Bitrate (kbit/s)");
                    rc_combobox(&d, 60, RC_RECOMMENDED);
                }
                rc_grid_end(&d);
            }
            rc_groupbox_end(&d);
        }
        rc_row_end(&d);

        rc_groupbox(&d, 265 + RC_HORIZONTAL_SPACING, 60, 0, L"Shor&tcuts");
        {
            rc_row(&d, RC_BUTTON_HEIGHT, 0, 0);
            {
                rc_label(&d, 80, RC_RECOMMENDED, L"Capture Monitor");
                rc_button(&d, 180, RC_RECOMMENDED, L"Ctrl + Sys Req");
            }
            rc_row_end(&d);

            rc_row(&d, RC_BUTTON_HEIGHT, 0, 0);
            {
                rc_label(&d, 80, RC_RECOMMENDED, L"Capture Window");
                rc_button(&d, 180, RC_RECOMMENDED, L"Ctrl + Win + Sys Req");
            }
            rc_row_end(&d);

            rc_row(&d, RC_BUTTON_HEIGHT, 0, 0);
            {
               rc_label(&d, 80, RC_RECOMMENDED, L"Capture Rectangle");
               rc_button(&d, 180, RC_RECOMMENDED, L"Ctrl + Shift + Sys Req");
            }
            rc_row_end(&d);
        }
        rc_groupbox_end(&d);

        rc_vskip(&d, RC_VERTICAL_SPACING);

        rc_row(&d, RC_RECOMMENDED, RC_RECOMMENDED, RC_FLAGS_ROW_RTL);
        {
            rc_hskip(&d, 110);
            rc_button(&d, RC_RECOMMENDED, RC_RECOMMENDED, L"OK");
            rc_button(&d, RC_RECOMMENDED, RC_RECOMMENDED, L"Cancel");
            rc_button(&d,RC_RECOMMENDED, RC_RECOMMENDED, L"Defaults");
        }
        rc_row_end(&d);
    }
    rc_column_end(&d);
    
    char data[4096];

    rc_dialog_solve(&d);
    rc_dialog_serialize(&d, data, sizeof(data));

    return DialogBoxIndirect(GetModuleHandleW(NULL), (LPCDLGTEMPLATEW) data, NULL, NULL);
}