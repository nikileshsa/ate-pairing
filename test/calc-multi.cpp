#include <stdio.h>
#include <memory.h>
#include "xbyak/xbyak_util.h"
extern Xbyak::util::Clock sclk;
#include "bn.h"
#include "bn-multi.h"
#include <iostream>
#include "util.h"

#define NUM_OF_ARRAY(x) (sizeof(x) / sizeof(*x))

using namespace bn;
using namespace ecop;

const mie::Vuint& p = Param::p;
const mie::Vuint& r = Param::r;

int g_count_m256;
int g_count_r512;
int g_count_add256;

void setQn(Fp2 Qn[8][3])
{
	Qn[0][0]=Fp2(Fp("11517672169287316361176188794386139376898724198982915810069006199968038420191"),Fp("10835361915836038157486987955558731390170808906118754364538491386961053655274"));
	Qn[0][1]=Fp2(Fp("1778069034406611548660214861031659277772705043859539974021985347836919469866"),Fp("13681662439753864346331726398353760699598198306861051081413655381467995631936"));
	Qn[0][2]=Fp2(Fp("1"),Fp("0"));

	Qn[1][0]=Fp2(Fp("11390001546001864402600540835220615821206485289741226204852043657103214655977"),Fp("213486898011631016451079100860803230206220441184056952439876131775418552592"));
	Qn[1][1]=Fp2(Fp("5965151828180680581929387432038466302456180976708258052725425526886445195733"),Fp("12702697697694570150381325292928077349338690254734620977056113962521966534889"));
	Qn[1][2]=Fp2(Fp("1"),Fp("0"));

	Qn[2][0]=Fp2(Fp("317827852388030917234217793035893093021483955938125136386577929112148939582"),Fp("9278896717834347780025960489604748790341527699671814896844481527020420813618"));
	Qn[2][1]=Fp2(Fp("4604308811892940474377307346344747500789112678674827642409858197234913520861"),Fp("1070757109492256048282468809751283274011240272126145201336641668031831023352"));
	Qn[2][2]=Fp2(Fp("1"),Fp("0"));

	Qn[3][0]=Fp2(Fp("15141009173450567653529452777511404553752377160725217686850520933697466610083"),Fp("16629468586199856290567823389372902473847532496895550111471616660495281795606"));
	Qn[3][1]=Fp2(Fp("16359294718145568711088295546349667627209864787890529421842732282659027460110"),Fp("4591971789740803293911582763487966490224517350021416286195900260588416606065"));
	Qn[3][2]=Fp2(Fp("1"),Fp("0"));

	Qn[4][0]=Fp2(Fp("10488487177327651775576239839542683920101411453666780842090527101415731001540"),Fp("2602939798302577793362090318279109373481010951639935585239213420440620860256"));
	Qn[4][1]=Fp2(Fp("9863609696075612874186230195435829835812764055187360738836832136664960165343"),Fp("9165168201048188946636094951894632779149730577982455486230427162387564949929"));
	Qn[4][2]=Fp2(Fp("1"),Fp("0"));

	Qn[5][0]=Fp2(Fp("5292072187326998897140340417009249180463237659578606571855121284655134215860"),Fp("10689661392573589494547928414723411741556083467963111340842112372714719466139"));
	Qn[5][1]=Fp2(Fp("10086838226925579349559952786276070806316934197051675278290874521792223694104"),Fp("13667433560602011720585073415592848192740319254215300890305616288368916516198"));
	Qn[5][2]=Fp2(Fp("1"),Fp("0"));

	Qn[6][0]=Fp2(Fp("3324985573750603904527543011287473872994996187451653207674213275952327637416"),Fp("11720559305674136650678029454774125211095289377686252974953815151770225790606"));
	Qn[6][1]=Fp2(Fp("16223977577427149050185260579094435839305544920515759664940268454263915683853"),Fp("11626391287612250863586482028776561221723650762976964634697768152047450671751"));
	Qn[6][2]=Fp2(Fp("1"),Fp("0"));

	Qn[7][0]=Fp2(Fp("11092045417843901853363139904491331290984929202254355616261729225837031249105"),Fp("16375618814835183841514709507215753591504289965694221131243608575439086917851"));
	Qn[7][1]=Fp2(Fp("11975761771951146049662473814541920209508901398043990032621984937015757049692"),Fp("9935470201967315109600907913103514245507786763785970745954141872305198444941"));
	Qn[7][2]=Fp2(Fp("1"),Fp("0"));
}

