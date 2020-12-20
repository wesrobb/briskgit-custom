/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


// D-Type header file (level 4)
#include "dtype_l4.h"
// D-Type's platform independent window support
#include "dtwindow.h"
// D-Type's extra system utility functions
#include "dtxsystem.h"
// D-Type CPP Engine
#include "dtengine.cpp"


#define DV_SURFACE_BPP 4
#define DV_SURFACE_FORMAT (DV_SURFACE_BPP * 8)


class CDTApplet : public CDTWindow
{
public:

	CDTApplet(CDTEngineV8& engine, CDTDisplay& display, DT_FLOAT zoom = 1.0) : CDTWindow(&display), Engine(engine)
	{
		Init();
		Zoom *= zoom;
	}

	CDTApplet(CDTEngineV8& engine, CDTDisplay& display, const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags = 0, DT_FLOAT zoom = 1.0) : CDTWindow(&display, title, w, h, flags), Engine(engine)
	{
		Init();
		Zoom *= zoom;
	}

	virtual ~CDTApplet()
	{
		Exit();
	}

	void DrawFrame()
	{
		DrawMain(); SetPixels(MemorySurface.m, 1);
	}

	bool Redraw;

private:

	bool Init();
	void Exit();

	void Reset()
	{
		Redraw = true;

		LeftMouseButton = false;
		MouseX = MouseY = 0;
		X = Y = 0;
		Zoom = GetDisplay()->GetScale();
		Page = 0;
	}

	bool InitDoc_00();
	bool InitDoc_01();
	bool InitDoc_02();
	bool InitDoc_03();
	bool InitDoc_04();
	bool InitDoc_05();
	bool InitDoc_06();
	bool InitDoc_07();

	bool DrawMain();
	void DrawBitmap(const DT_BMP& bmp, DT_SLONG x, DT_SLONG y, DT_UBYTE alpha);
	void LoadBitmap(DT_BMP& bmp, DT_SLONG w, DT_SLONG h, const DT_STREAM_DESC& sd);

private: // window callbacks

	void Event_KeyDown(DT_SLONG key);
	void Event_MouseButtonDown(DT_SLONG button, DT_SLONG x, DT_SLONG y);
	void Event_MouseMove(DT_SLONG x, DT_SLONG y);
	void Event_MouseButtonUp(DT_SLONG button, DT_SLONG x, DT_SLONG y);
	void Event_Resize(DT_SLONG w, DT_SLONG h);

private:

	bool LeftMouseButton;
	DT_SLONG MouseX, MouseY;
	DT_SLONG X, Y;
	DT_FLOAT Zoom;
	DT_SWORD Page;

	CDTDocV8 Doc[8];

	CDTEngineV8& Engine;

	DT_MDC MemorySurface;

	DT_BMP BitmapA;
	DT_BMP BitmapB;
};


const DT_TX_ATTRIBS myattr_Head[] =
{
	{TX_ATTR_FONT_UNIQUEID, {"F0010_GENTI__NS0" /* Gentium font id */}},
	{TX_ATTR_FONT_WIDTH,    {"70" /* in doc units */}},
	{TX_ATTR_FONT_HEIGHT,   {"70" /* in doc units */}},
	{TX_ATTR_BODY_RGBT,     {"5A31A500"}},
	{TX_ATTR_SHADOW_RGBT,   {"AD87E800"}},
	{TX_ATTR_SHADOW_EP,     {"030A" /* effect=03, parameter=0A */}},
	{TX_ATTR_END,           {"" /* mandatory end-of-array */}}
};

const DT_TX_ATTRIBS myattr_Subhead[] =
{
	{TX_ATTR_FONT_UNIQUEID, {"F0010_GENTI__NS4" /* Gentium italic font id */}},
	{TX_ATTR_FONT_WIDTH,    {"55" /* in doc units */}},
	{TX_ATTR_FONT_HEIGHT,   {"55" /* in doc units */}},
	{TX_ATTR_BODY_RGBT,     {"A0000000"}},
	{TX_ATTR_END,           {"" /* mandatory end-of-array */}}
};


bool CDTApplet::Init()
{
	Reset();

	MemorySurface.m = DV_NULL; MemorySurface.l = 0; MemorySurface.w = MemorySurface.h = 0;

	DT_STREAM_FILE(sa, "../../files/raw/base_180x165_8bpp.raw");  LoadBitmap(BitmapA, 180, 165, sa);
	DT_STREAM_FILE(sb, "../../files/raw/menu2_180x165_8bpp.raw"); LoadBitmap(BitmapB, 180, 165, sb);

	bool ret = true;

	ret &= InitDoc_00();
	ret &= InitDoc_01();
	ret &= InitDoc_02();
	ret &= InitDoc_03();
	ret &= InitDoc_04();
	ret &= InitDoc_05();
	ret &= InitDoc_06();
	ret &= InitDoc_07();

	return ret;
}


