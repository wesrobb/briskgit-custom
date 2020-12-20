class CDTAutoformatBase
{
private: // private copy constructor and assignment operator

	CDTAutoformatBase(const CDTAutoformatBase&);
	CDTAutoformatBase& operator = (const CDTAutoformatBase&);

public:

	CDTAutoformatBase() {}
	virtual bool Format(CDTBuilder& buider, bool reset = true) = 0;
};


class CDTMarkerPlus_Opener : public CDTMarker
{
private: // private copy constructor and assignment operator

	CDTMarkerPlus_Opener(const CDTMarkerPlus_Opener&);
	CDTMarkerPlus_Opener& operator = (const CDTMarkerPlus_Opener&);

private:

	typedef struct
	{
		DT_SLONG FlowIndex;
		DT_SLONG Pos;

	} DT_MARKER_PLUS_OPENER_FI_POSITIONS;

	void AddPos(DT_SLONG pos, DT_SLONG flow_index)
	{
		if (FIPositionsLen >= 4194304) return;
		DT_MARKER_PLUS_OPENER_FI_POSITIONS* fi_positions = static_cast<DT_MARKER_PLUS_OPENER_FI_POSITIONS*>(dtMemRealloc(FIPositions, (FIPositionsLen + 1) * sizeof(DT_MARKER_PLUS_OPENER_FI_POSITIONS))); if (fi_positions == DV_NULL) return;
		FIPositions = fi_positions; fi_positions += FIPositionsLen;
		fi_positions->FlowIndex = flow_index; fi_positions->Pos = pos;
		FIPositionsLen++;
	}

public:

	CDTMarkerPlus_Opener() : FIPositions(DV_NULL), Formats(DV_NULL), FIPositionsLen(0), FormatsLen(0), ExtA(0), ExtB(0) {}
	virtual ~CDTMarkerPlus_Opener() { dtMemFree(FIPositions); dtMemFree(Formats); }

	void SetPos(CDTBuilder& /*buider*/, DT_SLONG pos, DT_SLONG flow_index = 0)
	{
		//printf("OPEN: pos=%d, flow_index=%d\n", pos, flow_index);
		if (pos >= 0) AddPos(pos + 1, flow_index);
	}

	void SetPos_Close(DT_SLONG pos, DT_SLONG flow_index = 0)
	{
		//printf("CLOSE: pos=%d, flow_index=%d\n", pos, flow_index);
		if (pos >= 0) AddPos(-(pos + 1), flow_index);
	}

	void Reset() { dtMemFree(FIPositions); FIPositions = DV_NULL; FIPositionsLen = 0; }

	bool Format(CDTBuilder& buider)
	{
		//printf("Format FIPositionsLen=%d\n", FIPositionsLen);

		if (FIPositionsLen < 1) return true;

		DT_SLONG p_len_max = FIPositionsLen + 1; /* one more than absolutely the worst case scenario */
		DT_SLONG p_len = 0;
		DT_SLONG* p = static_cast<DT_SLONG*>(dtMemAlloc(p_len_max * sizeof(DT_SLONG))); if (p == DV_NULL) return false;
		//DT_SLONG p[100];

		for (DT_SLONG i = 0; i < FIPositionsLen; i++)
		{
			DT_SLONG flow_index = FIPositions[i].FlowIndex, pos = FIPositions[i].Pos; if (flow_index != buider.GetFlowIndex()) continue;
			if (pos > 0) { p[p_len] = pos; p_len++; continue; } else if (p_len < 1) continue;
			p_len--; DT_SLONG ppos = p[p_len]; buider.Format(Formats, DV_NULL, ppos - 1 + ExtA, -pos - 1 + ExtB);

			//printf("Format from %d to %d, ExtA=%d, ExtB=%d\n", ppos - 1 + ExtA, -pos - 1 + ExtB, ExtA, ExtB);
		}

		dtMemFree(p);
		return true;
	}

