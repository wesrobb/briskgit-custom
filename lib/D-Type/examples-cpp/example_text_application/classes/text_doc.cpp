/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"


class CDTTextDoc
{
private: // private copy constructor and assignment operator

	CDTTextDoc(const CDTTextDoc&);
	CDTTextDoc& operator = (const CDTTextDoc&);

protected:

	CDTTextDoc() {}
	virtual ~CDTTextDoc() {}

public:

	inline DT_TXDOC GetTextDoc() { return TextDoc.GetTxDoc(); }
	virtual DT_SLONG GetLastArea() { return 0; }
	virtual DT_SLONG GetLastPage() { return 0; }
	virtual DT_SLONG GetW(DT_SLONG /*page*/) { return 100; }
	virtual DT_SLONG GetH(DT_SLONG /*page*/) { return 100; }

protected:

	CDTDocV8::CDTTextStandalone TextDoc;
};
