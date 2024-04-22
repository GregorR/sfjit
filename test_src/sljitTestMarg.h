/*
 *    Stack-less Just-In-Time compiler
 *    Multi-arg tests
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

#define SLJIT_RTMP SLJIT_R(SLJIT_NUMBER_OF_ARG_REGISTERS)
#define SLJIT_FRTMP SLJIT_FR(SLJIT_NUMBER_OF_FLOAT_ARG_REGISTERS)

static void test_marg1(void)
{
	/* Test for multi-argument functions */
	executable_code code1, code2, code3;
	struct sljit_compiler* compiler;
	sljit_u32 u;
	sljit_s32 i, offset;
	sljit_s32 locs[10], flocs[10];
	sljit_sw offs[10], foffs[10];
	struct sljit_marg* marg;
	sljit_f64 res[20];

	if (verbose)
		printf("Run test_marg1\n");

	/* Test 1 (no floats) */
	compiler = sljit_create_compiler(NULL);
	FAILED(!compiler, "cannot create compiler\n");

	sljit_emit_enter_multiarg(compiler, 0, SLJIT_ARG_TYPE_W,
		SLJIT_NUMBER_OF_ARG_REGISTERS ?
			SLJIT_NUMBER_OF_ARG_REGISTERS + 1 : 2,
		0, 0, 0, 0);

	for (i = 0; i < 10; i += 2) {
		sljit_emit_get_marg(compiler, SLJIT_ARG_TYPE_W, SLJIT_R(i), locs + i, offs + i);
		sljit_emit_get_marg(compiler, SLJIT_ARG_TYPE_32, SLJIT_R(i+1), locs + i + 1, offs + i + 1);
	}

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_RTMP, 0, locs[0], offs[0]);
	for (i = 1; i < 9; i += 2) {
		sljit_emit_op1(compiler, SLJIT_MOV_S32, SLJIT_R0, 0, locs[i], offs[i]);
		sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_RTMP, 0, SLJIT_RTMP, 0,
			SLJIT_R0, 0);
		sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_RTMP, 0, SLJIT_RTMP, 0,
			locs[i+1], offs[i+1]);
	}
	sljit_emit_op1(compiler, SLJIT_MOV_S32, SLJIT_R0, 0, locs[9], offs[9]);
	sljit_emit_op2(compiler, SLJIT_MUL, SLJIT_RTMP, 0, SLJIT_RTMP, 0,
		SLJIT_R0, 0);

	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_RTMP, 0);

	code1.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	/* Test 2 (floats) */
	compiler = sljit_create_compiler(NULL);
	FAILED(!compiler, "cannot create compiler\n");

	sljit_emit_enter_multiarg(compiler, 0, SLJIT_ARG_TYPE_RET_VOID,
		SLJIT_NUMBER_OF_ARG_REGISTERS + 1, 0,
		SLJIT_NUMBER_OF_FLOAT_ARG_REGISTERS + 1, 0, 0);

	for (i = 0; i < 10; i += 2) {
		sljit_emit_get_marg(compiler, SLJIT_ARG_TYPE_W, SLJIT_R(i),
			locs + i, offs + i);
		sljit_emit_get_marg(compiler, SLJIT_ARG_TYPE_F32, SLJIT_FR(i),
			flocs + i, foffs + i);
		sljit_emit_get_marg(compiler, SLJIT_ARG_TYPE_32, SLJIT_R(i+1),
			locs + i + 1, offs + i + 1);
		sljit_emit_get_marg(compiler, SLJIT_ARG_TYPE_F64, SLJIT_FR(i+1),
			flocs + i + 1, foffs + i + 1);
	}

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_RTMP, 0, SLJIT_IMM, (sljit_sw) res);
	offset = 0;
	for (i = 0; i < 10; i += 2) {
		sljit_emit_fop1(compiler, SLJIT_CONV_F64_FROM_SW, SLJIT_FRTMP, 0,
			locs[i], offs[i]);
		sljit_emit_fop1(compiler, SLJIT_MOV_F64, SLJIT_MEM1(SLJIT_RTMP), offset,
			SLJIT_FRTMP, 0);
		offset += (sljit_s32) sizeof(sljit_f64);

		sljit_emit_fop1(compiler, SLJIT_CONV_F64_FROM_F32, SLJIT_FRTMP, 0,
			flocs[i], foffs[i]);
		sljit_emit_fop1(compiler, SLJIT_MOV_F64, SLJIT_MEM1(SLJIT_RTMP), offset,
			SLJIT_FRTMP, 0);
		offset += (sljit_s32) sizeof(sljit_f64);

		sljit_emit_fop1(compiler, SLJIT_CONV_F64_FROM_S32, SLJIT_FRTMP, 0,
			locs[i+1], offs[i+1]);
		sljit_emit_fop1(compiler, SLJIT_MOV_F64, SLJIT_MEM1(SLJIT_RTMP), offset,
			SLJIT_FRTMP, 0);
		offset += (sljit_s32) sizeof(sljit_f64);

		sljit_emit_fop1(compiler, SLJIT_MOV_F64, SLJIT_FRTMP, 0,
			flocs[i+1], foffs[i+1]);
		sljit_emit_fop1(compiler, SLJIT_MOV_F64, SLJIT_MEM1(SLJIT_RTMP), offset,
			SLJIT_FRTMP, 0);
		offset += (sljit_s32) sizeof(sljit_f64);
	}

	sljit_emit_return_void(compiler);

	code2.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	/* Test 3, calling multi-arg functions. Start with making the type */
	compiler = sljit_create_compiler(NULL);
	FAILED(!compiler, "cannot create compiler\n");

	marg = sljit_def_marg(compiler, NULL, SLJIT_ARG_TYPE_RET_VOID);
	for (i = 0; i < 10; i += 2) {
		marg = sljit_def_marg(compiler, marg, SLJIT_ARG_TYPE_W);
		marg = sljit_def_marg(compiler, marg, SLJIT_ARG_TYPE_F32);
		marg = sljit_def_marg(compiler, marg, SLJIT_ARG_TYPE_32);
		marg = sljit_def_marg(compiler, marg, SLJIT_ARG_TYPE_F64);
	}

	sljit_emit_enter(compiler, 0, SLJIT_ARGS0V(),
		SLJIT_NUMBER_OF_REGISTERS, 0,
		SLJIT_NUMBER_OF_FLOAT_REGISTERS, 0, sizeof(sljit_sw));

	{
		sljit_s32 argr, fargr, ssize;

		/* Get the argument info */
		sljit_marg_properties(compiler, marg, &argr, &fargr, &ssize);
		if (ssize)
			sljit_emit_alloca(compiler, (sljit_uw) ssize);

		/* Push the arguments one by one */
		u = 0;
		while (u < 10) {
			i = (sljit_s32) u;
			if (i < argr) {
				sljit_emit_op1(compiler, SLJIT_MOV,
					SLJIT_R(i), 0, SLJIT_IMM, 12345600 + i);
			} else {
				sljit_emit_set_marg(compiler, marg, u*2,
					SLJIT_IMM, 12345600 + i);
			}
			if (i < fargr) {
				sljit_emit_fop1(compiler, SLJIT_CONV_F32_FROM_SW,
					SLJIT_FR(i), 0, SLJIT_IMM, 12345600 + i);
			} else {
				sljit_emit_fop1(compiler, SLJIT_CONV_F32_FROM_SW,
					SLJIT_FR(fargr), 0, SLJIT_IMM, 12345600 + i);
				sljit_emit_set_marg(compiler, marg, u*2+1,
					SLJIT_FR(fargr), 0);
			}
			u++; i = (sljit_s32) u;
			if (i < argr) {
				sljit_emit_op1(compiler, SLJIT_MOV32,
					SLJIT_R(i), 0, SLJIT_IMM, 12345600 + i);
			} else {
				sljit_emit_set_marg(compiler, marg, u*2,
					SLJIT_IMM, 12345600 + i);
			}
			if (i < fargr) {
				sljit_emit_fop1(compiler, SLJIT_CONV_F64_FROM_SW,
					SLJIT_FR(i), 0, SLJIT_IMM, 12345600 + i);
			} else {
				sljit_emit_fop1(compiler, SLJIT_CONV_F64_FROM_SW,
					SLJIT_FR(fargr), 0, SLJIT_IMM, 12345600 + i);
				sljit_emit_set_marg(compiler, marg, u*2+1,
					SLJIT_FR(fargr), 0);
			}
			u++;
		}

		/* To make it a trickier case, get it into the stack */
		sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_FP), 0,
			SLJIT_IMM, SLJIT_FUNC_ADDR(code2.code));

		/* Make the call */
		sljit_emit_icall_multiarg(compiler, marg,
			SLJIT_MEM1(SLJIT_FP), 0);

		if (ssize)
			sljit_emit_pop(compiler, (sljit_uw) ssize);
	}

	sljit_emit_return_void(compiler);

	code3.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	code2.test_marg1_f2(
		85184216,
		1481955.125,
		-20251648,
		-6034305.5,
		-92654160,
		2971148.25,
		94611487,
		-8367898.5,
		-60559668,
		-1791444.125,
		75711612,
		-7172975.375,
		38396976,
		-661649.875,
		-88751410,
		8365837.875,
		-54926045,
		1607258.625,
		-1367,
		-9981201
	);

	FAILED(res[0] != 85184216, "test_marg1 case 1 failed\n");
	FAILED(res[1] != 1481955.125, "test_marg1 case 2 failed\n");
	FAILED(res[2] != -20251648, "test_marg1 case 3 failed\n");
	FAILED(res[3] != -6034305.5, "test_marg1 case 4 failed\n");
	FAILED(res[4] != -92654160, "test_marg1 case 5 failed\n");
	FAILED(res[5] != 2971148.25, "test_marg1 case 6 failed\n");
	FAILED(res[6] != 94611487, "test_marg1 case 7 failed\n");
	FAILED(res[7] != -8367898.5, "test_marg1 case 8 failed\n");
	FAILED(res[8] != -60559668, "test_marg1 case 9 failed\n");
	FAILED(res[9] != -1791444.125, "test_marg1 case 10 failed\n");
	FAILED(res[10] != 75711612, "test_marg1 case 11 failed\n");
	FAILED(res[11] != -7172975.375, "test_marg1 case 12 failed\n");
	FAILED(res[12] != 38396976, "test_marg1 case 13 failed\n");
	FAILED(res[13] != -661649.875, "test_marg1 case 14 failed\n");
	FAILED(res[14] != -88751410, "test_marg1 case 15 failed\n");
	FAILED(res[15] != 8365837.875, "test_marg1 case 16 failed\n");
	FAILED(res[16] != -54926045, "test_marg1 case 17 failed\n");
	FAILED(res[17] != 1607258.625, "test_marg1 case 18 failed\n");
	FAILED(res[18] != -1367, "test_marg1 case 19 failed\n");
	FAILED(res[19] != -9981201, "test_marg1 case 20 failed\n");

	FAILED(
		code1.test_marg1_f1(
			851842,
			-202516,
			-926541,
			946114,
			-605596,
			757116,
			383969,
			-887514,
			-549260,
			-136
		) != -430095920,
		"test_marg1 case 21 failed\n"
	);

