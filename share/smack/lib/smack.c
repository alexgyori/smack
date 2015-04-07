//
// This file is distributed under the MIT License. See LICENSE for details.

#include "smack.h"

/**
 * The SMACK "prelude" definitions
 *
 * TODO add more documentation
 *
 * NOTES ON MEMORY MODELS
 *
 * 1. addresses are (unbounded) integers
 * 2. one unbounded integer is stored at each address
 * 3. (NO-REUSE only) heap addresses are allocated in a strictly increasing
 *    fashion
 * 4. (NO-REUSE only) freed (heap) addresses are never reallocated
 * 5. the address space is partitioned as follows
 *
 * Address A                                        Allocation
 * ---------                                        ----------
 * A > 0                                            Heap
 * A = 0                                            Not allocated (null)
 * $GLOBALS_BOTTOM <= A < 0                         Static (global storage)
 * $GLOBALS_BOTTOM - 32768 < A < $GLOBALS_BOTTOM    Not allocated (padding)
 * A < $GLOBALS_BOTTOM - 32768                      External
 *
 */

void __SMACK_dummy(int v) {
  __SMACK_code("assume true;");
}

#define xstr(s) str(s)
#define str(s) #s
#define foo(x) 4 ## x

#define UNINTERPRETED_UNARY_OP(type,name) \
  function name.type(p: type) returns (type);

#define UNINTERPRETED_BINARY_OP(type,name) \
  function name.type(p1: type, p2: type) returns (type);

#define UNINTERPRETED_BINARY_PRED(type,name) \
  function name.type(p1: type, p2: type) returns (i1);

#define INLINE_UNARY_OP(type,name,body) \
  function {:inline} name.type(p: type) returns (type) body

#define INLINE_BINARY_OP(type,name,body) \
  function {:inline} name.type(p1: type, p2: type) returns (type) body

#define INLINE_BINARY_PRED(type,name,body) \
  function {:inline} name.type(p1: type, p2: type) returns (i1) body

#define INLINE_CONVERSION(t1,t2,name,body) \
  function {:inline} name.t1.t2(p: t1) returns (t2) body

#define BUILTIN_UNARY_OP(type,name,prim) \
  function {:builtin xstr(prim)} name.type(p: type) returns (type);

#define BUILTIN_BINARY_OP(type,name,prim) \
  function {:builtin xstr(prim)} name.type(p1: type, p2: type) returns (type);

#define BUILTIN_BINARY_PRED(type,name,prim) \
  function {:builtin xstr(prim)} name.type(p1: type, p2: type) returns (i1);

#define BVBUILTIN_UNARY_OP(type,name,prim) \
  function {:bvbuiltin xstr(prim)} name.type(p: type) returns (type);

#define BVBUILTIN_BINARY_OP(type,name,prim) \
  function {:bvbuiltin xstr(prim)} name.type(p1: type, p2: type) returns (type);

#define BVBUILTIN_BINARY_PRED(type,name,prim) \
  function {:bvbuiltin xstr(prim)} name.type(p1: type, p2: type) returns (i1);

#define INLINE_BVBUILTIN_BINARY_PRED(type,name,prim) \
  function {:bvbuiltin xstr(prim)} name.type.bi(p1: type, p2: type) returns (bool); \
  function {:inline} name.type(p1: type, p2: type) returns (i1) {if name.type.bi(p1,p2) then 1bv1 else 0bv1}

#define D(d) __SMACK_top_decl(d)

#define DECLARE_EACH_TYPE(M,args...) \
  D(xstr(M(ref,args))); \
  D(xstr(M(i64,args))); \
  D(xstr(M(i32,args))); \
  D(xstr(M(i16,args))); \
  D(xstr(M(i8,args))); \
  D(xstr(M(i1,args)));