	CDTMarkerPlus_Opener& Add(DT_SLONG id, const DT_CHAR* s)
	{
		if (FormatsLen >= 1024 || s == DV_NULL) return *this;
		DT_SLONG l = strlen(s); if (l >= 255) return *this;
		DT_TX_ATTRIBS* formats = static_cast<DT_TX_ATTRIBS*>(dtMemRealloc(Formats, (FormatsLen + 2) * sizeof(DT_TX_ATTRIBS))); if (formats == DV_NULL) return *this;
		Formats = formats; formats += FormatsLen;
		formats->Id = id; dtMemCopy(formats->Value, s, l + 1); formats++;
		formats->Id = TX_ATTR_END; formats->Value[0] = 0;
		FormatsLen++;
		return *this;
	}

	CDTMarkerPlus_Opener& Add(DT_SLONG id, DT_SLONG v) { DT_STRING_CHARS(s, 64, ""); dtStrConcatSLong(&s, v); return Add(id, s.str); }

	CDTMarkerPlus_Opener& Add(const DT_TX_ATTRIBS attr[])
	{
		if (attr == DV_NULL) return *this;
		for (DT_SLONG i = 0; attr[i].Id != TX_ATTR_END; i++) Add(attr[i].Id, attr[i].Value);
		return *this;
	}

	CDTMarkerPlus_Opener& Replace(DT_SLONG id, const DT_CHAR* s) { Replace_Internal(id, s); return *this; }
	CDTMarkerPlus_Opener& Replace(DT_SLONG id, DT_SLONG v) { DT_STRING_CHARS(s, 64, ""); dtStrConcatSLong(&s, v); Replace_Internal(id, s.str); return *this; }
	CDTMarkerPlus_Opener& Replace(const DT_TX_ATTRIBS attr[])
	{
		if (attr == DV_NULL) return *this;
		for (DT_SLONG i = 0; attr[i].Id != TX_ATTR_END; i++) Replace_Internal(attr[i].Id, attr[i].Value);
		return *this;
	}

	CDTMarkerPlus_Opener& Apply(DT_SLONG id, const DT_CHAR* s) { if (Replace_Internal(id, s) < 1) Add(id, s); return *this; }
	CDTMarkerPlus_Opener& Apply(DT_SLONG id, DT_SLONG v) { DT_STRING_CHARS(s, 64, ""); dtStrConcatSLong(&s, v); return Apply(id, s.str); }
	CDTMarkerPlus_Opener& Apply(const DT_TX_ATTRIBS attr[])
	{
		if (attr == DV_NULL) return *this;
		for (DT_SLONG i = 0; attr[i].Id != TX_ATTR_END; i++) Apply(attr[i].Id, attr[i].Value);
		return *this;
	}

	inline CDTMarkerPlus_Opener& operator + (const DT_TX_ATTRIBS attr[]) { return Add(attr); }
	//inline CDTMarkerPlus_Opener& operator += (const DT_SLONG temp[]) { return *this; }

	inline CDTMarkerPlus_Opener& Grow(DT_SLONG ext_a, DT_SLONG ext_b) { ExtA = ext_a; ExtB = ext_b; return *this; }

private:

	DT_SLONG Replace_Internal(DT_SLONG id, const DT_CHAR* s)
	{
		if (s == DV_NULL) return 0;
		DT_SLONG i, n = 0, l = strlen(s); if (l >= 255) return n;

		for (i = 0; i < FormatsLen; i++)
		{
			DT_TX_ATTRIBS* f = Formats + i; if (f->Id != id) continue;
			dtMemCopy(f->Value, s, l + 1); n++;
		}

		return n;
	}

private:

	DT_MARKER_PLUS_OPENER_FI_POSITIONS* FIPositions; /* dynamic array */
	DT_TX_ATTRIBS* Formats; /* dynamic array */
	DT_SLONG FIPositionsLen;
	DT_SLONG FormatsLen;
	DT_SLONG ExtA;
	DT_SLONG ExtB;
};


