// Skeleton.cpp : Defines the entry point for the application.
// Thanks goes out to Max Code for making this skeleton
// able to run with Voodoo2 OpenGL.

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#ifndef CDS_FULLSCREEN
#define CDS_FULLSCREEN      0x00000004
#endif

//#include "mp3lib/mp3.h"
//extern int nosound; 

//----------------------------------
// Skeleton functions and variables.
//-----------------
extern char szAppName[];

static int takarodj=0;

int fullscreen=0;
int fullscreen_bpp=0;
int fullscreen_xres=640;
int fullscreen_yres=480;

//-------
// Your Stuff:
int Load(int argc,char* argv[]);
int Init();
int Render();  // Draw all the scene related stuff.
void Shutdown();
void PrintMark();


// Good 'ol generic drawing stuff.
LRESULT CALLBACK SkeletonProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


void ChangeDisplayMode(){

  DEVMODE device_mode;
  memset(&device_mode, 0, sizeof(DEVMODE));
  device_mode.dmSize = sizeof(DEVMODE);

  device_mode.dmPelsWidth  = fullscreen_xres;
  device_mode.dmPelsHeight = fullscreen_yres;
  if(fullscreen_bpp){
    device_mode.dmBitsPerPel = fullscreen_bpp;
    device_mode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
  } else {
    device_mode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;
  }

  ChangeDisplaySettings(&device_mode, CDS_FULLSCREEN);

}
  
void RestoreDisplayMode(){
  ChangeDisplaySettings(0, 0);
}

// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC){
  int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,				// version				// Version of this structure
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,				// Double buffered mode
		PFD_TYPE_RGBA,					// RGBA Color mode
		16,	// Want the display bit depth
		0,0,0,0,0,0,					// Not used to select mode
		0,0,							// Not used to select mode
		0,0,0,0,0,						// Not used to select mode
		16,								// Size of depth buffer
		0,								// Not used to select mode
		0,								// Not used to select mode
		PFD_MAIN_PLANE,					// Draw in main plane
		0,								// Not used to select mode
		0,0,0
	};									// Not used to select mode
	pfd.cColorBits=GetDeviceCaps(hDC, BITSPIXEL);

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	printf("PixelFormat Selected: %d  BPP: %d\n", nPixelFormat, GetDeviceCaps(hDC, BITSPIXEL));

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}

void ChangeSize( GLsizei w, GLsizei h ){
	// Prevent a divide by Zero
	if( h == 0 )  h = 1;

	// Set viewport to our dimensions.
	glViewport( 0, 0, w, h);
}

GLsizei window_w, window_h;
int bReady;
HDC hDC;
HGLRC hRC;

static HCURSOR HCursor;
static BOOL SSOn;


