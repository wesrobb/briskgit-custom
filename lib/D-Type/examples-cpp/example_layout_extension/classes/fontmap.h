/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#ifndef DTYPE_EXAMPLE_FONTMAP_H
#define DTYPE_EXAMPLE_FONTMAP_H


class CDTFontMap
{
private: // private copy constructor and assignment operator

	CDTFontMap(const CDTFontMap&);
	CDTFontMap& operator = (const CDTFontMap&);

public: // Constructor and Destructor

	CDTFontMap(CDTEngineV8& engine) : Engine(engine)
	{
		Font_CMUSS = -1;
		Font_BCMPS = -1;
		Font_THRAB = -1;
		Font_UMING = -1;
		Font_NORAS = -1;
		Font_DJVUS = -1;
	}

	virtual ~CDTFontMap()
	{
		UnloadRequiredFonts();
	}

public: // Public methods

	//inline void SetEngine(CDTEngineV8& engine) { /* references can't be reassigned */ Engine = engine; }
	inline CDTEngineV8& GetEngine() const { return Engine; }

	DT_SWORD LoadRequiredFonts();
	void UnloadRequiredFonts();

	DT_ID_SWORD GetFontForUnicodeScript(DT_ID_SWORD script_code) const;

private:

	DT_ID_SWORD LoadSingleFont(DT_ID_UBYTE hinting, const DT_CHAR* file_name);
	void UnloadSingleFont(DT_ID_SWORD font_index);

private:

	CDTEngineV8& Engine;

	// Font IDs
	DT_ID_SWORD Font_CMUSS;
	DT_ID_SWORD Font_BCMPS;
	DT_ID_SWORD Font_THRAB;
	DT_ID_SWORD Font_UMING;
	DT_ID_SWORD Font_NORAS;
	DT_ID_SWORD Font_DJVUS;
};


#endif /* DTYPE_EXAMPLE_FONTMAP_H */
