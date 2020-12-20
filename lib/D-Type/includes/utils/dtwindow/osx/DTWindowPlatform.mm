/*
 *
 * Copyright (C) D-Type Solutions - All Rights Reserved
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 *
 * This file is part of D-Type Engine and may only be used, modified
 * and distributed under the terms of the applicable license agreement
 * For conditions of distribution and use, see license.txt
 *
 * ANY VIOLATIONS OF THE ABOVE LICENSE AGREEMENT WILL BE PROSECUTED TO
 * THE MAXIMUM EXTENT POSSIBLE UNDER THE LAW. NO VENDOR, DISTRIBUTOR,
 * DEALER, RETAILER, SALES PERSON OR OTHER PERSON IS AUTHORIZED TO MODIFY
 * THIS AGREEMENT OR TO MAKE ANY WARRANTY, REPRESENTATION OR PROMISE WHICH
 * IS DIFFERENT THAN, OR IN ADDITION TO, THIS AGREEMENT.
 *
 */


//#include <memory.h>
#include <OpenGL/glu.h>

#include "DTWindowPlatform.h"
#include "DTWindowPlatformPrivate.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CIContext.h>
#import <OpenGL/OpenGL.h>


#ifndef DEF_SUPPORT_RETINA_RESOLUTION
#define DEF_SUPPORT_RETINA_RESOLUTION 1
#endif

// adds Zoom1x, 2x, 3x, 4x to "Window" menu and allows resizing the window by dragging the lower right corner
#ifndef DEF_CONFIG_ALLOW_ZOOM
#define DEF_CONFIG_ALLOW_ZOOM 0
#endif

// enables some debug output, disables fades and capturing the mouse cursor
#ifndef DEF_CONFIG_DEBUG
#define DEF_CONFIG_DEBUG 0
#endif

#ifndef DEF_CONFIG_USE_TEXSUBIMAGE
#define DEF_CONFIG_USE_TEXSUBIMAGE 0
#endif

// vertical blank on/off
#define DEF_CONFIG_VBSYNC_OFF 0 /* noticeably faster but seems to cause a glitch -- sometimes newly opened windows appear blank */
#define DEF_CONFIG_VBSYNC_ON 1

