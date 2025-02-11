// Reading file 'blinky5.asc'..

module chip (input io_0_8_1, output io_13_12_1, output io_13_9_1, output io_13_11_0, output io_13_11_1, output io_13_12_0);

wire io_0_8_1;
// (0, 0, 'glb_netwk_6')
// (0, 8, 'fabout')
// (0, 8, 'io_1/D_IN_0')
// (0, 8, 'io_1/PAD')
// (0, 8, 'local_g1_4')
// (0, 8, 'span12_horz_12')
// (1, 7, 'neigh_op_tnl_2')
// (1, 7, 'neigh_op_tnl_6')
// (1, 8, 'neigh_op_lft_2')
// (1, 8, 'neigh_op_lft_6')
// (1, 8, 'sp12_h_r_15')
// (1, 9, 'neigh_op_bnl_2')
// (1, 9, 'neigh_op_bnl_6')
// (2, 8, 'sp12_h_r_16')
// (3, 8, 'sp12_h_r_19')
// (4, 8, 'sp12_h_r_20')
// (5, 8, 'sp12_h_r_23')
// (6, 8, 'sp12_h_l_23')
// (11, 6, 'lutff_global/clk')
// (11, 7, 'lutff_global/clk')
// (11, 8, 'lutff_global/clk')
// (11, 9, 'lutff_global/clk')
// (11, 10, 'lutff_global/clk')
// (11, 11, 'lutff_global/clk')
// (12, 9, 'lutff_global/clk')
// (12, 10, 'lutff_global/clk')

reg io_13_12_1 = 0;
// (9, 11, 'sp4_h_r_0')
// (10, 10, 'neigh_op_tnr_4')
// (10, 11, 'neigh_op_rgt_4')
// (10, 11, 'sp4_h_r_13')
// (10, 12, 'neigh_op_bnr_4')
// (11, 10, 'neigh_op_top_4')
// (11, 11, 'lutff_4/out')
// (11, 11, 'sp4_h_r_24')
// (11, 12, 'neigh_op_bot_4')
// (12, 10, 'neigh_op_tnl_4')
// (12, 11, 'neigh_op_lft_4')
// (12, 11, 'sp4_h_r_37')
// (12, 12, 'local_g3_4')
// (12, 12, 'lutff_0/in_1')
// (12, 12, 'neigh_op_bnl_4')
// (13, 11, 'span4_horz_37')
// (13, 11, 'span4_vert_t_14')
// (13, 12, 'io_1/D_OUT_0')
// (13, 12, 'io_1/PAD')
// (13, 12, 'local_g1_6')
// (13, 12, 'span4_vert_b_14')
// (13, 13, 'span4_vert_b_10')
// (13, 14, 'span4_vert_b_6')
// (13, 15, 'span4_vert_b_2')

reg n3 = 0;
// (10, 5, 'neigh_op_tnr_4')
// (10, 6, 'neigh_op_rgt_4')
// (10, 7, 'neigh_op_bnr_4')
// (11, 5, 'neigh_op_top_4')
// (11, 6, 'local_g0_4')
// (11, 6, 'lutff_4/in_0')
// (11, 6, 'lutff_4/out')
// (11, 7, 'local_g1_4')
// (11, 7, 'lutff_0/in_1')
// (11, 7, 'lutff_1/in_0')
// (11, 7, 'neigh_op_bot_4')
// (12, 5, 'neigh_op_tnl_4')
// (12, 6, 'neigh_op_lft_4')
// (12, 7, 'neigh_op_bnl_4')

reg n4 = 0;
// (10, 6, 'neigh_op_tnr_1')
// (10, 7, 'neigh_op_rgt_1')
// (10, 8, 'neigh_op_bnr_1')
// (11, 6, 'neigh_op_top_1')
// (11, 7, 'local_g1_1')
// (11, 7, 'lutff_1/in_1')
// (11, 7, 'lutff_1/out')
// (11, 8, 'neigh_op_bot_1')
// (12, 6, 'neigh_op_tnl_1')
// (12, 7, 'neigh_op_lft_1')
// (12, 8, 'neigh_op_bnl_1')

reg n5 = 0;
// (10, 6, 'neigh_op_tnr_2')
// (10, 7, 'neigh_op_rgt_2')
// (10, 8, 'neigh_op_bnr_2')
// (11, 6, 'neigh_op_top_2')
// (11, 7, 'local_g0_2')
// (11, 7, 'lutff_2/in_0')
// (11, 7, 'lutff_2/out')
// (11, 8, 'neigh_op_bot_2')
// (12, 6, 'neigh_op_tnl_2')
// (12, 7, 'neigh_op_lft_2')
// (12, 8, 'neigh_op_bnl_2')

reg n6 = 0;
// (10, 6, 'neigh_op_tnr_3')
// (10, 7, 'neigh_op_rgt_3')
// (10, 8, 'neigh_op_bnr_3')
// (11, 6, 'neigh_op_top_3')
// (11, 7, 'local_g0_3')
// (11, 7, 'lutff_3/in_0')
// (11, 7, 'lutff_3/out')
// (11, 8, 'neigh_op_bot_3')
// (12, 6, 'neigh_op_tnl_3')
// (12, 7, 'neigh_op_lft_3')
// (12, 8, 'neigh_op_bnl_3')

