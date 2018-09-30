/* Copyright (C) 2014 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _FSL_QBMAN_BASE_H
#define _FSL_QBMAN_BASE_H

/**
 * struct qbman_block_desc - qbman block descriptor structure
 *
 * Descriptor for a QBMan instance on the SoC. On partitions/targets that do not
 * control this QBMan instance, these values may simply be place-holders. The
 * idea is simply that we be able to distinguish between them, eg. so that SWP
 * descriptors can identify which QBMan instance they belong to.
 */
struct qbman_block_desc {
	void *ccsr_reg_bar; /* CCSR register map */
	int irq_rerr;  /* Recoverable error interrupt line */
	int irq_nrerr; /* Non-recoverable error interrupt line */
};

/**
 * struct qbman_swp_desc - qbman software portal descriptor structure
 *
 * Descriptor for a QBMan software portal, expressed in terms that make sense to
 * the user context. Ie. on MC, this information is likely to be true-physical,
 * and instantiated statically at compile-time. On GPP, this information is
 * likely to be obtained via "discovery" over a partition's "layerscape bus"
 * (ie. in response to a MC portal command), and would take into account any
 * virtualisation of the GPP user's address space and/or interrupt numbering.
 */
struct qbman_swp_desc {
	const struct qbman_block_desc *block; /* The QBMan instance */
	void *cena_bar; /* Cache-enabled portal register map */
	void *cinh_bar; /* Cache-inhibited portal register map */
	uint32_t qman_version;
};

/* Driver object for managing a QBMan portal */
struct qbman_swp;

/**
 * struct qbman_fd - basci structure for qbman frame descriptor
 *
 * Place-holder for FDs, we represent it via the simplest form that we need for
 * now. Different overlays may be needed to support different options, etc. (It
 * is impractical to define One True Struct, because the resulting encoding
 * routines (lots of read-modify-writes) would be worst-case performance whether
 * or not circumstances required them.)
 *
 * Note, as with all data-structures exchanged between software and hardware (be
 * they located in the portal register map or DMA'd to and from main-memory),
 * the driver ensures that the caller of the driver API sees the data-structures
 * in host-endianness. "struct qbman_fd" is no exception. The 32-bit words
 * contained within this structure are represented in host-endianness, even if
 * hardware always treats them as little-endian. As such, if any of these fields
 * are interpreted in a binary (rather than numerical) fashion by hardware
 * blocks (eg. accelerators), then the user should be careful. We illustrate
 * with an example;
 *
 * Suppose the desired behaviour of an accelerator is controlled by the "frc"
 * field of the FDs that are sent to it. Suppose also that the behaviour desired
 * by the user corresponds to an "frc" value which is expressed as the literal
 * sequence of bytes 0xfe, 0xed, 0xab, and 0xba. So "frc" should be the 32-bit
 * value in which 0xfe is the first byte and 0xba is the last byte, and as
 * hardware is little-endian, this amounts to a 32-bit "value" of 0xbaabedfe. If
 * the software is little-endian also, this can simply be achieved by setting
 * frc=0xbaabedfe. On the other hand, if software is big-endian, it should set
 * frc=0xfeedabba! The best away of avoiding trouble with this sort of thing is
 * to treat the 32-bit words as numerical values, in which the offset of a field
 * from the beginning of the first byte (as required or generated by hardware)
 * is numerically encoded by a left-shift (ie. by raising the field to a
 * corresponding power of 2).  Ie. in the current example, software could set
 * "frc" in the following way, and it would work correctly on both little-endian
 * and big-endian operation;
 *    fd.frc = (0xfe << 0) | (0xed << 8) | (0xab << 16) | (0xba << 24);
 */
struct qbman_fd {
	union {
		uint32_t words[8];
		struct qbman_fd_simple {
			uint32_t addr_lo;
			uint32_t addr_hi;
			uint32_t len;
			/* offset in the MS 16 bits, BPID in the LS 16 bits */
			uint32_t bpid_offset;
			uint32_t frc; /* frame context */
			/* "err", "va", "cbmt", "asal", [...] */
			uint32_t ctrl;
			/* flow context */
			uint32_t flc_lo;
			uint32_t flc_hi;
		} simple;
	};
};

#endif /* !_FSL_QBMAN_BASE_H */