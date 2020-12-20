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


class CDTMouse
{
public:

	CDTMouse()
	{
		x = y = 0;
		buttons.left = buttons.middle = buttons.right = 0;
	}

	// Indicates which mouse buttons are currently being pressed.

	class CDTButtons
	{
	public:
		bool left;	// true if left button is pressed
		bool middle; // true if middle button is pressed
		bool right; // true if right button is pressed
	};

	CDTButtons buttons; // mouse button state - indicates which mouse buttons are currently pressed.
	DT_FLOAT x; // current mouse cursor x position - standard range is from 0 to display width - 1, from left to right. values outside this range occur when the user drags the mouse outside the display window
	DT_FLOAT y; // current mouse cursor y position - standard range is from 0 to display height - 1, from top to bottom. values outside this range occur when the user drags the mouse outside the display window
};


class CDTKey
{
public:

	// Key code enumeration.
	// You should never need to use this enumeration type directly, just use the enumeration values Key::Enter, Key::Left, Key::Escape etc...

	enum Code
	{
		Enter = '\n', // enter key
		BackSpace = '\b', // backspace key
		Tab = '\t', // tab key
		Cancel = 0x03, // cancel key
		Clear = 0x0C, // clear key
		Shift = 0x10, // shift key
		Control = 0x11, // control key
		Alt = 0x12, // alt key
		Pause = 0x13, // pause key
		CapsLock = 0x14, // capslock key
		Escape = 0x1B, // escape key
		Space = 0x20, // space key
		PageUp = 0x21, // page up key
		PageDown = 0x22, // page down key
		End = 0x23, // end key
		Home = 0x24, // home key
		Left = 0x25, // left key
		Up = 0x26, // up arrow key
		Right = 0x27, // right arrow key
		Down = 0x28, // down arrow key
		Comma = 0x2C, // comma key ','
		Period = 0x2E, // period key '.'
		Slash = 0x2F, // slash key '/'
		Zero = 0x30, // zero key
		One = 0x31, // one key
		Two = 0x32, // two key
		Three = 0x33, // three key
		Four = 0x34, // four key
		Five = 0x35, // five key
		Six = 0x36, // six key
		Seven = 0x37, // seven key
		Eight = 0x38, // eight key
		Nine = 0x39, // nine key
		SemiColon = 0x3B, // semicolon key ';'
		Equals = 0x3D, // equals key '='
		A = 0x41, // a key
		B = 0x42, // b key
		C = 0x43, // c key
		D = 0x44, // d key
		E = 0x45, // e key
		F = 0x46, // f key
		G = 0x47, // g key
		H = 0x48, // h key
		I = 0x49, // i key
		J = 0x4A, // j key
		K = 0x4B, // k key
		L = 0x4C, // l key
		M = 0x4D, // m key
		N = 0x4E, // n key
		O = 0x4F, // o key
		P = 0x50, // p key
		Q = 0x51, // q key
		R = 0x52, // r key
		S = 0x53, // s key
		T = 0x54, // t key
		U = 0x55, // u key
		V = 0x56, // v key
		W = 0x57, // w key
		X = 0x58, // x key
		Y = 0x59, // y key
		Z = 0x5A, // z key
		OpenBracket	= 0x5B, // open bracket key '['
		BackSlash = 0x5C, // back slash key '\'
		CloseBracket = 0x5D, // close bracket key ']'
		NumPad0 = 0x60, // numpad 0 key
		NumPad1 = 0x61, // numpad 1 key
		NumPad2 = 0x62, // numpad 2 key
		NumPad3 = 0x63, // numpad 3 key
		NumPad4 = 0x64, // numpad 4 key
		NumPad5 = 0x65, // numpad 5 key
		NumPad6 = 0x66, // numpad 6 key
		NumPad7 = 0x67, // numpad 7 key
		NumPad8 = 0x68, // numpad 8 key
		NumPad9 = 0x69, // numpad 9 key
		Multiply = 0x6A, // multiply key '*'
		Add = 0x6B, // add key '+'
		Separator = 0x6C, // separator key '-'
		Subtract = 0x6D, // subtract key '-'
		Decimal = 0x6E, // decimal key '.'
		Divide = 0x6F, // divide key '/'
		F1 = 0x70, // F1 key
		F2 = 0x71, // F2 key
		F3 = 0x72, // F3 key
		F4 = 0x73, // F4 key
		F5 = 0x74, // F5 key
		F6 = 0x75, // F6 key
		F7 = 0x76, // F7 key
		F8 = 0x77, // F8 key
		F9 = 0x78, // F9 key
		F10 = 0x79, // F10 key
		F11 = 0x7A, // F11 key
		F12 = 0x7B, // F12 key
		Delete = 0x7F, // delete key
		NumLock = 0x90, // numlock key
		ScrollLock = 0x91, // scroll lock key
		PrintScreen	= 0x9A, // print screen key
		Insert = 0x9B, // insert key
		Help = 0x9C, // help key
		Meta = 0x9D, // meta key
		BackQuote = 0xC0, // backquote key
		Quote = 0xDE, // quote key
		Final = 0x18, // final key
		Convert = 0x1C, // convert key
		NonConvert = 0x1D, // non convert key
		Accept = 0x1E, // accept key
		ModeChange = 0x1F, // mode change key
		Kana = 0x15, // kana key
		Kanji = 0x19, // kanji key
		Undefined = 0x0 // undefined key
	};

	// The default constructor sets the key code to Undefined.

	CDTKey() { code = Undefined; }

	// Automatically converts from key code enumeration values to key objects.
	// @param code the key code.

	CDTKey(Code c) { this->code = c; }

	// Cast from key object to code.
	// Allows you to treat this class as if it was the code itself.
	// This enables the ==, != operators, and the use of key objects in a switch statement.

	operator Code() const { return code; }

private:

	Code code;
};