reg n7 = 0;
// (10, 6, 'neigh_op_tnr_4')
// (10, 7, 'neigh_op_rgt_4')
// (10, 8, 'neigh_op_bnr_4')
// (11, 6, 'neigh_op_top_4')
// (11, 7, 'local_g0_4')
// (11, 7, 'lutff_4/in_0')
// (11, 7, 'lutff_4/out')
// (11, 8, 'neigh_op_bot_4')
// (12, 6, 'neigh_op_tnl_4')
// (12, 7, 'neigh_op_lft_4')
// (12, 8, 'neigh_op_bnl_4')

reg n8 = 0;
// (10, 6, 'neigh_op_tnr_5')
// (10, 7, 'neigh_op_rgt_5')
// (10, 8, 'neigh_op_bnr_5')
// (11, 6, 'neigh_op_top_5')
// (11, 7, 'local_g0_5')
// (11, 7, 'lutff_5/in_0')
// (11, 7, 'lutff_5/out')
// (11, 8, 'neigh_op_bot_5')
// (12, 6, 'neigh_op_tnl_5')
// (12, 7, 'neigh_op_lft_5')
// (12, 8, 'neigh_op_bnl_5')

reg n9 = 0;
// (10, 6, 'neigh_op_tnr_6')
// (10, 7, 'neigh_op_rgt_6')
// (10, 8, 'neigh_op_bnr_6')
// (11, 6, 'neigh_op_top_6')
// (11, 7, 'local_g0_6')
// (11, 7, 'lutff_6/in_0')
// (11, 7, 'lutff_6/out')
// (11, 8, 'neigh_op_bot_6')
// (12, 6, 'neigh_op_tnl_6')
// (12, 7, 'neigh_op_lft_6')
// (12, 8, 'neigh_op_bnl_6')

reg n10 = 0;
// (10, 6, 'neigh_op_tnr_7')
// (10, 7, 'neigh_op_rgt_7')
// (10, 8, 'neigh_op_bnr_7')
// (11, 6, 'neigh_op_top_7')
// (11, 7, 'local_g0_7')
// (11, 7, 'lutff_7/in_0')
// (11, 7, 'lutff_7/out')
// (11, 8, 'neigh_op_bot_7')
// (12, 6, 'neigh_op_tnl_7')
// (12, 7, 'neigh_op_lft_7')
// (12, 8, 'neigh_op_bnl_7')

reg n11 = 0;
// (10, 7, 'neigh_op_tnr_0')
// (10, 8, 'neigh_op_rgt_0')
// (10, 9, 'neigh_op_bnr_0')
// (11, 7, 'neigh_op_top_0')
// (11, 8, 'local_g0_0')
// (11, 8, 'lutff_0/in_0')
// (11, 8, 'lutff_0/out')
// (11, 9, 'neigh_op_bot_0')
// (12, 7, 'neigh_op_tnl_0')
// (12, 8, 'neigh_op_lft_0')
// (12, 9, 'neigh_op_bnl_0')

reg n12 = 0;
// (10, 7, 'neigh_op_tnr_1')
// (10, 8, 'neigh_op_rgt_1')
// (10, 9, 'neigh_op_bnr_1')
// (11, 7, 'neigh_op_top_1')
// (11, 8, 'local_g0_1')
// (11, 8, 'lutff_1/in_0')
// (11, 8, 'lutff_1/out')
// (11, 9, 'neigh_op_bot_1')
// (12, 7, 'neigh_op_tnl_1')
// (12, 8, 'neigh_op_lft_1')
// (12, 9, 'neigh_op_bnl_1')

reg n13 = 0;
// (10, 7, 'neigh_op_tnr_2')
// (10, 8, 'neigh_op_rgt_2')
// (10, 9, 'neigh_op_bnr_2')
// (11, 7, 'neigh_op_top_2')
// (11, 8, 'local_g0_2')
// (11, 8, 'lutff_2/in_0')
// (11, 8, 'lutff_2/out')
// (11, 9, 'neigh_op_bot_2')
// (12, 7, 'neigh_op_tnl_2')
// (12, 8, 'neigh_op_lft_2')
// (12, 9, 'neigh_op_bnl_2')

reg n14 = 0;
// (10, 7, 'neigh_op_tnr_3')
// (10, 8, 'neigh_op_rgt_3')
// (10, 9, 'neigh_op_bnr_3')
// (11, 7, 'neigh_op_top_3')
// (11, 8, 'local_g0_3')
// (11, 8, 'lutff_3/in_0')
// (11, 8, 'lutff_3/out')
// (11, 9, 'neigh_op_bot_3')
// (12, 7, 'neigh_op_tnl_3')
// (12, 8, 'neigh_op_lft_3')
// (12, 9, 'neigh_op_bnl_3')

