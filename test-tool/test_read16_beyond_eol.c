/* 
   Copyright (C) 2013 Ronnie Sahlberg <ronneisahlberg@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>

#include <CUnit/CUnit.h>

#include "iscsi.h"
#include "scsi-lowlevel.h"
#include "iscsi-test-cu.h"


static int ilog2(size_t i)
{
	int j = -1;

	while (i) {
		i >>= 1;
		j++;
	}
	return j;
}

void
test_read16_beyond_eol(void)
{ 
	int i, ret;

	CHECK_FOR_SBC;

	logging(LOG_VERBOSE, LOG_BLANK_LINE);
	logging(LOG_VERBOSE, "Test READ16 1-256 blocks one block beyond the end");
	for (i = 1; i <= 256; i++) {
		if (maximum_transfer_length && maximum_transfer_length < i) {
			break;
		}

		ret = read16_lbaoutofrange(iscsic, tgt_lun, num_blocks + 1 - i,
					   i * block_size, block_size,
					   0, 0, 0, 0, 0, NULL);
		if (ret == -2) {
			logging(LOG_NORMAL, "[SKIPPED] READ16 is not implemented on this target and it does not claim SBC-3 support.");
			CU_PASS("READ16 is not implemented and no SBC-3 support claimed.");
			return;
		}	
		CU_ASSERT_EQUAL(ret, 0);
	}


	logging(LOG_VERBOSE, "Test READ16 1-256 blocks at LBA==2^63");
	for (i = 1; i <= 256; i++) {
		if (maximum_transfer_length && maximum_transfer_length < i) {
			break;
		}

		ret = read16_lbaoutofrange(iscsic, tgt_lun, 0x8000000000000000ULL,
					   i * block_size, block_size,
					   0, 0, 0, 0, 0, NULL);
		CU_ASSERT_EQUAL(ret, 0);
	}


	logging(LOG_VERBOSE, "Test READ16 1-256 blocks at LBA==2^%d",
		64 - ilog2(block_size));
	for (i = 1; i <= 256; i++) {
		if (maximum_transfer_length && maximum_transfer_length < i) {
			break;
		}

		ret = read16_lbaoutofrange(iscsic, tgt_lun,
					   1ULL << (64 - ilog2(block_size)),
					   i * block_size, block_size,
					   0, 0, 0, 0, 0, NULL);
		CU_ASSERT_EQUAL(ret, 0);
	}


	logging(LOG_VERBOSE, "Test READ16 1-256 blocks at LBA==2^%d",
		63 - ilog2(block_size));
	for (i = 1; i <= 256; i++) {
		if (maximum_transfer_length && maximum_transfer_length < i) {
			break;
		}

		ret = read16_lbaoutofrange(iscsic, tgt_lun,
					   1ULL << (63 - ilog2(block_size)),
					   i * block_size, block_size,
					   0, 0, 0, 0, 0, NULL);
		CU_ASSERT_EQUAL(ret, 0);
	}


	logging(LOG_VERBOSE, "Test READ16 1-256 blocks at LBA==-1");
	for (i = 1; i <= 256; i++) {
		if (maximum_transfer_length && maximum_transfer_length < i) {
			break;
		}

		ret = read16_lbaoutofrange(iscsic, tgt_lun, -1, i * block_size,
					   block_size, 0, 0, 0, 0, 0, NULL);
		CU_ASSERT_EQUAL(ret, 0);
	}


	logging(LOG_VERBOSE, "Test READ16 2-256 blocks all but one block beyond the end");
	for (i = 2; i <= 256; i++) {
		if (maximum_transfer_length && maximum_transfer_length < i) {
			break;
		}

		ret = read16_lbaoutofrange(iscsic, tgt_lun, num_blocks - 1,
					   i * block_size, block_size,
					   0, 0, 0, 0, 0, NULL);
		CU_ASSERT_EQUAL(ret, 0);
	}
}
