#include <windows.h>
#include <winuser.h>

#ifdef _DEBUG
#define Assert(Cond) do { if (!(Cond)) __debugbreak(); } while (0)
#else
#define Assert(Cond) (void)(Cond)
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