#if DEF_CONFIG_DEBUG
#define GL_CHECK_ERROR(cmd) \
cmd; \
{\
	GLenum error = glGetError(); \
	if (GL_NO_ERROR != error) fprintf(stderr, "%s:%d:: %s failed with error: '%s'\n", __FILE__, __LINE__, #cmd, gluErrorString(error)); \
}
#else
#define GL_CHECK_ERROR(cmd) cmd;
#endif

// to get rid of the unused parameter warnings in ObjC methods
#define PARAMETER_UNUSED(x) (void)(x);

const DT_FLOAT WINDOW_FADE_TIME = 0.5; // fade time in seconds
const DT_SLONG WINDOW_FADE_STEPS = 30; // number of steps per fade

const DT_SLONG WINDOW_START_X = 50;
const DT_SLONG WINDOW_START_Y = 50;
const DT_SLONG WINDOW_OFFSET_Y = 25;
const DT_SLONG WINDOW_OFFSET_X = 30;

DT_SLONG WINDOW_X = WINDOW_START_X + WINDOW_OFFSET_X;
DT_SLONG WINDOW_Y = WINDOW_START_Y + WINDOW_OFFSET_Y;

DT_ULONG WINDOW_MODIFIERS = 0;


class CDTRectRegion
{
public:

	CDTRectRegion(DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h) : X(x), Y(y), W(w), H(h) {}
	DT_SLONG X, Y, W, H;
};


DT_CHAR* ReadFile(const DT_STREAM_DESC* sd, DT_SLONG* len)
{
	*len = 0;

	if (sd == NULL) return NULL;

	DT_DTSTREAM f = dtStreamOpen(sd, DV_STREAM_OPEN_MUST);
	if (f == NULL) return NULL;

	/* Determine the file size */
	dtStreamSeek(f, 0, DV_SEEK_END);
	*len = dtStreamLoc(f);
	if (*len < 0) *len = 0;
	dtStreamSeek(f, 0, DV_SEEK_BGN);

	DT_UBYTE* addr = (DT_UBYTE*)dtMemAlloc(*len + 1 + 1);
	if (addr == NULL) *len = 0;
	else
	{
		dtStreamRead(f, addr, *len, 0);
		addr[*len] = 0;
	}

	dtStreamClose(f, DV_STREAM_CLOSE_YES);

	return (DT_CHAR*)addr;
}


DT_SLONG IntAtStringIndex(NSArray* array, DT_SLONG index)
{
	return [(NSString *)[array objectAtIndex:index] integerValue];
}


void GetSystemVersion(DT_SLONG* major, DT_SLONG* minor, DT_SLONG* bugfix)
{
	*major = 0;
	*minor = 0;
	*bugfix = 0;

	NSString* versionString = [[NSDictionary dictionaryWithContentsOfFile: @"/System/Library/CoreServices/SystemVersion.plist"] objectForKey:@"ProductVersion"];
	if (versionString == NULL) return;

	NSArray* versions = [versionString componentsSeparatedByString:@"."];
	if (versions == NULL) return;

	NSUInteger count = [versions count];
	if (count <= 0) return;

	*major = IntAtStringIndex(versions, 0);
	if (count <= 1) return;

	*minor = IntAtStringIndex(versions, 1);
	if (count <= 2) return;

	*bugfix = IntAtStringIndex(versions, 2);
}


#if !defined(MAC_OS_X_VERSION_10_7) || MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_7

@interface NSEvent (LionSDK)
-(CGFloat)scrollingDeltaX;
-(CGFloat)scrollingDeltaY;
-(BOOL)hasPreciseScrollingDeltas;
-(BOOL)isDirectionInvertedFromDevice;
@end

#endif


#pragma mark -
#pragma mark Objective-C classes
#pragma mark -

@interface CDTWindowApplication : NSApplication <NSApplicationDelegate>

{
	@private
	BOOL messageLoopIsRunning;
}
-(id)init;

// since we never actually call [NSApp run] we need to overwrite -isRunning
-(BOOL)isRunning;
-(void)setIsRunning : (BOOL)isRunning;
-(void)performAbout : (id)sender;
-(BOOL)modifyMenuItem : (DT_SLONG)menu_item modifyFlag : (DT_SLONG) flag isItemSet : (BOOL) is_set currMenu : (NSMenu*) menu;
-(void)setupApplicationMenu : (const DT_STREAM_DESC*)sd_menu;


@end


@implementation CDTWindowApplication

-(id)init
{
	self = [super init];
	if (self != nil)
	{
		messageLoopIsRunning = NO;
		[self setDelegate : self];
	}
	return self;
}


-(BOOL)isRunning
{
	return messageLoopIsRunning;
}


-(void)setIsRunning : (BOOL)isRunning
{
	messageLoopIsRunning = isRunning;
}

// Called on Cmd-Q/Quit menu item


-(NSApplicationTerminateReply)applicationShouldTerminate : (NSApplication*)sender
{
	PARAMETER_UNUSED(sender)

	// try to close all windows
	NSArray* windows = [self windows];
	NSEnumerator* enumerator = [windows objectEnumerator];
	NSWindow* window;
	while ((window = [enumerator nextObject]) != nil)
	{
		//[window close];
		//window->close();
		//window->onMenuCommand(31, 0);
		[window performClose : nil];
	}

	return NSTerminateCancel;

	// if no windows are left open, we allow the application to terminate
	//if ([[self windows] count] != 0) return NSTerminateCancel;

	//[self setIsRunning : NO];
	//return NSTerminateNow;
}


-(BOOL)modifyMenuItem : (DT_SLONG)menu_item modifyFlag : (DT_SLONG) flag isItemSet : (BOOL) is_set currMenu : (NSMenu*) menu
{
	// Try to find menu item in the current menu level
	NSMenuItem* item = [menu itemWithTag : menu_item];
	if (item != nil)
	{
		// We found it, so set the state and exit
		if (flag == 1)
		{
			[item setEnabled : is_set];
			/* This method has no effect unless the menu in which the item will be added or is already a part of has been sent setAutoenablesItems:NO */
		}
		else
		{
			[item setState : (is_set ? NSOnState : NSOffState)];
		}

		return YES;
	}

	// We didn't find it, so search in all submenus (using a recursive search method)

	DT_SLONG i = 0, n = [menu numberOfItems];

	for (i = 0; i < n; i++) // test all menu items in the current menu level for submenus
	{
		item = [menu itemAtIndex : i];
		if (item == nil) break; // should never happen

		NSMenu* submenu = [item submenu];
		if (submenu == nil) continue; // this menu item has no submenu so proceed to the next one

		// this menu item has a submenu so search in it
		BOOL result = [/*(CDTWindowApplication*)*/NSApp modifyMenuItem : menu_item modifyFlag : flag isItemSet : is_set currMenu : submenu];
		if (result == YES) return YES;
	}

	return NO;
}


// a small custom about box

-(void)performAbout : (id)sender
{
	PARAMETER_UNUSED(sender)

	NSString* versionString = [NSString stringWithFormat : @"using dtwindow"];
	NSAttributedString* creditString = [[[NSAttributedString alloc] initWithString : versionString] autorelease];
	NSString* copyRightString = @"";
	//NSString* copyRightString = [NSString stringWithCString : "Copyright \xc2\xa9 1996-2001\n" "dtwindow 1.5" encoding : NSUTF8StringEncoding];
	NSDictionary* dict = [NSDictionary dictionaryWithObjectsAndKeys : creditString, @"Credits", copyRightString, @"Copyright", nil];
	[NSApp orderFrontStandardAboutPanelWithOptions : dict];
}


/*
	Set up a menu bar, so the application looks a bit more native. Besides the well known Apple menu
	there is only a "Window" menu added. This only gets called when we create the NSApp instance, so if
	application is used from a true Cocoa context, this should not interfere.
*/

-(void)setupApplicationMenu : (const DT_STREAM_DESC*)sd_menu
{
	NSString* applicationName = [[NSProcessInfo processInfo] processName];

	NSMenu* mainMenu = [[NSMenu alloc] initWithTitle : @"MainMenu"];

	// Setup Application menu. This needs a hack (see below)
	{
		// the title of the menu and item don't matter, they will be replaced by the process name anyway
		NSMenu* appMenu = [[NSMenu alloc] initWithTitle : @"AppleMenu"];

		// "About "applicationName
		//[appMenu addItemWithTitle : [@"About " stringByAppendingString : applicationName] action : @selector(performAbout : ) keyEquivalent : @""];

		// ---
		[appMenu addItem : [NSMenuItem separatorItem]];

		// Preferences TODO: nothing implemented yet, we could let the user choose a code path or an initial screen here.
		[appMenu addItemWithTitle : @"Preferences" action : @selector(showPreferences : ) keyEquivalent : @","];

		// ---
		[appMenu addItem : [NSMenuItem separatorItem]];

		// Services
		NSMenuItem* servicesItem = [appMenu addItemWithTitle : @"Services" action : nil keyEquivalent : @""];
		NSMenu* servicesMenu = [[NSMenu alloc] initWithTitle : @""];

		[servicesItem setSubmenu : servicesMenu];
		[NSApp setServicesMenu : servicesMenu];
		[servicesMenu release];

		// ---
		[appMenu addItem : [NSMenuItem separatorItem]];

		// Hide Application
		[appMenu addItemWithTitle : [@"Hide " stringByAppendingString : applicationName] action : @selector(hide : ) keyEquivalent : @"h"];

		// Hide Others
		NSMenuItem* hideOthersItem = [appMenu addItemWithTitle : @"Hide Others" action : @selector(hideOtherApplications : ) keyEquivalent : @"h"];
		[hideOthersItem setKeyEquivalentModifierMask : (NSAlternateKeyMask | NSCommandKeyMask)];

		// Show All
		[appMenu addItemWithTitle : @"Show All" action : @selector(unhideAllApplications : ) keyEquivalent : @""];

		// ---
		[appMenu addItem : [NSMenuItem separatorItem]];

		// Quit <applicationName>
		[appMenu addItemWithTitle : [@"Quit " stringByAppendingString : applicationName] action : @selector(terminate : ) keyEquivalent : @"q"];
		//NSMenuItem* quitItem = [appMenu addItemWithTitle : [@"Quit " stringByAppendingString : applicationName] action : @selector(menuCommand : ) keyEquivalent : @"q"];
		//[quitItem setTag : 31];

		// setAppleMenu is private since Mac OS X 10.4 Tiger, but it still seems to be available (and does work) in 10.5 Leopard.
		if ([NSApp respondsToSelector : @selector(setAppleMenu : )])
		{
			[NSApp performSelector : @selector(setAppleMenu : ) withObject : appMenu];
		}

		NSMenuItem* appItem = [[NSMenuItem alloc] initWithTitle : @"AppMenuItem" action : nil keyEquivalent : @""];
		[appItem setSubmenu : appMenu];
		[mainMenu addItem : appItem];

		[appItem release];
		[appMenu release];
	}

	// Setup a static test menu
	/*
	{
		NSMenu* myOwnMenu = [[NSMenu alloc] initWithTitle : @"Static Menu"];
		NSMenuItem* myItem;

		myItem = [myOwnMenu addItemWithTitle : @"Menu Entry 1" action : @selector(zoomX : ) keyEquivalent : @"1"];
		myItem = [myOwnMenu addItemWithTitle : @"Menu Entry 2" action : @selector(zoomX : ) keyEquivalent : @"2"];
		myItem = [myOwnMenu addItemWithTitle : @"Menu Entry 3" action : @selector(zoomX : ) keyEquivalent : @"3"];
		myItem = [myOwnMenu addItemWithTitle : @"Menu Entry 4" action : @selector(zoomX : ) keyEquivalent : @"4"];
		[myOwnMenu addItem : [NSMenuItem separatorItem]];
		myItem = [myOwnMenu addItemWithTitle : @"More 1..." action : @selector(zoomX : ) keyEquivalent : @"A"];
		myItem = [myOwnMenu addItemWithTitle : @"More 2..." action : @selector(zoomX : ) keyEquivalent : @"B"];
		myItem = [myOwnMenu addItemWithTitle : @"More 3..." action : @selector(zoomX : ) keyEquivalent : @"C"];

		NSMenuItem* myOwnMenuItem = [[NSMenuItem alloc] initWithTitle : @"" action : nil keyEquivalent : @""];
		[mainMenu addItem : myOwnMenuItem];
		[myOwnMenuItem setSubmenu : myOwnMenu];
	}
	*/

	// Setup a custom menu system
	{
		DT_SLONG len = 0;
		DT_CHAR* menudata = ReadFile(sd_menu, &len);
		if (menudata == NULL) goto menu_exit;

		NSMenu* nsmenu[100];
		DT_CHAR s[1024], empty[1] = "";
		DT_SLONG i, j, menu_count, count_1 = 0, count_2 = 0;
		const DT_CHAR* addr;

		addr = menudata;
		while (1)
		{
			addr = strchr(addr, '}');
			if (addr == NULL) break;
			addr++;
			count_1++;
		}

		addr = menudata;
		while (1)
		{
			addr = strchr(addr, '{');
			if (addr == NULL) break;
			addr++;
			count_2++;
		}

		menu_count = count_1;
		if (count_1 != count_2) goto menu_end; /* bad menudata */
		if (menu_count <= 0 || menu_count >= 100) goto menu_end; /* bad menudata */

		for (i = 0; i < menu_count; i++) nsmenu[i] = NULL;

		//printf("menu_count=%ld\n", menu_count);

		addr = menudata;

		for (i = 0; i < menu_count;)
		{
			j = 0;
			while (*addr != 0)
			{
				DT_CHAR c = *addr;
				s[j] = c;
				addr++;
				if (c == 10) { if (s[j] == 13) addr++; break; } /* deal with OS specific linebreaks (10 or 10+13) */
				if (c == 13) { if (s[j] == 10) addr++; break; } /* deal with OS specific linebreaks (13 or 13+10) */
				j++;
			}
			s[j] = 0;

			     if (s[0] == '#') continue;
			else if (s[0] == '!') continue;
			else if (s[0] == '}') { i++; continue; }
			else if (s[0] == '{') continue;
			else if (strlen(s) < 2) continue;

			DT_SLONG child = atol(s); if (child < 0 || child >= menu_count) continue;
			DT_CHAR* menucode = strchr(s, '|'); if (menucode == NULL) continue; menucode++;
			DT_CHAR* text = strchr(menucode, '|'); if (text == NULL) continue; text++;
			DT_CHAR* key = strchr(text, '|'); if (key == NULL) key = empty; else { *key = 0; key++; }
			DT_SLONG type = (text[0] == 0 ? 0 : 1);

			//printf("%d, %s, %s, %s\n", child, menucode, text, key);

			NSString* nstext = [NSString stringWithUTF8String : text];

			if (nsmenu[child] == NULL) nsmenu[child] = [[NSMenu alloc] initWithTitle : nstext];

			if (i == 0) /* top level menu */
			{
				if (type == 0) continue;

				NSMenuItem* item = [[NSMenuItem alloc] initWithTitle : @"" action : nil keyEquivalent : @""];
				[mainMenu addItem : item];
				[item setSubmenu : nsmenu[child]];
				//printf("setTag %d\n", atoi(menucode));
				[item setTag : atoi(menucode)];
				continue;
			}

			/* regular menu */

			if (type == 0)
			{
				[nsmenu[i] addItem : [NSMenuItem separatorItem]];
				continue;
			}

			[nsmenu[i] setAutoenablesItems : NO];

			NSString* nskey = [NSString stringWithUTF8String : key];
			NSMenuItem* item = [nsmenu[i] addItemWithTitle : nstext action : @selector(menuCommand : ) keyEquivalent : nskey];
			//[item setState : NSOnState];
			//printf("setTag %d\n", atoi(menucode));
			[item setTag : atoi(menucode)];

			if (child > 0) [item setSubmenu : nsmenu[child]];
		}

menu_end:

		dtMemFree(menudata);

		// Some experiments
		/*
		{
			NSMenu* myOwnMenu = [[NSMenu alloc] initWithTitle : @"Menu1"];
			NSMenuItem* myItem = [myOwnMenu addItemWithTitle : @"A1" action : @selector(zoomX : ) keyEquivalent : @"1"];
			[myItem setSubmenu : nsmenu[1]];

			NSMenuItem* myOwnMenuItem = [[NSMenuItem alloc] initWithTitle : @"" action : nil keyEquivalent : @""];
			[mainMenu addItem : myOwnMenuItem];
			[myOwnMenuItem setSubmenu : nsmenu[1]];
		}

		{
			NSMenu* myOwnMenu = [[NSMenu alloc] initWithTitle : @"Menu2"];
			NSMenuItem* myItem = [myOwnMenu addItemWithTitle : @"A1" action : @selector(zoomX : ) keyEquivalent : @"1"];
			[myItem setSubmenu : nsmenu[2]];

			NSMenuItem* myOwnMenuItem = [[NSMenuItem alloc] initWithTitle : @"" action : nil keyEquivalent : @""];
			[mainMenu addItem : myOwnMenuItem];
			[myOwnMenuItem setSubmenu : nsmenu[2]];
		}
		*/
	}

menu_exit:

	// Setup "Window" menu
	{
		NSMenu* windowMenu = [[NSMenu alloc] initWithTitle : @"Window"];

		// Close Window
		[windowMenu addItemWithTitle : @"Close Window" action : @selector(performClose : ) keyEquivalent : @"w"];

		// ---
		[windowMenu addItem : [NSMenuItem separatorItem]];

		// Minimize
		[windowMenu addItemWithTitle : @"Minimize" action : @selector(performMiniaturize : ) keyEquivalent : @"m"];

#if DEF_CONFIG_ALLOW_ZOOM

		// Zoom
		[windowMenu addItemWithTitle : @"Zoom" action : @selector(performZoom : ) keyEquivalent : @""];

		// ---
		[windowMenu addItem : [NSMenuItem separatorItem]];

		// 1 x
		NSMenuItem* zoomItem;
		zoomItem = [windowMenu addItemWithTitle : @"1 x " action : @selector(zoomX : ) keyEquivalent : @"1"];
		[zoomItem setTag : 1];

		// 2 x
		zoomItem = [windowMenu addItemWithTitle : @"2 x" action : @selector(zoomX : ) keyEquivalent : @"2"];
		[zoomItem setTag : 2];

		// 3 x
		zoomItem = [windowMenu addItemWithTitle : @"3 x" action : @selector(zoomX : ) keyEquivalent : @"3"];
		[zoomItem setTag : 3];

		// 4 x
		zoomItem = [windowMenu addItemWithTitle : @"4 x" action : @selector(zoomX : ) keyEquivalent : @"4"];
		[zoomItem setTag : 4];

#endif

		// ---
		[windowMenu addItem : [NSMenuItem separatorItem]];

		// Bring All to Front
		[windowMenu addItemWithTitle : @"Bring All to Front" action : @selector(arrangeInFront : ) keyEquivalent : @""];

		// ---
		[windowMenu addItem : [NSMenuItem separatorItem]];

		NSMenuItem* windowItem = [[NSMenuItem alloc] initWithTitle : @"" action : nil keyEquivalent : @""];
		[mainMenu addItem : windowItem];
		[windowItem setSubmenu : windowMenu];
		[NSApp setWindowsMenu : windowMenu];
	}

	[NSApp setMainMenu : mainMenu];

	[mainMenu release];
}

@end


@interface CDTCustomNSWindow : NSWindow

{
    @public

    BOOL IsEnabled;
}

@end


@implementation CDTCustomNSWindow

//-(id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)windowStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferCreation
//{
//	self = [super initWithContentRect:contentRect styleMask:NSBorderlessWindowMask backing:bufferingType defer:deferCreation];
//	return self;
//}

/* https://stackoverflow.com/questions/10620106/how-to-create-no-activate-window-in-mac-os?rq=1 */

-(BOOL)canBecomeKeyWindow
{
	return IsEnabled;
}

//-(BOOL)canBecomeMainWindow
//{
//	return IsEnabled;
//}

@end



#pragma mark -


@interface CDTWindowView : NSView <NSWindowDelegate>

{
	@private

	NSOpenGLContext* _openGLContext;
	NSOpenGLPixelFormat* _pixelFormat;

	DT_FLOAT glVersion;
	GLint glMaxRectTextureSize;
	GLint glMaxTextureUnits;
	GLint glMaxTextureSize;
	GLboolean glHasHWFloats;
	GLboolean glHasTextureRange;
	GLboolean glHasClientStorage;
	GLboolean glHasTextureRectangle;
	bool glIsIntelIntegratedGraphics;
	bool glUseSharedHint;

	GLuint glTexture;

	// Gamma values used for fade operations
	CGGammaValue redMin, redMax, redGamma;
	CGGammaValue greenMin, greenMax, greenGamma;
	CGGammaValue blueMin, blueMax, blueGamma;

	// Window represented by this view
	CDTWindowPlatform* windowPlatform;

	// Rectangle the pixel buffers are copied/stretched to; usually the whole window content
	NSRect fillRect;

	// Screen ID that is used for output
	CGDirectDisplayID displayID;

	DT_SLONG resScaleX;
	DT_SLONG resScaleY;

	NSTimer* myTimer;
}

+(NSOpenGLPixelFormat*)defaultPixelFormatForDisplay : (CGDirectDisplayID)displayID;
+(NSOpenGLPixelFormat*)defaultPixelFormat;

-(id)initWithFrame : (NSRect)frameRect pixelFormat : (NSOpenGLPixelFormat*)format windowPlatform : (CDTWindowPlatform*)theWindow;
-(void)setOpenGLContext : (NSOpenGLContext*)context;
-(NSOpenGLContext*)openGLContext;
-(void)clearGLContext;
-(void)updateGLInfo;
-(void)releaseTexture;
-(void)prepareTexture : (const GLvoid*)textureData;
-(void)prepareOpenGL;
-(void)update;
-(void)reshape;
-(void)setPixelFormat : (NSOpenGLPixelFormat*)pixelFormat;
-(NSOpenGLPixelFormat*)pixelFormat;

// no setter for "windowPlatform", as this is set once in -init
-(CDTWindowPlatform*)windowPlatform;

-(CGDirectDisplayID)displayID;

-(void)setFillRect : (NSRect)newFillRect;
-(NSRect)fillRect;

-(void)killTimer;
-(void)setTimer : (DT_FLOAT)timeVal;
-(void)fireTimer;
-(void)setResolution : (bool)best;
-(DT_SLONG)resScaleX;
-(DT_SLONG)resScaleY;

-(void)hideCursor;
-(void)unhideCursor;

-(void)fadeInDisplay;
-(void)fadeOutDisplay;

-(void)clear;
-(void)copyTrueColorPixelsUsingOpenGL : (const void*)trueColorPixels dirtyRect : (const CDTRectRegion*)rect_region dX : (GLint)dx dY : (GLint)dy;
-(void)setSwapInterval : (GLint)swapInterval;

-(void)menuCommand : (id)sender;
-(void)zoomX : (id)sender;


@end


@implementation CDTWindowView

+(NSOpenGLPixelFormat*)defaultPixelFormat
{
	return [self defaultPixelFormatForDisplay : CGMainDisplayID()];
}


+(NSOpenGLPixelFormat*)defaultPixelFormatForDisplay : (CGDirectDisplayID)displayID
{
	NSOpenGLPixelFormat* pixelFormat = nil;
	NSOpenGLPixelFormatAttribute attributes[20];
	DT_SLONG at_index = 0;

	//attributes[at_index++] = NSOpenGLPFATripleBuffer;
	attributes[at_index++] = NSOpenGLPFADoubleBuffer;
	attributes[at_index++] = NSOpenGLPFAAccelerated;
	//attributes[at_index++] = NSOpenGLPFAColorFloat;
	attributes[at_index++] = NSOpenGLPFAColorSize;
	attributes[at_index++] = (NSOpenGLPixelFormatAttribute)24;
	attributes[at_index++] = NSOpenGLPFAScreenMask;
	attributes[at_index++] = (NSOpenGLPixelFormatAttribute)CGDisplayIDToOpenGLDisplayMask(displayID);

	attributes[at_index++] = NSOpenGLPFABackingStore; /* !!! */
	attributes[at_index++] = NSOpenGLPFANoRecovery;

	attributes[at_index] = (NSOpenGLPixelFormatAttribute)0;

	//NSOpenGLPixelFormatAttribute attribs[] = {NSOpenGLPFANoRecovery, NSOpenGLPFAWindow, NSOpenGLPFAAccelerated, NSOpenGLPFADoubleBuffer, NSOpenGLPFAColorSize, 24, NSOpenGLPFAAlphaSize, 8, NSOpenGLPFADepthSize, 24, NSOpenGLPFAStencilSize, 8, NSOpenGLPFAAccumSize, 0, 0};

	pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes : attributes];

	return [pixelFormat autorelease];
}