void CDTApplet::Exit()
{
	dtMemFree(MemorySurface.m);
	dtMemFree(BitmapA.m);
	dtMemFree(BitmapB.m);
}


bool CDTApplet::InitDoc_00()
{
	/* --- STEP 1: MAKE UNICODE TEXT -------------------------------------------
	   Make a sample document (Doc[0]).
	   -------------------------------------------------------------------------
	*/

	if (!Doc[0].Init(Engine)) return false;

	DT_STR_UTF8 text_utf8 =

	"English Text Example\r" \
	"Horizontal, Left-to-Right\r" \
	"\r" \
	"The Eiffel Tower was built for the Centennial Exposition of 1889, a fair to commemorate the 100th anniversary of the French Revolution. It was designed and constructed by French engineer Alexandre Gustave Eiffel, who was subsequently nicknamed the Magician of Iron after the tower was completed. It was finished in early 1889, in a shroud of amazement and criticism. The tower was widely disliked by many of the artists, writers and citizens of the day. Well known artists wrote that the tower was \"useless and monstrous\" much like the tower of Babel. The design would eventually vindicate itself of this criticism, as the structure was sound, and there was an air of aesthetic beauty to it.\r" \
	"\r" \
	"The tower was raised in just a few months with a small labor force and a relatively low cost. It was opened for the Exposition on June 10, 1889, and Eiffel spent the day ushering royalty to the top, showing off his magnificent piece of work. In the next few weeks, people such as the Shah of Persia, the Prince of Wales, the King of Siam, the Bey of Djibouti, the President of France, Buffalo Bill, and Thomas Edison were guests of the Eiffel Tower. More than 1.9 million people would visit the monument during the exposition.\r" \
	"\r" \
	"The actual structure of the tower itself was highly advanced for the time period. It consisted of four semi-circular arches at the base with iron trusses along the sides. It was constructed entirely of iron and steel. The original tower was 300 meters high, making it the tallest free standing, man-made structure in the world at the time, finally being surpassed with the completion of the Chrysler Building in New York City in 1930. It was twice as tall as the dome of St. Peter's Cathedral in Rome or the great pyramids of Giza. The first level, the largest of the three, has been built into a museum called the Cinemax, where videos are shown about the history of the tower and its construction. The central structure consists of two levels, each of which has a restaurant. The third level is the Salle Gustave Eiffel, or Eiffel Hall, which has space for business conferences, expositions, cultural events, and other social gatherings.\r";

	DT_STREAM_FILE(stream_fmap, "config/fontmap.inf");
	DT_STREAM_MEMORY(stream_text, reinterpret_cast<const DT_UBYTE*>(text_utf8), static_cast<DT_SLONG>(strlen(text_utf8)));

	DT_TX_TEXTFLOW_AREA textflow_area = {10, 10, 1000, 1500, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}};

	CDTDocV8::CDTTextEmbeddable doc_text(Doc[0]);

	if (doc_text.Make(TX_UTF8, stream_text, TX_IMPORT_FULL_COMPACT, textflow_area, stream_fmap, 1 /*1 = make text flow*/) < 0) return false;

	/* --- OPTIONAL STEP 2: TEXT STYLING ---------------------------------------
	   Format (i.e. style) the text. This step is necessary only if we really
	   need to style our text.
	   -------------------------------------------------------------------------
	*/

	doc_text.SetAttribs(0, 21, myattr_Head);
	doc_text.SetAttribs(21, 47, myattr_Subhead);

	return true;
}