void setPn(Fp Pn[8][3])
{
	const char *tbl[24] = {
		"15005210600138052046451246658259670676477715213603446606994004703153823368243", "4343577407084448292832047972972645626170560205031956773324523863608486249664", "1",
		"15457516573829718488435745408064186554479833481222785826769940346975045420508", "2364160699533534328839631410506819262600701890275670413410946410280478989668", "1",
		"15003436906848848651695496307412705267730348433833218834960455592435929249345", "10438564479059694929228270049917726710497181841089954820494133901102091831646", "1",
		"14972172091841485293055489162518763468703573114581772138812357514357617047461", "15977643626927900448562211593960826565665302700760283301009127977689582843731", "1",
		"15003436851221616142043118946721100720407021250819020617903326552093126299817", "10454848416660761219292910571291338353428640207324163522494422111725972067818", "1",
		"15455749699910633167624727292022947052630250106502406644415113459134577275001", "4855673267417630607411742108134371885186924557679407788265067025515460883257", "1",
		"15457516574066288719995335683976973092876237104757111162798583482630975970975", "15911868723492233724159130363827027376562792523950861658431560892650986377042", "1",
		"15457523448721782653342827839546724569763830282323659028716247514367541280549", "10929099066851842018351837306432073284036718863229651547413986283505570608536", "1",
	};
	Fp *p = &Pn[8][3];
	for (int i = 0; i < 24; i++) {
		p[i].set(tbl[i]);
	}
}

void testPairing()
{
	Fp2 Qn[8][3];
	Fp Pn[8][3];
	Fp6 l_T[8][70];
	Fp12 e;

	setQn(Qn);
	setPn(Pn);
	{
		Fp6 *p = &l_T[0][0];
		for (int i = 0; i < 8 * 70; i++) {
			p[i] = i + 1;
		}
	}
#if 0
	double pre = 0;
	for (int i = 1; i < 8; i++) {
		Xbyak::util::Clock clk;
		clk.begin();
		const int N = 3000;
		for (int j = 0; j < N; j++) {
			bn::opt_atePairingKnownG2Mixed(e, i, l_T, 1, Qn, Pn);
		}
		clk.end();
		double t = clk.getClock() / double(N) / 1e3;
		printf("pairing %d %7.2fKclk %7.2fKclk\n", i, t, t - pre);
		pre = t;
	}
#endif
#if 1
	{
		int c1[9];
		int c2[9];
		int c3[9];

		for (int i = 1; i < 8; i++) {
			g_count_m256 = 0;
			g_count_r512 = 0;
			g_count_add256 = 0;
//			opt_ateMultiPairingJ(e, i, Qn, Pn);
			bn::opt_atePairingKnownG2Mixed(e, i, l_T, 1, Qn, Pn);
			c1[i] = g_count_m256;
			c2[i] = g_count_r512;
			c3[i] = g_count_add256;
			printf("%d m=%d r=%d a=%d\n", i, g_count_m256, g_count_r512, g_count_add256);
		}
		puts("extra");
		for (int i = 1; i < 7; i++) {
			printf("%d %d %d\n", c1[i + 1] - c1[i], c2[i + 1] - c2[i], c3[i + 1] - c3[i]);
		}
		int m = c1[3] - c1[2];
		int r = c2[3] - c2[2];
		int a = c3[3] - c3[2];
		int em = 3526;
		int er = 1932;
		int ea = 18328;
		{
			g_count_m256 = 0;
			g_count_r512 = 0;
			g_count_add256 = 0;
			e.final_exp();
			em = g_count_m256;
			er = g_count_r512;
			ea = g_count_add256;
		}
		printf("(%dm + %dr + %da) * N + EXP + %dm + %dr + %da\n"
			, m, r, a
			, c1[2] - 2 * m - em, c2[2] - 2 * r - er, c3[2] - 2 * a - ea);
		printf("EXP = %dm + %dr + %da\n", em, er, ea);
	}
#endif
}

int main(int argc, char *argv[]) try
{
	argc--, argv++;
	Param::init(-1);
	testPairing();

	return 0;
} catch (Xbyak::Error err) {
	fprintf(stderr, "ExpCode ERR:%s(%d)\n", Xbyak::ConvertErrorToString(err), err);
	return 1;
} catch (std::exception& e) {
	fprintf(stderr, "std::exception %s\n", e.what());
	return 1;
}
