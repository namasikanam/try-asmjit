#include <asmjit/a64.h>
#include <stdio.h>
#include <iostream>
using namespace asmjit;
using namespace std;
typedef int (*Func)(void);
// Small helper function to print the current content of `cb`.
static void dumpCode(BaseBuilder& builder, const char* phase) {
  String sb;
  FormatOptions formatOptions {};
  Formatter::formatNodeList(sb, formatOptions, &builder);
  printf("%s:\n%s\n", phase, sb.data());
}
int main() {
  JitRuntime rt;                    // Create JIT Runtime.
  CodeHolder code;                  // Create a CodeHolder.
  code.init(rt.environment(),       // Initialize code to match the JIT environment.
            rt.cpuFeatures());
  a64::Builder cb(&code);           // Create and attach x86::Builder to `code`.
  // Create and initialize `FuncDetail`.
  FuncDetail func;
  func.init(FuncSignatureT<int>(CallConvId::kHost), rt.environment());
  // Remember prolog insertion point.
  BaseNode* prologInsertionPoint = cb.cursor();

  cb.mov(a64::x0, Imm(21));

  // Emit function body:
  // 把 x0 存到 a[1] 里，然后把 a[1] 加 1
  uint64_t a[2] = {0, 0};
  cb.mov(arm::GpX(1), Imm(a));
  cb.mov(arm::GpX(2), Imm(1));

  cb.str(arm::GpX(0), arm::Mem(arm::GpX(1), 4));

  // 尝试输出 mem size
  Operand memop = static_cast<InstNode *>(cb.lastNode())->op(1);
  cout << static_cast<arm::Mem *>(&memop)->size() << endl;


  // 把 x0 存到 x 里
  // uint64_t x;
  // cb.mov(arm::GpX(1), Imm(&x));
  // cb.str(arm::GpX(0), arm::Mem(arm::GpX(1)));
  // cb.msr(a64::Predicate::SysReg::kNZCV, 0);

  /*
  popcount
  ; %bb.1:
	mov	w8, #0
  LBB0_2:                                 ; =>This Inner Loop Header: Depth=1
    and	w9, w0, #0x1
    add	w8, w9, w8
    lsr	x9, x0, #1
    cmp	x0, #1
    mov	x0, x9
    b.hi	LBB0_2
  ; %bb.3:
    sxtw	x0, w8
  */
  // cb.mov(a64::x8, Imm(0));
  // Label loop = cb.newLabel();
  // cb.bind(loop);
  // cb.and_(a64::x9, a64::x0, Imm(1));
  // cb.add(a64::x8, a64::x9, a64::x8);
  // cb.lsr(a64::x9, a64::x0, Imm(1));
  // cb.cmp(a64::x0, Imm(1));
  // cb.mov(a64::x0, a64::x9);
  // cb.b_hi(loop);

  // cb.mov(a64::x0, a64::x8);

  /*
	lsr x0, x3, x1
  add x0, x1, -7
  lsr x3, x3, x0
  clz x1, x1
  */
  // cb.newLabel();
  // cb.lsr(a64::x0, a64::x3, a64::x1);
  // cb.add(a64::x0, a64::x1, Imm(7));
  // cb.lsr(a64::x3, a64::x3, a64::x0);
  // cb.clz(a64::x1, a64::x1);

  /*
  sub	x0, x0, #1
	orr	x0, x0, x0, asr #1
	orr	x0, x0, x0, asr #2
	orr	x0, x0, x0, asr #4
	orr	x0, x0, x0, asr #8
	orr	x0, x0, x0, asr #16
	orr	x0, x0, x0, asr #32
	add	x0, x0, #1
  */
  // cb.mov(a64::x0, Imm(15));
  // cb.sub(a64::x0, a64::x0, Imm(1));
  // cb.orr(a64::x0, a64::x0, a64::x0, Imm(a64::Shift(a64::ShiftOp::kASR, 1)));
  // cb.orr(a64::x0, a64::x0, a64::x0, Imm(a64::Shift(a64::ShiftOp::kASR, 2)));
  // cb.orr(a64::x0, a64::x0, a64::x0, Imm(a64::Shift(a64::ShiftOp::kASR, 4)));
  // cb.orr(a64::x0, a64::x0, a64::x0, Imm(a64::Shift(a64::ShiftOp::kASR, 8)));
  // cb.orr(a64::x0, a64::x0, a64::x0, Imm(a64::Shift(a64::ShiftOp::kASR, 16)));
  // cb.orr(a64::x0, a64::x0, a64::x0, Imm(a64::Shift(a64::ShiftOp::kASR, 32)));
  // cb.add(a64::x0, a64::x0, Imm(1));

  /*
    sub x0, x0, 1
    clz x1, x0
    mov x0, #-1
    lsr x2, x0, x1
    sub x0, x2, x0
  */
  // cb.mov(a64::x0, Imm(23));
  // cb.sub(a64::x0, a64::x0, Imm(1));
  // cb.clz(a64::x1, a64::x0);
  // cb.mov(a64::x0, Imm(-1));
  // cb.lsr(a64::x2, a64::x0, a64::x1);
  // cb.sub(a64::x0, a64::x2, a64::x0);

  /*
    cmp x0, x1         // 比较 x0 和 x1
    movge x0, x1       // 如果 x0 >= x1，那么就把 x1 赋给 x0
  */
  // cb.mov(a64::x0, Imm(25));
  // cb.mov(a64::x0, Imm(24));
  // cb.cmp(a64::x0, a64::x1);
  // cb.mov(a64::x0, a64::x0);

  // Remember epilog insertion point.
  BaseNode* epilogInsertionPoint = cb.cursor();
  // Let's see what we have now.
  dumpCode(cb, "Raw Function");
  // Now, after we emitted the function body, we can insert the prolog, arguments
  // allocation, and epilog. This is not possible with using pure x86::Assembler.
  FuncFrame frame;
  frame.init(func);
  FuncArgsAssignment args(&func);   // Create arguments assignment context.
  // args.assignAll(dst, srcA, srcB);  // Assign our registers to arguments.
  // args.updateFuncFrame(frame);          // Reflect our args in FuncFrame.
  frame.finalize();                 // Finalize the FuncFrame (updates it).
  // Insert function prolog and allocate arguments to registers.
  cb.setCursor(prologInsertionPoint);
  cb.emitProlog(frame);
  cb.emitArgsAssignment(frame, args);
  // Insert function epilog.
  cb.setCursor(epilogInsertionPoint);
  cb.emitEpilog(frame);
  // Let's see how the function's prolog and epilog looks.
  dumpCode(cb, "Prolog & Epilog");
  // IMPORTANT: Builder requires finalize() to be called to serialize its
  // code to the Assembler (it automatically creates one if not attached).
  cb.finalize();
  Func fn;
  Error err = rt.add(&fn, &code);   // Add the generated code to the runtime.
  if (err) return 1;                // Handle a possible error case.
  // Execute the generated function.
  uint64_t res = fn();
  // Prints {2}
  printf("result: %llu\n", res);
  printf("%llu\n", a[1]);
  rt.release(fn);                   // Explicitly remove the function from the runtime.
  return 0;
}
