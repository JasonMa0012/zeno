#pragma once

#include "IRVisitor.h"
#include "Stmts.h"

namespace zfx {

void apply_control_check(IR *ir);
void apply_symbol_check(IR *ir);
void apply_type_check(IR *ir);
std::unique_ptr<IR> apply_expand_functions(IR *ir);
std::unique_ptr<IR> apply_lower_math(IR *ir);
std::unique_ptr<IR> apply_math_functions(IR *ir);
std::unique_ptr<IR> apply_lower_access(IR *ir);
std::map<int, int> apply_reassign_parameters(IR *ir);
std::map<int, std::string> apply_const_parametrize(IR *ir);
void apply_register_allocation(IR *ir, int nregs);
std::unique_ptr<IR> apply_save_math_registers(IR *ir, int nregs);
std::map<int, int> apply_reassign_globals(IR *ir);
void apply_global_localize(IR *ir);
std::string apply_emit_assembly(IR *ir);

}