bool CDTApplet::InitDoc_01()
{
	/* --- STEP 1: MAKE UNICODE TEXT -------------------------------------------
	   Make a sample document (Doc[1]).
	   -------------------------------------------------------------------------
	*/

	if (!Doc[1].Init(Engine)) return false;

	DT_STR_UTF8 text_utf8 =

	"Russian Text Example\r" \
	"Horizontal, Left-to-Right\r" \
	"\r" \
	"Фаршированные банановые шарики, жаренные во фритюре (кервай)\r" \
	"\r" \
	"Очистите бананы от кожуры, нарежьте на кусочки и оставьте. Нагрейте 1-2 ст. л. топленого или сливочного масла и поджарьте орехи. Когда они станут коричневыми, добавьте изюм и жарьте еще несколько секунд. Добавьте к этой смеси мускатный орех и кардамон и снимите с огня.\r" \
	"\r" \
	"Нагрейте остатки масла и жарьте в нем бананы, пока они не станут мягкими и не покроются коричневыми пятнышками. Не пугайтесь, если на дне кастрюли образуется корка. Добавьте сахар и продолжайте помешивать, пока смесь не загустеет и не образуется однородная масса. Это займет 10-15 мин. Снимите кастрюлю с огня и дайте пасте остыть.\r" \
	"\r" \
	"Разделите банановую пасту на 15 частей и смешайте одну часть с орехами. Ореховую начинку разделите на 14 частей. Возьмите одну часть банановой смеси и сделайте из нее толстую круглую лепешку. Положите лепешку на ладонь и большим пальцем сделайте в ней углубление. положите в углубление одну часть ореховой начинки и соедините края лепешки. Залепите отверстие. Осторожно скатайте в ровный шарик. То же самое проделайте с остатками банановой пасты и ореховой начинки.\r" \
	"\r" \
	"Нагрейте масло, и жарьте шарики во фритюре около 2 мин до коричневого цвета. Следите, чтобы шарики не склеивались друг с другом при обжаривании. Посыпьте пожаренным кокосовым орехом и подавайте горячими.\r";

	DT_STREAM_FILE(stream_fmap, "config/fontmap.inf");
	DT_STREAM_MEMORY(stream_text, reinterpret_cast<const DT_UBYTE*>(text_utf8), static_cast<DT_SLONG>(strlen(text_utf8)));

	DT_TX_TEXTFLOW_AREA textflow_area = {10, 10, 1000, 1500, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}};

	CDTDocV8::CDTTextEmbeddable doc_text(Doc[1]);

	if (doc_text.Make(TX_UTF8, stream_text, TX_IMPORT_FULL_COMPACT, textflow_area, stream_fmap, 1 /*1 = make text flow*/) < 0) return false;

	/* --- OPTIONAL STEP 2: TEXT STYLING ---------------------------------------
	   Format (i.e. style) the text. This step is necessary only if we really
	   need to style our text.
	   -------------------------------------------------------------------------
	*/

	doc_text.SetAttribs(0, 21, myattr_Head);
	doc_text.SetAttribs(21, 47, myattr_Subhead);

	return true;
}


bool CDTApplet::InitDoc_02()
{
	/* --- STEP 1: MAKE UNICODE TEXT -------------------------------------------
	   Make a sample document (Doc[2]).
	   -------------------------------------------------------------------------
	*/

	if (!Doc[2].Init(Engine)) return false;

	DT_STR_UTF8 text_utf8 =

	"Greek Text Example\r" \
	"Horizontal, Left-to-Right\r" \
	"\r" \
	"Πίτα\r" \
	"\r" \
	"- Βάζετε σε μία λεκάνη το νερό και διαλύετε την μαγιά χρησιμοποιώντας ξύλινο κουτάλι περιστρέφοντας μόνο στην ίδια φορά\r" \
	"\r" \
	"- Προσθέτετε το αλεύρι ολικής αλέσεως 1 φλυτζάνι την φορά\r" \
	"\r" \
	"- Συνεχίζετε να ανακατεύετε με το κουτάλι κάνοντας τον κύκλο 100 φορές\r" \
	"\r" \
	"- Καλύψτε το μπωλ με διάφανη μεμβράνη και αφήστε την ζύμη να ξεκουραστεί τουλάχιστον για μισή ώρα. Το καλύτερο είναι να την αφήσετε για 8 ώρες σε δροσερό μέρος, έτσι θα έχει η πίτα πιο πλούσια γεύση\r" \
	"\r" \
	"- Μόλις βγάλετε την μεμβράνη πασπαλίστε την ζύμη σας με αλάτι και ανακατεύοντας πάντα στην ίδια φορά προσθέστε και το λάδι\r" \
	"\r" \
	"- Προσθέτετε το υπόλοιπο αλεύρι 1 φλυτζάνι την φορά και αρχίζετε να ζυμώνετε όταν πιά δεν μπορείτε με το κουτάλι σε ελαφρά αλευρωμένη επιφάνεια μέχρι να γίνει η ζύμη λεία και ελαστική. Πρέπει να είναι κάπως σφικτή\r" \
	"\r" \
	"- Πλάθετε τη ζύμη σε σχήμα μπάλας, την τοποθετείτε σε ένα λαδωμένο μπωλ και την φέρνετε μερικές βόλτες ώστε να λαδωθεί από όλες τις πλευρές και σκεπάζετε σφιχτά το μπωλ με πλαστική μεμβράνη\r" \
	"\r" \
	"- Αφήνετε να διπλασιαστεί η ζύμη σε μέγεθος, περίπου δύο με τρείς ώρες\r" \
	"\r" \
	"- Αφού φουσκώσει την ξεσκεπάζετε και την ζυμώνετε ελαφρά να ξεφουσκώσει\r" \
	"\r" \
	"- Προθερμαίνετε το φούρνο στους 180 C\r" \
	"\r" \
	"- Χωρίστε τη ζύμη σε δύο ίσα μέρη, όσο δουλεύετε το ένα κρατήστε το άλλο καλλυμένο\r" \
	"\r" \
	"- Χωρίστε τώρα τη ζύμη σε 8 ίσα κομμάτια και με ελαφρά αλευρωμένα χέρια κάντε τα μπάλλες\r" \
	"\r" \
	"- Ανοίξτε με τον πλάστη την κάθε μπάλλα σε μία πίτα\r" \
	"\r" \
	"- Βάλτε τις πίτες σε ταψί και ψήστε μέχρι να μοιάζουν με μπαλλόνια ή 3 με 5 λεπτά\r";

	DT_STREAM_FILE(stream_fmap, "config/fontmap.inf");
	DT_STREAM_MEMORY(stream_text, reinterpret_cast<const DT_UBYTE*>(text_utf8), static_cast<DT_SLONG>(strlen(text_utf8)));

	DT_TX_TEXTFLOW_AREA textflow_area = {10, 10, 1000, 1500, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}};

	CDTDocV8::CDTTextEmbeddable doc_text(Doc[2]);

	if (doc_text.Make(TX_UTF8, stream_text, TX_IMPORT_FULL_COMPACT, textflow_area, stream_fmap, 1 /*1 = make text flow*/) < 0) return false;

	/* --- OPTIONAL STEP 2: TEXT STYLING ---------------------------------------
	   Format (i.e. style) the text. This step is necessary only if we really
	   need to style our text.
	   -------------------------------------------------------------------------
	*/

	doc_text.SetAttribs(0, 19, myattr_Head);
	doc_text.SetAttribs(19, 45, myattr_Subhead);

	return true;
}