void __SMACK_decls() {

  D("const $TRUE, $FALSE: i1;");
  D("const $NULL: ref;");
  D("const $REF_CONST_1: ref;");
  D("const $REF_CONST_2: ref;");
  D("const $REF_CONST_3: ref;");
  D("const $REF_CONST_4: ref;");
  D("const $REF_CONST_5: ref;");
  D("const $REF_CONST_6: ref;");
  D("const $REF_CONST_7: ref;");

#ifdef BITPRECISE

  D("axiom $TRUE == 1bv1;");
  D("axiom $FALSE == 0bv1;");

  DECLARE_EACH_TYPE(BVBUILTIN_UNARY_OP, $neg, bvneg)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $add, bvadd)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $sub, bvsub)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $mul, bvmul)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $udiv, bvudiv)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $sdiv, bvsdiv)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $smod, bvsmod)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $srem, bvsrem)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $urem, bvurem)

  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $shl, bvshl)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $lshr, bvlshr)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $ashr, bvashr)
  DECLARE_EACH_TYPE(BVBUILTIN_UNARY_OP, $not, bvnot)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $and, bvand)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $or, bvor)
  DECLARE_EACH_TYPE(BVBUILTIN_BINARY_OP, $xor, bvxor)

  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $eq, {if p1 == p2 then 1bv1 else 0bv1})
  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $ne, {if p1 != p2 then 1bv1 else 0bv1})
  DECLARE_EACH_TYPE(INLINE_BVBUILTIN_BINARY_PRED, $ule, bvule)
  DECLARE_EACH_TYPE(INLINE_BVBUILTIN_BINARY_PRED, $ult, bvult)
  DECLARE_EACH_TYPE(INLINE_BVBUILTIN_BINARY_PRED, $uge, bvuge)
  DECLARE_EACH_TYPE(INLINE_BVBUILTIN_BINARY_PRED, $ugt, bvugt)
  DECLARE_EACH_TYPE(INLINE_BVBUILTIN_BINARY_PRED, $sle, bvsle)
  DECLARE_EACH_TYPE(INLINE_BVBUILTIN_BINARY_PRED, $slt, bvslt)
  DECLARE_EACH_TYPE(INLINE_BVBUILTIN_BINARY_PRED, $sge, bvsge)
  DECLARE_EACH_TYPE(INLINE_BVBUILTIN_BINARY_PRED, $sgt, bvsgt)

  D(xstr(INLINE_CONVERSION(i64,i32,$trunc,{p[32:0]})));
  D(xstr(INLINE_CONVERSION(i64,i16,$trunc,{p[16:0]})));
  D(xstr(INLINE_CONVERSION(i64,i8,$trunc,{p[8:0]})));
  D(xstr(INLINE_CONVERSION(i64,i1,$trunc,{if p == 0bv64 then 0bv1 else 1bv1})));
  D(xstr(INLINE_CONVERSION(i32,i16,$trunc,{p[16:0]})));
  D(xstr(INLINE_CONVERSION(i32,i8,$trunc,{p[8:0]})));
  D(xstr(INLINE_CONVERSION(i32,i1,$trunc,{if p == 0bv32 then 0bv1 else 1bv1})));
  D(xstr(INLINE_CONVERSION(i16,i8,$trunc,{p[8:0]})));
  D(xstr(INLINE_CONVERSION(i16,i1,$trunc,{if p == 0bv16 then 0bv1 else 1bv1})));
  D(xstr(INLINE_CONVERSION(i8,i1,$trunc,{if p == 0bv8 then 0bv1 else 1bv1})));

  D(xstr(INLINE_CONVERSION(i1,i8,$zext,{if p == 0bv1 then 0bv8 else 1bv8})));
  D(xstr(INLINE_CONVERSION(i1,i16,$zext,{if p == 0bv1 then 0bv16 else 1bv16})));
  D(xstr(INLINE_CONVERSION(i1,i32,$zext,{if p == 0bv1 then 0bv32 else 1bv32})));
  D(xstr(INLINE_CONVERSION(i1,i64,$zext,{if p == 0bv1 then 0bv64 else 1bv64})));
  D(xstr(INLINE_CONVERSION(i8,i16,$zext,{0bv8 ++ p})));
  D(xstr(INLINE_CONVERSION(i8,i32,$zext,{0bv24 ++ p})));
  D(xstr(INLINE_CONVERSION(i8,i64,$zext,{0bv56 ++ p})));
  D(xstr(INLINE_CONVERSION(i16,i32,$zext,{0bv16 ++ p})));
  D(xstr(INLINE_CONVERSION(i16,i64,$zext,{0bv48 ++ p})));
  D(xstr(INLINE_CONVERSION(i32,i64,$zext,{0bv32 ++ p})));

  D(xstr(INLINE_CONVERSION(i1,i8,$sext,{if p == 0bv1 then 0bv8 else 1bv8})));
  D(xstr(INLINE_CONVERSION(i1,i16,$sext,{if p == 0bv1 then 0bv16 else 1bv16})));
  D(xstr(INLINE_CONVERSION(i1,i32,$sext,{if p == 0bv1 then 0bv32 else 1bv32})));
  D(xstr(INLINE_CONVERSION(i1,i64,$sext,{if p == 0bv1 then 0bv64 else 1bv64})));
  D(xstr(INLINE_CONVERSION(i8,i16,$sext,{if $sge.i8.bi(p, 0bv8) then $zext.i8.i16(p) else $neg.i8(1bv8) ++ p})));
  D(xstr(INLINE_CONVERSION(i8,i32,$sext,{if $sge.i8.bi(p, 0bv8) then $zext.i8.i32(p) else $neg.i32(1bv32)[32:8] ++ p})));
  D(xstr(INLINE_CONVERSION(i8,i64,$sext,{if $sge.i8.bi(p, 0bv8) then $zext.i8.i64(p) else $neg.i64(1bv64)[64:8] ++ p})));
  D(xstr(INLINE_CONVERSION(i16,i32,$sext,{if $sge.i16.bi(p, 0bv16) then $zext.i16.i32(p) else $neg.i16(1bv16) ++ p})));
  D(xstr(INLINE_CONVERSION(i16,i64,$sext,{if $sge.i16.bi(p, 0bv16) then $zext.i16.i64(p) else $neg.i64(1bv64)[64:16] ++ p})));
  D(xstr(INLINE_CONVERSION(i32,i64,$sext,{if $sge.i32.bi(p, 0bv32) then $zext.i32.i64(p) else $neg.i32(1bv32) ++ p})));

  // TODO is this still used?
  D("function {:inline} $i2b(i: i1) returns (bool) {i != 0bv1}");
  D("function {:inline} $b2i(b: bool) returns (i1) {if b then 1bv1 else 0bv1}");

