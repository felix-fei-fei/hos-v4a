/* ------------------------------------------------------------------------ */
/*  Hyper Operating System V4  μITRON4.0仕様 Real-Time OS                  */
/*  μカーネル キュー の単体テストモジュール                                */
/*                                                                          */
/*                                  Copyright (C) 1998-2006 by Project HOS  */
/*                                  http://sourceforge.jp/projects/hos/     */
/* ------------------------------------------------------------------------ */



#include <stdio.h>
#include "kernel.h"
#include "arch/proc/win/win32/procatr.h"
#include "arch/proc/win/win32/proc.h"
#include "arch/irc/none/ircatr.h"
#include "arch/irc/none/irc.h"
#include "config/cfgknl.h"
#include "parser/parsknl.h"
#include "core/opttyp.h"
#include "object/tskobj.h"
#include "config/cfgerr.h"
#include "parser/parserr.h"



#include <assert.h>
#define TEST_ASSERT(a)		assert(a)

int que_test(void);


int main()
{
	que_test();

	printf("OK\n");

	return 0;
}


#define TEST_QUE_NUM	6

static _KERNEL_T_QUE test_que; 


#if _KERNEL_TCB_ALGORITHM == _KERNEL_TCB_ALG_BLKARRAY		/* %en{block array}%jp{TCBを単純配列で管理} */

_KERNEL_T_TCB _kernel_tcb_tbl[TEST_QUE_NUM];			/* %jp{タスクコントロールブロックのポインタを配列で持つ} */

#elif _KERNEL_TCB_ALGORITHM == _KERNEL_TCB_ALG_PTRARRAY		/* pointer array */


_KERNEL_T_TCB test_tcb[TEST_QUE_NUM];
_KERNEL_T_TCB *_kernel_tcb_tbl[TEST_QUE_NUM] =		/* %jp{タスクコントロールブロックのポインタを配列で持つ} */
	{
		&test_tcb[0],
		&test_tcb[1],
		&test_tcb[2],
		&test_tcb[3],
		&test_tcb[4],
		&test_tcb[5],
	};

#endif



/* キューが全部外れていることをチェック */
BOOL check_que_empty(void)
{
	_KERNEL_T_TSKHDL tskhdl;
	int i;

	if ( _KERNEL_REF_QUE(&test_que) != _KERNEL_TSKHDL_NULL )
	{
		return FALSE;		/* 異常 */
	}

	for ( i = 1; i <= TEST_QUE_NUM; i++ )
	{
		tskhdl = _KERNEL_TSK_ID2TSKHDL(i);

		if ( _KERNEL_TSK_GET_QUE(tskhdl) != NULL )
		{
			return FALSE;	/* 異常 */
		}
	}

	return TRUE;
}



int que_test(void)
{
	_KERNEL_T_TSKHDL tskhdl;

	/* Initialize */
	_KERNEL_CRE_QUE(&test_que);

	_KERNEL_TSK_SET_QUE   (_KERNEL_TSK_ID2TSKHDL(1), NULL);
	_KERNEL_TSK_SET_TSKPRI(_KERNEL_TSK_ID2TSKHDL(1), 1);
	_KERNEL_TSK_SET_QUE   (_KERNEL_TSK_ID2TSKHDL(2), NULL);
	_KERNEL_TSK_SET_TSKPRI(_KERNEL_TSK_ID2TSKHDL(2), 2);
	_KERNEL_TSK_SET_QUE   (_KERNEL_TSK_ID2TSKHDL(3), NULL);
	_KERNEL_TSK_SET_TSKPRI(_KERNEL_TSK_ID2TSKHDL(3), 3);
	_KERNEL_TSK_SET_QUE   (_KERNEL_TSK_ID2TSKHDL(4), NULL);
	_KERNEL_TSK_SET_TSKPRI(_KERNEL_TSK_ID2TSKHDL(4), 1);
	_KERNEL_TSK_SET_QUE   (_KERNEL_TSK_ID2TSKHDL(5), NULL);
	_KERNEL_TSK_SET_TSKPRI(_KERNEL_TSK_ID2TSKHDL(5), 2);
	_KERNEL_TSK_SET_QUE   (_KERNEL_TSK_ID2TSKHDL(6), NULL);
	_KERNEL_TSK_SET_TSKPRI(_KERNEL_TSK_ID2TSKHDL(6), 3);


	/* ----------------------------- */
	/*           FIFO順              */
	/* ----------------------------- */

	/* 1個の付け外し */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(check_que_empty());

	/* 1個の付け外し(頭抜き)  */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	tskhdl = _KERNEL_RMH_QUE(&test_que);
	TEST_ASSERT(tskhdl == _KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(check_que_empty());

	/* 2個の付け外し(先頭外し) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(check_que_empty());

	/* 2個の付け外し(末尾外し) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(check_que_empty());

	/* 2個の付け外し(頭抜き) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	tskhdl = _KERNEL_RMH_QUE(&test_que);
	TEST_ASSERT(tskhdl == _KERNEL_TSK_ID2TSKHDL(1));
	tskhdl = _KERNEL_RMH_QUE(&test_que);
	TEST_ASSERT(tskhdl == _KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(先→中→末の順で取り外し) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(check_que_empty());
	
	/* 3個の付け外し(先→末→中の順で取り外し) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(中→先→末の順で取り外し) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(中→末→先の順で取り外し) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(末→先→中の順で取り外し) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(末→中→先の順で取り外し) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(頭抜き) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TFIFO);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	tskhdl = _KERNEL_RMH_QUE(&test_que);
	TEST_ASSERT(tskhdl == _KERNEL_TSK_ID2TSKHDL(1));
	tskhdl = _KERNEL_RMH_QUE(&test_que);
	TEST_ASSERT(tskhdl == _KERNEL_TSK_ID2TSKHDL(2));
	tskhdl = _KERNEL_RMH_QUE(&test_que);
	TEST_ASSERT(tskhdl == _KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(check_que_empty());


	
	/* ----------------------------- */
	/*          優先度順             */
	/* ----------------------------- */

	/* 1個の付け外し */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(check_que_empty());


	/* 2個の付け外し(優先度：1→2) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(check_que_empty());

	/* 2個の付け外し(優先度：2→1) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(check_que_empty());

	/* 2個の付け外し(優先度：2→2 (FIFO順になるはず)) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(5), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(5));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(5));
	TEST_ASSERT(check_que_empty());
	
	/* 上の逆 */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(5), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(5));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(5));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(5));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(check_que_empty());


	/* 3個の付け外し(優先度：1→2→3) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(優先度：1→3→2) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(優先度：2→1→3) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(優先度：2→3→1) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(優先度：3→1→2) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(優先度：3→2→1) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(同優先混ぜ：先頭) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(4), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(4));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(4));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(同優先混ぜ：中) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(5), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(5));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(5));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(check_que_empty());

	/* 3個の付け外し(同優先混ぜ：末尾) */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(6), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(6));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(6));
	TEST_ASSERT(check_que_empty());


	/* ＡＬＬ：おまけ */
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(1), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(2), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(3), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(6), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(5), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));
	_KERNEL_ADD_QUE(&test_que, _KERNEL_TSK_ID2TSKHDL(4), TA_TPRI);
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(1));

	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(1));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(4));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(4));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(2));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(2));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(5));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(5));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(3));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(3));
	TEST_ASSERT(_KERNEL_REF_QUE(&test_que) == _KERNEL_TSK_ID2TSKHDL(6));
	_KERNEL_TRM_QUE(_KERNEL_TSK_ID2TSKHDL(6));
	TEST_ASSERT(check_que_empty());
	
	return TRUE;
}