bool CDTApplet::InitDoc_03()
{
	/* --- STEP 1: MAKE UNICODE TEXT -------------------------------------------
	   Make a sample document (Doc[3]).
	   -------------------------------------------------------------------------
	*/

	if (!Doc[3].Init(Engine)) return false;

	DT_STR_UTF8 text_utf8 =

	"Vietnamese Text Example\r" \
	"Horizontal, Left-to-Right\r" \
	"\r" \
	"Bánh Đúc Bột Gạo\r" \
	"\r" \
	"Nguyên Liệu\r" \
	"- 1 tô gạo, lưạ thứ trong và dài hột\r" \
	"- 5 tô nước\r" \
	"- 2 lượng bột năng\r" \
	"- 1 chén cơm nước, it mè trắng\r" \
	"- 1 cân đường thẽ\r" \
	"- 2 muổng vô trắng thứ ăn trầu\r" \
	"- 1 muổng café hàn the sống dã nhỏ, nữa trái dừa khô\r" \
	"- 2 muổng canh đầu ăn\r" \
	"\r" \
	"Cách Làm\r" \
	"Vôi trắng để vô chậu trộn với nước lã cho đều để lóng một đêm, sáng để gạo vô soong, lấy vãi lọc nước vôi, lấy nước trong đổ gạo đem nấu vừa sôi hột gạo còn búp lấy đũa sơ đều, đem ra đổ vô thúng vút gạo, đãi nhẹ tay 5-7 nước cho hột gạo bớt vàng. Trút gạo vô soong, lường năm tô nước để vô nấu hầm cho nhừ nát hột gạọ Lấy đũa bếp khuấy cho nhuyễn, 2 lượng bột năng và 1 ché nước nói trên, trộn chung lại cho đều, đừng cho đóng cục, chừng bột chín trong. để hàn the vô trộn đều, rồi để dầu ăn vô cũng trộn khuấy đềụ\r" \
	"\r" \
	"Lót vãi trắng trong mâm, trút bánh ra lấy xạn ém cho bằng mặt bánh, cho bề cao được ba phân, để thật nguội rồi cắt ra từng miếng.\r" \
	"\r" \
	"Thắng đường, chừng ăn rắc mè lên mặt, cũng làm giống kiểu bánh đúc bột năng.\r" \
	"\r" \
	"++ Muốn làm bánh đúc xanh, khi nấu gạo, lường nước lá dứa vô nấu, đừng để nước lã không thì bánh sẽ xanh, (bánh này ở xa chợ làm tiện lắm) để lâu được. Dùng thứ bột này làm bánh lọc cũng được, muốn làm bánh lọc. lấy miếng thiếc tròn đục lổ bằng ngón tay hay có khuôn càng tiện, để trên chậu nước lạnh, bột còn cóng lấy vá múc bột để lên khuôn chà cho lọc xuống nước, vớt ra để ráo, chừng ăn để bột vô nước đường và nước cốt dừa, thắng sơ để chút muối cho dừa đừng chuạ (bánh này dùng nước đường nóng)\r";

	DT_STREAM_FILE(stream_fmap, "config/fontmap.inf");
	DT_STREAM_MEMORY(stream_text, reinterpret_cast<const DT_UBYTE*>(text_utf8), static_cast<DT_SLONG>(strlen(text_utf8)));

	DT_TX_TEXTFLOW_AREA textflow_area = {10, 10, 1000, 1500, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}};

	CDTDocV8::CDTTextEmbeddable doc_text(Doc[3]);

	if (doc_text.Make(TX_UTF8, stream_text, TX_IMPORT_FULL_COMPACT, textflow_area, stream_fmap, 1 /*1 = make text flow*/) < 0) return false;

	/* --- OPTIONAL STEP 2: TEXT STYLING ---------------------------------------
	   Format (i.e. style) the text. This step is necessary only if we really
	   need to style our text.
	   -------------------------------------------------------------------------
	*/

	doc_text.SetAttribs(0, 24, myattr_Head);
	doc_text.SetAttribs(24, 50, myattr_Subhead);

	return true;
}


