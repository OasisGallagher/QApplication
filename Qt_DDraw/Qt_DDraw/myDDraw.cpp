#include "myDDraw.h"
#include <tchar.h>

int MyDDraw::Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename)
{
	// this function opens a bitmap file and loads the data into bitmap

	int file_handle,  // the file handle
		index;        // looping index

	UCHAR   *temp_buffer = NULL; // used to convert 24 bit images to 16 bit
	OFSTRUCT file_data;          // the file data information

	// open the file if it exists
	if ((file_handle = OpenFile(filename,&file_data,OF_READ))==-1)
		return(0);

	// now load the bitmap file header
	_lread(file_handle, &bitmap->bitmapfileheader,sizeof(BITMAPFILEHEADER));

	// test if this is a bitmap file
	if (bitmap->bitmapfileheader.bfType!=BITMAP_ID)
	{
		// close the file
		_lclose(file_handle);

		// return error
		return(0);
	} // end if

	// now we know this is a bitmap, so read in all the sections

	// first the bitmap infoheader

	// now load the bitmap file header
	_lread(file_handle, &bitmap->bitmapinfoheader,sizeof(BITMAPINFOHEADER));

	// now load the color palette if there is one
	if (bitmap->bitmapinfoheader.biBitCount == 8)
	{
		_lread(file_handle, &bitmap->palette,MAX_COLORS_PALETTE*sizeof(PALETTEENTRY));

		// now set all the flags in the palette correctly and fix the reversed 
		// BGR RGBQUAD data format
		for (index=0; index < MAX_COLORS_PALETTE; index++)
		{
			// reverse the red and green fields
			int temp_color                = bitmap->palette[index].peRed;
			bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
			bitmap->palette[index].peBlue = temp_color;

			// always set the flags word to this
			bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
		} // end for index

	} // end if

	// finally the image data itself
	//_lseek(file_handle,-(int)(bitmap->bitmapinfoheader.biSizeImage),SEEK_END);

	// now read in the image, if the image is 8 or 16 bit then simply read it
	// but if its 24 bit then read it into a temporary area and then convert
	// it to a 16 bit image

	if (bitmap->bitmapinfoheader.biBitCount==8 || bitmap->bitmapinfoheader.biBitCount==16 || 
		bitmap->bitmapinfoheader.biBitCount==24)
	{
		// delete the last image if there was one
		if (bitmap->buffer)
			free(bitmap->buffer);

		// allocate the memory for the image
		if (!(bitmap->buffer = (UCHAR *)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		{
			// close the file
			_lclose(file_handle);

			// return error
			return(0);
		} // end if

		// now read it in
		_lread(file_handle,bitmap->buffer,bitmap->bitmapinfoheader.biSizeImage);

	} // end if
	else
	{
		// serious problem
		return(0);

	} // end else

#if 0
	// write the file info out 
	printf("\nfilename:%s \nsize=%d \nwidth=%d \nheight=%d \nbitsperpixel=%d \ncolors=%d \nimpcolors=%d",
		filename,
		bitmap->bitmapinfoheader.biSizeImage,
		bitmap->bitmapinfoheader.biWidth,
		bitmap->bitmapinfoheader.biHeight,
		bitmap->bitmapinfoheader.biBitCount,
		bitmap->bitmapinfoheader.biClrUsed,
		bitmap->bitmapinfoheader.biClrImportant);
#endif

	// close the file
	_lclose(file_handle);

	// flip the bitmap
	Flip_Bitmap(bitmap->buffer, 
		bitmap->bitmapinfoheader.biWidth*(bitmap->bitmapinfoheader.biBitCount/8), 
		bitmap->bitmapinfoheader.biHeight);

	// return success
	return(1);

} // end Load_Bitmap_File

///////////////////////////////////////////////////////////

int MyDDraw::Unload_Bitmap_File(BITMAP_FILE_PTR bitmap)
{
	// this function releases all memory associated with "bitmap"
	if (bitmap->buffer)
	{
		// release memory
		free(bitmap->buffer);

		// reset pointer
		bitmap->buffer = NULL;

	} // end if

	// return success
	return(1);

} // end Unload_Bitmap_File

///////////////////////////////////////////////////////////

int MyDDraw::Flip_Bitmap(UCHAR *image, int bytes_per_line, int height)
{
	// this function is used to flip bottom-up .BMP images

	UCHAR *buffer; // used to perform the image processing
	int index;     // looping index

	// allocate the temporary buffer
	if (!(buffer = (UCHAR *)malloc(bytes_per_line*height)))
		return(0);

	// copy image to work area
	memcpy(buffer,image,bytes_per_line*height);

	// flip vertically
	for (index=0; index < height; index++)
		memcpy(&image[((height-1) - index)*bytes_per_line],
		&buffer[index*bytes_per_line], bytes_per_line);

	// release the memory
	free(buffer);

	// return success
	return(1);

} // end Flip_Bitmap

///////////////////////////////////////////////////////////   

int MyDDraw::DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds,int color)
{
	DDBLTFX ddbltfx; // this contains the DDBLTFX structure

	// clear out the structure and set the size field 
	DDRAW_INIT_STRUCT(ddbltfx);

	// set the dwfillcolor field to the desired color
	ddbltfx.dwFillColor = color; 

	// ready to blt to surface
	lpdds->Blt(NULL,       // ptr to dest rectangle
		NULL,       // ptr to source surface, NA            
		NULL,       // ptr to source rectangle, NA
		DDBLT_COLORFILL | DDBLT_WAIT,   // fill and wait                   
		&ddbltfx);  // ptr to DDBLTFX structure

	// return success
	return(1);
} // end DDraw_Fill_Surface

///////////////////////////////////////////////////////////////

int MyDDraw::DDraw_Draw_Surface(LPDIRECTDRAWSURFACE7 source, // source surface to draw
					   int x, int y,                 // position to draw at
					   int width_src, int height_src,// size of source surface
					   int width_dest, int height_dest,// size of dest surface
					   LPDIRECTDRAWSURFACE7 dest,    // surface to draw the surface on
					   int transparent = 1)          // transparency flag
{
	// draw a bob at the x,y defined in the BOB
	// on the destination surface defined in dest

	RECT dest_rect,   // the destination rectangle
		source_rect; // the source rectangle                             

	// fill in the destination rect
	dest_rect.left   = x;
	dest_rect.top    = y;
	dest_rect.right  = x+width_dest-1;
	dest_rect.bottom = y+height_dest-1;

	// fill in the source rect
	source_rect.left    = 0;
	source_rect.top     = 0;
	source_rect.right   = width_src-1;
	source_rect.bottom  = height_src-1;

	// test transparency flag

	if (transparent)
	{
		// enable color key blit
		// blt to destination surface
		if (FAILED(dest->Blt(&dest_rect, source,
			&source_rect,(DDBLT_WAIT | DDBLT_KEYSRC),
			NULL)))
			return(0);

	} // end if
	else
	{
		// perform blit without color key
		// blt to destination surface
		if (FAILED(dest->Blt(&dest_rect, source,
			&source_rect,(DDBLT_WAIT),
			NULL)))
			return(0);

	} // end if

	// return success
	return(1);

} // end DDraw_Draw_Surface

///////////////////////////////////////////////////////////////

LPDIRECTDRAWSURFACE7 MyDDraw::DDraw_Create_Surface(int width, int height, int mem_flags, int color_key = 0)
{
	// this function creates an offscreen plain surface

	DDSURFACEDESC2 ddsd;         // working description
	LPDIRECTDRAWSURFACE7 lpdds;  // temporary surface

	// set to access caps, width, and height
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize  = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	// set dimensions of the new bitmap surface
	ddsd.dwWidth  =  width;
	ddsd.dwHeight =  height;

	// set surface to offscreen plain
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

	// create the surface
	if (FAILED(lpdd->CreateSurface(&ddsd,&lpdds,NULL)))
		return(NULL);

	// test if user wants a color key
	if (color_key >= 0)
	{
		// set color key to color 0
		DDCOLORKEY color_key; // used to set color key
		color_key.dwColorSpaceLowValue  = 0;
		color_key.dwColorSpaceHighValue = 0;

		// now set the color key for source blitting
		lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);
	} // end if

	// return surface
	return(lpdds);
} // end DDraw_Create_Surface



