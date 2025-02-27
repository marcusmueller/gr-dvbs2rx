/* -*- c++ -*- */
/*
 * Copyright 2018 Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "bbdescrambler_bb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dvbs2rx {

bbdescrambler_bb::sptr bbdescrambler_bb::make(dvb_standard_t standard,
                                              dvb_framesize_t framesize,
                                              dvb_code_rate_t rate)
{
    return gnuradio::get_initial_sptr(
        new bbdescrambler_bb_impl(standard, framesize, rate));
}

/*
 * The private constructor
 */
bbdescrambler_bb_impl::bbdescrambler_bb_impl(dvb_standard_t standard,
                                             dvb_framesize_t framesize,
                                             dvb_code_rate_t rate)
    : gr::sync_block("bbdescrambler_bb",
                     gr::io_signature::make(1, 1, sizeof(unsigned char)),
                     gr::io_signature::make(1, 1, sizeof(unsigned char)))
{
    if (framesize == FECFRAME_NORMAL) {
        switch (rate) {
        case C1_4:
            kbch = 16008;
            break;
        case C1_3:
            kbch = 21408;
            break;
        case C2_5:
            kbch = 25728;
            break;
        case C1_2:
            kbch = 32208;
            break;
        case C3_5:
            kbch = 38688;
            break;
        case C2_3:
            kbch = 43040;
            break;
        case C3_4:
            kbch = 48408;
            break;
        case C4_5:
            kbch = 51648;
            break;
        case C5_6:
            kbch = 53840;
            break;
        case C8_9:
            kbch = 57472;
            break;
        case C9_10:
            kbch = 58192;
            break;
        case C2_9_VLSNR:
            kbch = 14208;
            break;
        case C13_45:
            kbch = 18528;
            break;
        case C9_20:
            kbch = 28968;
            break;
        case C90_180:
            kbch = 32208;
            break;
        case C96_180:
            kbch = 34368;
            break;
        case C11_20:
            kbch = 35448;
            break;
        case C100_180:
            kbch = 35808;
            break;
        case C104_180:
            kbch = 37248;
            break;
        case C26_45:
            kbch = 37248;
            break;
        case C18_30:
            kbch = 38688;
            break;
        case C28_45:
            kbch = 40128;
            break;
        case C23_36:
            kbch = 41208;
            break;
        case C116_180:
            kbch = 41568;
            break;
        case C20_30:
            kbch = 43008;
            break;
        case C124_180:
            kbch = 44448;
            break;
        case C25_36:
            kbch = 44808;
            break;
        case C128_180:
            kbch = 45888;
            break;
        case C13_18:
            kbch = 46608;
            break;
        case C132_180:
            kbch = 47328;
            break;
        case C22_30:
            kbch = 47328;
            break;
        case C135_180:
            kbch = 48408;
            break;
        case C140_180:
            kbch = 50208;
            break;
        case C7_9:
            kbch = 50208;
            break;
        case C154_180:
            kbch = 55248;
            break;
        default:
            kbch = 0;
            break;
        }
    } else if (framesize == FECFRAME_SHORT) {
        switch (rate) {
        case C1_4:
            kbch = 3072;
            break;
        case C1_3:
            kbch = 5232;
            break;
        case C2_5:
            kbch = 6312;
            break;
        case C1_2:
            kbch = 7032;
            break;
        case C3_5:
            kbch = 9552;
            break;
        case C2_3:
            kbch = 10632;
            break;
        case C3_4:
            kbch = 11712;
            break;
        case C4_5:
            kbch = 12432;
            break;
        case C5_6:
            kbch = 13152;
            break;
        case C8_9:
            kbch = 14232;
            break;
        case C11_45:
            kbch = 3792;
            break;
        case C4_15:
            kbch = 4152;
            break;
        case C14_45:
            kbch = 4872;
            break;
        case C7_15:
            kbch = 7392;
            break;
        case C8_15:
            kbch = 8472;
            break;
        case C26_45:
            kbch = 9192;
            break;
        case C32_45:
            kbch = 11352;
            break;
        case C1_5_VLSNR_SF2:
            kbch = 2512;
            break;
        case C11_45_VLSNR_SF2:
            kbch = 3792;
            break;
        case C1_5_VLSNR:
            kbch = 3072;
            break;
        case C4_15_VLSNR:
            kbch = 4152;
            break;
        case C1_3_VLSNR:
            kbch = 5232;
            break;
        default:
            kbch = 0;
            break;
        }
    } else {
        switch (rate) {
        case C1_5_MEDIUM:
            kbch = 5660;
            break;
        case C11_45_MEDIUM:
            kbch = 7740;
            break;
        case C1_3_MEDIUM:
            kbch = 10620;
            break;
        default:
            kbch = 0;
            break;
        }
    }

    init_bb_derandomiser();
    set_output_multiple(kbch);
}

/*
 * Our virtual destructor.
 */
bbdescrambler_bb_impl::~bbdescrambler_bb_impl() {}

void bbdescrambler_bb_impl::init_bb_derandomiser(void)
{
    int sr = 0x4A80;
    for (int i = 0; i < FRAME_SIZE_NORMAL; i++) {
        int b = ((sr) ^ (sr >> 1)) & 1;
        bb_derandomise[i] = b;
        sr >>= 1;
        if (b) {
            sr |= 0x4000;
        }
    }
}

int bbdescrambler_bb_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    for (int i = 0; i < noutput_items; i += kbch) {
        for (int j = 0; j < (int)kbch; ++j) {
            out[i + j] = in[i + j] ^ bb_derandomise[j];
        }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dvbs2rx */
} /* namespace gr */