#else

  D("axiom $TRUE == 1;");
  D("axiom $FALSE == 0;");

  DECLARE_EACH_TYPE(INLINE_UNARY_OP, $neg, {0 - p})
  DECLARE_EACH_TYPE(INLINE_BINARY_OP, $add, {p1 + p2})
  DECLARE_EACH_TYPE(INLINE_BINARY_OP, $sub, {p1 - p2})
  DECLARE_EACH_TYPE(INLINE_BINARY_OP, $mul, {p1 * p2})
  DECLARE_EACH_TYPE(BUILTIN_BINARY_OP, $udiv, div)
  DECLARE_EACH_TYPE(BUILTIN_BINARY_OP, $sdiv, div)
  DECLARE_EACH_TYPE(BUILTIN_BINARY_OP, $smod, mod)
  DECLARE_EACH_TYPE(BUILTIN_BINARY_OP, $srem, rem)
  DECLARE_EACH_TYPE(BUILTIN_BINARY_OP, $urem, rem)

  DECLARE_EACH_TYPE(UNINTERPRETED_BINARY_OP, $shl)
  DECLARE_EACH_TYPE(UNINTERPRETED_BINARY_OP, $lshr)
  DECLARE_EACH_TYPE(UNINTERPRETED_BINARY_OP, $ashr)
  DECLARE_EACH_TYPE(UNINTERPRETED_UNARY_OP, $not)
  DECLARE_EACH_TYPE(UNINTERPRETED_BINARY_OP, $and)
  DECLARE_EACH_TYPE(UNINTERPRETED_BINARY_OP, $or)
  DECLARE_EACH_TYPE(UNINTERPRETED_BINARY_OP, $xor)

  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $eq, {if p1 == p2 then 1 else 0})
  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $ne, {if p1 != p2 then 1 else 0})
  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $ule, {if p1 <= p2 then 1 else 0})
  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $ult, {if p1 < p2 then 1 else 0})
  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $uge, {if p1 >= p2 then 1 else 0})
  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $ugt, {if p1 > p2 then 1 else 0})
  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $sle, {if p1 <= p2 then 1 else 0})
  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $slt, {if p1 < p2 then 1 else 0})
  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $sge, {if p1 >= p2 then 1 else 0})
  DECLARE_EACH_TYPE(INLINE_BINARY_PRED, $sgt, {if p1 > p2 then 1 else 0})

  D("axiom $and.i1(0,0) == 0;");
  D("axiom $and.i1(0,1) == 0;");
  D("axiom $and.i1(1,0) == 0;");
  D("axiom $and.i1(1,1) == 1;");
  D("axiom $or.i1(0,0) == 0;");
  D("axiom $or.i1(0,1) == 1;");
  D("axiom $or.i1(1,0) == 1;");
  D("axiom $or.i1(1,1) == 1;");
  D("axiom $xor.i1(0,0) == 0;");
  D("axiom $xor.i1(0,1) == 1;");
  D("axiom $xor.i1(1,0) == 1;");
  D("axiom $xor.i1(1,1) == 0;");

  D(xstr(INLINE_CONVERSION(i64,i32,$trunc,{p})));
  D(xstr(INLINE_CONVERSION(i64,i16,$trunc,{p})));
  D(xstr(INLINE_CONVERSION(i64,i8,$trunc,{p})));
  D(xstr(INLINE_CONVERSION(i64,i1,$trunc,{p})));
  D(xstr(INLINE_CONVERSION(i32,i16,$trunc,{p})));
  D(xstr(INLINE_CONVERSION(i32,i8,$trunc,{p})));
  D(xstr(INLINE_CONVERSION(i32,i1,$trunc,{p})));
  D(xstr(INLINE_CONVERSION(i16,i8,$trunc,{p})));
  D(xstr(INLINE_CONVERSION(i16,i1,$trunc,{p})));
  D(xstr(INLINE_CONVERSION(i8,i1,$trunc,{p})));

  D(xstr(INLINE_CONVERSION(i1,i8,$zext,{p})));
  D(xstr(INLINE_CONVERSION(i1,i16,$zext,{p})));
  D(xstr(INLINE_CONVERSION(i1,i32,$zext,{p})));
  D(xstr(INLINE_CONVERSION(i1,i64,$zext,{p})));
  D(xstr(INLINE_CONVERSION(i8,i16,$zext,{p})));
  D(xstr(INLINE_CONVERSION(i8,i32,$zext,{p})));
  D(xstr(INLINE_CONVERSION(i8,i64,$zext,{p})));
  D(xstr(INLINE_CONVERSION(i16,i32,$zext,{p})));
  D(xstr(INLINE_CONVERSION(i16,i64,$zext,{p})));
  D(xstr(INLINE_CONVERSION(i32,i64,$zext,{p})));

  D(xstr(INLINE_CONVERSION(i1,i8,$sext,{p})));
  D(xstr(INLINE_CONVERSION(i1,i16,$sext,{p})));
  D(xstr(INLINE_CONVERSION(i1,i32,$sext,{p})));
  D(xstr(INLINE_CONVERSION(i1,i64,$sext,{p})));
  D(xstr(INLINE_CONVERSION(i8,i16,$sext,{p})));
  D(xstr(INLINE_CONVERSION(i8,i32,$sext,{p})));
  D(xstr(INLINE_CONVERSION(i8,i64,$sext,{p})));
  D(xstr(INLINE_CONVERSION(i16,i32,$sext,{p})));
  D(xstr(INLINE_CONVERSION(i16,i64,$sext,{p})));
  D(xstr(INLINE_CONVERSION(i32,i64,$sext,{p})));

  // TODO is this still used?
  D("function {:inline} $i2b(i: i1) returns (bool) {i != 0}");
  D("function {:inline} $b2i(b: bool) returns (i8) {if b then 1 else 0}");

