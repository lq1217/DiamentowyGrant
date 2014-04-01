/*
 * Copyright (c) 2003, 2007-8 Matteo Frigo
 * Copyright (c) 2003, 2007-8 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sun Jul 12 06:44:32 EDT 2009 */

#include "../../codelet-rdft.h"

#ifdef HAVE_FMA

/* Generated by: ../../../genfft/gen_r2cf -fma -reorder-insns -schedule-for-pipeline -compact -variables 4 -pipeline-latency 4 -n 20 -name r2cfII_20 -dft-II -include r2cfII.h */

/*
 * This function contains 102 FP additions, 63 FP multiplications,
 * (or, 39 additions, 0 multiplications, 63 fused multiply/add),
 * 67 stack variables, 10 constants, and 40 memory accesses
 */
#include "../r2cfII.h"

static void r2cfII_20(float *R0, float *R1, float *Cr, float *Ci, stride rs, stride csr, stride csi, INT v, INT ivs, INT ovs)
{
     DK(KP707106781, +0.707106781186547524400844362104849039284835938);
     DK(KP951056516, +0.951056516295153572116439333379382143405698634);
     DK(KP559016994, +0.559016994374947424102293417182819058860154590);
     DK(KP690983005, +0.690983005625052575897706582817180941139845410);
     DK(KP552786404, +0.552786404500042060718165266253744752911876328);
     DK(KP447213595, +0.447213595499957939281834733746255247088123672);
     DK(KP809016994, +0.809016994374947424102293417182819058860154590);
     DK(KP250000000, +0.250000000000000000000000000000000000000000000);
     DK(KP618033988, +0.618033988749894848204586834365638117720309180);
     DK(KP381966011, +0.381966011250105151795413165634361882279690820);
     INT i;
     for (i = v; i > 0; i = i - 1, R0 = R0 + ivs, R1 = R1 + ivs, Cr = Cr + ovs, Ci = Ci + ovs, MAKE_VOLATILE_STRIDE(rs), MAKE_VOLATILE_STRIDE(csr), MAKE_VOLATILE_STRIDE(csi)) {
	  E Tv, TK, TN, Th, T1l, T1n, Ts, TH;
	  {
	       E Ti, T1d, T1f, T1e, T1g, T1p, TS, Tg, To, T8, T7, T19, T1r, T1k, Tx;
	       E Tp, TX, Ty, TF, Tr, TV, Tz, TA, TI;
	       {
		    E Ta, Tb, Td, Te;
		    Ti = R1[WS(rs, 2)];
		    T1d = R0[WS(rs, 5)];
		    Ta = R0[WS(rs, 9)];
		    Tb = R0[WS(rs, 1)];
		    Td = R0[WS(rs, 3)];
		    Te = R0[WS(rs, 7)];
		    {
			 E T1, T2, T5, T3, T4, T1i, Tc, Tf;
			 T1 = R0[0];
			 T1f = Ta + Tb;
			 Tc = Ta - Tb;
			 T1e = Td + Te;
			 Tf = Td - Te;
			 T2 = R0[WS(rs, 4)];
			 T5 = R0[WS(rs, 6)];
			 T1g = FMA(KP381966011, T1f, T1e);
			 T1p = FMA(KP381966011, T1e, T1f);
			 TS = FMA(KP618033988, Tc, Tf);
			 Tg = FNMS(KP618033988, Tf, Tc);
			 T3 = R0[WS(rs, 8)];
			 T4 = R0[WS(rs, 2)];
			 T1i = T2 + T5;
			 {
			      E Tj, Tu, Tm, Tt, Tn, Tq, TU;
			      Tj = R1[WS(rs, 8)];
			      To = R1[WS(rs, 6)];
			      {
				   E T6, T1j, Tk, Tl;
				   T6 = T2 + T3 - T4 - T5;
				   T8 = (T3 + T5 - T2) - T4;
				   T1j = T3 + T4;
				   Tk = R1[0];
				   Tl = R1[WS(rs, 4)];
				   T7 = FNMS(KP250000000, T6, T1);
				   T19 = T1 + T6;
				   T1r = FNMS(KP618033988, T1i, T1j);
				   T1k = FMA(KP618033988, T1j, T1i);
				   Tu = Tk - Tl;
				   Tm = Tk + Tl;
			      }
			      Tt = To + Tj;
			      Tx = R1[WS(rs, 7)];
			      Tn = Tj - Tm;
			      Tp = Tj + Tm;
			      Tv = FNMS(KP618033988, Tu, Tt);
			      TX = FMA(KP618033988, Tt, Tu);
			      Tq = FMA(KP809016994, Tp, To);
			      TU = FMA(KP447213595, Tp, Tn);
			      Ty = R1[WS(rs, 1)];
			      TF = R1[WS(rs, 3)];
			      Tr = FNMS(KP552786404, Tq, Tn);
			      TV = FNMS(KP690983005, TU, To);
			      Tz = R1[WS(rs, 5)];
			      TA = R1[WS(rs, 9)];
			      TI = TF + Ty;
			 }
		    }
	       }
	       {
		    E T1w, TJ, TB, T1a;
		    T1w = T1f + T1d - T1e;
		    TJ = Tz - TA;
		    TB = Tz + TA;
		    T1a = Ti + To - Tp;
		    {
			 E T9, T12, TT, T15, TG, TD, T1s, T1u, TW, T11, T10, T1h;
			 {
			      E TE, TC, TR, T1b;
			      T9 = FNMS(KP559016994, T8, T7);
			      TR = FMA(KP559016994, T8, T7);
			      TK = FMA(KP618033988, TJ, TI);
			      T12 = FNMS(KP618033988, TI, TJ);
			      TE = Ty - TB;
			      TC = Ty + TB;
			      TT = FMA(KP951056516, TS, TR);
			      T15 = FNMS(KP951056516, TS, TR);
			      TG = FNMS(KP552786404, TF, TE);
			      T1b = TC - TF - Tx;
			      {
				   E TZ, T1q, T1c, T1x;
				   TZ = FMA(KP447213595, TC, TE);
				   TD = FMA(KP250000000, TC, Tx);
				   T1q = FNMS(KP809016994, T1p, T1d);
				   T1c = T1a + T1b;
				   T1x = T1a - T1b;
				   T10 = FNMS(KP690983005, TZ, TF);
				   T1s = FNMS(KP951056516, T1r, T1q);
				   T1u = FMA(KP951056516, T1r, T1q);
				   Ci[WS(csi, 7)] = FMA(KP707106781, T1x, T1w);
				   Ci[WS(csi, 2)] = FMS(KP707106781, T1x, T1w);
				   Cr[WS(csr, 7)] = FMA(KP707106781, T1c, T19);
				   Cr[WS(csr, 2)] = FNMS(KP707106781, T1c, T19);
			      }
			 }
			 TW = FNMS(KP809016994, TV, Ti);
			 T11 = FNMS(KP809016994, T10, Tx);
			 T1h = FMA(KP809016994, T1g, T1d);
			 {
			      E T17, TY, T16, T13;
			      T17 = FNMS(KP951056516, TX, TW);
			      TY = FMA(KP951056516, TX, TW);
			      T16 = FMA(KP951056516, T12, T11);
			      T13 = FNMS(KP951056516, T12, T11);
			      TN = FMA(KP951056516, Tg, T9);
			      Th = FNMS(KP951056516, Tg, T9);
			      {
				   E T18, T1v, T1t, T14;
				   T18 = T16 - T17;
				   T1v = T17 + T16;
				   T1t = TY + T13;
				   T14 = TY - T13;
				   Cr[WS(csr, 1)] = FMA(KP707106781, T18, T15);
				   Cr[WS(csr, 8)] = FNMS(KP707106781, T18, T15);
				   Ci[WS(csi, 3)] = FMA(KP707106781, T1v, T1u);
				   Ci[WS(csi, 6)] = FMS(KP707106781, T1v, T1u);
				   Ci[WS(csi, 1)] = FNMS(KP707106781, T1t, T1s);
				   Ci[WS(csi, 8)] = -(FMA(KP707106781, T1t, T1s));
				   Cr[WS(csr, 3)] = FMA(KP707106781, T14, TT);
				   Cr[WS(csr, 6)] = FNMS(KP707106781, T14, TT);
				   T1l = FMA(KP951056516, T1k, T1h);
				   T1n = FNMS(KP951056516, T1k, T1h);
			      }
			 }
			 Ts = FNMS(KP559016994, Tr, Ti);
			 TH = FNMS(KP559016994, TG, TD);
		    }
	       }
	  }
	  {
	       E TO, Tw, TP, TL;
	       TO = FMA(KP951056516, Tv, Ts);
	       Tw = FNMS(KP951056516, Tv, Ts);
	       TP = FMA(KP951056516, TK, TH);
	       TL = FNMS(KP951056516, TK, TH);
	       {
		    E TQ, T1m, T1o, TM;
		    TQ = TO - TP;
		    T1m = TO + TP;
		    T1o = Tw + TL;
		    TM = Tw - TL;
		    Cr[WS(csr, 4)] = FMA(KP707106781, TQ, TN);
		    Cr[WS(csr, 5)] = FNMS(KP707106781, TQ, TN);
		    Ci[WS(csi, 9)] = FNMS(KP707106781, T1m, T1l);
		    Ci[0] = -(FMA(KP707106781, T1m, T1l));
		    Ci[WS(csi, 5)] = FNMS(KP707106781, T1o, T1n);
		    Ci[WS(csi, 4)] = -(FMA(KP707106781, T1o, T1n));
		    Cr[0] = FMA(KP707106781, TM, Th);
		    Cr[WS(csr, 9)] = FNMS(KP707106781, TM, Th);
	       }
	  }
     }
}

