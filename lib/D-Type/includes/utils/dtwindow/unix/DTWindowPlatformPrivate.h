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


class CDTKey
{
public:

	// Key code enumeration.
	// You should never need to use this enumeration type directly, just use the enumeration values CDTKey::Enter, CDTKey::Left, CDTKey::Escape etc...

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
		Undefined = 0x0		// undefined key
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

public:

	Code code;
};


//typedef CDTKey::Code TKeyMap[keyMapSize_];
const DT_SLONG keyMapSize_ = 256;
CDTKey::Code normalKeys_[keyMapSize_];
CDTKey::Code functionKeys_[keyMapSize_];


bool InitializeKeyMaps()
{
	DT_SLONG i;

	for (i = 0; i < keyMapSize_; i++)
	{
		normalKeys_[i] = CDTKey::Undefined;
		functionKeys_[i] = CDTKey::Undefined;
	}

	normalKeys_[XK_space] = CDTKey::Space;
	normalKeys_[XK_comma] = CDTKey::Comma;
	normalKeys_[XK_period] = CDTKey::Period;
	normalKeys_[XK_slash] = CDTKey::Slash;
	normalKeys_[XK_0] = CDTKey::Zero;
	normalKeys_[XK_1] = CDTKey::One;
	normalKeys_[XK_2] = CDTKey::Two;
	normalKeys_[XK_3] = CDTKey::Three;
	normalKeys_[XK_4] = CDTKey::Four;
	normalKeys_[XK_5] = CDTKey::Five;
	normalKeys_[XK_6] = CDTKey::Six;
	normalKeys_[XK_7] = CDTKey::Seven;
	normalKeys_[XK_8] = CDTKey::Eight;
	normalKeys_[XK_9] = CDTKey::Nine;
	normalKeys_[XK_semicolon] = CDTKey::SemiColon;
	normalKeys_[XK_equal] = CDTKey::Equals;
	normalKeys_[XK_a] = CDTKey::A;
	normalKeys_[XK_b] = CDTKey::B;
	normalKeys_[XK_c] = CDTKey::C;
	normalKeys_[XK_d] = CDTKey::D;
	normalKeys_[XK_e] = CDTKey::E;
	normalKeys_[XK_f] = CDTKey::F;
	normalKeys_[XK_g] = CDTKey::G;
	normalKeys_[XK_h] = CDTKey::H;
	normalKeys_[XK_i] = CDTKey::I;
	normalKeys_[XK_j] = CDTKey::J;
	normalKeys_[XK_k] = CDTKey::K;
	normalKeys_[XK_l] = CDTKey::L;
	normalKeys_[XK_m] = CDTKey::M;
	normalKeys_[XK_n] = CDTKey::N;
	normalKeys_[XK_o] = CDTKey::O;
	normalKeys_[XK_p] = CDTKey::P;
	normalKeys_[XK_q] = CDTKey::Q;
	normalKeys_[XK_r] = CDTKey::R;
	normalKeys_[XK_s] = CDTKey::S;
	normalKeys_[XK_t] = CDTKey::T;
	normalKeys_[XK_u] = CDTKey::U;
	normalKeys_[XK_v] = CDTKey::V;
	normalKeys_[XK_w] = CDTKey::W;
	normalKeys_[XK_x] = CDTKey::X;
	normalKeys_[XK_y] = CDTKey::Y;
	normalKeys_[XK_z] = CDTKey::Z;
	normalKeys_[XK_bracketleft] = CDTKey::OpenBracket;
	normalKeys_[XK_backslash] = CDTKey::BackSlash;
	normalKeys_[XK_bracketright] = CDTKey::CloseBracket;

	functionKeys_[0xff & XK_BackSpace] = CDTKey::BackSpace;
	functionKeys_[0xff & XK_Tab] = CDTKey::Tab;
	functionKeys_[0xff & XK_Linefeed] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Clear] = CDTKey::Clear;
	functionKeys_[0xff & XK_Return] = CDTKey::Enter;
	functionKeys_[0xff & XK_Pause] = CDTKey::Pause;
	functionKeys_[0xff & XK_Scroll_Lock] = CDTKey::ScrollLock;
	functionKeys_[0xff & XK_Sys_Req] = CDTKey::PrintScreen;
	functionKeys_[0xff & XK_Escape] = CDTKey::Escape;
	functionKeys_[0xff & XK_Delete] = CDTKey::Delete;
	functionKeys_[0xff & XK_Kanji] = CDTKey::Kanji;
	functionKeys_[0xff & XK_Kana_Shift] = CDTKey::Kana;
	functionKeys_[0xff & XK_Home] = CDTKey::Home;
	functionKeys_[0xff & XK_Left] = CDTKey::Left;
	functionKeys_[0xff & XK_Up] = CDTKey::Up;
	functionKeys_[0xff & XK_Right] = CDTKey::Right;
	functionKeys_[0xff & XK_Down] = CDTKey::Down;
	functionKeys_[0xff & XK_Prior] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Page_Up] = CDTKey::PageUp;
	functionKeys_[0xff & XK_Next] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Page_Down] = CDTKey::PageDown;
	functionKeys_[0xff & XK_End] = CDTKey::End;
	functionKeys_[0xff & XK_Begin] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Select] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Print] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Execute] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Insert] = CDTKey::Insert;
	functionKeys_[0xff & XK_Undo] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Redo] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Menu] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Find] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Cancel] = CDTKey::Cancel;
	functionKeys_[0xff & XK_Help] = CDTKey::Help;
	functionKeys_[0xff & XK_Break] = CDTKey::Undefined;
	functionKeys_[0xff & XK_Mode_switch] = CDTKey::ModeChange;
	functionKeys_[0xff & XK_Num_Lock] = CDTKey::NumLock;
	functionKeys_[0xff & XK_KP_Space] = CDTKey::Space;
	functionKeys_[0xff & XK_KP_Tab] = CDTKey::Tab;
	functionKeys_[0xff & XK_KP_Enter] = CDTKey::Enter;
	functionKeys_[0xff & XK_KP_F1] = CDTKey::F1;
	functionKeys_[0xff & XK_KP_F2] = CDTKey::F2;
	functionKeys_[0xff & XK_KP_F3] = CDTKey::F3;
	functionKeys_[0xff & XK_KP_F4] = CDTKey::F4;
	functionKeys_[0xff & XK_KP_Home] = CDTKey::Home;
	functionKeys_[0xff & XK_KP_Left] = CDTKey::Left;
	functionKeys_[0xff & XK_KP_Right] = CDTKey::Right;
	functionKeys_[0xff & XK_KP_Down] = CDTKey::Down;
	functionKeys_[0xff & XK_KP_Prior] = CDTKey::Undefined;
	functionKeys_[0xff & XK_KP_Page_Up] = CDTKey::PageUp;
	functionKeys_[0xff & XK_KP_Next] = CDTKey::Undefined;
	functionKeys_[0xff & XK_KP_Page_Down] = CDTKey::PageDown;
	functionKeys_[0xff & XK_KP_End] = CDTKey::End;
	functionKeys_[0xff & XK_KP_Begin] = CDTKey::Undefined;
	functionKeys_[0xff & XK_KP_Insert] = CDTKey::Insert;
	functionKeys_[0xff & XK_KP_Delete] = CDTKey::Delete;
	functionKeys_[0xff & XK_KP_Equal] = CDTKey::Equals;
	functionKeys_[0xff & XK_KP_Multiply] = CDTKey::Multiply;
	functionKeys_[0xff & XK_KP_Add] = CDTKey::Add;
	functionKeys_[0xff & XK_KP_Separator] = CDTKey::Separator;
	functionKeys_[0xff & XK_KP_Subtract] = CDTKey::Subtract;
	functionKeys_[0xff & XK_KP_Decimal] = CDTKey::Decimal;
	functionKeys_[0xff & XK_KP_Divide] = CDTKey::Divide;
	functionKeys_[0xff & XK_KP_0] = CDTKey::NumPad0;
	functionKeys_[0xff & XK_KP_1] = CDTKey::NumPad1;
	functionKeys_[0xff & XK_KP_2] = CDTKey::NumPad2;
	functionKeys_[0xff & XK_KP_3] = CDTKey::NumPad3;
	functionKeys_[0xff & XK_KP_4] = CDTKey::NumPad4;
	functionKeys_[0xff & XK_KP_5] = CDTKey::NumPad5;
	functionKeys_[0xff & XK_KP_6] = CDTKey::NumPad6;
	functionKeys_[0xff & XK_KP_7] = CDTKey::NumPad7;
	functionKeys_[0xff & XK_KP_8] = CDTKey::NumPad8;
	functionKeys_[0xff & XK_KP_9] = CDTKey::NumPad9;

	functionKeys_[0xff & XK_KP_Up] = CDTKey::Up; /* this was omitted in the original code */
	/* re-definitions by D-Type */
	functionKeys_[0xff & XK_KP_Home] = CDTKey::NumPad7;
	functionKeys_[0xff & XK_KP_Up] = CDTKey::NumPad8;
	functionKeys_[0xff & XK_KP_Page_Up] = CDTKey::NumPad9;
	functionKeys_[0xff & XK_KP_Left] = CDTKey::NumPad4;
	functionKeys_[0xff & 65437] = CDTKey::NumPad5;
	functionKeys_[0xff & XK_KP_Right] = CDTKey::NumPad6;
	functionKeys_[0xff & XK_KP_End] = CDTKey::NumPad1;
	functionKeys_[0xff & XK_KP_Down] = CDTKey::NumPad2;
	functionKeys_[0xff & XK_KP_Page_Down] = CDTKey::NumPad3;
	functionKeys_[0xff & XK_KP_Insert] = CDTKey::NumPad0;
	functionKeys_[0xff & 126] = CDTKey::Alt;

	functionKeys_[0xff & XK_F1] = CDTKey::F1;
	functionKeys_[0xff & XK_F2] = CDTKey::F2;
	functionKeys_[0xff & XK_F3] = CDTKey::F3;
	functionKeys_[0xff & XK_F4] = CDTKey::F4;
	functionKeys_[0xff & XK_F5] = CDTKey::F5;
	functionKeys_[0xff & XK_F6] = CDTKey::F6;
	functionKeys_[0xff & XK_F7] = CDTKey::F7;
	functionKeys_[0xff & XK_F8] = CDTKey::F8;
	functionKeys_[0xff & XK_F9] = CDTKey::F9;
	functionKeys_[0xff & XK_F10] = CDTKey::F10;
	functionKeys_[0xff & XK_F11] = CDTKey::F11;
	functionKeys_[0xff & XK_F12] = CDTKey::F12;
	functionKeys_[0xff & XK_Shift_L] = CDTKey::Shift;
	functionKeys_[0xff & XK_Shift_R] = CDTKey::Shift;
	functionKeys_[0xff & XK_Control_L] = CDTKey::Control;
	functionKeys_[0xff & XK_Control_R] = CDTKey::Control;
	functionKeys_[0xff & XK_Caps_Lock] = CDTKey::CapsLock;
	functionKeys_[0xff & XK_Shift_Lock] = CDTKey::CapsLock;
	functionKeys_[0xff & XK_Meta_L] = CDTKey::Meta;
	functionKeys_[0xff & XK_Meta_R] = CDTKey::Meta;
	functionKeys_[0xff & XK_Alt_L] = CDTKey::Alt;
	functionKeys_[0xff & XK_Alt_R] = CDTKey::Alt;

	return true;
}

const bool keyMapsInitialized_ = InitializeKeyMaps();

/*
class CDTVector
{
public:

	explicit CDTVector(size_t size = 0)
	{
		data_ = (size <= 0) ? DV_NULL : (DT_CHAR*)dtMemAlloc(64 + size * sizeof(DT_CHAR));
	}

	~CDTVector()
	{
		if (data_ != DV_NULL) { dtMemFree(data_); data_ = DV_NULL; }
	}

	void reset(size_t size = 0)
	{
		CDTVector temp(size);
		swap(temp);
	}

	DT_CHAR& operator[](size_t i)
	{
		// assert(data_);
		return data_[i];
	}

	DT_CHAR* get() { return data_;}

	bool isEmpty() const { return data_ == DV_NULL; }

	void swap(CDTVector& other)
	{
		DT_CHAR* temp = data_;
		data_ = other.data_;
		other.data_ = temp;
	}

private:

	DT_CHAR* data_;
};
*/