///////////////////////////////////////////////////////////////
MyDDraw::MyDDraw(CWnd *cWnd) : m_pWnd(cWnd)
{
	Init_DDraw();
}

int MyDDraw::Init_DDraw()
{
	// this is called once after the initial window is created and
	// before the main event loop is entered, do all your initialization
	// here

	lpdd = NULL;   // dd4 object
	lpddsprimary = NULL;   // dd primary surface
	lpddclipper = NULL;   // dd clipper
	lpddsbackground = NULL;
	lpddColorControl = NULL;
	bitmap.buffer = NULL;

	// create IDirectDraw interface 7.0 object and test for error
	if (FAILED(DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)))
		return(0);

	// set cooperation to full screen
	if (FAILED(lpdd->SetCooperativeLevel(m_pWnd->GetSafeHwnd(), DDSCL_NORMAL | DDSCL_NOWINDOWCHANGES)))
		return(0);

	// we need a complex surface system with a primary and backbuffer

	// clear ddsd and set size
	DDRAW_INIT_STRUCT(ddsd); 

	// enable valid fields
	ddsd.dwFlags = DDSD_CAPS;

	// request a primary surface
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	
	// create the primary surface
	if (FAILED(lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL)))
		return(0);

	// 创建剪裁器并与主页面关联
	if (FAILED(lpdd->CreateClipper(0,&lpddclipper,NULL)))
	{
		return(0);
	}
	if (FAILED(lpddclipper->SetHWnd(0, m_pWnd->GetSafeHwnd())))
	{
		return 0;
	}
	if (FAILED(lpddsprimary->SetClipper(lpddclipper)))
	{
		return 0;
	}


	// 仅仅是为了测试显卡是否支持ColorControl和Gamma
	/*
	DDCAPS hel_caps, hal_caps;

	// initialize the structures
	DDRAW_INIT_STRUCT(hel_caps);
	DDRAW_INIT_STRUCT(hal_caps);

	if (FAILED(lpdd->GetCaps(&hal_caps, &hel_caps)))
		return(0);

	if (hal_caps.dwCaps2 & DDCAPS2_COLORCONTROLPRIMARY)
		OutputDebugString(_T("Supports primary surface contains color controls\n"));

	if (hal_caps.dwCaps2 & DDCAPS2_COLORCONTROLOVERLAY)
		OutputDebugString(_T("Supports overlay surface contains color controls\n"));

	if (hal_caps.dwCaps2 & DDCAPS2_PRIMARYGAMMA)
		OutputDebugString(_T("Supports loadable gamma ramps for the primary surface\n"));
	*/

	/*
	HRESULT hRes = lpddsprimary->QueryInterface(IID_IDirectDrawColorControl, (void **)&lpddColorControl);
	// user color control
	if (FAILED(lpddColorControl->GetColorControls(&ddcolor)))
	{
		return 0;
	}
	
	if (FAILED(lpddsprimary->QueryInterface(IID_IDirectDrawGammaControl, (void **)&lpddGramma)))
	{
		OutputDebugString(_T("QueryInterface of Gramma failed!\n"));
		return 0;
	}
	*/
	
	
	// load the 24-bit image
	if (!Load_Bitmap_File(&bitmap,"bitmap24.bmp"))
		return(0);

	// clean the surfaces
	DDraw_Fill_Surface(lpddsprimary,0);

	// create the buffer to hold the background
	lpddsbackground = DDraw_Create_Surface(SCREEN_WIDTH,SCREEN_HEIGHT,0,-1);

	// copy the background bitmap image to the background surface 

	// lock the surface
	lpddsbackground->Lock(NULL,&ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,NULL);

	// get video pointer to primary surfce
	UINT *image_buffer = (UINT *)ddsd.lpSurface;       

	// 32bits mode r-shift 2 bits
	int lpitch32 = ddsd.lPitch >> 2;

	// memory is non-linear, copy line by line
	for (int y=0; y < SCREEN_HEIGHT; y++)
	{
		for (int x=0; x < SCREEN_WIDTH; x++)
		{
			UCHAR blue = bitmap.buffer[3*x + 3*y*SCREEN_WIDTH + 0];
			UCHAR green = bitmap.buffer[3*x + 3*y*SCREEN_WIDTH + 1];
			UCHAR red = bitmap.buffer[3*x + 3*y*SCREEN_WIDTH +2];

			image_buffer[x + y*lpitch32] = _RGB32BIT(0, red, green, blue);
		} // end for x
	} // end for y

	// now unlock the primary surface
	if (FAILED(lpddsbackground->Unlock(NULL)))
		return(0);

	// unload the bitmap file, we no longer need it
	Unload_Bitmap_File(&bitmap);

	return(1);
}