reg n15 = 0;
// (10, 7, 'neigh_op_tnr_4')
// (10, 8, 'neigh_op_rgt_4')
// (10, 9, 'neigh_op_bnr_4')
// (11, 7, 'neigh_op_top_4')
// (11, 8, 'local_g0_4')
// (11, 8, 'lutff_4/in_0')
// (11, 8, 'lutff_4/out')
// (11, 9, 'neigh_op_bot_4')
// (12, 7, 'neigh_op_tnl_4')
// (12, 8, 'neigh_op_lft_4')
// (12, 9, 'neigh_op_bnl_4')

reg n16 = 0;
// (10, 7, 'neigh_op_tnr_5')
// (10, 8, 'neigh_op_rgt_5')
// (10, 9, 'neigh_op_bnr_5')
// (11, 7, 'neigh_op_top_5')
// (11, 8, 'local_g0_5')
// (11, 8, 'lutff_5/in_0')
// (11, 8, 'lutff_5/out')
// (11, 9, 'neigh_op_bot_5')
// (12, 7, 'neigh_op_tnl_5')
// (12, 8, 'neigh_op_lft_5')
// (12, 9, 'neigh_op_bnl_5')

reg n17 = 0;
// (10, 7, 'neigh_op_tnr_6')
// (10, 8, 'neigh_op_rgt_6')
// (10, 9, 'neigh_op_bnr_6')
// (11, 7, 'neigh_op_top_6')
// (11, 8, 'local_g0_6')
// (11, 8, 'lutff_6/in_0')
// (11, 8, 'lutff_6/out')
// (11, 9, 'neigh_op_bot_6')
// (12, 7, 'neigh_op_tnl_6')
// (12, 8, 'neigh_op_lft_6')
// (12, 9, 'neigh_op_bnl_6')

reg n18 = 0;
// (10, 7, 'neigh_op_tnr_7')
// (10, 8, 'neigh_op_rgt_7')
// (10, 9, 'neigh_op_bnr_7')
// (11, 7, 'neigh_op_top_7')
// (11, 8, 'local_g0_7')
// (11, 8, 'lutff_7/in_0')
// (11, 8, 'lutff_7/out')
// (11, 9, 'neigh_op_bot_7')
// (12, 7, 'neigh_op_tnl_7')
// (12, 8, 'neigh_op_lft_7')
// (12, 9, 'neigh_op_bnl_7')

reg n19 = 0;
// (10, 8, 'neigh_op_tnr_0')
// (10, 9, 'neigh_op_rgt_0')
// (10, 10, 'neigh_op_bnr_0')
// (11, 8, 'neigh_op_top_0')
// (11, 9, 'local_g0_0')
// (11, 9, 'lutff_0/in_0')
// (11, 9, 'lutff_0/out')
// (11, 10, 'neigh_op_bot_0')
// (12, 8, 'neigh_op_tnl_0')
// (12, 9, 'neigh_op_lft_0')
// (12, 10, 'neigh_op_bnl_0')

reg n20 = 0;
// (10, 8, 'neigh_op_tnr_1')
// (10, 9, 'neigh_op_rgt_1')
// (10, 10, 'neigh_op_bnr_1')
// (11, 8, 'neigh_op_top_1')
// (11, 9, 'local_g0_1')
// (11, 9, 'lutff_1/in_0')
// (11, 9, 'lutff_1/out')
// (11, 10, 'neigh_op_bot_1')
// (12, 8, 'neigh_op_tnl_1')
// (12, 9, 'neigh_op_lft_1')
// (12, 10, 'neigh_op_bnl_1')

reg n21 = 0;
// (10, 8, 'neigh_op_tnr_2')
// (10, 9, 'neigh_op_rgt_2')
// (10, 10, 'neigh_op_bnr_2')
// (11, 8, 'neigh_op_top_2')
// (11, 9, 'local_g0_2')
// (11, 9, 'lutff_2/in_0')
// (11, 9, 'lutff_2/out')
// (11, 10, 'neigh_op_bot_2')
// (12, 8, 'neigh_op_tnl_2')
// (12, 9, 'neigh_op_lft_2')
// (12, 10, 'neigh_op_bnl_2')

reg n22 = 0;
// (10, 8, 'neigh_op_tnr_3')
// (10, 9, 'neigh_op_rgt_3')
// (10, 10, 'neigh_op_bnr_3')
// (11, 8, 'neigh_op_top_3')
// (11, 9, 'local_g0_3')
// (11, 9, 'lutff_3/in_0')
// (11, 9, 'lutff_3/out')
// (11, 10, 'neigh_op_bot_3')
// (12, 8, 'neigh_op_tnl_3')
// (12, 9, 'neigh_op_lft_3')
// (12, 10, 'neigh_op_bnl_3')