bool CDTApplet::InitDoc_04()
{
	/* --- STEP 1: MAKE UNICODE TEXT -------------------------------------------
	   Make a sample document (Doc[4]).
	   -------------------------------------------------------------------------
	*/

	if (!Doc[4].Init(Engine)) return false;

	DT_STR_UTF8 text_utf8 =

	"Hebrew Text Example\r" \
	"Horizontal, Right-to-Left\r" \
	"\r" \
	"לביבות תפוחי אדמה ובצל לחנוכה\r" \
	"\r" \
	"לקלוף את תפוחי האדמה והבצל ולרסקם על פומפיה דקה\r" \
	"\r" \
	"מיד להכניס לתוך קערה גדולה ובה שתי כפות הלימון ולערבב, על מנת שתפוחי האדמה לא ישחירו\r" \
	"\r" \
	"לאחר הערבוב עם הלימון להחזיר למסננת כדי שהמים מתפוחי האדמה והבצל ידלפו החוצה\r" \
	"\r" \
	"לאחר דליפת המים, להחזיר את תפוחי האדמה והבצל לקערה, להוסיף את הקמח, הביצים המלח והפלפל ולערבב\r" \
	"\r" \
	"לחמם שמן במחבת בחום בינוני - גבוה\r" \
	"\r" \
	"לצקת אל תוך המחבת כפות-כפות מתוך העיסה, 4 - 5 לביבות בפעם אחת ולטגן. יש האוהבים את הלביבות שחומות ויש האוהבים אותן פחות שחומות. אפשר להשחימן מבחוץ ולהמשיך את הבישול בתנור או במיקרוגל\r" \
	"\r" \
	"להניח את הלביבות על נייר מגבת ולהספיג את השמן\r";

	DT_STREAM_FILE(stream_fmap, "config/fontmap.inf");
	DT_STREAM_MEMORY(stream_text, reinterpret_cast<const DT_UBYTE*>(text_utf8), static_cast<DT_SLONG>(strlen(text_utf8)));

	DT_TX_TEXTFLOW_AREA textflow_area = {10, 10, 1000, 1500, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}};
	DT_TX_DEFAULT_ATTRIBS attribs = {"00000000" /*rgbt*/ , "36" /*font_width*/, "36" /*font_height*/, "0" /*skew_hor*/, "0" /*skew_ver*/, "10" /*spacing_row*/, "0" /*spacing_letter*/, "2" /*align*/, "2" /*align_end*/, "0" /*hinting*/, DV_NULL /*font_index*/, "0" /*reserved*/};

	CDTDocV8::CDTTextEmbeddable doc_text(Doc[4]);

	if (doc_text.Make(TX_UTF8, stream_text, TX_IMPORT_FULL_COMPACT, textflow_area, attribs, stream_fmap, 1 /*1 = make text flow*/) < 0) return false;

	/* --- OPTIONAL STEP 2: TEXT STYLING ---------------------------------------
	   Format (i.e. style) the text. This step is necessary only if we really
	   need to style our text.
	   -------------------------------------------------------------------------
	*/

	doc_text.SetAttribs(0, 20, myattr_Head);
	doc_text.SetAttribs(20, 46, myattr_Subhead);

	return true;
}