class CDTMarkerPlus_Closer : public CDTMarker
{
private: // private copy constructor and assignment operator

	CDTMarkerPlus_Closer(const CDTMarkerPlus_Closer&);
	CDTMarkerPlus_Closer& operator = (const CDTMarkerPlus_Closer&);

public:

	CDTMarkerPlus_Closer(CDTMarkerPlus_Opener* opener = DV_NULL) : Opener(opener) {}
	virtual ~CDTMarkerPlus_Closer() {}

	void SetPos(CDTBuilder& /*buider*/, DT_SLONG pos, DT_SLONG flow_index = 0) { Opener->SetPos_Close(pos, flow_index); }
	inline void SetOpener(CDTMarkerPlus_Opener* opener) { Opener = opener; }
	inline CDTMarkerPlus_Opener* GetOpener() { return Opener; }

private:

	CDTMarkerPlus_Opener* Opener;
};


class CDTMarkerPlus_NoOp : public CDTMarkerPlus_Closer
{
	void SetPos(CDTBuilder& /*buider*/, DT_SLONG /*pos*/, DT_SLONG /*flow_index*/ = 0) { /* do nothing (no-operation) */ }
};


class CDTMarkerPlus_Autoformat : public CDTMarker
{
private: // private copy constructor and assignment operator

	CDTMarkerPlus_Autoformat(const CDTMarkerPlus_Autoformat&);
	CDTMarkerPlus_Autoformat& operator = (const CDTMarkerPlus_Autoformat&);

public:

	CDTMarkerPlus_Autoformat(CDTAutoformatBase& autoformat, bool cleanup_and_reset) : Autoformat(autoformat), CleanupAndReset(cleanup_and_reset) {}
	virtual ~CDTMarkerPlus_Autoformat() {}

	void SetPos(CDTBuilder& buider, DT_SLONG /*pos*/, DT_SLONG /*flow_index*/ = 0)
	{
		//printf("DO: pos=%d, flow_index=%d\n", pos, flow_index);
		Autoformat.Format(buider, CleanupAndReset);
		//if (CleanupAndReset) buider.Doc.Cleanup(); /* not needed */
	}

public:

	CDTAutoformatBase& Autoformat;
	bool CleanupAndReset;
};


class CDTAutoformat : public CDTAutoformatBase
{
public:

	CDTAutoformat() : Openers(DV_NULL), OpenersLen(0), DO(*this, false), END(*this, true) {}
	virtual ~CDTAutoformat() { dtMemFree(Openers); }

	CDTMarkerPlus_Opener& Make(CDTMarkerPlus_Opener& opener, CDTMarkerPlus_Closer& closer, DT_SLONG ext_a = 0, DT_SLONG ext_b = 0)
	{
		if (OpenersLen >= 4194304) return opener;

		CDTMarkerPlus_Opener** openers = static_cast<CDTMarkerPlus_Opener**>(dtMemRealloc(Openers, (OpenersLen + 1) * sizeof(CDTMarkerPlus_Opener*))); if (openers == DV_NULL) return opener;
		Openers = openers; openers += OpenersLen;
		*openers = &opener;
		OpenersLen++;

		opener.Grow(ext_a, ext_b);
		closer.SetOpener(&opener);
		return opener;
	}

	void Reset()
	{
		for (DT_SLONG i = 0; i < OpenersLen; i++) Openers[i]->Reset();
	}

	bool Format(CDTBuilder& buider, bool reset = true)
	{
		//printf("Format flow index %d, OpenersLen=%d\n", buider.GetFlowIndex(), OpenersLen);

		bool ret = true;

		for (DT_SLONG i = 0; i < OpenersLen; i++) { if (!Openers[i]->Format(buider)) ret = false; }
		if (reset) Reset();

		return ret;
	}

private:

	CDTMarkerPlus_Opener** Openers; /* dynamic array */
	DT_SLONG OpenersLen;

public:

	CDTMarkerPlus_Autoformat DO;
	CDTMarkerPlus_Autoformat END;
};