reg n23 = 0;
// (10, 8, 'neigh_op_tnr_4')
// (10, 9, 'neigh_op_rgt_4')
// (10, 10, 'neigh_op_bnr_4')
// (11, 8, 'neigh_op_top_4')
// (11, 9, 'local_g0_4')
// (11, 9, 'lutff_4/in_0')
// (11, 9, 'lutff_4/out')
// (11, 10, 'neigh_op_bot_4')
// (12, 8, 'neigh_op_tnl_4')
// (12, 9, 'neigh_op_lft_4')
// (12, 10, 'neigh_op_bnl_4')

reg n24 = 0;
// (10, 8, 'neigh_op_tnr_5')
// (10, 9, 'neigh_op_rgt_5')
// (10, 10, 'neigh_op_bnr_5')
// (11, 8, 'neigh_op_top_5')
// (11, 9, 'local_g0_5')
// (11, 9, 'lutff_5/in_0')
// (11, 9, 'lutff_5/out')
// (11, 10, 'neigh_op_bot_5')
// (12, 8, 'neigh_op_tnl_5')
// (12, 9, 'local_g1_5')
// (12, 9, 'lutff_4/in_0')
// (12, 9, 'neigh_op_lft_5')
// (12, 10, 'neigh_op_bnl_5')

reg n25 = 0;
// (10, 8, 'neigh_op_tnr_6')
// (10, 9, 'neigh_op_rgt_6')
// (10, 10, 'neigh_op_bnr_6')
// (11, 8, 'neigh_op_top_6')
// (11, 9, 'local_g0_6')
// (11, 9, 'lutff_6/in_0')
// (11, 9, 'lutff_6/out')
// (11, 10, 'neigh_op_bot_6')
// (12, 8, 'neigh_op_tnl_6')
// (12, 9, 'local_g0_6')
// (12, 9, 'lutff_0/in_0')
// (12, 9, 'neigh_op_lft_6')
// (12, 10, 'neigh_op_bnl_6')

reg n26 = 0;
// (10, 8, 'neigh_op_tnr_7')
// (10, 9, 'neigh_op_rgt_7')
// (10, 10, 'neigh_op_bnr_7')
// (11, 8, 'neigh_op_top_7')
// (11, 9, 'local_g0_7')
// (11, 9, 'lutff_7/in_0')
// (11, 9, 'lutff_7/out')
// (11, 10, 'neigh_op_bot_7')
// (12, 8, 'neigh_op_tnl_7')
// (12, 9, 'neigh_op_lft_7')
// (12, 10, 'local_g3_7')
// (12, 10, 'lutff_2/in_0')
// (12, 10, 'neigh_op_bnl_7')

reg n27 = 0;
// (10, 9, 'neigh_op_tnr_0')
// (10, 10, 'neigh_op_rgt_0')
// (10, 11, 'neigh_op_bnr_0')
// (11, 9, 'neigh_op_top_0')
// (11, 10, 'local_g0_0')
// (11, 10, 'lutff_0/in_0')
// (11, 10, 'lutff_0/out')
// (11, 11, 'neigh_op_bot_0')
// (12, 9, 'neigh_op_tnl_0')
// (12, 10, 'local_g0_0')
// (12, 10, 'lutff_3/in_1')
// (12, 10, 'neigh_op_lft_0')
// (12, 11, 'neigh_op_bnl_0')

reg n28 = 0;
// (10, 9, 'neigh_op_tnr_1')
// (10, 10, 'neigh_op_rgt_1')
// (10, 11, 'neigh_op_bnr_1')
// (11, 9, 'neigh_op_top_1')
// (11, 10, 'local_g0_1')
// (11, 10, 'lutff_1/in_0')
// (11, 10, 'lutff_1/out')
// (11, 11, 'local_g1_1')
// (11, 11, 'lutff_4/in_0')
// (11, 11, 'neigh_op_bot_1')
// (12, 9, 'neigh_op_tnl_1')
// (12, 10, 'neigh_op_lft_1')
// (12, 11, 'neigh_op_bnl_1')

wire n29;
// (11, 7, 'lutff_1/cout')
// (11, 7, 'lutff_2/in_3')

wire n30;
// (11, 7, 'lutff_2/cout')
// (11, 7, 'lutff_3/in_3')

wire n31;
// (11, 7, 'lutff_3/cout')
// (11, 7, 'lutff_4/in_3')

wire n32;
// (11, 7, 'lutff_4/cout')
// (11, 7, 'lutff_5/in_3')

wire n33;
// (11, 7, 'lutff_5/cout')
// (11, 7, 'lutff_6/in_3')

wire n34;
// (11, 7, 'lutff_6/cout')
// (11, 7, 'lutff_7/in_3')

wire n35;
// (11, 7, 'lutff_7/cout')
// (11, 8, 'carry_in')
// (11, 8, 'carry_in_mux')
// (11, 8, 'lutff_0/in_3')

wire n36;
// (11, 8, 'lutff_0/cout')
// (11, 8, 'lutff_1/in_3')

wire n37;
// (11, 8, 'lutff_1/cout')
// (11, 8, 'lutff_2/in_3')

wire n38;
// (11, 8, 'lutff_2/cout')
// (11, 8, 'lutff_3/in_3')