static const kr2c_desc desc = { 20, "r2cfII_20", {39, 0, 63, 0}, &fftwf_rdft_r2cfII_genus };

void fftwf_codelet_r2cfII_20 (planner *p) {
     fftwf_kr2c_register (p, r2cfII_20, &desc);
}

#else				/* HAVE_FMA */

/* Generated by: ../../../genfft/gen_r2cf -compact -variables 4 -pipeline-latency 4 -n 20 -name r2cfII_20 -dft-II -include r2cfII.h */

/*
 * This function contains 102 FP additions, 34 FP multiplications,
 * (or, 86 additions, 18 multiplications, 16 fused multiply/add),
 * 60 stack variables, 13 constants, and 40 memory accesses
 */
#include "../r2cfII.h"

static void r2cfII_20(float *R0, float *R1, float *Cr, float *Ci, stride rs, stride csr, stride csi, INT v, INT ivs, INT ovs)
{
     DK(KP572061402, +0.572061402817684297600072783580302076536153377);
     DK(KP218508012, +0.218508012224410535399650602527877556893735408);
     DK(KP309016994, +0.309016994374947424102293417182819058860154590);
     DK(KP809016994, +0.809016994374947424102293417182819058860154590);
     DK(KP559016994, +0.559016994374947424102293417182819058860154590);
     DK(KP951056516, +0.951056516295153572116439333379382143405698634);
     DK(KP587785252, +0.587785252292473129168705954639072768597652438);
     DK(KP250000000, +0.250000000000000000000000000000000000000000000);
     DK(KP176776695, +0.176776695296636881100211090526212259821208984);
     DK(KP395284707, +0.395284707521047416499861693054089816714944392);
     DK(KP672498511, +0.672498511963957326960058968885748755876783111);
     DK(KP415626937, +0.415626937777453428589967464113135184222253485);
     DK(KP707106781, +0.707106781186547524400844362104849039284835938);
     INT i;
     for (i = v; i > 0; i = i - 1, R0 = R0 + ivs, R1 = R1 + ivs, Cr = Cr + ovs, Ci = Ci + ovs, MAKE_VOLATILE_STRIDE(rs), MAKE_VOLATILE_STRIDE(csr), MAKE_VOLATILE_STRIDE(csi)) {
	  E T8, TD, Tm, TN, T9, TC, TY, TE, Te, TF, Tl, TK, T12, TL, Tk;
	  E TM, T1, T6, Tq, T1l, T1c, Tp, T1f, T1e, T1d, Ty, TW, T1g, T1m, Tx;
	  E Tu;
	  T8 = R1[WS(rs, 2)];
	  TD = KP707106781 * T8;
	  Tm = R1[WS(rs, 7)];
	  TN = KP707106781 * Tm;
	  {
	       E Ta, TA, Td, TB, Tb, Tc;
	       T9 = R1[WS(rs, 6)];
	       Ta = R1[WS(rs, 8)];
	       TA = T9 + Ta;
	       Tb = R1[0];
	       Tc = R1[WS(rs, 4)];
	       Td = Tb + Tc;
	       TB = Tb - Tc;
	       TC = FMA(KP415626937, TA, KP672498511 * TB);
	       TY = FNMS(KP415626937, TB, KP672498511 * TA);
	       TE = KP395284707 * (Ta - Td);
	       Te = Ta + Td;
	       TF = KP176776695 * Te;
	  }
	  {
	       E Tg, TJ, Tj, TI, Th, Ti;
	       Tg = R1[WS(rs, 1)];
	       Tl = R1[WS(rs, 3)];
	       TJ = Tg + Tl;
	       Th = R1[WS(rs, 5)];
	       Ti = R1[WS(rs, 9)];
	       Tj = Th + Ti;
	       TI = Th - Ti;
	       TK = FNMS(KP415626937, TJ, KP672498511 * TI);
	       T12 = FMA(KP415626937, TI, KP672498511 * TJ);
	       TL = KP395284707 * (Tg - Tj);
	       Tk = Tg + Tj;
	       TM = KP176776695 * Tk;
	  }
	  {
	       E T2, T5, T3, T4, T1a, T1b;
	       T1 = R0[0];
	       T2 = R0[WS(rs, 6)];
	       T5 = R0[WS(rs, 8)];
	       T3 = R0[WS(rs, 2)];
	       T4 = R0[WS(rs, 4)];
	       T1a = T4 + T2;
	       T1b = T5 + T3;
	       T6 = T2 + T3 - (T4 + T5);
	       Tq = FMA(KP250000000, T6, T1);
	       T1l = FNMS(KP951056516, T1b, KP587785252 * T1a);
	       T1c = FMA(KP951056516, T1a, KP587785252 * T1b);
	       Tp = KP559016994 * (T5 + T2 - (T4 + T3));
	  }
	  T1f = R0[WS(rs, 5)];
	  {
	       E Tv, Tw, Ts, Tt;
	       Tv = R0[WS(rs, 9)];
	       Tw = R0[WS(rs, 1)];
	       Tx = Tv - Tw;
	       T1e = Tv + Tw;
	       Ts = R0[WS(rs, 3)];
	       Tt = R0[WS(rs, 7)];
	       Tu = Ts - Tt;
	       T1d = Ts + Tt;
	  }
	  Ty = FMA(KP951056516, Tu, KP587785252 * Tx);
	  TW = FNMS(KP951056516, Tx, KP587785252 * Tu);
	  T1g = FMA(KP809016994, T1d, KP309016994 * T1e) + T1f;
	  T1m = FNMS(KP809016994, T1e, T1f) - (KP309016994 * T1d);
	  {
	       E T7, T1r, To, T1q, Tf, Tn;
	       T7 = T1 - T6;
	       T1r = T1e + T1f - T1d;
	       Tf = T8 + (T9 - Te);
	       Tn = (Tk - Tl) - Tm;
	       To = KP707106781 * (Tf + Tn);
	       T1q = KP707106781 * (Tf - Tn);
	       Cr[WS(csr, 2)] = T7 - To;
	       Ci[WS(csi, 2)] = T1q - T1r;
	       Cr[WS(csr, 7)] = T7 + To;
	       Ci[WS(csi, 7)] = T1q + T1r;
	  }
	  {
	       E T1h, T1j, TX, T15, T10, T16, T13, T17, TV, TZ, T11;
	       T1h = T1c - T1g;
	       T1j = T1c + T1g;
	       TV = Tq - Tp;
	       TX = TV - TW;
	       T15 = TV + TW;
	       TZ = FMA(KP218508012, T9, TD) + TF - TE;
	       T10 = TY + TZ;
	       T16 = TZ - TY;
	       T11 = FNMS(KP218508012, Tl, TL) - (TM + TN);
	       T13 = T11 - T12;
	       T17 = T11 + T12;
	       {
		    E T14, T19, T18, T1i;
		    T14 = T10 + T13;
		    Cr[WS(csr, 5)] = TX - T14;
		    Cr[WS(csr, 4)] = TX + T14;
		    T19 = T17 - T16;
		    Ci[WS(csi, 5)] = T19 - T1h;
		    Ci[WS(csi, 4)] = T19 + T1h;
		    T18 = T16 + T17;
		    Cr[WS(csr, 9)] = T15 - T18;
		    Cr[0] = T15 + T18;
		    T1i = T13 - T10;
		    Ci[0] = T1i - T1j;
		    Ci[WS(csi, 9)] = T1i + T1j;
	       }
	  }
	  {
	       E T1n, T1p, Tz, TR, TH, TS, TP, TT, Tr, TG, TO;
	       T1n = T1l + T1m;
	       T1p = T1m - T1l;
	       Tr = Tp + Tq;
	       Tz = Tr + Ty;
	       TR = Tr - Ty;
	       TG = TD + TE + FNMS(KP572061402, T9, TF);
	       TH = TC + TG;
	       TS = TC - TG;
	       TO = TL + TM + FNMS(KP572061402, Tl, TN);
	       TP = TK - TO;
	       TT = TK + TO;
	       {
		    E TQ, T1o, TU, T1k;
		    TQ = TH + TP;
		    Cr[WS(csr, 6)] = Tz - TQ;
		    Cr[WS(csr, 3)] = Tz + TQ;
		    T1o = TT - TS;
		    Ci[WS(csi, 6)] = T1o - T1p;
		    Ci[WS(csi, 3)] = T1o + T1p;
		    TU = TS + TT;
		    Cr[WS(csr, 8)] = TR - TU;
		    Cr[WS(csr, 1)] = TR + TU;
		    T1k = TP - TH;
		    Ci[WS(csi, 8)] = T1k - T1n;
		    Ci[WS(csi, 1)] = T1k + T1n;
	       }
	  }
     }
}

static const kr2c_desc desc = { 20, "r2cfII_20", {86, 18, 16, 0}, &fftwf_rdft_r2cfII_genus };

void fftwf_codelet_r2cfII_20 (planner *p) {
     fftwf_kr2c_register (p, r2cfII_20, &desc);
}

#endif				/* HAVE_FMA */