#endif

  // TODO it’s not clear whether these are used.
  D("function $nand(p1:int, p2:int) returns (int);");
  D("function {:inline} $max(p1:int, p2:int) returns (int) {if p1 > p2 then p1 else p2}");
  D("function {:inline} $min(p1:int, p2:int) returns (int) {if p1 > p2 then p2 else p1}");
  D("function {:inline} $umax(p1:int, p2:int) returns (int) {if p1 > p2 then p1 else p2}");
  D("function {:inline} $umin(p1:int, p2:int) returns (int) {if p1 > p2 then p2 else p1}");

  D("function {:inline} $isExternal(p: ref) returns (bool) {$slt.ref(p,$EXTERNS_BOTTOM) == $TRUE}");

  D("type float;");
  D("function $fp(ipart:int, fpart:int, epart:int) returns (float);");
  D("function $fadd(f1:float, f2:float) returns (float);");
  D("function $fsub(f1:float, f2:float) returns (float);");
  D("function $fmul(f1:float, f2:float) returns (float);");
  D("function $fdiv(f1:float, f2:float) returns (float);");
  D("function $frem(f1:float, f2:float) returns (float);");
  D("function $ffalse(f1:float, f2:float) returns (i1);");
  D("function $ftrue(f1:float, f2:float) returns (i1);");
  D("function $foeq(f1:float, f2:float) returns (i1);");
  D("function $foge(f1:float, f2:float) returns (i1);");
  D("function $fogt(f1:float, f2:float) returns (i1);");
  D("function $fole(f1:float, f2:float) returns (i1);");
  D("function $folt(f1:float, f2:float) returns (i1);");
  D("function $fone(f1:float, f2:float) returns (i1);");
  D("function $ford(f1:float, f2:float) returns (i1);");
  D("function $fueq(f1:float, f2:float) returns (i1);");
  D("function $fuge(f1:float, f2:float) returns (i1);");
  D("function $fugt(f1:float, f2:float) returns (i1);");
  D("function $fule(f1:float, f2:float) returns (i1);");
  D("function $fult(f1:float, f2:float) returns (i1);");
  D("function $fune(f1:float, f2:float) returns (i1);");
  D("function $funo(f1:float, f2:float) returns (i1);");
  D("function $fp2si.i64(f:float) returns (i64);");
  D("function $fp2ui.i64(f:float) returns (i64);");
  D("function $si2fp.i64(i:i64) returns (float);");
  D("function $ui2fp.i64(i:i64) returns (float);");
  D("function $fp2si.i32(f:float) returns (i32);");
  D("function $fp2ui.i32(f:float) returns (i32);");
  D("function $si2fp.i32(i:i32) returns (float);");
  D("function $ui2fp.i32(i:i32) returns (float);");
  D("function $fp2si.i16(f:float) returns (i16);");
  D("function $fp2ui.i16(f:float) returns (i16);");
  D("function $si2fp.i16(i:i16) returns (float);");
  D("function $ui2fp.i16(i:i16) returns (float);");
  D("function $fp2si.i8(f:float) returns (i8);");
  D("function $fp2ui.i8(f:float) returns (i8);");
  D("function $si2fp.i8(i:i8) returns (float);");
  D("function $ui2fp.i8(i:i8) returns (float);");

  D("axiom (forall f1, f2: float :: f1 != f2 || $i2b($foeq(f1,f2)));");
  D("axiom (forall i: i64 :: $fp2ui.i64($ui2fp.i64(i)) == i);");
  D("axiom (forall f: float :: $ui2fp.i64($fp2ui.i64(f)) == f);");
  D("axiom (forall i: i64 :: $fp2si.i64($si2fp.i64(i)) == i);");
  D("axiom (forall f: float :: $si2fp.i64($fp2si.i64(f)) == f);");
  D("axiom (forall i: i32 :: $fp2ui.i32($ui2fp.i32(i)) == i);");
  D("axiom (forall f: float :: $ui2fp.i32($fp2ui.i32(f)) == f);");
  D("axiom (forall i: i32 :: $fp2si.i32($si2fp.i32(i)) == i);");
  D("axiom (forall f: float :: $si2fp.i32($fp2si.i32(f)) == f);");
  D("axiom (forall i: i16 :: $fp2ui.i16($ui2fp.i16(i)) == i);");
  D("axiom (forall f: float :: $ui2fp.i16($fp2ui.i16(f)) == f);");
  D("axiom (forall i: i16 :: $fp2si.i16($si2fp.i16(i)) == i);");
  D("axiom (forall f: float :: $si2fp.i16($fp2si.i16(f)) == f);");
  D("axiom (forall i: i8 :: $fp2ui.i8($ui2fp.i8(i)) == i);");
  D("axiom (forall f: float :: $ui2fp.i8($fp2ui.i8(f)) == f);");
  D("axiom (forall i: i8 :: $fp2si.i8($si2fp.i8(i)) == i);");
  D("axiom (forall f: float :: $si2fp.i8($fp2si.i8(f)) == f);");

  // Memory Model
  D("const $GLOBALS_BOTTOM: ref;");
  D("const $EXTERNS_BOTTOM: ref;");
  D("const $MALLOC_TOP: ref;");
  D("function $base(ref) returns (ref);");
  D("function {:inline} $b2p(b: bool) returns (ref) {if b then $REF_CONST_1 else $NULL}");
  D("function {:inline} $p2b(p: ref) returns (bool) {p != $NULL}");