bool CDTApplet::InitDoc_05()
{
	/* --- STEP 1: MAKE UNICODE TEXT -------------------------------------------
	   Make a sample document (Doc[5]).
	   -------------------------------------------------------------------------
	*/

	if (!Doc[5].Init(Engine)) return false;

	DT_STR_UTF8 text_utf8 =

	"Arabic Text Example\r" \
	"Horizontal, Right-to-Left\r" \
	"\r" \
	"كيكة الكابتشينو بجوز الهند\r" \
	"\r" \
	"المقادير:\r" \
	"\r" \
	"مقادير الكيكة :\r" \
	"2 كوب طحين مع 3 ملعقة كوب بيكن بودر + 4 بيض + 2/1 ملعقة شاي فانيليا + 1 كوب جوز الهند + 2 باكيت زبدة + 1 حليب كرنيشن + 5 ملاعق من علبة الكابتشينو + 1 كوب سكر .\r" \
	"\r" \
	"مقادير الصلصة :\r" \
	"1 باكيت زبدة + 1 حليب مركز كبير + 4 ملاعق كاكاو بودرة + ملعقة كابتشينو .\r" \
	"\r" \
	"الطريقة :\r" \
	"\r" \
	"- نخفق 2 زبدة مع السكر في الخلاط ثم نضيف البيض مع الفانيليا .\r" \
	"- نضيف الطحين بالتناوب مع حليب الكارنيشن ، ثم جوز الهند .\r" \
	"- بعد ذلك نضع نصف الكمية في ماعون مدهون بقليل من الزيت ، اما الخليط المتبقي نضيف إليه 5 ملاعق من الكابتشينو و يخلط ثم نضيفه على المقدار السابق في الماعون و ندخله في فرن درجته 180 لمدة 45 دقيقة .\r" \
	"- و لعمل الصلصة ، نسيح الزبدة في قدر على نار خفيفة ثم نضيف الحليب المركز ثم ملعقة كابتشينو و الكاكاو البودرة .\r" \
	"- بعد اخراج الكيكة من الفرن تترك لتبرد ، ثم تخرج من الماعون و نصب فوقها الصلصة و هي حارة .\r";

	DT_STREAM_FILE(stream_fmap, "config/fontmap.inf");
	DT_STREAM_MEMORY(stream_text, reinterpret_cast<const DT_UBYTE*>(text_utf8), static_cast<DT_SLONG>(strlen(text_utf8)));

	DT_TX_TEXTFLOW_AREA textflow_area = {10, 10, 1000, 1500, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}};
	DT_TX_DEFAULT_ATTRIBS attribs = {"00000000" /*rgbt*/ , "36" /*font_width*/, "36" /*font_height*/, "0" /*skew_hor*/, "0" /*skew_ver*/, "10" /*spacing_row*/, "0" /*spacing_letter*/, "2" /*align*/, "2" /*align_end*/, "0" /*hinting*/, DV_NULL /*font_index*/, "0" /*reserved*/};

	CDTDocV8::CDTTextEmbeddable doc_text(Doc[5]);

	if (doc_text.Make(TX_UTF8, stream_text, TX_IMPORT_FULL_COMPACT, textflow_area, attribs, stream_fmap, 1 /*1 = make text flow*/) < 0) return false;

	/* --- OPTIONAL STEP 2: TEXT STYLING ---------------------------------------
	   Format (i.e. style) the text. This step is necessary only if we really
	   need to style our text.
	   -------------------------------------------------------------------------
	*/

	doc_text.SetAttribs(0, 20, myattr_Head);
	doc_text.SetAttribs(20, 46, myattr_Subhead);

	return true;
}


bool CDTApplet::InitDoc_06()
{
	/* --- STEP 1: MAKE UNICODE TEXT -------------------------------------------
	   Make a sample document (Doc[6]).
	   -------------------------------------------------------------------------
	*/

	if (!Doc[6].Init(Engine)) return false;

	DT_STR_UTF8 text_utf8 =

	"Chinese Text Example\r" \
	"Horizontal, Left-to-Right\r" \
	"\r" \
	"如今，幾個東亞主要的語言(國語、日語、韓語)都已經在電腦上有蠻不錯的支援了，但是仍有些人沒法在電腦上找到他們的母語。對這些在台灣各佔 70% 及 30% 的台灣閩南語及客家語使用人口來說卻是事實，當然除此之外，還包括了許多少數民族的語言，這些少數語言的分佈遍及整個東亞。\r" \
	"\r" \
	"至今，若想要在電腦上使用台灣閩南語或是客家語的人，就必須去買特殊的商業軟體，使用商業軟體的解決方案。但是這些商業軟體帶來了另一個問題：『不能相容』。每個軟體廠商推廣各自的解決方案，但是文件的格式只能在單獨的軟體內顯示，在其他軟體上卻無法正常顯示，其他使用者如果要使用，就也必須去購買這樣一個「不相容」的軟體並安裝在電腦上。\r" \
	"\r" \
	"會產生這個問題的原因是：至今在台灣或其他地方都不存在一個包含這些問題的標準，沒有任何標準提供了輸入法表格，或者包含所有這些語言需要的字元集及輸入方式。\r" \
	"\r" \
	"雖然大多數台灣閩南語及客家語所用的漢字都在 Big5 標準內，但仍缺少許多字。甚至 CNS11643 國家標準也不包含這些為了輸入及顯示台灣閩南語及客家語所需要的字。\r" \
	"\r" \
	"在最近幾年，有個叫做 Unicode(萬國碼) 的國際標準開始展露鋒芒。儘管 Unicode 是為了支援世界上所有語言及文字而設計，但還是缺少許多台灣閩南語及客家語所需要的文字。\r" \
	"\r" \
	"因此，沒有任何免費的字型有包含這些字元，也沒有任何輸入法可以輸入這些字。\r" \
	"\r" \
	"我前面提到過的那些商用軟體，都有了各自的解決方案解決了上述問題，卻也造成了另外一個問題 - 使用者們不使用這些軟體，就不能恰當的顯示或輸入台灣閩南語或是客家語，而可能的文件的互相交換則更加複雜且麻煩了。\r" \
	"\r" \
	"這就是為什麼我要著手 CJK-Unifonts 計畫。\r";

	DT_STREAM_FILE(stream_fmap, "config/fontmap.inf");
	DT_STREAM_MEMORY(stream_text, reinterpret_cast<const DT_UBYTE*>(text_utf8), static_cast<DT_SLONG>(strlen(text_utf8)));

	DT_TX_TEXTFLOW_AREA textflow_area = {10, 10, 1000, 1500, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}};

	CDTDocV8::CDTTextEmbeddable doc_text(Doc[6]);

	if (doc_text.Make(TX_UTF8, stream_text, TX_IMPORT_FULL_COMPACT, textflow_area, stream_fmap, 1 /*1 = make text flow*/) < 0) return false;

	/* --- OPTIONAL STEP 2: TEXT STYLING ---------------------------------------
	   Format (i.e. style) the text. This step is necessary only if we really
	   need to style our text.
	   -------------------------------------------------------------------------
	*/

	doc_text.SetAttribs(0, 21, myattr_Head);
	doc_text.SetAttribs(21, 47, myattr_Subhead);

	return true;
}