// Good ol' creation code.
int main(int argc,char* argv[]){
	WNDCLASS wc;
	HWND hWnd;
  MSG  msg;
	BOOL bGotMsg;
	// Say we're not ready to render yet.
	bReady = 0;

  // Load scene, etc.
	if( Load(argc,argv) == 0 )	{
		MessageBox( NULL, "Failed to load.", "Fatal Blow", MB_OK | MB_ICONERROR );
		return FALSE;	// Failed init, so just quit right away.
	}

/*
    if(!MP3_OpenDevice(NULL)){
      MessageBox(NULL, "MP3_OpenDevice() failed:  "
      "Cannot open SOUND device... NO MUSIC :(", "Error", MB_OK);
      nosound=1;
    }
*/

	// Setup the window class.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc	= (WNDPROC)SkeletonProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance=GetModuleHandle(NULL);
//	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );

	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
//	wc.lpszClassName = "OpenGL";
	wc.lpszClassName = szAppName;

	// Announce it to the Microsloth OS.
	if( RegisterClass( &wc ) == 0){
		MessageBox( NULL, "Failed to register the idiot window class, right before creating the window.", "Fatal Blow", MB_OK | MB_ICONERROR );
		return FALSE;	// Failed to create window, so just quit right away.
	}

	// Make it go.......
	if( fullscreen == 0 )	{
    // Default startup mode, windowed.
		hWnd = CreateWindow(
				szAppName,
				szAppName,
				WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
				CW_USEDEFAULT, CW_USEDEFAULT,
				fullscreen_xres, fullscreen_yres,
				NULL, NULL,
				GetModuleHandle(NULL),
				NULL );
	} else {
    // alternate startup mode, to bring up full screen.
		hWnd = CreateWindow(
				szAppName,
				szAppName,
				WS_POPUP,
				0, 0,
				fullscreen_xres, fullscreen_yres,
				NULL, NULL,
				GetModuleHandle(NULL),
				NULL );
	}

	if( hWnd == NULL )	{
		MessageBox( NULL, "Failed to create the effect window.", "Fatal Blow", MB_OK | MB_ICONERROR );
		return FALSE;	// Failed to create window, so just quit right away.
	}

	// Make it visible...
	ShowWindow( hWnd, SW_SHOW );
	UpdateWindow( hWnd );

	// Prepare the scene for rendering.
	if( Init() == 0 )	{
		MessageBox( hWnd, "Failed to initialize the engine.", "Fatal Blow", MB_OK | MB_ICONERROR );
		return FALSE;	// Failed init, so just quit right away.
	}

	if(fullscreen){
		printf("Disabling cursor & screensaver\n");
			// disable scrensaver
			SystemParametersInfo(SPI_GETSCREENSAVEACTIVE,0,&SSOn,SPIF_SENDCHANGE);
			if (SSOn==TRUE) SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,FALSE,NULL,SPIF_SENDCHANGE);
//			HCursor=SetCursor(NULL);	// disable mouse
	}

	// We're now ready.
	bReady = 1;

	// Usual running around in circles bit...
	  PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

    while( WM_QUIT!=msg.message && !takarodj ){
	  	// Use PeekMessage() if the app is active, so we can use idle time to
  		// render the scene. Else, use GetMessage() to avoid eating CPU time.
		  bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
  		if( bGotMsg ) {
	  		// Translate and dispatch the message
		  	TranslateMessage( &msg );
			  DispatchMessage( &msg );
      }	else {
	  		InvalidateRect( hWnd, NULL, FALSE );
		  }
    }

	// Not ready to render anymore.
	bReady = 0;

	if( fullscreen )	{
		wglMakeCurrent( hDC, NULL );
		wglDeleteContext( hRC );
		RestoreDisplayMode();

		printf("Enabling cursor & screensaver\n");

		HCursor=SetCursor(HCursor);	// enable mouse
		// enable screensaver
		if (SSOn==TRUE) SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,SSOn,NULL,SPIF_SENDCHANGE);

	}

	// Nuke all applicable scene stuff.
	Shutdown();

	return msg.wParam;
}

// Stupid usual callback window proc message spud gun.
LRESULT CALLBACK SkeletonProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ){
	// Keep drawing stuff around.

	switch( uMsg ){
	case WM_CREATE:
		// Remember our mSloth drawing context.
		hDC = GetDC( hWnd );

		if( fullscreen ){
			ChangeDisplayMode();
			HCursor=SetCursor(NULL);	// disable mouse
			ShowCursor(0);	// biztos a biztos :)
		}

    // Select our precious pixel format.
		SetDCPixelFormat( hDC );

		// Yeppers, make something that OpenGL understands.
		hRC = wglCreateContext( hDC );
		wglMakeCurrent( hDC, hRC );
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
		  DestroyWindow(hWnd);break;
		}
		if (wParam == 'P') {
                  PrintMark();break;
		}
		printf("Key pressed: %d\n",wParam);
		break;
  
	case WM_DESTROY:
		wglMakeCurrent( hDC, NULL );
		wglDeleteContext( hRC );

		if( fullscreen ) {
		  RestoreDisplayMode();
		}

		PostQuitMessage( 0 );
		break;

	case WM_SIZE:
		window_w = LOWORD(lParam);
		window_h = HIWORD(lParam);
		ChangeSize( window_w, window_h );
		break;

	case WM_PAINT:
		if( bReady ){

			// Draw all the scene related stuff.
			if(Render()==0) takarodj=1;

			// Make it all visible at once!
			SwapBuffers( hDC );

			// We actually did something with this rect, so announce it to the mSloth OS.
			ValidateRect( hWnd, NULL );
		}
		break;

	default:
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}