#ifdef BITPRECISE
  D("function {:inline} $load.i64(M:[ref]i8, p:ref) returns (i64){$load.i32(M, $add.ref(p, $REF_CONST_4))++$load.i32(M, p)}");
  D("function {:inline} $load.i32(M:[ref]i8, p:ref) returns (i32){M[$add.ref(p, $REF_CONST_3)]++M[$add.ref(p, $REF_CONST_2)]++M[$add.ref(p, $REF_CONST_1)]++M[p]}");
  D("function {:inline} $load.i16(M:[ref]i8, p:ref) returns (i16){M[$add.ref(p, $REF_CONST_1)]++M[p]}");
  D("function {:inline} $load.i8(M:[ref]i8, p:ref) returns (i8){M[p]}");

  D("function {:inline} $store.i64(M:[ref]i8, p:ref, v:i64) returns ([ref]i8)"
    "{M[p := v[8:0]][$add.ref(p, $REF_CONST_1) := v[16:8]][$add.ref(p, $REF_CONST_2) := v[24:16]][$add.ref(p, $REF_CONST_3) := v[32:24]]"
    "[$add.ref(p, $REF_CONST_4) := v[40:32]][$add.ref(p, $REF_CONST_5) := v[48:40]][$add.ref(p, $REF_CONST_6) := v[56:48]][$add.ref(p, $REF_CONST_7) := v[64:56]]}");
  D("function {:inline} $store.i32(M:[ref]i8, p:ref, v:i32) returns ([ref]i8) {M[p := v[8:0]][$add.ref(p, $REF_CONST_1) := v[16:8]][$add.ref(p, $REF_CONST_2) := v[24:16]][$add.ref(p, $REF_CONST_3) := v[32:24]]}");
  D("function {:inline} $store.i16(M:[ref]i8, p:ref, v:i16) returns ([ref]i8) {M[p := v[8:0]][$add.ref(p, $REF_CONST_1) := v[16:8]]}");
  D("function {:inline} $store.i8(M:[ref]i8, p:ref, v:i8) returns ([ref]i8) {M[p := v]}");