-(void)surfaceNeedsUpdate : (NSNotification*)notification
{
	PARAMETER_UNUSED(notification)

	[self update];
}


-(id)initWithFrame : (NSRect)frameRect pixelFormat : (NSOpenGLPixelFormat*)format windowPlatform : (CDTWindowPlatform*)theWindow
{
	self = [super initWithFrame : frameRect];

	myTimer = NULL;
	windowPlatform = theWindow;
	displayID = CGMainDisplayID();
	glTexture = 0;
	resScaleX = 1;
	resScaleY = 1;

	if (self == nil) return self;

	_pixelFormat = [format retain];
	[[NSNotificationCenter defaultCenter] addObserver : self selector : @selector(surfaceNeedsUpdate : ) name : NSViewGlobalFrameDidChangeNotification object : self];
	return self;
}


-(void)releaseTexture
{
	if (glTexture > 0) glDeleteTextures(1, &glTexture);
	glTexture = 0;
}


-(void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver : self name : NSViewGlobalFrameDidChangeNotification object : self];
	[self releaseTexture];
	[self clearGLContext];
	[_pixelFormat release];
	[super dealloc];
}


-(void)setPixelFormat : (NSOpenGLPixelFormat*)pixelFormat
{
	[_pixelFormat release];
	_pixelFormat = [pixelFormat retain];
}


-(NSOpenGLPixelFormat*)pixelFormat
{
	return _pixelFormat;
}


-(void)setOpenGLContext : (NSOpenGLContext*)context
{
	[self clearGLContext];
	_openGLContext = [context retain];
}


-(NSOpenGLContext*)openGLContext
{
	// create a context if none exists
	if (_openGLContext == nil)
	{
		[self updateGLInfo];

		if (_pixelFormat == nil)
		{
			[self setPixelFormat : [[self class] defaultPixelFormatForDisplay : displayID]];
		}

		_openGLContext = [[NSOpenGLContext alloc] initWithFormat : _pixelFormat shareContext : nil];
		[_openGLContext makeCurrentContext];
		[self prepareOpenGL];
	}

	if (_openGLContext == nil || _pixelFormat == nil) return nil;

	return _openGLContext;
}


-(void)clearGLContext
{
	if (_openGLContext != nil)
	{
		[self releaseTexture];

		if ([_openGLContext view] == self)
		{
			[_openGLContext clearDrawable];
		}

		[_openGLContext release];
		_openGLContext = nil;
	}

	// release pixel format too
	[self setPixelFormat : nil];
}


-(void)updateGLInfo
{
	const GLubyte *glVersionString, *glExtensions, *glRendererString;

	if (displayID == 0) displayID = CGMainDisplayID();

	CGOpenGLDisplayMask displayMask = CGDisplayIDToOpenGLDisplayMask(displayID);

	// Check capabilities of display represented by display mask
	CGLPixelFormatAttribute attribs[] = {kCGLPFADisplayMask, (CGLPixelFormatAttribute)displayMask, (CGLPixelFormatAttribute)0};
	CGLPixelFormatObj pixelFormat = NULL;
	GLint numPixelFormats = 0;
	CGLContextObj cglContext = 0;
	CGLContextObj curr_ctx = CGLGetCurrentContext();

	CGLChoosePixelFormat(attribs, &pixelFormat, &numPixelFormats);

	if (pixelFormat)
	{
		CGLCreateContext(pixelFormat, NULL, &cglContext);
		CGLDestroyPixelFormat(pixelFormat);
		CGLSetCurrentContext(cglContext);

		if (cglContext)
		{
			// Check for capabilities and functionality
			glVersionString = glGetString(GL_VERSION);
			glExtensions = glGetString(GL_EXTENSIONS);
			glRendererString = glGetString(GL_RENDERER);

			sscanf((const DT_CHAR*)glVersionString, "%lf", &glVersion);

			glGetIntegerv(GL_MAX_TEXTURE_UNITS, &glMaxTextureUnits);
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTextureSize);

			glHasHWFloats = gluCheckExtension((const GLubyte*)"GL_APPLE_DT_FLOAT_pixels", glExtensions);
			glHasTextureRange = gluCheckExtension((const GLubyte*)"GL_APPLE_texture_range", glExtensions);
			glHasClientStorage = gluCheckExtension((const GLubyte*)"GL_APPLE_client_storage", glExtensions);
			glHasTextureRectangle = gluCheckExtension((const GLubyte*)"GL_EXT_texture_rectangle", glExtensions);

			/*
				On my iMac handing floats directly to the GPU can keep up without dtwindow's conversion, but on all other tested machines it
				absolutely destroys performance. We will need this later for dtwindow's ToneMapping extension, but for now we leave it disabled
				glHasFastFloats=false;

				The integrated graphics chipsets I could test on were slower with client storage, so we set a flag for this code path.
			*/

			glIsIntelIntegratedGraphics = (NULL != strstr((const DT_CHAR*)glRendererString, "Intel"));

			/*
				Generally GL_STORAGE_CACHED_APPLE seems to get the higher performance, but at least on the new iMacs (ATI HD 2600 Pro in my case)
				GL_STORAGE_SHARED_APPLE is a lot faster for TrueColor
			*/

			glUseSharedHint = (NULL != strstr((const DT_CHAR*)glRendererString, "Radeon HD 2600"));

			if (glHasTextureRectangle) glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, &glMaxRectTextureSize);
			else glMaxRectTextureSize = 0;

		#if DEF_CONFIG_DEBUG

			const GLubyte* glVendorString = glGetString(GL_VENDOR);
			printf("OpenGL Version:             %s\n", glVersionString);
			printf("Vendor:                     %s\n", glVendorString);
			printf("Renderer:                   %s\n", glRendererString);
			printf("Max Texture Size:           %d\n", (DT_SLONG)glMaxTextureSize);
			printf("Max Texture Size:           %d\n", (DT_SLONG)glMaxTextureSize);
			printf("Max Rectangle Texture Size: %d\n", (DT_SLONG)glMaxRectTextureSize);
			printf("Texture Rectangle:          %savailable\n", glHasTextureRectangle ? "" : "not ");
			printf("Float Buffers:              %savailable\n", glHasHWFloats ? "" : "not ");
			printf("Texture Range:              %savailable\n", glHasTextureRange ? "" : "not ");
			printf("Client Storage:             %savailable\n", glHasClientStorage ? "" : "not ");

		#endif
		}
	}

	CGLDestroyContext(cglContext);
	CGLSetCurrentContext(curr_ctx);
}


