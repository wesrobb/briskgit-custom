/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


//#define DEF_USE_CHAR_CODE


DT_ID_ULONG KeyToChar(DT_SLONG key, DT_ULONG flags, DT_ID_ULONG char_code)
{
#ifdef DEF_USE_CHAR_CODE

	if (char_code == 8 || char_code == 127) return 0; /* BAKSPACE and DELETE are not characters (but delete instructions) */
	if (char_code >= 57344 && char_code <= 63743) return 0; /* PUA */
	return char_code;

#else

	(void)char_code;

	DT_ULONG shift = flags & DV_KEY_SHIFT;
	DT_ULONG ctrl = flags & DV_KEY_CTRL;

	/* numbers and symbols */
	if (key >= 48 && key <= 57)
	{
		if (!shift) return '0' + (key - 48);
		else if (key == 48) return ')';
		else if (key == 49) return '!';
		else if (key == 50) return '@';
		else if (key == 51) return '#';
		else if (key == 52) return '$';
		else if (key == 53) return '%';
		else if (key == 54) return '^';
		else if (key == 55) return '&';
		else if (key == 56) return '*';
		else if (key == 57) return '(';
	}

	/* letters */
	else if (key >= 65 && key <= 90)
	{
		if (flags & DV_KEY_CAPS) shift = !shift;
		if (!shift) return 'a' + (key - 65);
		else return 'A' + (key - 65);
	}

	/* other symbols */
	else if (key == 44) return (shift ? '<' : ',');
	else if (key == 46) return (shift ? '>' : '.');
	else if (key == 47) return (shift ? '?' : '/');
	else if (key == 59) return (shift ? ':' : ';');
	else if (key == 61) return (shift ? '+' : '=');
	else if (key == 91) return (shift ? '{' : '[');
	else if (key == 92) return (shift ? '|' : 92);
	else if (key == 93) return (shift ? '}' : ']');
	else if (key == 108) return (shift ? '_' : '-');
	else if (key == 192) return (shift ? '~' : '`'); /* does not work on Mac */
	else if (key == 222) return (shift ? '"' : 39);

	/* numeric pad */
	else if (key >= 96 && key <= 105) return '0' + (key - 96);
	else if (key == 106) return '*';
	else if (key == 107) return '+';
	else if (key == 109) return '-';
	else if (key == 110) return '.';
	else if (key == 111) return '/';

	/* space */
	else if (key == 32) return 32 /*(shift ? 32 : 32)*/;

	/* enter and column break */
	else if (key == 13) { if (ctrl) return 12; return 13 /*(shift ? 13 : 13)*/; }
	else if (key == 10) { if (ctrl) return 12; return 13 /*(shift ? 13 : 13)*/; } /* Mac */

	/* tab */
	else if (key == 9) return 9;

	return 0;

#endif
}