bool CDTApplet::InitDoc_07()
{
	/* --- STEP 1: MAKE UNICODE TEXT -------------------------------------------
	   Make a sample document (Doc[7]).
	   -------------------------------------------------------------------------
	*/

	if (!Doc[7].Init(Engine)) return false;

	DT_STR_UTF8 text_utf8 =

	"Chinese Text Example\r" \
	"Vertical, Right-to-Left\r" \
	"\r" \
	"如今，幾個東亞主要的語言(國語、日語、韓語)都已經在電腦上有蠻不錯的支援了，但是仍有些人沒法在電腦上找到他們的母語。對這些在台灣各佔 70% 及 30% 的台灣閩南語及客家語使用人口來說卻是事實，當然除此之外，還包括了許多少數民族的語言，這些少數語言的分佈遍及整個東亞。\r" \
	"\r" \
	"至今，若想要在電腦上使用台灣閩南語或是客家語的人，就必須去買特殊的商業軟體，使用商業軟體的解決方案。但是這些商業軟體帶來了另一個問題：『不能相容』。每個軟體廠商推廣各自的解決方案，但是文件的格式只能在單獨的軟體內顯示，在其他軟體上卻無法正常顯示，其他使用者如果要使用，就也必須去購買這樣一個「不相容」的軟體並安裝在電腦上。\r" \
	"\r" \
	"會產生這個問題的原因是：至今在台灣或其他地方都不存在一個包含這些問題的標準，沒有任何標準提供了輸入法表格，或者包含所有這些語言需要的字元集及輸入方式。\r" \
	"\r" \
	"雖然大多數台灣閩南語及客家語所用的漢字都在 Big5 標準內，但仍缺少許多字。甚至 CNS11643 國家標準也不包含這些為了輸入及顯示台灣閩南語及客家語所需要的字。\r" \
	"\r" \
	"在最近幾年，有個叫做 Unicode(萬國碼) 的國際標準開始展露鋒芒。儘管 Unicode 是為了支援世界上所有語言及文字而設計，但還是缺少許多台灣閩南語及客家語所需要的文字。\r" \
	"\r" \
	"因此，沒有任何免費的字型有包含這些字元，也沒有任何輸入法可以輸入這些字。\r" \
	"\r" \
	"我前面提到過的那些商用軟體，都有了各自的解決方案解決了上述問題，卻也造成了另外一個問題 - 使用者們不使用這些軟體，就不能恰當的顯示或輸入台灣閩南語或是客家語，而可能的文件的互相交換則更加複雜且麻煩了。\r" \
	"\r" \
	"這就是為什麼我要著手 CJK-Unifonts 計畫。\r";

	DT_STREAM_FILE(stream_fmap, "config/fontmap.inf");
	DT_STREAM_MEMORY(stream_text, reinterpret_cast<const DT_UBYTE*>(text_utf8), static_cast<DT_SLONG>(strlen(text_utf8)));

	DT_TX_TEXTFLOW_AREA textflow_area = {10, 10, 1000, 1500, TX_RECT, TX_TYPOROW, 0, TX_DIR_COL_TB_RL, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}};

	CDTDocV8::CDTTextEmbeddable doc_text(Doc[7]);

	if (doc_text.Make(TX_UTF8, stream_text, TX_IMPORT_FULL_COMPACT, textflow_area, stream_fmap, 1 /*1 = make text flow*/) < 0) return false;

	/* --- OPTIONAL STEP 2: TEXT STYLING ---------------------------------------
	   Format (i.e. style) the text. This step is necessary only if we really
	   need to style our text.
	   -------------------------------------------------------------------------
	*/

	doc_text.SetAttribs(0, 21, myattr_Head);
	doc_text.SetAttribs(21, 45, myattr_Subhead);

	return true;
}