-(void)prepareTexture : (const GLvoid*)textureData
{
	const DT_SLONG width = windowPlatform->Width;
	const DT_SLONG height = windowPlatform->Height;

	if (glTexture == 0)
	{
		GL_CHECK_ERROR(glGenTextures(1, &glTexture));
		GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_RECTANGLE_ARB, glTexture));

		if (!glIsIntelIntegratedGraphics)
		{
			GL_CHECK_ERROR(glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE));
			GL_CHECK_ERROR(glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_PRIORITY, 0.0));
			GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_STORAGE_HINT_APPLE, glUseSharedHint ? GL_STORAGE_SHARED_APPLE : GL_STORAGE_CACHED_APPLE));
		}

		GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GL_CHECK_ERROR(glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL));

		GL_CHECK_ERROR(glPixelStorei(GL_UNPACK_ROW_LENGTH, width));
		GL_CHECK_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	}

	if (glHasTextureRange)
	{
		GL_CHECK_ERROR(glTextureRangeAPPLE(GL_TEXTURE_RECTANGLE_ARB, width * height * 4, textureData));
	}

	GL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB8, /* see Note 1 */ width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV, textureData));

	// Note 1: using floats here directly is faster on my iMac, but slower on my MBP and slower on both compared to dtwindow's software conversion :-P
}


-(void)prepareOpenGL
{
	// prepare OpenGL context (state, texture, etc.)

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	GL_CHECK_ERROR(glEnable(GL_TEXTURE_RECTANGLE_ARB));

	NSOpenGLContext* context = _openGLContext;

	// Clear the view, so we get a black background
	[self clear];
	[context flushBuffer];
	//[self clear];
	//[context flushBuffer];

	// enable/disable sync to VB
	GLint swapInterval = DEF_CONFIG_VBSYNC_OFF;
	[context setValues : &swapInterval forParameter : NSOpenGLCPSwapInterval];
}


-(void)updateDisplayID
{
	NSScreen* screen = [[self window] screen];

	if (screen == nil) return;

	NSDictionary* screenInfo = [screen deviceDescription];
	NSNumber* screenID = [screenInfo objectForKey : @"NSScreenNumber"];
	displayID = (CGDirectDisplayID)[screenID longValue];
}


-(void)update
{
	if ([_openGLContext view] != self) return;

	[_openGLContext update];
	[self updateDisplayID];
}


// In NSOpenGLView this is called automatically, for normal NSViews it is not, which is why the sample code does not implement it, we just call it manually when necessary


-(void)reshape
{
	NSOpenGLContext* context = [self openGLContext];

	[context makeCurrentContext];
	[context update];

	NSRect drawableRect = [self convertRect : [self bounds] toView : nil];

	/*

	// calculate fillRect
	DT_FLOAT drawableRatio = NSWidth(drawableRect) / NSHeight(drawableRect);
	DT_FLOAT outputRatio = windowPlatform->Width / (DT_FLOAT)windowPlatform->Height;

	if (drawableRatio <= outputRatio)
	{
		// window is wider than screen, so we leave black borders at top and bottom (letterbox)
		DT_FLOAT usedHeight = ceilf(NSWidth(drawableRect) / outputRatio);
		[self setFillRect : NSMakeRect(0.0, (NSHeight(drawableRect) - usedHeight) * 0.5, NSWidth(drawableRect), usedHeight)];
	}
	else
	{
		// screen is wider than window, leave black borders to the left and right
		DT_FLOAT usedWidth = ceilf(NSHeight(drawableRect) * outputRatio);
		[self setFillRect : NSMakeRect((NSWidth(drawableRect) - usedWidth) * 0.5, 0.0, usedWidth, NSHeight(drawableRect))];
	}

	*/

	[self setFillRect : NSMakeRect(0.0, 0.0, NSWidth(drawableRect), NSHeight(drawableRect))];

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport((GLint)fillRect.origin.x, (GLint)fillRect.origin.y, resScaleX * (GLsizei)fillRect.size.width, resScaleY * (GLsizei)fillRect.size.height);

	[NSOpenGLContext clearCurrentContext];
}


-(CGDirectDisplayID)displayID
{
	return displayID;
}


-(void)lockFocus
{
	NSOpenGLContext* context = [self openGLContext];

	[super lockFocus];

	if ([context view] != self)
	{
		[context setView : self];
	}

	[context makeCurrentContext];
}


-(void)viewDidMoveToWindow
{
	[super viewDidMoveToWindow];

	if ([self window] == nil)
	{
		[_openGLContext clearDrawable];
	}
	else
	{
		[self updateDisplayID];
	}
}


-(CDTWindowPlatform*)windowPlatform
{
	return windowPlatform;
}


-(void)setFillRect : (NSRect)newFillRect
{
	fillRect = newFillRect;
}


-(NSRect)fillRect
{
	return fillRect;
}


-(void)setResolution : (bool)best
{
	resScaleX = 1;
	resScaleY = 1;

#if DEF_SUPPORT_RETINA_RESOLUTION
	if (!best) return;
	[self setWantsBestResolutionOpenGLSurface : YES];
	NSRect in_points = NSMakeRect(0, 0, 1024, 1024);
	NSRect out_pixels = [self convertRectToBacking : in_points];
	resScaleX = (DT_SLONG)NSWidth(out_pixels); resScaleX /= 1024;
	resScaleY = (DT_SLONG)NSHeight(out_pixels); resScaleY /= 1024;
#else
	(void)best;
#endif
}


-(void)killTimer
{
	if (myTimer) [myTimer invalidate];
	myTimer = NULL;
}


-(void)setTimer : (DT_FLOAT)timeVal
{
	if (myTimer) [myTimer invalidate];
	myTimer = [NSTimer scheduledTimerWithTimeInterval : timeVal target : self selector : @selector(fireTimer) userInfo : nil repeats : YES];
}


-(void)fireTimer
{
	if (windowPlatform) windowPlatform->onPeriodic();
}


-(DT_SLONG)resScaleX
{
	return resScaleX;
}


-(DT_SLONG)resScaleY
{
	return resScaleY;
}


-(void)menuCommand : (id)sender
{
	DT_SLONG menuCommandCode = (DT_SLONG)[sender tag];
	if (!windowPlatform) return;
	if (windowPlatform->IsDisabled) return;
	windowPlatform->onMenuCommand(menuCommandCode, 0);
}

// resize window using a factor taken from the tag of the menu item sending this message


-(void)zoomX : (id)sender
{
	PARAMETER_UNUSED(sender)

#if DEF_CONFIG_ALLOW_ZOOM
	NSWindow* window = [self window];
	DT_SLONG zoomFactor = (DT_SLONG)[sender tag];

	if (windowPlatform == nil || window == nil) return;

	NSRect contentRect = [window contentRectForFrameRect : [window frame]];
	NSRect newContentRect = contentRect;

	newContentRect.size.width = windowPlatform->Width * zoomFactor;
	newContentRect.size.height = windowPlatform->Height * zoomFactor;

	// fix the y origin (else the window will keep it's *lower* right corner)
	newContentRect.origin.y -= (newContentRect.size.height - contentRect.size.height);

	[window setFrame : [window frameRectForContentRect : newContentRect] display : YES];
	[self reshape];
#endif
}


-(void)hideCursor
{
	// the parameter to CGDisplayHideCursor is unused, so we just pass main display
	CGDisplayHideCursor(kCGDirectMainDisplay);
	CGAssociateMouseAndMouseCursorPosition(false);

	// position the mouse cursor in the middle of the screen
	size_t displayWidth = CGDisplayPixelsWide(displayID);
	size_t displayHeight = CGDisplayPixelsHigh(displayID);
	CGWarpMouseCursorPosition(CGPointMake(displayWidth / 2, displayHeight / 2));
}


-(void)unhideCursor
{
	// the parameter to CGDisplayShowCursor is unused, so we just pass main display
	CGDisplayShowCursor(kCGDirectMainDisplay);
	CGAssociateMouseAndMouseCursorPosition(true);
}


-(void)fadeOutDisplay
{
#if !DEF_CONFIG_DEBUG
	const DT_FLOAT FADE_INTERVAL = (1.0 / (DT_FLOAT)WINDOW_FADE_STEPS);
	const useconds_t SLEEP_TIME = (useconds_t)(1000000 * (WINDOW_FADE_TIME / (DT_FLOAT)WINDOW_FADE_STEPS));

	CGGammaValue fade;
	CGGetDisplayTransferByFormula(displayID, &redMin, &redMax, &redGamma, &greenMin, &greenMax, &greenGamma, &blueMin, &blueMax, &blueGamma);
	for (DT_SLONG step = 0; step < WINDOW_FADE_STEPS; ++step)
	{
		fade = 1.0 - (step * FADE_INTERVAL);
		CGSetDisplayTransferByFormula(displayID, redMin, fade * redMax, redGamma, greenMin, fade * greenMax, greenGamma, blueMin, fade * blueMax, blueGamma);
		usleep(SLEEP_TIME);
	}
#endif
}