wire n39;
// (11, 8, 'lutff_3/cout')
// (11, 8, 'lutff_4/in_3')

wire n40;
// (11, 8, 'lutff_4/cout')
// (11, 8, 'lutff_5/in_3')

wire n41;
// (11, 8, 'lutff_5/cout')
// (11, 8, 'lutff_6/in_3')

wire n42;
// (11, 8, 'lutff_6/cout')
// (11, 8, 'lutff_7/in_3')

wire n43;
// (11, 8, 'lutff_7/cout')
// (11, 9, 'carry_in')
// (11, 9, 'carry_in_mux')
// (11, 9, 'lutff_0/in_3')

reg n44 = 0;
// (11, 8, 'neigh_op_tnr_0')
// (11, 9, 'neigh_op_rgt_0')
// (11, 10, 'neigh_op_bnr_0')
// (12, 8, 'neigh_op_top_0')
// (12, 9, 'local_g1_0')
// (12, 9, 'lutff_0/out')
// (12, 9, 'lutff_3/in_0')
// (12, 10, 'local_g1_0')
// (12, 10, 'lutff_7/in_0')
// (12, 10, 'neigh_op_bot_0')
// (13, 8, 'logic_op_tnl_0')
// (13, 9, 'logic_op_lft_0')
// (13, 10, 'logic_op_bnl_0')

wire io_13_9_1;
// (11, 8, 'neigh_op_tnr_3')
// (11, 9, 'neigh_op_rgt_3')
// (11, 10, 'neigh_op_bnr_3')
// (12, 8, 'neigh_op_top_3')
// (12, 9, 'lutff_3/out')
// (12, 10, 'neigh_op_bot_3')
// (13, 8, 'logic_op_tnl_3')
// (13, 9, 'io_1/D_OUT_0')
// (13, 9, 'io_1/PAD')
// (13, 9, 'local_g0_3')
// (13, 9, 'logic_op_lft_3')
// (13, 10, 'logic_op_bnl_3')

reg n46 = 0;
// (11, 8, 'neigh_op_tnr_4')
// (11, 9, 'neigh_op_rgt_4')
// (11, 10, 'neigh_op_bnr_4')
// (12, 8, 'neigh_op_top_4')
// (12, 9, 'local_g0_4')
// (12, 9, 'lutff_3/in_1')
// (12, 9, 'lutff_4/out')
// (12, 10, 'neigh_op_bot_4')
// (13, 8, 'logic_op_tnl_4')
// (13, 9, 'logic_op_lft_4')
// (13, 10, 'logic_op_bnl_4')

wire n47;
// (11, 9, 'lutff_0/cout')
// (11, 9, 'lutff_1/in_3')

wire n48;
// (11, 9, 'lutff_1/cout')
// (11, 9, 'lutff_2/in_3')

wire n49;
// (11, 9, 'lutff_2/cout')
// (11, 9, 'lutff_3/in_3')

wire n50;
// (11, 9, 'lutff_3/cout')
// (11, 9, 'lutff_4/in_3')

wire n51;
// (11, 9, 'lutff_4/cout')
// (11, 9, 'lutff_5/in_3')

wire n52;
// (11, 9, 'lutff_5/cout')
// (11, 9, 'lutff_6/in_3')

wire n53;
// (11, 9, 'lutff_6/cout')
// (11, 9, 'lutff_7/in_3')

wire n54;
// (11, 9, 'lutff_7/cout')
// (11, 10, 'carry_in')
// (11, 10, 'carry_in_mux')
// (11, 10, 'lutff_0/in_3')

reg n55 = 0;
// (11, 9, 'neigh_op_tnr_2')
// (11, 10, 'neigh_op_rgt_2')
// (11, 11, 'neigh_op_bnr_2')
// (12, 9, 'neigh_op_top_2')
// (12, 10, 'local_g0_2')
// (12, 10, 'lutff_2/out')
// (12, 10, 'lutff_7/in_1')
// (12, 11, 'local_g0_2')
// (12, 11, 'lutff_3/in_1')
// (12, 11, 'neigh_op_bot_2')
// (13, 9, 'logic_op_tnl_2')
// (13, 10, 'logic_op_lft_2')
// (13, 11, 'logic_op_bnl_2')