#endif

  // Memory debugging symbols
  D("type $mop;");
  D("procedure boogie_si_record_mop(m: $mop);");
  D("procedure boogie_si_record_bool(b: bool);");
  D("procedure boogie_si_record_i1(i: i1);");
  D("procedure boogie_si_record_i8(i: i8);");
  D("procedure boogie_si_record_i16(i: i16);");
  D("procedure boogie_si_record_i32(i: i32);");
  D("procedure boogie_si_record_i64(i: i64);");
  D("procedure boogie_si_record_ref(i: ref);");
  D("procedure boogie_si_record_float(f: float);");
  D("const $MOP: $mop;");

#if MEMORY_MODEL_NO_REUSE_IMPLS
  D("var $Alloc: [ref] bool;");
  D("var $CurrAddr:ref;");

  D("procedure $malloc(n: size) returns (p: ref)\n"
    "modifies $CurrAddr, $Alloc;\n"
    "{\n"
    "  assume $i2b($sgt.ref($CurrAddr, $NULL));\n"
    "  p := $CurrAddr;\n"
    "  if ($i2b($sgt.ref(n, $NULL))) {\n"
    "    $CurrAddr := $add.ref($CurrAddr, n);\n"
    "  } else {\n"
    "    $CurrAddr := $add.ref($CurrAddr, $REF_CONST_1);\n"
    "  }\n"
    "  $Alloc[p] := true;\n"
    "}");

  D("procedure $free(p: ref)\n"
    "modifies $Alloc;\n"
    "{\n"
    "  $Alloc[p] := false;\n"
    "}");

  D("procedure $alloca(n: size) returns (p: ref)\n"
    "modifies $CurrAddr, $Alloc;\n"
    "{\n"
    "  assume $i2b($sgt.ref($CurrAddr, $NULL));\n"
    "  p := $CurrAddr;\n"
    "  if ($i2b($sgt.ref(n, $NULL))) {\n"
    "    $CurrAddr := $add.ref($CurrAddr, n);\n"
    "  } else {\n"
    "    $CurrAddr := $add.ref($CurrAddr, $REF_CONST_1);\n"
    "  }\n"
    "  $Alloc[p] := true;\n"
    "}");