-(void)fadeInDisplay
{
#if !DEF_CONFIG_DEBUG
	const DT_FLOAT FADE_INTERVAL = (1.0 / (DT_FLOAT)WINDOW_FADE_STEPS);
	const useconds_t SLEEP_TIME = (useconds_t)(1000000 * (WINDOW_FADE_TIME / (DT_FLOAT)WINDOW_FADE_STEPS));

	CGGammaValue fade;
	for (DT_SLONG step = 0; step < WINDOW_FADE_STEPS; ++step)
	{
		fade = step * FADE_INTERVAL;
		CGSetDisplayTransferByFormula(displayID, redMin, fade * redMax, redGamma, greenMin, fade * greenMax, greenGamma, blueMin, fade * blueMax, blueGamma);
		usleep(SLEEP_TIME);
	}
#endif
	CGDisplayRestoreColorSyncSettings();
}


-(void)clear
{
	//glClearColor(0, 0, 0, 0);
	glClearColor(255, 255, 255, 255);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}


/*
-(void)blitOpenGLTexture
{
	if (glTexture == 0) return;

	const DT_SLONG width = windowPlatform->Width;
	const DT_SLONG height = windowPlatform->Height;

	[[self openGLContext] makeCurrentContext];

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);          glVertex2f(-1.0, 1.0);
	glTexCoord2i(width, 0);      glVertex2f(1.0, 1.0);
	glTexCoord2i(width, height); glVertex2f(1.0, -1.0);
	glTexCoord2i(0, height);     glVertex2f(-1.0, -1.0);
	glEnd();

	[[self openGLContext] flushBuffer];

	[NSOpenGLContext clearCurrentContext];
}
*/

-(void)copyTrueColorPixelsUsingOpenGL : (const void*)trueColorPixels dirtyRect : (const CDTRectRegion*)rect_region dX : (GLint)dx dY : (GLint)dy
{
	if (trueColorPixels == NULL) return;

	const GLvoid* bufferStart = (const GLvoid*)trueColorPixels;
	DT_SLONG width = windowPlatform->Width;
	DT_SLONG height = windowPlatform->Height;

	[[self openGLContext] makeCurrentContext];

	if (glTexture == 0) [self prepareTexture : bufferStart];

#if DEF_CONFIG_USE_TEXSUBIMAGE

	else
	{
		DT_UBYTE* buffer = (DT_UBYTE*)bufferStart;

		GLint x = 0;
		GLint y = 0;
		GLsizei w = width;
		GLsizei h = height;

		if (rect_region != NULL)
		{
			x = rect_region->X;
			y = rect_region->Y;
			w = rect_region->W;
			h = rect_region->H;
			buffer += (width * y + x) * 4;
		}

		GL_CHECK_ERROR(glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, x, y, w, h, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV, (GLvoid*)buffer));
	}

#else
	(void)rect_region;
#endif

	width += dx; height += dy;

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);          glVertex2f(-1.0, 1.0);
	glTexCoord2i(width, 0);      glVertex2f(1.0, 1.0);
	glTexCoord2i(width, height); glVertex2f(1.0, -1.0);
	glTexCoord2i(0, height);     glVertex2f(-1.0, -1.0);
	glEnd();

	[[self openGLContext] flushBuffer];

#if DEF_CONFIG_USE_TEXSUBIMAGE
#else
	[self releaseTexture];
#endif

	[NSOpenGLContext clearCurrentContext];

	//NSOpenGLContext* context = _openGLContext;
	//GLint swapInterval = DEF_CONFIG_VBSYNC_OFF;
	//[context setValues : &swapInterval forParameter : NSOpenGLCPSwapInterval];
}


-(void)setSwapInterval : (GLint)swapInterval
{
	NSOpenGLContext* context = _openGLContext;
	[context setValues : &swapInterval forParameter : NSOpenGLCPSwapInterval];
}


-(BOOL)isOpaque
{
	return YES;
}


// dummy function to prevent beeps due to unhandled keyDown events

-(void)keyDown : (NSEvent*)theEvent
{
	PARAMETER_UNUSED(theEvent)
}


-(BOOL)acceptsFirstResponder
{
	return YES;
}


-(void)windowDidResignKey : (NSNotification*)notification
{
	PARAMETER_UNUSED(notification)

	windowPlatform->onActivate(false);
}


-(void)windowDidBecomeKey : (NSNotification*)notification
{
	PARAMETER_UNUSED(notification)

	windowPlatform->onActivate(true);
}


-(void)windowDidResize : (NSNotification*)notification
{
	if (!windowPlatform->IsOpen) return; /* ignore this event until the window is fully built */

	PARAMETER_UNUSED(notification)

	//[self reshape];
	//[self blitOpenGLTexture];

	windowPlatform->DiffX = windowPlatform->DiffY = 0;

	[self reshape];
	[self releaseTexture];

	NSRect drawableRect = [self convertRect : [self bounds] toView : nil];
	windowPlatform->Width = resScaleX * NSWidth(drawableRect);
	windowPlatform->Height = resScaleY * NSHeight(drawableRect);
	windowPlatform->onResize(windowPlatform->Width, windowPlatform->Height);
}


//-(BOOL)windowShouldClose : (NSNotification*)notification
//{
//	PARAMETER_UNUSED(notification)
//
//	return YES;
//}


-(void)windowWillClose : (NSNotification*)notification
{
	PARAMETER_UNUSED(notification)

	if (!windowPlatform) return;

	// this needs to go first, as defaults() is called, which resets ShouldClose
	windowPlatform->shutdown();
	windowPlatform->ShouldClose = true;
}

@end




#pragma mark -
#pragma mark C++ part
#pragma mark -


// Small helper class so we do not need to keep track of the NSAutoreleasePools

class CDTWindowPlatform_AutoreleasePoolAutoPtr
{

public:

	CDTWindowPlatform_AutoreleasePoolAutoPtr() { pool = [[NSAutoreleasePool alloc] init]; }
	~CDTWindowPlatform_AutoreleasePoolAutoPtr() { [pool release]; }

private:
	NSAutoreleasePool* pool;
};

#pragma mark -


// If NSApp is not set (which will be the case for most apps), we need to create one and make it the foreground application.

void SetupApplication(const DT_STREAM_DESC* sd_menu)
{
	if (NSApp != NULL) return;

	CDTWindowPlatform_AutoreleasePoolAutoPtr pool;

	// initialize application (also sets NSApp)
	CDTWindowApplication* app = (CDTWindowApplication*)[CDTWindowApplication sharedApplication];

	// make our process a foreground application, so we get a dock icon and a menu bar
	ProcessSerialNumber psn = {0, kCurrentProcess};
	TransformProcessType(&psn, kProcessTransformToForegroundApplication);

	[app setupApplicationMenu : sd_menu];

	[NSApp activateIgnoringOtherApps : YES];
}



#pragma mark -

class CDTWindowPlatform::CDTWindowPlatformPrivate
{

public:

	CDTWindowPlatformPrivate(CDTWindowPlatform* theWindow) : window(0), view(0), windowPlatform(theWindow)
	{
		ctrlWasPressed = false;
		hasPreciseDeltas = false;

		//window_modifiers=0;

		DT_SLONG major, minor, bugfix;
		GetSystemVersion(&major, &minor, &bugfix);
		if (major >= 10 && minor >= 7) hasPreciseDeltas = true;

		//printf("GetSystemVersion major=%d, minor=%d, bugfix=%d\n", major, minor, bugfix);
	}


	void closeWindow()
	{
		// Close old window if it exists. This will also release it since the view is retained, it will not be deallocated
		[window close];
		window = nil;
	}


#pragma mark -
	// -------------------------
#pragma mark Mouse Event Handling
	// -------------------------


	bool updateMouseCoordsFromEvent(NSEvent* theEvent)
	{
		NSRect fill_rect = [view fillRect];
		DT_FLOAT ratio_w = windowPlatform->Width; ratio_w /= NSWidth(fill_rect);
		DT_FLOAT ratio_h = windowPlatform->Height; ratio_h /= NSHeight(fill_rect);

		NSRect frame = [view frame];
		NSPoint event_location = [theEvent locationInWindow];

		// flip y value, as Mac OS X counts from bottom to top, while dtwindow counts from top to bottom
		event_location.y = NSHeight(frame) - event_location.y;

		// transform coordinates to native display
		mouse.x = ratio_w * (event_location.x - fill_rect.origin.x);
		mouse.y = ratio_h * (event_location.y - fill_rect.origin.y);

		DT_SLONG kx = [view resScaleX];
		DT_SLONG ky = [view resScaleY];

		if (mouse.x < 0 || mouse.x > kx * NSWidth(fill_rect)) return false;
		if (mouse.y < 0 || mouse.y > ky * NSHeight(fill_rect)) return false;
		return true;
	}


	void handleMouseDown(NSEvent* theEvent, DT_SLONG button)
	{
		mouse.buttons.left = mouse.buttons.right = mouse.buttons.middle = false;

		if (!updateMouseCoordsFromEvent(theEvent)) return; /* don't produce onMouseButtonDown if mouse is outside of window */

		switch (button)
		{
		case 1:
			ctrlWasPressed = (([theEvent modifierFlags] & NSControlKeyMask) != 0);
			if (ctrlWasPressed) mouse.buttons.right = true;
			else mouse.buttons.left = true;
			break;

		case 2:
			mouse.buttons.right = true;
			break;

			//case 3:
			//default:
			//	mouse.buttons.middle = true;
			//	break;
		}

		if (mouse.buttons.left == mouse.buttons.right == mouse.buttons.middle == false) return; /* don't produce onMouseButtonDown if no button was pressed */
		windowPlatform->onMouseButtonDown((mouse.buttons.right ? 1 : 0), mouse.x, mouse.y);
	}


	void handleMouseUp(NSEvent* theEvent, DT_SLONG button)
	{
		if (mouse.buttons.left == mouse.buttons.right == mouse.buttons.middle == false) return; /* don't produce onMouseButtonUp without a prior onMouseButtonDown */

		mouse.buttons.left = mouse.buttons.right = mouse.buttons.middle = false;

		updateMouseCoordsFromEvent(theEvent);

		switch (button)
		{
		case 1:
			if (ctrlWasPressed) mouse.buttons.right = true /*false*/;
			else mouse.buttons.left = true /*false*/;
			break;

		case 2:
			mouse.buttons.right = true; //false;
			break;

			//case 3:
			//default:
			//	mouse.buttons.middle = true; //false;
			//	break;
		}

		windowPlatform->onMouseButtonUp((mouse.buttons.right ? 1 : 0), mouse.x, mouse.y);
		mouse.buttons.left = mouse.buttons.right = mouse.buttons.middle = false;
	}