reg n56 = 0;
// (11, 9, 'neigh_op_tnr_3')
// (11, 9, 'sp4_r_v_b_38')
// (11, 10, 'neigh_op_rgt_3')
// (11, 10, 'sp4_r_v_b_27')
// (11, 11, 'neigh_op_bnr_3')
// (11, 11, 'sp4_r_v_b_14')
// (11, 12, 'sp4_r_v_b_3')
// (12, 1, 'sp12_v_t_22')
// (12, 2, 'sp12_v_b_22')
// (12, 3, 'sp12_v_b_21')
// (12, 4, 'sp12_v_b_18')
// (12, 5, 'sp12_v_b_17')
// (12, 6, 'sp12_v_b_14')
// (12, 7, 'sp12_v_b_13')
// (12, 8, 'sp12_v_b_10')
// (12, 8, 'sp4_v_t_38')
// (12, 9, 'neigh_op_top_3')
// (12, 9, 'sp12_v_b_9')
// (12, 9, 'sp4_v_b_38')
// (12, 10, 'lutff_3/out')
// (12, 10, 'sp12_v_b_6')
// (12, 10, 'sp4_v_b_27')
// (12, 11, 'local_g0_3')
// (12, 11, 'lutff_3/in_0')
// (12, 11, 'neigh_op_bot_3')
// (12, 11, 'sp12_v_b_5')
// (12, 11, 'sp4_v_b_14')
// (12, 12, 'local_g1_3')
// (12, 12, 'lutff_0/in_0')
// (12, 12, 'sp12_v_b_2')
// (12, 12, 'sp4_v_b_3')
// (12, 13, 'sp12_v_b_1')
// (13, 9, 'logic_op_tnl_3')
// (13, 10, 'logic_op_lft_3')
// (13, 11, 'logic_op_bnl_3')

wire io_13_11_0;
// (11, 9, 'neigh_op_tnr_7')
// (11, 10, 'neigh_op_rgt_7')
// (11, 11, 'neigh_op_bnr_7')
// (12, 9, 'neigh_op_top_7')
// (12, 10, 'lutff_7/out')
// (12, 11, 'neigh_op_bot_7')
// (13, 9, 'logic_op_tnl_7')
// (13, 10, 'logic_op_lft_7')
// (13, 11, 'io_0/D_OUT_0')
// (13, 11, 'io_0/PAD')
// (13, 11, 'local_g1_7')
// (13, 11, 'logic_op_bnl_7')

wire n58;
// (11, 10, 'lutff_0/cout')
// (11, 10, 'lutff_1/in_3')

wire io_13_11_1;
// (11, 10, 'neigh_op_tnr_3')
// (11, 11, 'neigh_op_rgt_3')
// (11, 12, 'neigh_op_bnr_3')
// (12, 10, 'neigh_op_top_3')
// (12, 11, 'lutff_3/out')
// (12, 12, 'neigh_op_bot_3')
// (13, 10, 'logic_op_tnl_3')
// (13, 11, 'io_1/D_OUT_0')
// (13, 11, 'io_1/PAD')
// (13, 11, 'local_g0_3')
// (13, 11, 'logic_op_lft_3')
// (13, 12, 'logic_op_bnl_3')

wire io_13_12_0;
// (11, 11, 'neigh_op_tnr_0')
// (11, 12, 'neigh_op_rgt_0')
// (11, 13, 'neigh_op_bnr_0')
// (12, 11, 'neigh_op_top_0')
// (12, 12, 'lutff_0/out')
// (12, 13, 'neigh_op_bot_0')
// (13, 11, 'logic_op_tnl_0')
// (13, 12, 'io_0/D_OUT_0')
// (13, 12, 'io_0/PAD')
// (13, 12, 'local_g0_0')
// (13, 12, 'logic_op_lft_0')
// (13, 13, 'logic_op_bnl_0')

wire n61;
// (11, 7, 'lutff_0/cout')

wire n62;
// (12, 10, 'lutff_7/lout')

wire n63;
// (12, 9, 'lutff_3/lout')

wire n64;
// (11, 7, 'lutff_1/lout')

wire n65;
// (11, 7, 'lutff_0/out')

wire n66;
// (11, 7, 'lutff_0/lout')

wire n67;
// (11, 7, 'carry_in_mux')

// Carry-In for (11 7)
assign n67 = 1;

wire n68;
// (11, 8, 'lutff_0/lout')

wire n69;
// (11, 7, 'lutff_3/lout')

wire n70;
// (11, 6, 'lutff_4/lout')

wire n71;
// (11, 8, 'lutff_1/lout')

wire n72;
// (11, 7, 'lutff_2/lout')

wire n73;
// (12, 10, 'lutff_3/lout')

wire n74;
// (11, 8, 'lutff_2/lout')

wire n75;
// (12, 10, 'lutff_2/lout')

wire n76;
// (11, 8, 'lutff_3/lout')

wire n77;
// (12, 12, 'lutff_0/lout')

wire n78;
// (11, 8, 'lutff_4/lout')

wire n79;
// (11, 8, 'lutff_5/lout')

wire n80;
// (12, 11, 'lutff_3/lout')

wire n81;
// (11, 9, 'lutff_7/lout')

wire n82;
// (11, 8, 'lutff_6/lout')

wire n83;
// (11, 9, 'lutff_6/lout')

wire n84;
// (11, 8, 'lutff_7/lout')

wire n85;
// (11, 9, 'lutff_5/lout')

wire n86;
// (11, 9, 'lutff_4/lout')

wire n87;
// (12, 9, 'lutff_4/lout')

wire n88;
// (11, 11, 'lutff_4/lout')

wire n89;
// (11, 9, 'lutff_3/lout')

wire n90;
// (11, 9, 'lutff_2/lout')

wire n91;
// (11, 10, 'lutff_0/lout')

wire n92;
// (11, 9, 'lutff_1/lout')