#elif MEMORY_MODEL_REUSE // can reuse previously-allocated and freed addresses
  D("var $Alloc: [ref] bool;");
  D("var $Size: [ref] size;");
  D("var $CurrAddr:ref;");

  D("procedure $malloc(n: size) returns (p: ref);\n"
    "modifies $Alloc, $Size;\n"
    "ensures $i2b($sgt.ref(p, $NULL));\n"
    "ensures $i2b($slt.ref(p, $MALLOC_TOP));\n"
    "ensures !old($Alloc[p]);\n"
    "ensures (forall q: ref :: old($Alloc[q]) ==> ($i2b($slt.ref($add.ref(p, n), q)) || $i2b($sgt.ref(p, $add.ref(q, $Size[q])))));\n"
    "ensures $Alloc[p];\n"
    "ensures $Size[p] == n;\n"
    "ensures (forall q: ref :: {$Size[q]} q != p ==> $Size[q] == old($Size[q]));\n"
    "ensures (forall q: ref :: {$Alloc[q]} q != p ==> $Alloc[q] == old($Alloc[q]));\n"
    "ensures $i2b($sge.ref(n, $NULL)) ==> (forall q: ref :: {$base(q)} $i2b($sle.ref(p, q)) && $i2b($slt.ref(q, $add.ref(p, n))) ==> $base(q) == p);");

  D("procedure $free(p: ref);\n"
    "modifies $Alloc;\n"
    "ensures !$Alloc[p];\n"
    "ensures (forall q: ref :: {$Alloc[q]} q != p ==> $Alloc[q] == old($Alloc[q]));");

  D("procedure $alloca(n: size) returns (p: ref);\n"
    "modifies $Alloc, $Size;\n"
    "ensures $i2b($sgt.ref(p, $NULL));\n"
    "ensures $i2b($slt.ref(p, $MALLOC_TOP));\n"
    "ensures !old($Alloc[p]);\n"
    "ensures (forall q: ref :: old($Alloc[q]) ==> ($i2b($slt.ref($add.ref(p, n), q)) || $i2b($sgt.ref(p, $add.ref(q, $Size[q])))));\n"
    "ensures $Alloc[p];\n"
    "ensures $Size[p] == n;\n"
    "ensures (forall q: ref :: {$Size[q]} q != p ==> $Size[q] == old($Size[q]));\n"
    "ensures (forall q: ref :: {$Alloc[q]} q != p ==> $Alloc[q] == old($Alloc[q]));\n"
    "ensures $i2b($sge.ref(n, $NULL)) ==> (forall q: ref :: {$base(q)} $i2b($sle.ref(p, q)) && $i2b($slt.ref(q, $add.ref(p, n))) ==> $base(q) == p);");