	void handleMouseMoved(NSEvent* theEvent)
	{
		DT_FLOAT x = mouse.x;
		DT_FLOAT y = mouse.y;
		if ((!updateMouseCoordsFromEvent(theEvent)) && (mouse.buttons.left == mouse.buttons.right == mouse.buttons.middle == false)) return; /* don't produce onMouseMove if mouse is outside of window and there was no prior onMouseButtonDown */
		if (x < mouse.x || x > mouse.x || y < mouse.y || y > mouse.y) windowPlatform->onMouseMove(mouse.x, mouse.y); /* produce onMouseMove only if mouse was moved */
	}


	void handleMouseScroll(NSEvent* theEvent)
	{
		DT_FLOAT k = 120;
		DT_FLOAT dx = [theEvent deltaX];
		DT_FLOAT dy = [theEvent deltaY];

		if (hasPreciseDeltas)
		{
			dx = [theEvent scrollingDeltaX];
			dy = [theEvent scrollingDeltaY];
			if ([theEvent hasPreciseScrollingDeltas]) k = 1;
			//if ([theEvent isDirectionInvertedFromDevice]) k = -k;
		}

		//printf("hasPreciseDeltas = %d, k = %f\n", hasPreciseDeltas, k);

		DT_SLONG sdx = (DT_SLONG)(k * dx);
		DT_SLONG sdy = (DT_SLONG)(k * dy);

		if (sdx == 0 && sdy == 0) return;

		updateMouseCoordsFromEvent(theEvent);
		windowPlatform->onMouseScroll(mouse.x, mouse.y, sdx, sdy);
	}


#pragma mark -
	// -----------------------------
#pragma mark Keyboard Event Handling
	// -----------------------------


	/*
		Translates key event to CDTKey value.
		I decided to map alphabetical keys using their "string", which should help with international
		keymaps, but map special CDTKey values to key codes. The X11 implementation uses the X11
		keymap, which unfortunately leads to lots of unmapped/dead keys and unreachable codes.
	*/

	static CDTKey keyFromNSEvent(NSEvent* theEvent, DT_ULONG* unicode_char)
	{
		*unicode_char = 0;
		NSString* keyStringUnicode = [theEvent characters];
		if ([keyStringUnicode length] > 0)
		{
			unichar	ch_unicode;
			[keyStringUnicode getCharacters : &ch_unicode range : NSMakeRange(0, 1)];
			*unicode_char = ch_unicode;
		}

		// NOTE: charactersIgnoringModifiers does not ignore "shift"

		NSString* keyString = [theEvent charactersIgnoringModifiers];
		DT_UWORD keyCode = [theEvent keyCode];
		CDTKey key;

#if 0 // DEBUG

		if ([keyString length] > 0)
		{
			unichar	ch2;
			[keyString getCharacters : &ch2 range : NSMakeRange(0, 1)];
			NSLog(@"key down=%d/0x%x/%c\n", keyCode, keyCode, keyCode);
			NSLog(@"ignoringModifiers='%@' char bytes=0x%x\n", keyString, ch2);
		}

#endif

		// translate some fixed keys
		switch (keyCode)
		{
		case 0x0A: return CDTKey::BackQuote; // first key, second row (^ and � on german keyboard)
		case 0x18: return CDTKey::Equals; // key next to backspace
		case 0x1B: return CDTKey::Separator; // key next to "0" (�/? on german keyboard)
		case 0x21: return CDTKey::OpenBracket; // key next to "P" (� on german keyboard)
		case 0x1e: return CDTKey::CloseBracket; // key next to OpenBracket ("+" on german keyboard)
		case 0x27: return CDTKey::Quote; // key next to Enter (next to � on german keyboard -> �)
		case 0x29: return CDTKey::SemiColon; // key next to "L" (� on german keyboard)
		case 0x2A: return CDTKey::BackSlash; // key next to CloseBracket (# on germany keyboard, one row below)

		case 0x1D: return CDTKey::Zero;
		case 0x12: return CDTKey::One;
		case 0x13: return CDTKey::Two;
		case 0x14: return CDTKey::Three;
		case 0x15: return CDTKey::Four;
		case 0x17: return CDTKey::Five;
		case 0x16: return CDTKey::Six;
		case 0x1A: return CDTKey::Seven;
		case 0x1C: return CDTKey::Eight;
		case 0x19: return CDTKey::Nine;

		case 0x2B: return CDTKey::Comma; // last three keys above space, next to right shift
		case 0x2F: return CDTKey::Period;
		case 0x2C: return CDTKey::Slash;

		case 0x24: return CDTKey::Enter;
		case 0x30: return CDTKey::Tab;
		case 0x31: return CDTKey::Space;
		case 0x33: return CDTKey::BackSpace;
		case 0x35: return CDTKey::Escape;

		case 0x41: return CDTKey::Decimal; // NumPad "."
		case 0x43: return CDTKey::Multiply; // NumPad "*"
		case 0x45: return CDTKey::Add; // NumPad "+"
		case 0x4B: return CDTKey::Divide; // NumPad "/"
		case 0x4C: return CDTKey::Enter; // NumPad Enter
		case 0x4E: return CDTKey::Subtract; // NumPad "-"

		case 0x51: return CDTKey::Equals; // NumPad "="
		case 0x52: return CDTKey::NumPad0;
		case 0x53: return CDTKey::NumPad1;
		case 0x54: return CDTKey::NumPad2;
		case 0x55: return CDTKey::NumPad3;
		case 0x56: return CDTKey::NumPad4;
		case 0x57: return CDTKey::NumPad5;
		case 0x58: return CDTKey::NumPad6;
		case 0x59: return CDTKey::NumPad7;
		case 0x5B: return CDTKey::NumPad8;
		case 0x5C: return CDTKey::NumPad9;

#if 0
		/*
			TODO:

			These codes cannot be mapped?
				Clear clear key
				Insert insert key
				Cancel cancel key
				Final final key
				Convert convert key
				NonConvert non convert key
				Accept accept key
				ModeChange mode change key NSModeSwitchFunctionKey ?

			Undefined on german keyboards:
				"<"/">" first key, last row, next to left shift

			Undefined on Mac Keyboards:
				NumPad "=" (aliased for now to Equals which matches for US keyboards, but not for international ones)
				fn (NSFunctionKeyMask?)
		*/
#endif

		default: break;
		}

		// if we cannot get the unicode character either, bail so we do not get an exception on -getCharacters
		if ([keyString length] == 0) return CDTKey::Undefined;

		unichar	ch;
		[keyString getCharacters : &ch range : NSMakeRange(0, 1)];

		if (ch <= 0x00FF)
		{
			// Normal keymap
			DT_UBYTE ascii = ch;

			// catch non-capital letters
			if ((ascii >= 'a') && (ascii <= 'z')) return (CDTKey::Code)(ascii - 0x20);
			if ((ascii >= 'A') && (ascii <= 'Z')) return (CDTKey::Code)ascii;
		}
		else
		{
			// Function keys
			switch (ch)
			{
			case NSF1FunctionKey: return CDTKey::F1;
			case NSF2FunctionKey: return CDTKey::F2;
			case NSF3FunctionKey: return CDTKey::F3;
			case NSF4FunctionKey: return CDTKey::F4;
			case NSF5FunctionKey: return CDTKey::F5;
			case NSF6FunctionKey: return CDTKey::F6;
			case NSF7FunctionKey: return CDTKey::F7;
			case NSF8FunctionKey: return CDTKey::F8;
			case NSF9FunctionKey: return CDTKey::F9;
			case NSF10FunctionKey: return CDTKey::F10;
			case NSF11FunctionKey: return CDTKey::F11;
			case NSF12FunctionKey: return CDTKey::F12;

			case NSDeleteFunctionKey: return CDTKey::Delete;
			case NSPageUpFunctionKey: return CDTKey::PageUp;
			case NSPageDownFunctionKey: return CDTKey::PageDown;
			case NSEndFunctionKey: return CDTKey::End;
			case NSHomeFunctionKey: return CDTKey::Home;

				// NumLock key is "clear line" on Mac
			case NSClearLineFunctionKey: return CDTKey::NumLock;
			case NSScrollLockFunctionKey: return CDTKey::ScrollLock;
			case NSPauseFunctionKey: return CDTKey::Pause;

			case NSPrintScreenFunctionKey:return CDTKey::PrintScreen;
			case NSUpArrowFunctionKey: return CDTKey::Up;
			case NSDownArrowFunctionKey: return CDTKey::Down;
			case NSLeftArrowFunctionKey: return CDTKey::Left;
			case NSRightArrowFunctionKey: return CDTKey::Right;

			case NSHelpFunctionKey: return CDTKey::Help;

			default: break;
			}
		}

#if 0 // DEBUG
		NSLog(@"undefined key:\n");
		NSLog(@"key down=%d/0x%x/%c\n", keyCode, keyCode, keyCode);
		NSLog(@"ignoringModifiers='%@' char bytes=0x%x\n", keyString, ch);
#endif

		return CDTKey::Undefined;
	}


	void handleKeyDown(NSEvent* theEvent)
	{
		DT_ULONG unicode_char = 0;
		//bool defaultKeyHandlers = true;
		CDTKey key = keyFromNSEvent(theEvent, &unicode_char);

		//if (![theEvent isARepeat])
		windowPlatform->onKeyDown(key);
		//windowPlatform->onKeyPressed(key);

		if (unicode_char > 0) windowPlatform->onCharDown(unicode_char);
		//printf("handleKeyDown: unicode_char=%ld\n", unicode_char);

		//if (defaultKeyHandlers)
		//{
		[[NSApp mainMenu] performKeyEquivalent : theEvent];

		if (key == CDTKey::Escape /*&& window_modifiers==0*/)
		{
			[window performClose : nil];
		}
		//}
	}


	void handleKeyUp(NSEvent* theEvent)
	{
		DT_ULONG unicode_char = 0;
		CDTKey key = keyFromNSEvent(theEvent, &unicode_char);
		windowPlatform->onKeyUp(key);

		if (unicode_char > 0) windowPlatform->onCharUp(unicode_char);
		//printf("handleKeyUp: unicode_char=%ld\n", unicode_char);
	}


	// modifier status changed

