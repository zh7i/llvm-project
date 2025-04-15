
let TargetPrefix = "drai" in {

defvar llvm_vi32_ty = llvm_v64i32_ty;
defvar llvm_vf32_ty = LLVMType<v64f32>;
defvar llvm_vf16_ty = LLVMType<v64f16>;
defvar llvm_vi1_ty = llvm_v64i1_ty;

def int_drai_print_s_i : ClangBuiltin<"__builtin_drai_print_s_i">,
  Intrinsic<[], [llvm_i32_ty], [IntrHasSideEffects]>;

def int_drai_print_s_f : ClangBuiltin<"__builtin_drai_print_s_f">,
  Intrinsic<[], [llvm_float_ty], [IntrHasSideEffects]>;

def int_drai_print_s_h : ClangBuiltin<"__builtin_drai_print_s_h">,
  Intrinsic<[], [llvm_half_ty], [IntrHasSideEffects]>;

def int_drai_print_v_i : ClangBuiltin<"__builtin_drai_print_v_i">,
  Intrinsic<[], [llvm_vi32_ty], [IntrHasSideEffects]>;

def int_drai_print_v_f : ClangBuiltin<"__builtin_drai_print_v_f">,
  Intrinsic<[], [llvm_vf32_ty], [IntrHasSideEffects]>;

def int_drai_print_v_h : ClangBuiltin<"__builtin_drai_print_v_h">,
  Intrinsic<[], [llvm_vf16_ty], [IntrHasSideEffects]>;

def int_drai_print_v_b : ClangBuiltin<"__builtin_drai_print_v_b">,
  Intrinsic<[], [llvm_vi1_ty], [IntrHasSideEffects]>;

def int_drai_print_e : ClangBuiltin<"__builtin_drai_print_e">,
  Intrinsic<[], [llvm_ptr_ty], [IntrHasSideEffects]>;

def int_drai_thread_index : ClangBuiltin<"__builtin_drai_thread_index">,
  Intrinsic<[llvm_vi32_ty], [], [IntrNoMem, IntrSpeculatable, NoUndef<RetIndex>]>;

def int_drai_core_index : ClangBuiltin<"__builtin_drai_core_index">,
  Intrinsic<[llvm_i32_ty], [], [IntrNoMem, IntrSpeculatable, NoUndef<RetIndex>]>;

def int_drai_core_dim : ClangBuiltin<"__builtin_drai_core_dim">,
  Intrinsic<[llvm_i32_ty], [], [IntrNoMem, IntrSpeculatable, NoUndef<RetIndex>]>;

def int_drai_sync_threads : ClangBuiltin<"__builtin_drai_sync_threads">,
  Intrinsic<[], [llvm_i32_ty], [IntrHasSideEffects, IntrConvergent]>;

def int_drai_memory_fence : ClangBuiltin<"__builtin_drai_memory_fence">,
  Intrinsic<[], [llvm_i32_ty], [IntrHasSideEffects]>;

def int_drai_scatter_store_i :
  Intrinsic<[], [llvm_anyptr_ty, llvm_vi32_ty, llvm_vi32_ty, llvm_vi1_ty, llvm_i32_ty], [IntrArgMemOnly]>;  // anyptr is overloaded

def int_drai_scatter_store_f :
  Intrinsic<[], [llvm_anyptr_ty, llvm_vi32_ty, llvm_vf32_ty, llvm_vi1_ty, llvm_i32_ty], [IntrArgMemOnly]>;

def int_drai_scatter_store_h :
  Intrinsic<[], [llvm_anyptr_ty, llvm_vi32_ty, llvm_vf16_ty, llvm_vi1_ty, llvm_i32_ty], [IntrArgMemOnly]>;

def int_drai_reduceshift_leftc_i : ClangBuiltin<"__builtin_drai_pure_reduceshift_leftc_i">,
  Intrinsic<[llvm_vi32_ty], [llvm_vi32_ty, llvm_i32_ty /* input, shiftnum */], [IntrNoMem, IntrSpeculatable, NoUndef<RetIndex>]>;

def int_drai_reduceshift_leftc_f : ClangBuiltin<"__builtin_drai_pure_reduceshift_leftc_f">,
  Intrinsic<[llvm_vf32_ty], [llvm_vf32_ty, llvm_i32_ty], [IntrNoMem, IntrSpeculatable, NoUndef<RetIndex>]>;

def int_drai_reduceshift_leftc_h : ClangBuiltin<"__builtin_drai_pure_reduceshift_leftc_h">,
  Intrinsic<[llvm_vf16_ty], [llvm_vf16_ty, llvm_i32_ty], [IntrNoMem, IntrSpeculatable, NoUndef<RetIndex>]>;

def int_drai_reduceshift_rightc_i : ClangBuiltin<"__builtin_drai_pure_reduceshift_rightc_i">,
  Intrinsic<[llvm_vi32_ty], [llvm_vi32_ty, llvm_i32_ty], [IntrNoMem, IntrSpeculatable, NoUndef<RetIndex>]>;

def int_drai_reduceshift_rightc_f : ClangBuiltin<"__builtin_drai_pure_reduceshift_rightc_f">,
  Intrinsic<[llvm_vf32_ty], [llvm_vf32_ty, llvm_i32_ty], [IntrNoMem, IntrSpeculatable, NoUndef<RetIndex>]>;

def int_drai_reduceshift_rightc_h : ClangBuiltin<"__builtin_drai_pure_reduceshift_rightc_h">,
  Intrinsic<[llvm_vf16_ty], [llvm_vf16_ty, llvm_i32_ty], [IntrNoMem, IntrSpeculatable, NoUndef<RetIndex>]>;

def int_drai_loop_set_iter :
  Intrinsic<[], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty /* loop id, depth, loop count */], [IntrHasSideEffects]>;

def int_drai_loop_iter_dec :
  Intrinsic<[llvm_i1_ty /* jmp cond */], [llvm_i32_ty /* loop id */], [IntrHasSideEffects]>;

}