bool CDTApplet::DrawMain()
{
	DT_UBYTE* m;
	DT_UBYTE clear_byte = 255;
	DT_SLONG w = GetW(); if (w < 1 || w > 16384) return false;
	DT_SLONG h = GetH(); if (h < 1 || h > 16384) return false;
	DT_SLONG l = DV_SURFACE_BPP * w * h;

	if (l <= MemorySurface.l) m = MemorySurface.m; else m = static_cast<DT_UBYTE*>(dtMemRealloc(MemorySurface.m, l));
	if (m == DV_NULL) return false;

	dtMemSet(m, clear_byte, l);

	MemorySurface.m = m;
	MemorySurface.l = l;
	MemorySurface.w = w;
#ifdef DEF_WIN32 /* flip the surface vertically */
	MemorySurface.h = h;
#else
	MemorySurface.h = -h;
#endif

	DT_SLONG p = Page; if (Page < 0 || Page > 7) p = 0;

	Doc[p].SetScale(Zoom);
	Doc[p].Draw(0, X, Y, DV_SURFACE_FORMAT, 0, MemorySurface);

	DrawBitmap(BitmapA, w - BitmapA.w, h - BitmapA.h, 0);
	DrawBitmap(BitmapB, w - BitmapB.w, h - BitmapB.h, 255);

	return true;
}


void CDTApplet::DrawBitmap(const DT_BMP& bmp, DT_SLONG x, DT_SLONG y, DT_UBYTE alpha)
{
	DT_STYLE_EFFECTS style1 = {1, {0}, {alpha, alpha, alpha, 0}, DV_NULL};
	Engine.OutputSetStyleEffects(style1);
	Engine.BitmapDoOutput(bmp, x, y);
}


void CDTApplet::LoadBitmap(DT_BMP& bmp, DT_SLONG w, DT_SLONG h, const DT_STREAM_DESC& sd)
{
	DT_SLONG l = w * h; 

	bmp.m = DV_NULL; bmp.l = 0; bmp.x = bmp.y = bmp.w = bmp.h = 0;
	bmp.m = static_cast<DT_UBYTE*>(dtMemAlloc(l)); if (bmp.m == DV_NULL) return;
	bmp.w = w;
	bmp.h = h;
	bmp.l = l;

	dtMemSet(bmp.m, 0, bmp.l);

	DT_DTSTREAM f = dtStreamOpen(&sd, DV_STREAM_OPEN_MUST); if (f == DV_NULL) return;
	dtStreamRead(f, bmp.m, bmp.l, 0);
	dtStreamClose(f, DV_STREAM_CLOSE_YES);
}


void CDTApplet::Event_KeyDown(DT_SLONG key)
{
	if (key == 109 || key == 108) /* - */
	{
		if (Zoom > 0.2) Zoom /= 1.2; else return;
	}
	else if (key == 107 || key == 61) /* + */
	{
		if (Zoom < 10.0) Zoom *= 1.2; else return;
	}
	else if (key == 37) /* LEFT */
	{
		X += 10;
	}
	else if (key == 39) /* RIGHT */
	{
		X -= 10;
	}
	else if (key == 38) /* UP */
	{
		Y += 10;
	}
	else if (key == 40) /* DOWN */
	{
		Y -= 10;
	}
	else if (key == 82) /* R */
	{
		Reset();
	}
	else if (key == 33) /* PageUp */
	{
		if (Page > 0) Page--; else return;
	}
	else if (key == 34) /* PageDown */
	{
		if (Page < 7) Page++; else return;
	}
	else return;

	Redraw = true;
}


void CDTApplet::Event_MouseButtonDown(DT_SLONG button, DT_SLONG x, DT_SLONG y)
{
	if (button == 0) LeftMouseButton = true;

	MouseX = x;
	MouseY = y;
}


void CDTApplet::Event_MouseMove(DT_SLONG x, DT_SLONG y)
{
	if (!LeftMouseButton) return;

	X += MouseX - x; MouseX = x;
	Y += MouseY - y; MouseY = y;

	Redraw = true;
}


void CDTApplet::Event_MouseButtonUp(DT_SLONG button, DT_SLONG x, DT_SLONG y)
{
	if (button == 0) LeftMouseButton = false;

	MouseX = x;
	MouseY = y;
}


void CDTApplet::Event_Resize(DT_SLONG /*w*/, DT_SLONG /*h*/)
{
	Redraw = true;
}