wire n93;
// (11, 10, 'lutff_1/lout')

wire n94;
// (11, 7, 'lutff_5/lout')

wire n95;
// (11, 9, 'lutff_0/lout')

wire n96;
// (12, 9, 'lutff_0/lout')

wire n97;
// (11, 7, 'lutff_4/lout')

wire n98;
// (11, 7, 'lutff_7/lout')

wire n99;
// (11, 7, 'lutff_6/lout')

assign n66 = /* LUT   11  7  0 */ 1'b0;
assign n62 = /* LUT   12 10  7 */ (n55 ? !n44 : n44);
assign n63 = /* LUT   12  9  3 */ (n46 ? !n44 : n44);
assign n64 = /* LUT   11  7  1 */ (n4 ? !n3 : n3);
assign n68 = /* LUT   11  8  0 */ (n35 ? !n11 : n11);
assign n69 = /* LUT   11  7  3 */ (n30 ? !n6 : n6);
assign n70 = /* LUT   11  6  4 */ !n3;
assign n71 = /* LUT   11  8  1 */ (n36 ? !n12 : n12);
assign n72 = /* LUT   11  7  2 */ (n29 ? !n5 : n5);
assign n73 = /* LUT   12 10  3 */ n27;
assign n74 = /* LUT   11  8  2 */ (n37 ? !n13 : n13);
assign n75 = /* LUT   12 10  2 */ n26;
assign n76 = /* LUT   11  8  3 */ (n38 ? !n14 : n14);
assign n77 = /* LUT   12 12  0 */ (io_13_12_1 ? !n56 : n56);
assign n78 = /* LUT   11  8  4 */ (n39 ? !n15 : n15);
assign n79 = /* LUT   11  8  5 */ (n40 ? !n16 : n16);
assign n80 = /* LUT   12 11  3 */ (n55 ? !n56 : n56);
assign n81 = /* LUT   11  9  7 */ (n53 ? !n26 : n26);
assign n82 = /* LUT   11  8  6 */ (n41 ? !n17 : n17);
assign n83 = /* LUT   11  9  6 */ (n52 ? !n25 : n25);
assign n84 = /* LUT   11  8  7 */ (n42 ? !n18 : n18);
assign n85 = /* LUT   11  9  5 */ (n51 ? !n24 : n24);
assign n86 = /* LUT   11  9  4 */ (n50 ? !n23 : n23);
assign n87 = /* LUT   12  9  4 */ n24;
assign n88 = /* LUT   11 11  4 */ n28;
assign n89 = /* LUT   11  9  3 */ (n49 ? !n22 : n22);
assign n90 = /* LUT   11  9  2 */ (n48 ? !n21 : n21);
assign n91 = /* LUT   11 10  0 */ (n54 ? !n27 : n27);
assign n92 = /* LUT   11  9  1 */ (n47 ? !n20 : n20);
assign n93 = /* LUT   11 10  1 */ (n58 ? !n28 : n28);
assign n94 = /* LUT   11  7  5 */ (n32 ? !n8 : n8);
assign n95 = /* LUT   11  9  0 */ (n43 ? !n19 : n19);
assign n96 = /* LUT   12  9  0 */ n25;
assign n97 = /* LUT   11  7  4 */ (n31 ? !n7 : n7);
assign n98 = /* LUT   11  7  7 */ (n34 ? !n10 : n10);
assign n99 = /* LUT   11  7  6 */ (n33 ? !n9 : n9);
assign n29 = /* CARRY 11  7  1 */ (n4 & 1'b0) | ((n4 | 1'b0) & n61);
assign n61 = /* CARRY 11  7  0 */ (n3 & 1'b0) | ((n3 | 1'b0) & n67);
assign n36 = /* CARRY 11  8  0 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n35);
assign n31 = /* CARRY 11  7  3 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n30);
assign n37 = /* CARRY 11  8  1 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n36);
assign n30 = /* CARRY 11  7  2 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n29);
assign n38 = /* CARRY 11  8  2 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n37);
assign n39 = /* CARRY 11  8  3 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n38);
assign n40 = /* CARRY 11  8  4 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n39);
assign n41 = /* CARRY 11  8  5 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n40);
assign n54 = /* CARRY 11  9  7 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n53);
assign n42 = /* CARRY 11  8  6 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n41);
assign n53 = /* CARRY 11  9  6 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n52);
assign n43 = /* CARRY 11  8  7 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n42);
assign n52 = /* CARRY 11  9  5 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n51);
assign n51 = /* CARRY 11  9  4 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n50);
assign n50 = /* CARRY 11  9  3 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n49);
assign n49 = /* CARRY 11  9  2 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n48);
assign n58 = /* CARRY 11 10  0 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n54);
assign n48 = /* CARRY 11  9  1 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n47);
assign n33 = /* CARRY 11  7  5 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n32);
assign n47 = /* CARRY 11  9  0 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n43);
assign n32 = /* CARRY 11  7  4 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n31);
assign n35 = /* CARRY 11  7  7 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n34);
assign n34 = /* CARRY 11  7  6 */ (1'b0 & 1'b0) | ((1'b0 | 1'b0) & n33);
/* FF 12 10  7 */ assign io_13_11_0 = n62;
/* FF 12  9  3 */ assign io_13_9_1 = n63;
/* FF 11  7  1 */ always @(posedge io_0_8_1) if (1'b1) n4 <= 1'b0 ? 1'b0 : n64;
/* FF 11  7  0 */ assign n65 = n66;
/* FF 11  8  0 */ always @(posedge io_0_8_1) if (1'b1) n11 <= 1'b0 ? 1'b0 : n68;
/* FF 11  7  3 */ always @(posedge io_0_8_1) if (1'b1) n6 <= 1'b0 ? 1'b0 : n69;
/* FF 11  6  4 */ always @(posedge io_0_8_1) if (1'b1) n3 <= 1'b0 ? 1'b0 : n70;
/* FF 11  8  1 */ always @(posedge io_0_8_1) if (1'b1) n12 <= 1'b0 ? 1'b0 : n71;
/* FF 11  7  2 */ always @(posedge io_0_8_1) if (1'b1) n5 <= 1'b0 ? 1'b0 : n72;
/* FF 12 10  3 */ always @(posedge io_0_8_1) if (1'b1) n56 <= 1'b0 ? 1'b0 : n73;
/* FF 11  8  2 */ always @(posedge io_0_8_1) if (1'b1) n13 <= 1'b0 ? 1'b0 : n74;
/* FF 12 10  2 */ always @(posedge io_0_8_1) if (1'b1) n55 <= 1'b0 ? 1'b0 : n75;
/* FF 11  8  3 */ always @(posedge io_0_8_1) if (1'b1) n14 <= 1'b0 ? 1'b0 : n76;
/* FF 12 12  0 */ assign io_13_12_0 = n77;
/* FF 11  8  4 */ always @(posedge io_0_8_1) if (1'b1) n15 <= 1'b0 ? 1'b0 : n78;
/* FF 11  8  5 */ always @(posedge io_0_8_1) if (1'b1) n16 <= 1'b0 ? 1'b0 : n79;
/* FF 12 11  3 */ assign io_13_11_1 = n80;
/* FF 11  9  7 */ always @(posedge io_0_8_1) if (1'b1) n26 <= 1'b0 ? 1'b0 : n81;
/* FF 11  8  6 */ always @(posedge io_0_8_1) if (1'b1) n17 <= 1'b0 ? 1'b0 : n82;
/* FF 11  9  6 */ always @(posedge io_0_8_1) if (1'b1) n25 <= 1'b0 ? 1'b0 : n83;
/* FF 11  8  7 */ always @(posedge io_0_8_1) if (1'b1) n18 <= 1'b0 ? 1'b0 : n84;
/* FF 11  9  5 */ always @(posedge io_0_8_1) if (1'b1) n24 <= 1'b0 ? 1'b0 : n85;
/* FF 11  9  4 */ always @(posedge io_0_8_1) if (1'b1) n23 <= 1'b0 ? 1'b0 : n86;
/* FF 12  9  4 */ always @(posedge io_0_8_1) if (1'b1) n46 <= 1'b0 ? 1'b0 : n87;
/* FF 11 11  4 */ always @(posedge io_0_8_1) if (1'b1) io_13_12_1 <= 1'b0 ? 1'b0 : n88;
/* FF 11  9  3 */ always @(posedge io_0_8_1) if (1'b1) n22 <= 1'b0 ? 1'b0 : n89;
/* FF 11  9  2 */ always @(posedge io_0_8_1) if (1'b1) n21 <= 1'b0 ? 1'b0 : n90;
/* FF 11 10  0 */ always @(posedge io_0_8_1) if (1'b1) n27 <= 1'b0 ? 1'b0 : n91;
/* FF 11  9  1 */ always @(posedge io_0_8_1) if (1'b1) n20 <= 1'b0 ? 1'b0 : n92;
/* FF 11 10  1 */ always @(posedge io_0_8_1) if (1'b1) n28 <= 1'b0 ? 1'b0 : n93;
/* FF 11  7  5 */ always @(posedge io_0_8_1) if (1'b1) n8 <= 1'b0 ? 1'b0 : n94;
/* FF 11  9  0 */ always @(posedge io_0_8_1) if (1'b1) n19 <= 1'b0 ? 1'b0 : n95;
/* FF 12  9  0 */ always @(posedge io_0_8_1) if (1'b1) n44 <= 1'b0 ? 1'b0 : n96;
/* FF 11  7  4 */ always @(posedge io_0_8_1) if (1'b1) n7 <= 1'b0 ? 1'b0 : n97;
/* FF 11  7  7 */ always @(posedge io_0_8_1) if (1'b1) n10 <= 1'b0 ? 1'b0 : n98;
/* FF 11  7  6 */ always @(posedge io_0_8_1) if (1'b1) n9 <= 1'b0 ? 1'b0 : n99;

endmodule