// （未实现）参考链接：http://www.vckbase.com/document/viewdoc/?id=747
HRESULT MyDDraw::UpdateGammaRamp(int value)
{
	HRESULT     hRes;
	DDGAMMARAMP DDGammaRamp;
	WORD dwGamma;

	ZeroMemory(&DDGammaRamp, sizeof(DDGammaRamp));

	lpddsprimary->QueryInterface(IID_IDirectDrawGammaControl, (void **)&lpddGramma);

	if( FAILED(hRes = lpddGramma->GetGammaRamp(0, &DDGammaRamp)))
		return hRes;

	for(int blackloop=0;blackloop<256;blackloop++)
	{
		//如果一个值大于0的话，就把它设成0
		if(DDGammaRamp.red[blackloop] > 0) 
		{
			//set the current value of DDGammaRamp.Red to 0.
			DDGammaRamp.red[blackloop]=0;
			//now let's update our primary
			lpddGramma->SetGammaRamp(0, &DDGammaRamp);
		}
		Sleep(1);
		if(DDGammaRamp.green[blackloop] > 0)
		{
			//设当前DDGammaRamp.yellow的值到0.
			DDGammaRamp.green[blackloop]=0;
			lpddGramma->SetGammaRamp(DDSGR_CALIBRATE, &DDGammaRamp);
		} 
		Sleep(1);
		if(DDGammaRamp.blue[blackloop] > 0)
		{
			//设当前DDGammaRamp.Blue的值到0.
			DDGammaRamp. blue [blackloop]=0;
			lpddGramma->SetGammaRamp(DDSGR_CALIBRATE, &DDGammaRamp);
		} 
		Sleep(1);
	}

	return S_OK;
}

int MyDDraw::Update_Screen()
{
	RECT rc;
	m_pWnd->GetClientRect(&rc);
	m_pWnd->ClientToScreen(&rc);

	// copy background to back buffer
	DDraw_Draw_Surface(lpddsbackground,rc.left,rc.top,SCREEN_WIDTH, SCREEN_HEIGHT, rc.right-rc.left, rc.bottom-rc.top, lpddsprimary,0);   

	return(1);

} // end Update_Screen

int MyDDraw::Shut_Down()
{
	// kill all the surfaces

	if (lpddsprimary)
	{
		lpddsprimary->Release();
		lpddsprimary = NULL;
	}

	if (lpdd)
	{
		lpdd->Release();
		lpdd = NULL;
	}

	return(1);

} // end Game_Shutdown