#else // NO_REUSE does not reuse previously-allocated addresses
  D("var $Alloc: [ref] bool;");
  D("var $CurrAddr:ref;");
  D("procedure $malloc(n: size) returns (p: ref);\n"
    "modifies $CurrAddr, $Alloc;\n"
    "ensures $i2b($sgt.ref(p, $NULL));\n"
    "ensures p == old($CurrAddr);\n"
    "ensures $i2b($sgt.ref($CurrAddr, old($CurrAddr)));\n"
    "ensures $i2b($sge.ref(n, $NULL)) ==> $i2b($sge.ref($CurrAddr, $add.ref(old($CurrAddr), n)));\n"
    "ensures $Alloc[p];\n"
    "ensures (forall q: ref :: {$Alloc[q]} q != p ==> $Alloc[q] == old($Alloc[q]));\n"
    "ensures $i2b($sge.ref(n, $NULL)) ==> (forall q: ref :: {$base(q)} $i2b($sle.ref(p, q)) && $i2b($slt.ref(q, $add.ref(p, n))) ==> $base(q) == p);");

  D("procedure $free(p: ref);\n"
    "modifies $Alloc;\n"
    "ensures !$Alloc[p];\n"
    "ensures (forall q: ref :: {$Alloc[q]} q != p ==> $Alloc[q] == old($Alloc[q]));");

  D("procedure $alloca(n: size) returns (p: ref);\n"
    "modifies $CurrAddr, $Alloc;\n"
    "ensures $i2b($sgt.ref(p, $NULL));\n"
    "ensures p == old($CurrAddr);\n"
    "ensures $i2b($sgt.ref($CurrAddr, old($CurrAddr)));\n"
    "ensures $i2b($sge.ref(n, $NULL)) ==> $i2b($sge.ref($CurrAddr, $add.ref(old($CurrAddr), n)));\n"
    "ensures $Alloc[p];\n"
    "ensures (forall q: ref :: {$Alloc[q]} q != p ==> $Alloc[q] == old($Alloc[q]));\n"
    "ensures $i2b($sge.ref(n, $NULL)) ==> (forall q: ref :: {$base(q)} $i2b($sle.ref(p, q)) && $i2b($slt.ref(q, $add.ref(p, n))) ==> $base(q) == p);");
#endif

  D("var $exn: bool;");
  D("var $exnv: int;");
  D("function $extractvalue(p: int, i: int) returns (int);");

#undef D
}

