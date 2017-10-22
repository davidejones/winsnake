#include <windows.h>
#include <cstdint>
#include <ctime>
#include <iostream>

static bool Running;
static BITMAPINFO BitmapInfo;
static void *BitmapMemory;
static int BitmapWidth;
static int BitmapHeight;
static int BytesPerPixel = 4;

int applex = -1;
int appley = -1;

void drawRect(int rectx, int recty, int width, int height, int hexColor) {
    unsigned char R = static_cast<unsigned char>(((hexColor >> 16) & 0xFF));
    unsigned char G = static_cast<unsigned char>(((hexColor >> 8) & 0xFF));
    unsigned char B = static_cast<unsigned char>((hexColor & 0xFF));
    int Pitch = BitmapWidth*BytesPerPixel;
    uint8_t *Row = (uint8_t *)BitmapMemory;
    for(int Y = 0; Y < BitmapHeight; ++Y) {
        uint8_t *Pixel = (uint8_t *)Row;
        for(int X = 0; X < BitmapWidth; ++X) {
            if( (X >= rectx && Y >= recty) && (X <= (rectx + width) && Y <= (recty + height)) )
            {
                //blue
                *Pixel = B;
                ++Pixel;

                //green
                *Pixel = G;
                ++Pixel;

                //red
                *Pixel = R;
                ++Pixel;

                *Pixel = 0;
                ++Pixel;
            } else {
                // move along
                ++Pixel;
                ++Pixel;
                ++Pixel;
                ++Pixel;
            }
        }
        Row += Pitch;
    }
}

int RandRange(int range_min, int range_max) {
    static bool first = true;
    if ( first )
    {
        srand(time(NULL)); //seeding for the first time only!
        first = false;
    }
    return range_min + rand() % (range_max - range_min);
}

void setApple() {
    /*
     * Randomly place a 10x10 green apple on screen
     * if snake exists in random location generate again
     */
    int x = RandRange(0, BitmapWidth-10);
    int y = RandRange(0, BitmapHeight-10);
    std::cout << "setting apple.." << std::endl;
    std::cout << x << "," << y << std::endl;
    drawRect(x, y, 10, 10, 0x00FF00);
    applex = x;
    appley = y;
}

void init(int Width, int Height) {
    // we are initializing the bitmap memory buffer here
    // this can be called on resize too but for now stick to fixed window

    if(BitmapMemory) {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = Width;
    BitmapHeight = Height;

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = Width;
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    BitmapInfo.bmiHeader.biSizeImage = 0;
    BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    BitmapInfo.bmiHeader.biClrUsed = 0;
    BitmapInfo.bmiHeader.biClrImportant = 0;

    int BitmapMemorySize = (Width*Height)*BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

void draw(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height) {
    // update memory state bitmap to window
    // this is a rect to rect copy
    int WindowWidth = WindowRect->right - WindowRect->left;
    int WindowHeight = WindowRect->bottom - WindowRect->top;
    StretchDIBits(
        DeviceContext,
        //X, Y, Width, Height, X, Y, Width, Height,
        0, 0, BitmapWidth, BitmapHeight,
        0, 0, WindowWidth, WindowHeight,
        BitmapMemory,
        &BitmapInfo,
        DIB_RGB_COLORS, SRCCOPY
    );
}

void update() {
    // update game simulation

    //drawRect(200, 200, 300, 300);

    if(applex == -1 && appley == -1) {
        setApple();
    }

    /*int Pitch = Width*BytesPerPixel;
    uint8_t *Row = (uint8_t *)BitmapMemory;
    for(int Y = 0; Y < BitmapHeight; ++Y) {
        uint8_t *Pixel = (uint8_t *)Row;
        for(int X = 0; X < BitmapWidth; ++X) {
            //blue
            *Pixel = 255;
            ++Pixel;

            //green
            *Pixel = 0;
            ++Pixel;

            //red
            *Pixel = 0;
            ++Pixel;

            *Pixel = 0;
            ++Pixel;
        }
        Row += Pitch;
    }*/
}

LRESULT CALLBACK
MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_SIZE:
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            init(Width, Height);
        } break;

        case WM_CLOSE:
        {
            Running = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            //std::cout << "WM_ACTIVATEAPP" << std::endl;
        } break;

        case WM_DESTROY:
        {
            Running = false;
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            update();
            draw(DeviceContext, &ClientRect, X, Y, Width, Height);
            EndPaint(Window, &Paint);
        } break;

        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    return(Result);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{
    WNDCLASS WindowClass = {};

    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = Instance;
    //WindowClass.hIcon;
    WindowClass.lpszClassName = "WinSnakeWindowClass";

    if(RegisterClassA(&WindowClass))
    {
        HWND WindowHandle = CreateWindowExA(
                0, WindowClass.lpszClassName, "Snake",
                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
                CW_USEDEFAULT, CW_USEDEFAULT,
                800, 600, 0, 0, Instance, 0
        );
        if(WindowHandle)
        {
            Running = true;
            while(Running)
            {
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if(Message.message == WM_QUIT) {
                        Running = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }

                HDC DeviceContext = GetDC(WindowHandle);
                RECT ClientRect;
                GetClientRect(WindowHandle, &ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;

                update();
                draw(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
                ReleaseDC(WindowHandle, DeviceContext);
            }
        }
        else
        {
            MessageBox(0, "Failed to create window","Error", MB_ICONEXCLAMATION | MB_OK);
        }
    }
    else
    {
        MessageBox(0, "Failed to create window","Error", MB_ICONEXCLAMATION | MB_OK);
    }

    return(0);
}