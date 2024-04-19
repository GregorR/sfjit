/*
 *    Stack-less Just-In-Time compiler
 *    Alloca tests
 *
 * Copyright (c) Gregor Richards 2024
 *
 * NOTE: This license applies only to this file.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

static void test_alloca1(void)
{
	/* Test for alloca access via various means */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	struct sljit_alloca *allocs[2];
	sljit_sw buf[5];
	sljit_s32 i;

	if (verbose)
		printf("Run test_alloca1\n");

	FAILED(!compiler, "cannot create compiler\n");

	for (i = 0; i < 5; i++)
		buf[i] = -1;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS1V(W), 0, 1, 0, 0, 0);

	/* NOTE: This test assumes that 2-word alignment is fine on all
	 * platforms! */

	/* buf[4], buf[3] */
	allocs[0] = sljit_emit_alloca(compiler, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), 0, SLJIT_IMM, 3);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_FP), -((int) sizeof(sljit_sw)), SLJIT_IMM, 4);

	/* buf[2], buf[1] allocated but not set */
	allocs[1] = sljit_emit_alloca(compiler, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_FP), -3 * ((int) sizeof(sljit_sw)), SLJIT_IMM, 2);

	/* buf[0] allocated and set, buf[1] set */
	sljit_emit_alloca(compiler, 2 * sizeof(sljit_sw));
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), sizeof(sljit_sw), SLJIT_IMM, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), 2 * sizeof(sljit_sw), SLJIT_IMM, 1);

	/* pop buf[0] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_MEM1(SLJIT_SP), sizeof(sljit_sw));
	sljit_emit_pop(compiler, 2 * sizeof(sljit_sw));

	/* resolve and pop buf[1], buf[2] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw), SLJIT_MEM1(SLJIT_SP), 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), 2 * sizeof(sljit_sw), SLJIT_MEM1(SLJIT_SP), sizeof(sljit_sw));
	sljit_set_alloca(compiler, allocs[1], 2 * sizeof(sljit_sw));
	sljit_emit_pop(compiler, 2 * sizeof(sljit_sw));

	/* resolve but don't pop buf[3], buf[4] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), 3 * sizeof(sljit_sw), SLJIT_MEM1(SLJIT_SP), 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), 4 * sizeof(sljit_sw), SLJIT_MEM1(SLJIT_SP), sizeof(sljit_sw));
	sljit_set_alloca(compiler, allocs[0], 2 * sizeof(sljit_sw));

	sljit_emit_return_void(compiler);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	code.func1((sljit_sw)buf);

	FAILED(buf[0] != 0, "testa1 case 1 failed\n");
	FAILED(buf[1] != 1, "testa1 case 2 failed\n");
	FAILED(buf[2] != 2, "testa1 case 3 failed\n");
	FAILED(buf[3] != 3, "testa1 case 4 failed\n");
	FAILED(buf[4] != 4, "testa1 case 5 failed\n");

	sljit_free_code(code.code, NULL);
	successful_tests++;
}
