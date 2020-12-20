enum EDTBalignType
{
	DE_BASEALIGN_FIRST = 0,
	DE_BASEALIGN_TOP = 0,
	DE_BASEALIGN_BOTTOM = 1,
	DE_BASEALIGN_MIDDLE = 2,
	DE_BASEALIGN_LAST = 2
};


enum EDTQualityType
{
	DE_QUALITY_FIRST = 0,
	DE_QUALITY_FAST = 0,
	DE_QUALITY_HIGH = 1,
	DE_QUALITY_BEST1 = 2,
	DE_QUALITY_BEST2 = 3,
	DE_QUALITY_LAST = 3
};


class CDTBuilderRast : public CDTBuilderUTF8
{
public:

	class CDTAttribs : public CDTAttribsBase
	{
	public:

		CDTAttribs() : ASrc(DV_NULL), ABaseAlign(DE_BASEALIGN_BOTTOM), AQuality(DE_QUALITY_BEST1) {}
		virtual ~CDTAttribs() { StrFree(ASrc); }

		DEF_ATTRIBS_BASE_METHODS

		inline CDTAttribs& Src(const DT_CHAR* src) { StrCopy(ASrc, src); return *this; }
		inline CDTAttribs& BaseAlign(EDTBalignType base_align) { ABaseAlign = DF_MIN(DF_MAX(DE_BASEALIGN_FIRST, base_align), DE_BASEALIGN_LAST); return *this; }
		inline CDTAttribs& Quality(EDTQualityType quality) { AQuality = DF_MIN(DF_MAX(DE_QUALITY_FIRST, quality), DE_QUALITY_LAST); return *this; }

		DT_BUILDER_STR ASrc;

		EDTBalignType ABaseAlign;
		EDTQualityType AQuality;
	};

private: // private copy constructor and assignment operator

	CDTBuilderRast(const CDTBuilderRast&);
	CDTBuilderRast& operator = (const CDTBuilderRast&);

public:

	CDTBuilderRast(CDTDocV8* doc) : CDTBuilderUTF8(false, doc), ExternalMemory(DV_NULL) {}
	CDTBuilderRast(CDTDocV8& doc) : CDTBuilderUTF8(false, doc), ExternalMemory(DV_NULL) {}
	virtual ~CDTBuilderRast() { if (ExternalMemory != DV_NULL) dtMemFree(ExternalMemory); }

	CDTBuilderRast* New() const { CDTBuilderRast* n = new CDTBuilderRast(Doc); Doc->AddAtom(n); return n; }

public:

	inline CDTBuilderRast& Img(const CDTAttribs& a = CDTAttribs()) const { CDTBuilderRast* b = New(); b->Img_Internal(a); return *b; }

	inline CDTBuilderRast::CDTAttribs operator () () { return CDTAttribs(); }

private:

	bool Img_Autoload(DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_UBYTE quality, const DT_CHAR* file_name);
	CDTBuilderRast& Img_Internal(const CDTAttribs& a);

private:

	DT_UBYTE* ExternalMemory;
};