#if (defined SLJIT_64BIT_ARCHITECTURE && SLJIT_64BIT_ARCHITECTURE)
	FAILED(
		code1.test_marg1_f1(
			85184216,
			-20251648,
			-92654160,
			94611487,
			-60559668,
			75711612,
			38396976,
			-88751410,
			-54926045,
			-1367
		) != -432309859518L,
		"test_marg1 case 22 failed\n"
	);
#endif

	code3.func0();

	FAILED(res[0] != 12345600, "test_marg1 case 23 failed\n");
	FAILED(res[1] != 12345600, "test_marg1 case 24 failed\n");
	FAILED(res[2] != 12345601, "test_marg1 case 25 failed\n");
	FAILED(res[3] != 12345601, "test_marg1 case 26 failed\n");
	FAILED(res[4] != 12345602, "test_marg1 case 27 failed\n");
	FAILED(res[5] != 12345602, "test_marg1 case 28 failed\n");
	FAILED(res[6] != 12345603, "test_marg1 case 29 failed\n");
	FAILED(res[7] != 12345603, "test_marg1 case 30 failed\n");
	FAILED(res[8] != 12345604, "test_marg1 case 31 failed\n");
	FAILED(res[9] != 12345604, "test_marg1 case 32 failed\n");
	FAILED(res[10] != 12345605, "test_marg1 case 33 failed\n");
	FAILED(res[11] != 12345605, "test_marg1 case 34 failed\n");
	FAILED(res[12] != 12345606, "test_marg1 case 35 failed\n");
	FAILED(res[13] != 12345606, "test_marg1 case 36 failed\n");
	FAILED(res[14] != 12345607, "test_marg1 case 37 failed\n");
	FAILED(res[15] != 12345607, "test_marg1 case 38 failed\n");
	FAILED(res[16] != 12345608, "test_marg1 case 39 failed\n");
	FAILED(res[17] != 12345608, "test_marg1 case 40 failed\n");
	FAILED(res[18] != 12345609, "test_marg1 case 41 failed\n");
	FAILED(res[19] != 12345609, "test_marg1 case 42 failed\n");

	sljit_free_code(code1.code, NULL);
	sljit_free_code(code2.code, NULL);
	sljit_free_code(code3.code, NULL);
	successful_tests++;
}