	void handleFlagsChanged(NSEvent* theEvent)
	{
		DT_ULONG newModifiers = ([theEvent modifierFlags] & NSDeviceIndependentModifierFlagsMask);
		DT_ULONG changedModifiers = newModifiers ^ WINDOW_MODIFIERS;
		WINDOW_MODIFIERS = newModifiers;

		if ((changedModifiers & NSAlphaShiftKeyMask) != 0)
		{
			if ((newModifiers & NSAlphaShiftKeyMask) != 0) windowPlatform->onKeyDown(CDTKey::CapsLock);
			else windowPlatform->onKeyUp(CDTKey::CapsLock);
		}

		if ((changedModifiers & NSCommandKeyMask) != 0)
		{
			if ((newModifiers & NSCommandKeyMask) != 0) windowPlatform->onKeyDown(CDTKey::Meta);
			else windowPlatform->onKeyUp(CDTKey::Meta);
		}

		if ((changedModifiers & NSShiftKeyMask) != 0)
		{
			if ((newModifiers & NSShiftKeyMask) != 0) windowPlatform->onKeyDown(CDTKey::Shift);
			else windowPlatform->onKeyUp(CDTKey::Shift);
		}

		if ((changedModifiers & NSControlKeyMask) != 0)
		{
			if ((newModifiers & NSControlKeyMask) != 0) windowPlatform->onKeyDown(CDTKey::Control);
			else windowPlatform->onKeyUp(CDTKey::Control);
		}

		if ((changedModifiers & NSAlternateKeyMask) != 0)
		{
			if ((newModifiers & NSAlternateKeyMask) != 0) windowPlatform->onKeyDown(CDTKey::Alt);
			else windowPlatform->onKeyUp(CDTKey::Alt);
		}

		// TODO: NSFunctionKeyMask
	}


	static void handleEvent(NSEvent* event)
	{
		NSWindow* eventWindow = [event window];
		CDTWindowPlatformPrivate* receiver = NULL;

		// if this event was aimed at a window, try to find the corresponding receiver
		if (eventWindow != 0)
		{
			// the view may be nil in fullscreen mode, but the delegate is set even in this case
			CDTWindowView* eventDelegate = (CDTWindowView*)[eventWindow delegate];
			if (eventDelegate && [eventDelegate isKindOfClass : [CDTWindowView class]])
			{
				CDTWindowPlatform* tmpEventWindow = [eventDelegate windowPlatform];
				receiver = tmpEventWindow->Priv;
			}
		}

		if (receiver == NULL || receiver->windowPlatform->IsDisabled)
		{
			[NSApp sendEvent : event];
			return;
		}

		NSEventType eventType = [event type];

		switch (eventType)
		{
		case NSLeftMouseDown: receiver->handleMouseDown(event, 1);
			// delegate event to app so activation events get handled
			[NSApp sendEvent : event];
			break;

		case NSLeftMouseUp: receiver->handleMouseUp(event, 1);
			[NSApp sendEvent : event];
			break;

		case NSRightMouseDown: receiver->handleMouseDown(event, 2);
			break;
		case NSRightMouseUp: receiver->handleMouseUp(event, 2);
			break;
		case NSOtherMouseDown: receiver->handleMouseDown(event, [event buttonNumber]);
			break;
		case NSOtherMouseUp: receiver->handleMouseUp(event, [event buttonNumber]);
			break;

		case NSLeftMouseDragged:
		case NSRightMouseDragged:
		case NSOtherMouseDragged:
		case NSMouseMoved: receiver->handleMouseMoved(event);
			break;

		case NSKeyDown: receiver->handleKeyDown(event);
			break;
		case NSKeyUp: receiver->handleKeyUp(event);
			break;
		case NSFlagsChanged: receiver->handleFlagsChanged(event);
			break;
		case NSScrollWheel: receiver->handleMouseScroll(event);
			break;
		default: [NSApp sendEvent : event];
			break;
		}
	}


	// handle the event message queue

	static void pumpMessages(bool block)
	{
		CDTWindowPlatform_AutoreleasePoolAutoPtr pool;
		NSEvent* event;
		NSDate* theNextDate = (block ? [NSDate dateWithTimeIntervalSinceNow : 0.2] /* block for 0.2 seconds */ : [NSDate distantPast] /* do not block */);

		while (nil != (event = [NSApp nextEventMatchingMask : NSAnyEventMask untilDate : theNextDate inMode : NSDefaultRunLoopMode dequeue : YES]))
		{
			handleEvent(event);
		}
	};

	// current mouse status
	CDTMouse mouse;

	// mouse wheel precise deltas (OS X 10.7 and higher)
	bool hasPreciseDeltas;

	// on mouseDown CTRL was pressed, so we sent a onRightMouse and need to reset rightButton on mouseUp, no matter what the current modifiers say
	bool ctrlWasPressed;

	//DT_ULONG window_modifiers;

	// output window
	//NSWindow* window;
	CDTCustomNSWindow* window;

	// custom content view
	CDTWindowView* view;

	// position and size of window
	//NSRect windowFrame;

	CDTWindowPlatform* windowPlatform;
};


#pragma mark -


DT_FLOAT CDTDisplayPlatform::GetScale() const
{
	//return 1.0;

	DT_FLOAT display_scale = [[NSScreen mainScreen] backingScaleFactor];
	//printf("display_scale = %f\n", display_scale);
	return display_scale;
}


DT_SLONG CDTDisplayPlatform::Dist(DT_SLONG d) const
{
	return (DT_SLONG)(d * GetScale() + 0.5);
}


CDTWindowPlatform::CDTWindowPlatform(bool /*flags*/, CDTDisplayPlatform* display)
{
	CDTWindowPlatform_AutoreleasePoolAutoPtr pool;

	defaults();

	Display = display;

	Priv = NULL; //new CDTWindowPlatformPrivate(this);
}


CDTWindowPlatform::CDTWindowPlatform(CDTDisplayPlatform* display, const DT_STREAM_DESC* sd_menu)
{
	CDTWindowPlatform_AutoreleasePoolAutoPtr pool;

	defaults();

	Display = display;

	Priv = new CDTWindowPlatformPrivate(this);

	if (NSApp == nil) SetupApplication(sd_menu);

	// create content view, it is resized to the correct values later
	NSRect viewRect = NSMakeRect(0.0, 0.0, 50.0, 50.0);
	NSOpenGLPixelFormat* format = [CDTWindowView defaultPixelFormatForDisplay : CGMainDisplayID()];
	Priv->view = [[CDTWindowView alloc] initWithFrame : viewRect pixelFormat : format windowPlatform : this];
	[Priv->view setAutoresizingMask : NSViewWidthSizable | NSViewHeightSizable];
}


CDTWindowPlatform::~CDTWindowPlatform()
{
	CDTWindowPlatform_AutoreleasePoolAutoPtr pool;

	close();

	if (!Priv) return;

	[Priv->view release];

	delete Priv;
}


bool CDTWindowPlatform::open(const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG width, DT_SLONG height, DT_ULONG flags)
{
	CDTWindowPlatform_AutoreleasePoolAutoPtr pool;

	if (IsOpen) return 0; /* we can't open twice! */
	if (!Priv) return false;

	bool best = false; if (flags & DV_WINDOW_HINT_FULLRESOLUTION) best = true;
	DT_FLOAT time_val = 0.04;

	[Priv->view setResolution : best];

	DT_SLONG kx = [Priv->view resScaleX];
	DT_SLONG ky = [Priv->view resScaleY];

	NSScreen* mainScreen = [NSScreen mainScreen];
	NSRect screenRect = [mainScreen frame];

	if (flags & DV_WINDOW_HINT_FULLSCREEN) { width = kx * NSWidth(screenRect); height = ky * NSHeight(screenRect); }

	// not using "output" in the next call to suppress unwanted fade operations
	// adapter's open(title, x, y, width, height, flags);
	Width = width;
	Height = height;
	DiffX = DiffY = 0;
	Flags = flags;
	ShouldClose = false;

	// clamp cascading windows to screen size
	if (kx * WINDOW_X + width > kx * NSWidth(screenRect)) WINDOW_X = WINDOW_START_X;
	if (ky * WINDOW_Y + height > ky * NSHeight(screenRect)) WINDOW_Y = WINDOW_START_Y;

	DT_SLONG x0 = kx * WINDOW_X;
	DT_SLONG y0 = ky * NSHeight(screenRect) - height - ky * WINDOW_Y;

	if (flags & DV_WINDOW_HINT_CENTER)
	{
		x0 = (kx * NSWidth(screenRect) - width) / 2;
		y0 = (ky * NSHeight(screenRect) - height) / 2;
	}
	else if (flags & DV_WINDOW_HINT_FULLSCREEN)
	{
		x0 = 0;
		y0 = 0;
	}
	else
	{
		WINDOW_X += WINDOW_OFFSET_X;
		WINDOW_Y += WINDOW_OFFSET_Y;
	}

	if (x >= 0) x0 = kx * x;
	if (y >= 0) y0 = ky * NSHeight(screenRect) - height - ky * y;

	if (!windowed(title, x0, y0)) return false;

	// we have a window!

	Priv->window->IsEnabled = YES;

	[Priv->view setTimer : time_val];

	//Priv->pumpMessages(true); /* Fix for 10.14 Mojave - without it the initial window might be blank (until it's refreshed or resized) */
	CDTWindowPlatformPrivate::pumpMessages(true); /* Fix for 10.14 Mojave - without it the initial window might be blank (until it's refreshed or resized) */

	//WindowAdapter::onOpen();

	IsOpen = true;

	if (Flags & DV_WINDOW_HINT_RESIZE)
	{
		Priv->window.styleMask |= NSResizableWindowMask;
		//Priv->window.styleMask |= NSWindowStyleMaskResizable;
	}

	return true;
}


// called when the window is closed by user interaction

void CDTWindowPlatform::shutdown()
{
	if (!Priv) return;

	[Priv->view killTimer];
	[Priv->view clearGLContext];

	// window and view will be autoreleased when closed, just clear pointers
	Priv->window = nil;

	close();
}


void CDTWindowPlatform::close()
{
	if (!Priv) return;

	CDTWindowPlatform_AutoreleasePoolAutoPtr pool;

	[Priv->view clearGLContext];
	Priv->closeWindow();

	[Priv->view unhideCursor];

	CGDisplayRestoreColorSyncSettings();

	// adapter's defaults();
	Width = Height = 0;
	DiffX = DiffY = 0;
	Flags = 0;
	IsOpen = false;

	//Display->WindowUnregsiter(this);
}


void CDTWindowPlatform::eventLoop()
{
	//Priv->pumpMessages(true);
	CDTWindowPlatformPrivate::pumpMessages(true);
}


DT_SLONG CDTWindowPlatform::modify(DT_ULONG flags)
{
	if (!Priv) return 0;

	if (!IsOpen) return 0;
	if (flags == 0) return 1;

	else if (flags == DV_WINDOW_ATTRIB_TOP) { [Priv->window makeKeyAndOrderFront : NSApp]; /*[Priv->window orderFront : NSApp];*/ return 1; } /* TOP */
	else if (flags == DV_WINDOW_ATTRIB_BOTTOM) { [Priv->window orderBack : NSApp]; return 1; } /* BOTTOM */
	else if (flags == DV_WINDOW_ATTRIB_TOPMOST) { [Priv->window setLevel : NSModalPanelWindowLevel]; return 1; } /* TOPMOST */
	else if (flags == DV_WINDOW_ATTRIB_DISABLE) { IsDisabled = true; Priv->window->IsEnabled = NO; [Priv->window setLevel : NSNormalWindowLevel]; [Priv->view setAlphaValue : 0.8]; /* [Priv->window setAlphaValue : 0.95]; [Priv->window setIgnoresMouseEvents : YES]; */ return 1; } /* Disable */
	else if (flags == DV_WINDOW_ATTRIB_ENABLE) { IsDisabled = false; Priv->window->IsEnabled = YES; [Priv->window setLevel : NSNormalWindowLevel]; [Priv->view setAlphaValue : 1.0]; /* [Priv->window setAlphaValue : 1.00]; [Priv->window setIgnoresMouseEvents : NO]; */ return 1; } /* Enable */
	else if (flags == DV_WINDOW_ATTRIB_VBSYNC_OFF) { [Priv->view setSwapInterval : DEF_CONFIG_VBSYNC_OFF]; return 1; }
	else if (flags == DV_WINDOW_ATTRIB_VBSYNC_ON) { [Priv->view setSwapInterval : DEF_CONFIG_VBSYNC_ON]; return 1; }

	return 0;
}


void CDTWindowPlatform::modify_timer(DT_SLONG value)
{
	if (!Priv) return;

	DT_FLOAT time_val = value * 0.000001;
	[Priv->view setTimer : time_val];
}


void CDTWindowPlatform::modify_menu_item(DT_SLONG menu_item, DT_SLONG flag, bool is_set)
{
	if (!(Flags & DV_WINDOW_HINT_SYSMENU)) return; /* ignore menu modifications if this window is not associated with one */

	CDTWindowPlatform_AutoreleasePoolAutoPtr pool;

	[/*(CDTWindowApplication*)*/NSApp modifyMenuItem : menu_item modifyFlag : flag isItemSet : (is_set ? YES : NO) currMenu : [NSApp mainMenu]];
}


DT_SLONG CDTWindowPlatform::dialog_fileselect(DT_SLONG save_flag, DT_SLONG config_flags, DT_SLONG selected_filter, const DT_CHAR* title_bar, const DT_CHAR* /*file_types*/, DT_CHAR* default_filename, DT_SLONG max_length)
{
	PARAMETER_UNUSED(config_flags);
	PARAMETER_UNUSED(selected_filter);
	PARAMETER_UNUSED(title_bar);

	DT_SLONG l, ret = -1;
	DT_ULONG i;
	const DT_CHAR* s;

	if (max_length > 4096) goto end;
	if (save_flag) goto save;

//open:

	{
		// Create the File Open Dialog class.
		NSOpenPanel* openDlg = [NSOpenPanel openPanel];

		// Enable the selection of files in the dialog.
		[openDlg setCanChooseFiles : YES];

		// Enable the selection of directories in the dialog.
		[openDlg setCanChooseDirectories : NO];

		// Display the dialog. If the OK button was no pressed, exit.
		//if ([openDlg runModalForDirectory : nil file : nil] != NSOKButton) goto end; // deprecated
		if ([openDlg runModal] != NSOKButton) goto end; // use instead

		// Get an array containing the full filenames of all files and directories selected.
		NSArray* files = [openDlg filenames]; // deprecated !?
		//NSArray* files = [openDlg URLs]; // use instead

		// Loop through all the files and process them.
		for (i = 0; i < [files count]; i++)
		{
			NSString* fileName = [files objectAtIndex : i];

			//s = [fileName cStringWithEncoding : [NSString defaultCStringEncoding]];
			//s = [fileName cStringUsingEncoding : ASCIIEncoding];
			s = [fileName UTF8String];
			l = (DT_SLONG)strlen(s);
			if (l >= max_length) goto end;

			strcpy(default_filename, s);
			break; // only get the first file name
		}

		ret = 0;
		goto end; // everything was OK
	}

save:

	{
		// Create the File Save Dialog class.
		NSSavePanel* saveDlg = [NSSavePanel savePanel];

		// Display the dialog. If the OK button was no pressed, exit.
		//if ([saveDlg runModalForDirectory : nil file : nil] != NSOKButton) goto end; // deprecated
		if ([saveDlg runModal] != NSOKButton) goto end; // use instead

		// Get an array containing the full filenames of all files and directories selected.
		NSString* fileName = [saveDlg filename]; // deprecated !?
		//NSString* fileName = [saveDlg URL]; // use instead

		s = [fileName UTF8String];
		l = (DT_SLONG)strlen(s);
		if (l >= max_length) goto end;

		strcpy(default_filename, s);

		ret = 0;
		goto end; // everything was OK
	}

end:

	return ret;
}


bool CDTWindowPlatform::update(DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG event_flag)
{
	return update(CachedBuffer, true, x, y, w, h, event_flag);
}


bool CDTWindowPlatform::update(void* trueColorPixels, DT_ULONG event_flag)
{
	CachedBuffer = NULL;
	bool ret = update(trueColorPixels, false, 0, 0, 0, 0, event_flag);
	if (ret) CachedBuffer = trueColorPixels;
	return ret;
}


bool CDTWindowPlatform::update(void* pixels, bool update_region, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG event_flag)
{
	if (!Priv) return false;

	//if (!pixels) return false;

	CDTWindowPlatform_AutoreleasePoolAutoPtr pool;

	if (ShouldClose || ([Priv->view openGLContext] == nil))
	{
		close();
		return false;
	}

	if (![NSApp isRunning])
	{
		[NSApp finishLaunching];
		[(CDTWindowApplication*)NSApp setIsRunning : YES];
		[NSApp setWindowsNeedUpdate : YES];
	}

	//[Priv->view copyTrueColorPixelsUsingOpenGL : pixels dirtyRect : (_should_toggle ? 0 : rect_region)];

	// If the window is still hidden, now is a good time to make it visible
	if (![Priv->window isVisible])
	{
		[Priv->window makeKeyAndOrderFront : nil];
	}

	if (event_flag & 4) { /* response to resize event */ }

	if (update_region)
	{
		CDTRectRegion rect_region(x, y, w, h);
		[Priv->view copyTrueColorPixelsUsingOpenGL : pixels dirtyRect : &rect_region dX : DiffX dY : DiffY];
	}
	else
	{
		[Priv->view copyTrueColorPixelsUsingOpenGL : pixels dirtyRect : NULL dX : DiffX dY : DiffY];
	}

	if (!(event_flag & 1)) return true;

	//Priv->pumpMessages(false);
	CDTWindowPlatformPrivate::pumpMessages(false);

	return true;
}


void CDTWindowPlatform::set_title(const DT_CHAR* title)
{
	if (!Priv) return;

	//WindowAdapter::set_title(title);

	CDTWindowPlatform_AutoreleasePoolAutoPtr pool;

	[Priv->window setTitle : [NSString stringWithCString : title encoding : NSUTF8StringEncoding]];
}


bool CDTWindowPlatform::windowed(const DT_CHAR* window_title, DT_SLONG x0, DT_SLONG y0)
{
	if (!Priv) return false;

	DT_SLONG kx = [Priv->view resScaleX];
	DT_SLONG ky = [Priv->view resScaleY];

	NSRect window_frame = NSMakeRect(x0 / kx, y0 / ky, Width / kx, Height / ky);

	DiffX = (DT_SLONG)(kx * NSWidth(window_frame)) - Width;
	DiffY = (DT_SLONG)(ky * NSHeight(window_frame)) - Height;
	//printf("DiffX=%ld, DiffY=%ld, Width=%ld, Height=%ld\n", DiffX, DiffY, Width, Height);

	//if (!IsOpen) return false;

	if (Priv->window != 0) return true;

	[Priv->view clearGLContext];

	CDTCustomNSWindow* window = Priv->window;

	//if (NSWidth(windowFrame) <= 0 || NSHeight(windowFrame) <= 0)
	//{
	//	// if we do not yet have a window frame, just make one up.
	//	Priv->windowFrame = NSMakeRect(WINDOW_START_X, WINDOW_START_Y, Width * 0.5, Height * 0.5);
	//}

	DT_ULONG style = NSBorderlessWindowMask; /* 0 */

	if (Flags & DV_WINDOW_HINT_NOFRAME) style = NSClosableWindowMask | NSMiniaturizableWindowMask;
	else if (Flags & DV_WINDOW_HINT_RESIZE) style = /*NSResizableWindowMask |*/ NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
	else style = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;

	if (window == nil)
	{
		window = [[CDTCustomNSWindow alloc] initWithContentRect : window_frame styleMask : style backing : NSBackingStoreBuffered defer : NO];

		//NSString* title = [NSString stringWithCString : WindowAdapter::get_title() encoding : NSUTF8StringEncoding];
		NSString* title = [NSString stringWithCString : window_title encoding : NSUTF8StringEncoding];

		//if (Flags & DV_WINDOW_HINT_NOFRAME) { [window setLevel : NSModalPanelWindowLevel]; }
		[window setIgnoresMouseEvents : NO];
		[window setAcceptsMouseMovedEvents : YES];
		[window setInitialFirstResponder : Priv->view];
		[window setTitle : title];
		[window setReleasedWhenClosed : YES];
		[window setOneShot : NO];
		[window setCanHide : YES];

		if (NSAppKitVersionNumber > 1500) [window setValue : [NSNumber numberWithInt : 2] forKey : @"tabbingMode"]; /* Fix for 10.12 - to remove "Show Tab Bar" from our menu */

		//[window setContentAspectRatio : window_frame.size];
		[window setDelegate : Priv->view];
		Priv->window = window;
	}
	else
	{
		NSRect frameRect = [window frameRectForContentRect : window_frame];
		[window setFrame : frameRect display : NO];
	}

	[window setContentView : Priv->view];

	NSOpenGLContext* context = [Priv->view openGLContext];

	[context setView : Priv->view];

	[Priv->view unhideCursor];

	return true;
}


void CDTWindowPlatform::defaults()
{
	// adapter's defaults();
	CachedBuffer = NULL;
	Width = Height = 0;
	Flags = 0;
	IsOpen = false;

	ShouldClose = false;
	//_should_toggle = false;
	IsDisabled = false;
}

