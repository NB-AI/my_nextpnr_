/*
 *  nextpnr -- Next Generation Place and Route
 *
 *  Copyright (C) 2018  Claire Xenia Wolf <claire@yosyshq.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <chrono>
#include <cmath>
#include <queue>

#include "log.h"
#include "router1.h"
#include "scope_lock.h"
#include "timing.h"

//#include "../../ice40/arch.h" // mine to get coordinate measures of chip
//#include "../kernel/nextpnr.h"

// newly added:
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//#include </usr/local/bin/cadical/cadical.hpp>
//#include </home/c/FPGA_programs/cadical-master/src/cadical.hpp>
//#include </usr/local/bin/cadical>
//#include <cadical.hpp>
#include <cadical.hpp>
#include <limits>

// to compare sets' overlaps:
#include <algorithm>
#include <iterator>


namespace {

USING_NEXTPNR_NAMESPACE

struct arc_key
{
    NetInfo *net_info;
    // logical user cell port index
    store_index<PortRef> user_idx;
    // physical index into cell->bel pin mapping (usually 0)
    unsigned phys_idx;

    bool operator==(const arc_key &other) const
    {
        return (net_info == other.net_info) && (user_idx == other.user_idx) && (phys_idx == other.phys_idx);
    }
    bool operator<(const arc_key &other) const
    {
        return net_info == other.net_info
                       ? (user_idx == other.user_idx ? phys_idx < other.phys_idx : user_idx < other.user_idx)
                       : net_info->name < other.net_info->name;
    }

    unsigned int hash() const
    {
        std::size_t seed = std::hash<NetInfo *>()(net_info);
        seed ^= user_idx.hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(phys_idx) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

struct arc_entry
{
    arc_key arc;
    delay_t pri;
    int randtag = 0;

    struct Less
    {
        bool operator()(const arc_entry &lhs, const arc_entry &rhs) const noexcept
        {
            if (lhs.pri != rhs.pri)
                return lhs.pri < rhs.pri;
            return lhs.randtag < rhs.randtag;
        }
    };
};

////////////////////// my classes //////////

class Path{

    public:

        // std::set<std::tuple<int, int, int, int>> channel_segment_loc;
        std::vector<std::tuple<int, int, int, int, int, int>> channel_segment_loc;
        //// WireId found_dst_wire = WireId();
        //// std::set<arc_key*> solution_for_arcs; // stores the arcs to which the path is already a solution such that in the upcoming fpl18 round the path isn't selected again

        //// bool found_main_dst;

        //// bool explored = false;
        ////std::set<WireId> dst_set{};

        float real_delay = 0;


};


class AStar{ // for each net

    public:

        std::priority_queue<float, std::vector<float>, std::greater<float>> min_heap;
        std::map<float, std::vector<Path*>> delay2Paths; // A* storage of non-complete paths to follow // each delay stores vector of paths,
        // the paths are stored as vectors of channel segments

};

class Assump_l{

    public:

        //NetInfo* net_info;
        int dimacs_var;
        arc_key arc;
        int delay;



};

struct boolSetter{bool val = false;};


//////////////////////////////////


struct QueuedWire
{
    WireId wire;
    PipId pip;

    delay_t delay = 0, penalty = 0, bonus = 0, togo = 0;
    int randtag = 0;

    struct Greater
    {
        bool operator()(const QueuedWire &lhs, const QueuedWire &rhs) const noexcept
        {
            delay_t l = lhs.delay + lhs.penalty + lhs.togo;
            delay_t r = rhs.delay + rhs.penalty + rhs.togo;
            NPNR_ASSERT(l >= 0);
            NPNR_ASSERT(r >= 0);
            l -= lhs.bonus;
            r -= rhs.bonus;
            return l == r ? lhs.randtag > rhs.randtag : l > r;
        }
    };
};

struct Router1
{
    // global routing:
    // first go over all nets starting with the source wire. Fill up A* with channel segments such that it can start global routing:
    std::map<NetInfo*, std::map<arc_key, AStar*>> net2arc2AStar;

    std::map<NetInfo*, std::vector<arc_entry>> arc_map;

    // SAT assumption combination:
    std::map<arc_key, std::vector<Assump_l*>> nested_assump_map;
    std::set<std::string> used_assump_combis; // dimacs sorted in fixed order by string
    std::vector<std::vector<int>> container_combi_vec;
    std::vector<std::string> container_combi_str;

    std::map<std::tuple<NetInfo*, int, int, int, int, int, int>, std::vector<PipId>> ChannelSegment; // key: (start pip x, start pip y, end pip x, end pip y)

    // Transfer variables to SAT DIMACS format and back:
    std::map<NetInfo*, std::map<WireId, int>> netinfo2wireId2Dimacs;
    std::map<int, std::pair<NetInfo*, WireId>> dimacs2NetinfoWireId;
    std::map<NetInfo*, std::map<std::pair<int, int>, PipId>> netinfo2DimacsPair2PipId;

	// for clause 10 collect all destination wires per net:
	std::map<NetInfo*, std::vector<WireId>> net2Dsts;

    // conflict clauses:
    std::map<WireId, std::vector<int>> wireId2DimacsVec;
    std::map<WireId, int> startIndexOf_wireId2DimacsVec; // where to start in the next round such that we don't have to reformulate all conflict clauses

    // SAT assumptions:
    std::vector<Assump_l*> assumpVec;//Dimacs2Netinfo;
    int startIndexOf_assumpVec = 0;
    std::vector<int> pow_collector;

    // For the first line of the DIMACS CNF following information is needed:
    int max_dimacs_var = 0; // also needed to assign the wires of certain nets DIMCACS variables
    int cnf_clause_counter = 0;

    // Finding conflicting nets:
    std::map<WireId, std::vector<Assump_l*>> wireId2assumpVec;
    std::map<WireId, int> startIndexOfConflicts; // where to start in the next round such that we don't have to reformulate all conflict clauses

	// For an UNSAT solution find out which assumptions could be participated in the congestion process:
    std::map<Assump_l*, std::set<WireId>> assump2wireSet;

    // initialize sat solver instance:
    CaDiCaL::Solver * solver = new CaDiCaL::Solver;

    // needed for switch backtranslation, when we have a DIMACS which belongs to an arc, the other wire of a switch needs to have the same arc:
    std::map<NetInfo*, std::map<WireId, std::set<arc_key>>> wireId2activeArcSet;

    Context *ctx;
    const Router1Cfg &cfg;

    std::priority_queue<arc_entry, std::vector<arc_entry>, arc_entry::Less> arc_queue;
    dict<WireId, pool<arc_key>> wire_to_arcs;
    dict<arc_key, pool<WireId>> arc_to_wires;
    pool<arc_key> queued_arcs;

    std::priority_queue<QueuedWire, std::vector<QueuedWire>, QueuedWire::Greater> queue;

    dict<WireId, int> wireScores;
    dict<NetInfo *, int, hash_ptr_ops> netScores;

    int arcs_with_ripup = 0;
    int arcs_without_ripup = 0;
    bool ripup_flag;

    TimingAnalyser tmg;

    bool timing_driven = false; //true;

    Router1(Context *ctx, const Router1Cfg &cfg) : ctx(ctx), cfg(cfg), tmg(ctx)
    {
        timing_driven = false; //ctx->setting<bool>("timing_driven");
        tmg.setup_only = false;
        tmg.with_clock_skew = true;
        tmg.setup();
        tmg.run();
    }

    void arc_queue_insert(const arc_key &arc, WireId src_wire, WireId dst_wire)
    {
        if (queued_arcs.count(arc))
            return;

        delay_t pri = (arc.net_info->constant_value == IdString())
                              ? (ctx->estimateDelay(src_wire, dst_wire) *
                                 (100 * tmg.get_criticality(CellPortKey(arc.net_info->users.at(arc.user_idx)))))
                              : 0;

        arc_entry entry;
        entry.arc = arc;
        entry.pri = pri;
        entry.randtag = ctx->rng();

#if 0
        if (ctx->debug)
            log("[arc_queue_insert] %s (%d) %s %s [%d %d]\n", ctx->nameOf(entry.arc.net_info), entry.arc.user_idx,
                ctx->nameOfWire(src_wire), ctx->nameOfWire(dst_wire), (int)entry.pri, entry.randtag);
#endif
		// mine:
		//const arc_key* arc_ptr = &arc;
    	//arc_map[arc_ptr->net_info].push_back(arc_ptr);
    	//arc_map[arc.net_info].push_back(arc);
		//


		arc_map[arc.net_info].push_back(entry);

        arc_queue.push(entry);
        queued_arcs.insert(arc);
    }

    void arc_queue_insert(const arc_key &arc)
    {
        if (queued_arcs.count(arc))
            return;

        NetInfo *net_info = arc.net_info;
        auto user_idx = arc.user_idx;
        unsigned phys_idx = arc.phys_idx;

        auto src_wire = ctx->getNetinfoSourceWire(net_info);
        auto dst_wire = ctx->getNetinfoSinkWire(net_info, net_info->users[user_idx], phys_idx);

        arc_queue_insert(arc, src_wire, dst_wire);
    }

    arc_key arc_queue_pop()
    {
        arc_entry entry = arc_queue.top();

#if 0
        if (ctx->debug)
            log("[arc_queue_pop] %s (%d) [%d %d]\n", ctx->nameOf(entry.arc.net_info), entry.arc.user_idx,
                (int)entry.pri, entry.randtag);
#endif

        arc_queue.pop();
        queued_arcs.erase(entry.arc);
        return entry.arc;
    }


    bool skip_net(NetInfo *net_info)
    {
#ifdef ARCH_ECP5
        // ECP5 global nets currently appear part-unrouted due to arch database limitations
        // Don't touch them in the router
        if (net_info->is_global)
            return true;
#endif
        if (net_info->driver.cell == nullptr && net_info->constant_value == IdString())
            return true;

        return false;
    }

    void check()
    {
        pool<arc_key> valid_arcs;

        for (auto &net_it : ctx->nets) {
            NetInfo *net_info = net_it.second.get();
            pool<WireId> valid_wires_for_net;

            if (skip_net(net_info))
                continue;

#if 0
            if (ctx->debug)
                log("[check] net: %s\n", ctx->nameOf(net_info));
#endif

            auto src_wire = ctx->getNetinfoSourceWire(net_info);
            log_assert(src_wire != WireId());

            for (auto user : net_info->users.enumerate()) {
                unsigned phys_idx = 0;
                for (auto dst_wire : ctx->getNetinfoSinkWires(net_info, user.value)) {
                    log_assert(dst_wire != WireId());

                    arc_key arc;
                    arc.net_info = net_info;
                    arc.user_idx = user.index;
                    arc.phys_idx = phys_idx++;
                    valid_arcs.insert(arc);
#if 0
                    if (ctx->debug)
                    log("[check]   arc: %s %s\n", ctx->nameOfWire(src_wire), ctx->nameOfWire(dst_wire));
#endif

                    for (WireId wire : arc_to_wires[arc]) {
#if 0
                        if (ctx->debug)
                        log("[check]     wire: %s\n", ctx->nameOfWire(wire));
#endif
                        valid_wires_for_net.insert(wire);
                        log_assert(wire_to_arcs[wire].count(arc));
                        log_assert(net_info->wires.count(wire));
                    }
                }
            }

            for (auto &it : net_info->wires) {
                WireId w = it.first;
                log_assert(valid_wires_for_net.count(w));
            }
        }

        for (auto &it : wire_to_arcs) {
            for (auto &arc : it.second)
                log_assert(valid_arcs.count(arc));
        }

        for (auto &it : arc_to_wires) {
            log_assert(valid_arcs.count(it.first));
        }
    }

    void setup()
    {
        dict<WireId, NetInfo *> src_to_net;
        dict<WireId, arc_key> dst_to_arc;

        std::vector<IdString> net_names;
        for (auto &net_it : ctx->nets)
            net_names.push_back(net_it.first);

        ctx->sorted_shuffle(net_names);

        for (IdString net_name : net_names) {
            NetInfo *net_info = ctx->nets.at(net_name).get();

            if (skip_net(net_info))
                continue;

            auto src_wire = ctx->getNetinfoSourceWire(net_info);

            if (src_wire == WireId() && net_info->constant_value == IdString())
                log_error("No wire found for port %s on source cell %s.\n", ctx->nameOf(net_info->driver.port),
                          ctx->nameOf(net_info->driver.cell));

            if (src_to_net.count(src_wire))
                log_error("Found two nets with same source wire %s: %s vs %s\n", ctx->nameOfWire(src_wire),
                          ctx->nameOf(net_info), ctx->nameOf(src_to_net.at(src_wire)));

            if (dst_to_arc.count(src_wire))
                log_error("Wire %s is used as source and sink in different nets: %s vs %s (%d)\n",
                          ctx->nameOfWire(src_wire), ctx->nameOf(net_info),
                          ctx->nameOf(dst_to_arc.at(src_wire).net_info), dst_to_arc.at(src_wire).user_idx.idx());

            for (auto user : net_info->users.enumerate()) {
                unsigned phys_idx = 0;
                for (auto dst_wire : ctx->getNetinfoSinkWires(net_info, user.value)) {
                    arc_key arc;
                    arc.net_info = net_info;
                    arc.user_idx = user.index;
                    arc.phys_idx = phys_idx++;

                    if (dst_wire == WireId())
                        log_error("No wire found for port %s on destination cell %s.\n", ctx->nameOf(user.value.port),
                                  ctx->nameOf(user.value.cell));

                    if (dst_to_arc.count(dst_wire)) {
                        if (dst_to_arc.at(dst_wire).net_info == net_info)
                            continue;
                        log_error("Found two arcs with same sink wire %s: %s (%d) vs %s (%d)\n",
                                  ctx->nameOfWire(dst_wire), ctx->nameOf(net_info), user.index.idx(),
                                  ctx->nameOf(dst_to_arc.at(dst_wire).net_info),
                                  dst_to_arc.at(dst_wire).user_idx.idx());
                    }

                    if (src_to_net.count(dst_wire))
                        log_error("Wire %s is used as source and sink in different nets: %s vs %s (%d)\n",
                                  ctx->nameOfWire(dst_wire), ctx->nameOf(src_to_net.at(dst_wire)),
                                  ctx->nameOf(net_info), user.index.idx());

                    dst_to_arc[dst_wire] = arc;

                    if (net_info->wires.count(dst_wire) == 0) {

                        arc_queue_insert(arc, src_wire, dst_wire);
                        continue;
                    }

                    WireId cursor = dst_wire;
                    wire_to_arcs[cursor].insert(arc);
                    arc_to_wires[arc].insert(cursor);

                    while (src_wire != cursor && (net_info->constant_value == IdString() ||
                                                  ctx->getWireConstantValue(cursor) != net_info->constant_value)) {
                        auto it = net_info->wires.find(cursor);
                        if (it == net_info->wires.end()) {


                            arc_queue_insert(arc, src_wire, dst_wire);
                            break;
                        }

                        NPNR_ASSERT(it->second.pip != PipId());
                        cursor = ctx->getPipSrcWire(it->second.pip);
                        wire_to_arcs[cursor].insert(arc);
                        arc_to_wires[arc].insert(cursor);
                    }
                }
                // TODO: this matches the situation before supporting multiple cell->bel pins, but do we want to keep
                // this invariant?
                if (phys_idx == 0)
                    log_warning("No wires found for port %s on destination cell %s.\n", ctx->nameOf(user.value.port),
                                ctx->nameOf(user.value.cell));
            }

            src_to_net[src_wire] = net_info;

            std::vector<WireId> unbind_wires;

            for (auto &it : net_info->wires)
                if (it.second.strength < STRENGTH_LOCKED && wire_to_arcs.count(it.first) == 0)
                    unbind_wires.push_back(it.first);

            for (auto it : unbind_wires)
                ctx->unbindWire(it);
        }
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int my_wire_no(WireId wire, bool wire_type_13)
{
    std::string wire_name = ctx->nameOfWire(wire);


    int wire_no = -1;

    int find_start_wire_no = wire_name.find(":in_");

    int find_end_wire_no;
    if(wire_type_13)
        find_end_wire_no = wire_name.find("_lut");

    else // but wire type 12
        find_end_wire_no = static_cast<int>(wire_name.size());

    int len_number = find_end_wire_no - (find_start_wire_no + 4);
    std::string wire_no_string = wire_name.substr(find_start_wire_no + 4, len_number);
     std::stringstream wire_no_string_ss(wire_no_string);
    wire_no_string_ss >> wire_no; // intra_coord gets its value here

    return wire_no;
}

	int my_intra_coord_finder(WireId wire, PipId pip){


		//// different wire types in the iCE40:

        std::string plb_wire_type11 = "/local_g";
        std::string plb_wire_type6_type12_type13_type14_part1 = "/lutff_";
        std::string plb_wire_type13_part2 = "_lut";
        std::string plb_wire_type12_part2 = ":in_";
		std::string plb_wire_type14_part2a = ":cout";
		std::string plb_wire_type14_part2b = ":lout";
        std::string plb_wire_type6_part2 = ":out";
		std::string plb_wire_type10 = "/lutff_global:";

		std::string inter_wire_type8 = "/glb_netw_";
		std::string inter_wire_type9 = "/glb2local_";
        std::string inter_wire_typeVertical_typeHorizontal_part1 = "/sp";
        std::string inter_wire_typeVertical_part2 = "_v";
        std::string inter_wire_typeHorizontal_part2 = "_h";
		std::string inter_wire_type15 = "/carry_in_mux"; // no competitioners, thus comment out


		std::string ram_wire_part1 = "/ram:";
		std::string ram_wire_type6_type7_part2 = "RDATA_";
		std::string ram_wire_type10_a = "CLK";
		std::string ram_wire_type10_b = "E";
		std::string ram_wire_type12_a = "MASK";
		std::string ram_wire_type12_b = "DATA"; // run after ram6 and ram7 test!
		std::string ram_wire_type12_c = "ADDR";


		std::string io_wire_type6_type7_type10_type12_type15_part1 = "/io_";
		std::string io_wire_type6_type7_part2 = ":D_IN_";
		std::string io_wire_type12_part2 = "OUT_";
		std::string io_wire_type15_part2 = ":latch";
		std::string io_wire_type10_part2 = "global"; // has to be tried out after io15!
		std::string io_wire_typeFabout = "/fabout";


		//// fix the coordinate to describe the wire type:

		std::string wire_name = ctx->nameOfWire(wire);

        int find_not_happen = wire_name.find("neigh_op_");
        if(find_not_happen != -1){log_info("ALERT: new wire type! %s\n", ctx->nameOfWire(wire));}

        int pip_end_x;
        int pip_end_y;

        if(pip != PipId()){

            Loc loc = ctx->getPipLocation(pip);
		    pip_end_x = loc.x;
            pip_end_y = loc.y;
        }
        else{

            pip_end_x = -1;
            pip_end_y = -1;
        }

   		int intra_coord = -1;


		int find_type11 = wire_name.find(plb_wire_type11);

		if(find_type11 != -1){

			intra_coord = -11;

		}
		else{ // wire not of type 11

		    int find_mix = wire_name.find(plb_wire_type6_type12_type13_type14_part1); // find "/lutff_"

		    if(find_mix != -1){

				int find_type12_type13 = wire_name.find(plb_wire_type12_part2); // find ":in_"

		        if(find_type12_type13 != -1){

					int len_number = find_type12_type13 - (find_mix + 7);
					std::string intra_coord_string = wire_name.substr(find_mix + 7, len_number);
			 		std::stringstream intra_coord_string_ss(intra_coord_string);
					intra_coord_string_ss >> intra_coord; // intra_coord gets its value here

					int find_type13 = wire_name.find(plb_wire_type13_part2); // find "_lut"

					if(find_type13 != -1){

						intra_coord = intra_coord + 20;
					}

		        }
		        else{ // wire not of type 12, 13 | 11

		            int find_type14_a = wire_name.find(plb_wire_type14_part2a);
		            int find_type14_b = wire_name.find(plb_wire_type14_part2b);


		            if(find_type14_a != -1 || find_type14_b != -1){

		                intra_coord = -14;

		            }
		            else{ // wire not of type 14 | 11, 12, 13

		           		// type7 is competitioner of type6. Still, this should be no problem as they point in the same direction of information flow and can't be circular, one net
		           		// automatically collects only one of them. One net flow: 6 or 7 -> 11 -> 12 -> 13 (impasse because it is the input of a LUT)
		                int find_type6 = wire_name.find(plb_wire_type6_part2);

		        		if(find_type6 != -1){
		        	
		                    intra_coord = -67;

		                }
		                else{// next wire not of type 6, 7, 11, 12, 13, 14

							int find_type10 = wire_name.find(plb_wire_type10);

							if(find_type10 != -1){

								intra_coord = -10;
							}
						}
		            }
		        }
		    }
	        else{ // wire not of type 10 | 6, 7, 11, 12, 13, 14

				// check out horizontal wires between PLB boxes:

	            int find_interMix = wire_name.find(inter_wire_typeVertical_typeHorizontal_part1); // find "/sp"

	            if(find_interMix != -1){

	            	int find_horizontal = wire_name.find(inter_wire_typeHorizontal_part2);

	            	if(find_horizontal != -1){

	            		intra_coord = -45;
	            	}
	            	else{ // wire not of type 4, 5 | 6, 7, 10, 11, 12, 13, 14

	            		int find_vertical = wire_name.find(inter_wire_typeVertical_part2);

	            		if(find_vertical != -1){

	            			// find out if we have wire types 1/2 or 3 because they have different vertical channel segments. This question is only relevant when the
	            			// previous switches are local:

	            			int wire_x;
	            			int find_x_part1 = wire_name.find("X");
	            			int find_x_part2 = wire_name.find("/Y");

            				int len_number = find_x_part2 - (find_x_part1 + 1);
							std::string intra_coord_string = wire_name.substr(find_x_part1 + 1, len_number);
					 		std::stringstream intra_coord_string_ss(intra_coord_string);
							intra_coord_string_ss >> wire_x; // intra_coord gets its value here

	            			if(pip_end_x == wire_x){intra_coord = -12;}
	            			else{intra_coord = -3;}
	            		}
	            	}
	            }
	        	else{ // wire not of type 1, 2, 3, 4, 5 | 6, 7, 10, 11, 12, 13, 14

					int find_type8 = wire_name.find(inter_wire_type8);

					if(find_type8 != -1){

						intra_coord = -8;
					}
					else{ // wire not of type 8 | 1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14

						int find_type9 = wire_name.find(inter_wire_type9);

						//if(find_interMix != -1 || find_type8 != -1 || find_type9 != -1){
						if(find_type9 != -1){

							intra_coord = -9;
						}
						else{ // wire not of type 9 | 1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14

							int find_type15 = wire_name.find(inter_wire_type15);

							if(find_type15 != -1){

								intra_coord = -15;
							}
							else{ // wire not of type 15 | 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14

								// consider IO blocks:
								// idential: type8, type11

								int find_ioMix = wire_name.find(io_wire_type6_type7_type10_type12_type15_part1); // find "/io_"

								if(find_ioMix != -1){

									int find_io6_io7 = wire_name.find(io_wire_type6_type7_part2);

									if(find_io6_io7 != -1){

			            				for (auto pip_end_end : ctx->getPipsDownhill(wire)){

				                    		Loc loc_pip_end_end = ctx->getPipLocation(pip_end_end);
				                    		int pip_end_end_x = loc_pip_end_end.x;
				                    		int pip_end_end_y = loc_pip_end_end.y;

				                    		if((pip_end_end_x != pip_end_x) || (pip_end_end_y != pip_end_y)){intra_coord = -6;}
				                    		else{intra_coord = -7;}

		                        			break;
		                        		}
									}
									else{ // wire not of type io6, io7 | 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15

										int find_io15 = wire_name.find(io_wire_type15_part2);

										if(find_io15 != -1){

											intra_coord = -15;
										}
										else{ // // wire not of type io15 | 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, io6, io7

											int find_io10 = wire_name.find(io_wire_type10_part2);

											if(find_io10 != -1){

												intra_coord = -10;
											}
											else{ // wire not of type io10 | 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, io6, io7, io15

												int find_io12 = wire_name.find(io_wire_type12_part2);

												if(find_io12 != -1){

													// find out coord as there are several channel options for io12 (like in PLB type12):

													int find_stopper = wire_name.find(":");
													int len_number = find_stopper - (find_ioMix + 4); // + 4 because "/io_" has length of 4
													std::string intra_coord_string = wire_name.substr(find_ioMix + 4, len_number);
							     					std::stringstream intra_coord_string_ss(intra_coord_string);
													intra_coord_string_ss >> intra_coord;
												}
											}
										}
									}
								}
								else{ // wire not of type io6, io7, io10, io12, io15 | 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15

									// last io wire:
									int find_ioFab = wire_name.find(io_wire_typeFabout);

									if(find_ioFab != -1){

										intra_coord = -16;
									}
									else{ // wire not of type io16 | 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, io6, io7, io10, io12, io15

										// Finally, the RAM box is to consider:
										// wire type8 and type11 seem to be identical.

										int find_ramMix = wire_name.find(ram_wire_part1);

										if(find_ramMix != -1){

											int find_ram6_ram7 = wire_name.find(ram_wire_type6_type7_part2);

											if(find_ram6_ram7 != -1){

											for (auto pip_end_end : ctx->getPipsDownhill(wire)){

						                		Loc loc_pip_end_end = ctx->getPipLocation(pip_end_end);
						                		int pip_end_end_x = loc_pip_end_end.x;
						                		int pip_end_end_y = loc_pip_end_end.y;

						                		if((pip_end_end_x != pip_end_x) || (pip_end_end_y != pip_end_y)){intra_coord = -6;}
						                		else{intra_coord = -7;}

				                    			break;
				                    			}

											//	intra_coord = -67;
											}
											else{ // wire not of type ram6, ram7 | 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, io6, io7, io10, io12, io15, io16

												int find_ram10_a = wire_name.find(ram_wire_type10_a);
												int find_ram10_b = wire_name.find(ram_wire_type10_b);

												if(find_ram10_a != -1 || find_ram10_b != -1){

													intra_coord = -10;
												}
												else{ // wire not of type ram10 | 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, io6, io7, io10, io12, io15, io16, ram6, ram7

													// check for channel 12 in ram: this one seems to be one channel only as they come from and lead to the identical boxes
													// (contrary to IO and PLB). No need of separate coordinates here:

													int find_ram12_a = wire_name.find(ram_wire_type12_a);
													int find_ram12_b = wire_name.find(ram_wire_type12_b);
													int find_ram12_c = wire_name.find(ram_wire_type12_c);

													if(find_ram12_a != -1 || find_ram12_b != -1 || find_ram12_c != -1){

														intra_coord = -12;
													}
													else{// wire not of any type!
														// wire not of type ram12 | 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, io6, io7, io10, io12, io15, io16, ram6, ram7, ram10

														log_info("Problem: no wire type found of wire %s\n", ctx->nameOfWire(wire));
													}
												}
											}
										}
										else{// wire not of any type!
												// wire not of type 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, io6, io7, io10, io12, io15, io16, ram6, ram7, ram10, ram12

												log_info("Problem: no wire type found of wire %s\n", ctx->nameOfWire(wire));
										}
									}
								}
							}
						}
					}
	        	}
	    	}

		} // outest else
 		
		return intra_coord;
	}



    void my_local_conflict_clauses_creator(Router1 router){ // call at end of each round

        // create a file which stores the at-least-one clauses to solve:
        std::ofstream file("CNF.csv", std::ios_base::app);

        // consider the case that one wire is wanted by several nets - clause 7:
        for(auto& ele : wireId2assumpVec){


            auto& wire = ele.first;
            auto& assump_vec = ele.second;

         
            if(assump_vec.size() > 1 && (static_cast<int>(assump_vec.size()) != startIndexOfConflicts[wire])){ // second test shows that new elements were added in the new round
            // then we need a conflict clause

                for(auto it = assump_vec.begin() + startIndexOfConflicts[wire]; it != assump_vec.end(); ++it){

                    auto* assump1 = *it; // get the pointer element
                    auto net1 = assump1->arc.net_info;

                    bool free_pass = false;
                    if(assump1->dimacs_var==-1){free_pass = true;}

                    for(auto it2 = it; it2 >= assump_vec.begin(); --it2){

                        auto* assump2 = *it2; 
                        auto net2 = assump2->arc.net_info;

                        bool free_pass2 = false;
                        if(assump2->dimacs_var==-1){free_pass2 = true;}

                        if(net1 != net2){ // assumption don't come from the same net and thus are in conflict

                            // get belonging net-wire DIMACS variables:
                            auto outer1 = netinfo2wireId2Dimacs[net1];
                            auto dimacs_wire1 = outer1[wire];

                            auto outer2 = netinfo2wireId2Dimacs[net2];
                            auto dimacs_wire2 = outer2[wire]; // we have one wire but two DIMACS values belonging to the different nets


                            //// create conflict clauses:

                            if(free_pass && free_pass2){ // we will get UNSAT result because of both variables are src and dst wires at once and always have to be true but exclude each other here by creating clause 7

                                file <<  "-" << std::to_string(dimacs_wire1) << " -" << std::to_string(dimacs_wire2) << " 0\n";
                                ++cnf_clause_counter;
                                                                
                                solver->clause(-dimacs_wire1, -dimacs_wire2);
                            }
                            else if(free_pass){

                                file <<  "-" << std::to_string(dimacs_wire1) << " -" << std::to_string(dimacs_wire2) << " " << std::to_string(assump2->dimacs_var) << " 0\n";
                                ++cnf_clause_counter;
                                                                
                                solver->clause(-dimacs_wire1, -dimacs_wire2, assump2->dimacs_var);
                            }
                            else if(free_pass2){

                                file <<  "-" << std::to_string(dimacs_wire1) << " -" << std::to_string(dimacs_wire2) << " " << std::to_string(assump1->dimacs_var) << " 0\n";
                                ++cnf_clause_counter;
                                
                                solver->clause(-dimacs_wire1, -dimacs_wire2, assump1->dimacs_var);

                            }
                            else{ // usual case: No one of the variables/wires is at the same time source and destination wire
                                                
                                file <<  "-" << std::to_string(dimacs_wire1) << " -" << std::to_string(dimacs_wire2) << " " << std::to_string(assump1->dimacs_var) << " " << std::to_string(assump2->dimacs_var) << " 0\n";
                                ++cnf_clause_counter;
                                
                                solver->clause(-dimacs_wire1, -dimacs_wire2, assump1->dimacs_var, assump2->dimacs_var);
                            }
                        }
                    }
                }

                startIndexOfConflicts[wire] = static_cast<int>(assump_vec.size());
            }
        }
    }
    


    void my_local_router(Router1 router, Path* path, arc_key arc, float delay){ // path is pointer thus Path* to pass it
        // for the needed arcs call it each round to append SAT formula by new path clauses

        NetInfo *net_info = arc.net_info;

        // we want to fasten up the color/wire choice of an arc. For this purpose we want to assign the colors first to wires which have the least follow wires.
        // count the follow-up wires of each wire:
        std::map<WireId, std::vector<WireId>> wireId2Followers; // temporary
        wireId2Followers.clear();

        // create a file which stores the CNF formula:
        std::ofstream file("CNF.csv", std::ios_base::app);

        // the destination wire of the path has to be set to true:
        
        auto dst_wire = ctx->getNetinfoSinkWire(net_info, net_info->users[arc.user_idx], arc.phys_idx);

        // find out wire no of dst_wire:
        int type_dst = my_intra_coord_finder(dst_wire, PipId());
        int dst_wire_no = -1;
        if(type_dst >= 20){
            
            dst_wire_no = my_wire_no(dst_wire, true);
            log_info("h3: dst wire %s with wire no %d\n", ctx->nameOfWire(dst_wire), dst_wire_no);
        }

        auto& find_dst1 = netinfo2wireId2Dimacs[net_info];
        auto find_dst2 = find_dst1.find(dst_wire);
        if(find_dst2 == find_dst1.end()){

        		netinfo2wireId2Dimacs[net_info][dst_wire] = ++max_dimacs_var;

                std::pair<NetInfo*, WireId> nw_dst(net_info, dst_wire);
                dimacs2NetinfoWireId[netinfo2wireId2Dimacs[net_info][dst_wire]] = nw_dst;
        }

        // for a given global routing in a certain round we define SAT assumption variables to switch on and of paths:
        Assump_l* assump_l = new Assump_l();
        assump_l->delay = delay;
        assump_l->arc = arc;


        if(dst_wire == ctx->getNetinfoSourceWire(net_info)){ // when the source wire is the same as the destination wire then a simple clause which has to be given for the
            // current net is formulated

            file << std::to_string(netinfo2wireId2Dimacs[net_info][dst_wire]) << " 0\n";
            ++cnf_clause_counter;
            
            solver->clause(netinfo2wireId2Dimacs[net_info][dst_wire]);

            // consider also clause 7. However, we don't need an assumption as the dimacs var relates to the whole net which has to be always true:
            assump_l->dimacs_var = -1;
            wireId2assumpVec[dst_wire].push_back(assump_l);
            assump2wireSet[assump_l].insert(dst_wire);

            return;
        }

        assump_l->dimacs_var = ++max_dimacs_var;


        // for later usage store the assump_l in a map:
        nested_assump_map[arc].push_back(assump_l);

        int number_slots = static_cast<int>(nested_assump_map[arc].size());
        if(number_slots > 1){

            if(number_slots > 2){

                pow_collector[number_slots-1] = pow_collector[number_slots-1] - 1;
            }

            pow_collector[number_slots] = pow_collector[number_slots] + 1;

        }


        file << std::to_string(netinfo2wireId2Dimacs[net_info][dst_wire]) << " " << std::to_string(assump_l->dimacs_var) << " 0\n";
        ++cnf_clause_counter;
                
        solver->clause(netinfo2wireId2Dimacs[net_info][dst_wire], assump_l->dimacs_var);


        // collect assumptions:
        assumpVec.push_back(assump_l);

        // loop over the found channels:
        int start_dimacs;
        int next_dimacs;
        int helper_var_dimacs;

        std::set<int> dimacs_wires_of_one_level;


		std::map<bool, std::map<int, std::vector<int>>> wireType2wires; // bool to determine start and end dimacs


		int ind_counter = 0;
        for(auto coord : path->channel_segment_loc){ // all channel segements


			for(auto& outer : wireType2wires){

				outer.second.clear();
			}

			wireType2wires.clear();

            dimacs_wires_of_one_level.clear();

            int x1 = std::get<0>(coord);
            int y1 = std::get<1>(coord);
            int x2 = std::get<2>(coord);
            int y2 = std::get<3>(coord);
			int intra1 = std::get<4>(coord);
            int intra2 = std::get<5>(coord);


            std::vector<int> next_dimacs_col;
            next_dimacs_col.clear();

            ++ind_counter;


            // first find the single wires of the current channel segment
            for(auto pip : ChannelSegment[std::tuple_cat(std::make_tuple(net_info), coord)]){ // gives a vector of pips // one specific channel segment

                WireId start_wire = ctx->getPipSrcWire(pip);
                WireId next_wire = ctx->getPipDstWire(pip);



             
                wireId2Followers[start_wire].push_back(next_wire);
				wireId2Followers[next_wire]; // create new entry for clause 10


                //// care for the start wire:
                // collect the wires for DIMACS translation and for congestion avoidance:
                auto& inner = netinfo2wireId2Dimacs[net_info];
                auto find_wire = inner.find(start_wire);


                if(find_wire == inner.end()){ // so far no DIMACS variable for the wire-net combination exists
                    // create one:
                    start_dimacs = ++max_dimacs_var;
                    netinfo2wireId2Dimacs[net_info][start_wire] = start_dimacs;

                    std::pair<NetInfo*, WireId> nw(net_info, start_wire);
                    dimacs2NetinfoWireId[start_dimacs] = nw;
                }
                else{start_dimacs = inner[start_wire];}


                if(start_wire==ctx->getNetinfoSourceWire(net_info)){

                    // add source wire to CNF formula as a clause which means that the source wire always has to be true:
                    file << std::to_string(start_dimacs) << " 0\n"; // each net has to be fulfilled. Thus, this clause is assumption indepenedent // only source wire
                    ++cnf_clause_counter;
                    
                    solver->clause(start_dimacs);
                }

                wireId2assumpVec[start_wire].push_back(assump_l); // needed for clause 7: one wire wanted by several paths
				assump2wireSet[assump_l].insert(start_wire);

                //// care for the follow-up wire:
                auto find_wire2 = inner.find(next_wire);

                if(find_wire2 == inner.end()){

                    next_dimacs = ++max_dimacs_var;
                    netinfo2wireId2Dimacs[net_info][next_wire] = next_dimacs;

                    std::pair<NetInfo*, WireId> nw2(net_info, next_wire);
                    dimacs2NetinfoWireId[next_dimacs] = nw2;
                }
                else{next_dimacs = inner[next_wire];}


                // check if next wire is part of a wanted wire type. This type is needed when it has competitioners of the same type:
                int type_next_wire = intra2; //my_intra_coord_finder(next_wire, pip);
                //log_info("h1: type next wire %d | intra coord2 %d | intra coord1 %d\n", type_next_wire, intra2, intra1);

                if(type_next_wire >= 0 && type_next_wire < 20 && dst_wire_no > -1){ 
                    // then we have wire type 12 whose wire no. needs to be aligned with the destination wire wire no. when it is of type 13

                    int next_wire_no = my_wire_no(next_wire, false);
                    
                    if(next_wire_no != dst_wire_no && next_wire_no != 1 && next_wire_no != 2){ // then we can set the variable for the next wire to true as it leads to the correct wire

                        log_info("h2: no. alignment dst wire %s with no. %d | next next wire %s with no. %d\n", ctx->nameOfWire(dst_wire), dst_wire_no, ctx->nameOfWire(next_wire), next_wire_no);
                        
                        file << "-"<< std::to_string(next_dimacs) << " " << std::to_string(assump_l->dimacs_var) << " 0\n";
                        ++cnf_clause_counter;
                                
                        solver->clause(next_dimacs, assump_l->dimacs_var);
                    }
                }



                wireType2wires[false][type_next_wire].push_back(next_dimacs);
                wireType2wires[true][type_next_wire].push_back(start_dimacs);

				next_dimacs_col.push_back(next_dimacs);
                   

                // for conflict clause collection:
                wireId2assumpVec[next_wire].push_back(assump_l);
                assump2wireSet[assump_l].insert(next_wire);


                std::pair<int, int> dimacs_pair;
                if(start_dimacs < next_dimacs){

                    dimacs_pair.first = start_dimacs;
                    dimacs_pair.second = next_dimacs;
                }
                else{

                    dimacs_pair.first = next_dimacs;
                    dimacs_pair.second = start_dimacs;
                }

                netinfo2DimacsPair2PipId[net_info][dimacs_pair] = pip;

            } // end one loop over all pips of one specific channel segment



            // formulate clauses 5 modified:
            for(auto& pair_pair : wireType2wires){

            	for(auto& pair_ : pair_pair.second){

		            std::vector<int> wires = pair_.second;

		            if(static_cast<int>(wires.size()) <= 1){continue;}

		            // create a string stream which stores all or clauses which are later then written to the final cnf file, clause 6:
		            int wire_ind = 0;

		            for(int& wire_dimacs : wires){


		                for(auto it = wires.begin() + wire_ind; it != wires.end(); ++it){

		                    int other_wire_dimacs = *it;

		                    if(wire_dimacs != other_wire_dimacs){
		                    
		                        file << "-" << std::to_string(wire_dimacs) << " -" << std::to_string(other_wire_dimacs) << " " << std::to_string(assump_l->dimacs_var) <<" 0\n";
		                        ++cnf_clause_counter;
		               
		                        solver->clause(-wire_dimacs, -other_wire_dimacs, assump_l->dimacs_var);
		                    }
		                }

		                ++wire_ind;
		            }
				}
            }
        } // end loop over all wire segments



        // loop over the wires to use:

        // create a string stream which stores all or clauses which are later then written to the final cnf file, clause 6:
        std::stringstream OR_stream;

        for(auto& ele: wireId2Followers){ // current start wire of interest

            auto& curr_wire = ele.first;
            auto& vec_follow_children = ele.second;


            int curr_dimacs = netinfo2wireId2Dimacs[net_info][curr_wire];

            if(vec_follow_children.empty()){

                // clause 10: when the wire is a impasse and at the same time not the destination wire, it is set to false such that the competitioner which leads to the destination wire
                // can be true:
                bool is_dst = false;

                for(WireId& arc_dst_wire : net2Dsts[net_info]){

                    if(curr_wire == arc_dst_wire){

                        is_dst = true;
                        break;
                    }
                }

                if(!is_dst){

                    file << "-" << std::to_string(curr_dimacs) << " " << std::to_string(assump_l->dimacs_var) <<" 0\n";
                    ++cnf_clause_counter;                    
                    
                    solver->clause(-curr_dimacs, assump_l->dimacs_var);
                }
            }

            else if(vec_follow_children.size()==1){ // when there is only one follow-up wire, we don't need a helping variable for SAT-formulation but use the follow-up wire directly

                // OR_stream is much shorter when we only have one follow-up wire:
                auto& follow_wire = vec_follow_children.at(0);
                int follow_wire_dimacs = netinfo2wireId2Dimacs[net_info][follow_wire];

                // clauses for choice of follow-up wire/color when the start wire is true, are not needed (clauses 5 and 6)
                // create clauses 1 and 2:
                // clause 1 positive implication:
                file << "-" << std::to_string(curr_dimacs) << " " << std::to_string(follow_wire_dimacs) << " " << std::to_string(assump_l->dimacs_var) <<" 0\n";
                ++cnf_clause_counter;

                solver->clause(-curr_dimacs, follow_wire_dimacs, assump_l->dimacs_var);

                // clause 2 negative implication: // Remark: not needed as follow-up wires already exclude each other

            }
            else{ // helper variable is needed // clauses 5 and 6

                // the helper clause switches the choice of follow-up wire on when the start wire is true, else it deactivates the clause:
                helper_var_dimacs = ++max_dimacs_var;

                      
                // also append the or_stream, clause 6:
                OR_stream << std::to_string(assump_l->dimacs_var) << " " << std::to_string(helper_var_dimacs); // << as output data TO file // new 241210
                solver->add(assump_l->dimacs_var);
                solver->add(helper_var_dimacs);

                for(auto& follow_wire : vec_follow_children){ // follow-up wires of interest

                    ////////////////////////////

                    // get wire's DIMACS value:
                    int follow_wire_dimacs = netinfo2wireId2Dimacs[net_info][follow_wire];

                    // also append the or_stream:
                    OR_stream << " " << std::to_string(follow_wire_dimacs); // << as output data TO file
                    solver->add(follow_wire_dimacs);

                } // end loop over all children of one wire

              
                // end or-clause of or stream:
                OR_stream << " 0\n"; // now it contains an OR-clause for all follow-up wires of one wire, the assumption variable and the helper variable
                solver->add(0);

                // Store the generated OR_stream in the file:
                file << OR_stream.str();
                ++cnf_clause_counter;

                // create clauses 1 and 2:
                // clause 1 positive implication:
                file << "-" << std::to_string(curr_dimacs) << " -" << std::to_string(helper_var_dimacs) << " " << std::to_string(assump_l->dimacs_var) << " 0\n";
                ++cnf_clause_counter;

                solver->clause(-curr_dimacs, -helper_var_dimacs, assump_l->dimacs_var);
            } // end else

            // clauses 1, 2, 3: not needed
        }

        // the wireID2NumberFollowers and wireId2Followers only relevant for one round, then clear them for next round:
        wireId2Followers.clear();
    }


    bool my_global_router_start(Router1 router){

        int chip_width = ctx->chip_info->width;
        int chip_height = ctx->chip_info->height;

        float real_delay;

        float alpha = 0.6;

        for (auto &net : ctx->nets){

            NetInfo *net_info = net.second.get();

            if(skip_net(net_info)){continue;}


            bool constant_router = false;
            if(net_info->constant_value != IdString()){

                constant_router = true;
            }

            for(auto& entry: arc_map[net_info]){
            
                net2arc2AStar[net_info][entry.arc] = new AStar();
            }

            auto src_wire = ctx->getNetinfoSourceWire(net_info);

            // Remark: It can happen that src_wire.idx is -1, in this case for normal it is also a skip net to ignore, like it was done in the original router1.cc setup().
            // Needed code implemented above.

            // We don't know whether the wire enters the chip from above, below, left or right. Therefore, chip dimensions needed to evaluate the travel distance
            // to the switch boxes:
            int x_coord = ctx->chip_info->wire_data[src_wire.index].x; // each wire gives its smallest X-coordinate in WireID
            int y_coord = ctx->chip_info->wire_data[src_wire.index].y; // each wire gives its largest Y-coordinate in WireID


            std::string wire_name = ctx->nameOfWire(src_wire); // C string c_str // eg X11/Y9/sp4_h_r_0
            bool horizontal = false;
            int h_find1 = static_cast<int>(wire_name.find("_horz_"));
            int h_find2 = wire_name.find("_h_");


            if (h_find1 != -1 || h_find2 != -1){horizontal=true;}


            int sp_find1 = wire_name.find("/span"); // find starting index of span, more precisely the position of '/'
            int sp_find2 = wire_name.find("/sp");
            int stopper = wire_name.find("_");

            int span;

            if (sp_find1 != -1) {

                int len_number = stopper - (sp_find1 + 5); // exclude the _ symbol, we want number inbetween: spanNUMBER_ // /span has 5 symbols
                std::string number_string = wire_name.substr(sp_find1 + 5, len_number);
                std::stringstream number_string_ss(number_string);
                number_string_ss >> span;
            }
            else if(sp_find2 != -1){

                int len_number = stopper - (sp_find2 + 3);
                std::string number_string = wire_name.substr(sp_find2 + 3, len_number);
                std::stringstream number_string_ss(number_string);
                number_string_ss >> span;
            }
            else{span = -1;} // we have a local wire


            bool extended_src_wire = false;

            int intra_coord1 = -1; // the intra_coord1 for the source wire is always -1. Through intra_coord2 of the follow-up wire an unique ChannelSegment key is created.


            for (auto pip : ctx->getPipsDownhill(src_wire)){

                WireId next_wire = ctx->getPipDstWire(pip);

            
                // for the case of a constant net:
                if(constant_router){

                    if(ctx->getWireConstantValue(next_wire) != net_info->constant_value){continue;} // ignore the pip which doesn't lead to the fitting constant wire
                }
                extended_src_wire = true;

                Loc loc = ctx->getPipLocation(pip);

                if(x_coord != 0 && horizontal && (x_coord + span == chip_width)){ // horizontal signal enters from the right

                    real_delay = chip_width - loc.x;

                }
                else if(y_coord != chip_height && !horizontal && (y_coord - span == 0) ){ // vertical signal enters from below

                    real_delay = loc.y;

                }
                else if(horizontal && x_coord == 0){ // horizontal signal enters from the left

                    real_delay = loc.x;

                }
                else if(y_coord == chip_height && !horizontal && (y_coord + span == chip_height)){ // vertical signal enters from above

                    real_delay = chip_height - loc.y;
                }
                else if(horizontal){ // horizontal wire WITHIN chip

                    real_delay = std::abs(loc.x - x_coord);
                }
                else{ // vertical wire WITHIN chip or a local wire (then this clause gives 0)

                    real_delay = std::abs(loc.y - y_coord);
                }


				int intra_coord2 = my_intra_coord_finder(next_wire, pip);


                if(real_delay == 0){ // source wire touches a PLB and the pip is within it such that the distance is local

                    real_delay = 0.25;
                }

                // create channel segement:
                auto channel_seg_coord = std::make_tuple(-1, -1, loc.x, loc.y, intra_coord1, intra_coord2);

                auto key_channel_seg = std::tuple_cat(std::make_tuple(net_info), channel_seg_coord);

                // Does the found channel entry already exist? When so, no need of creating a new path:
                auto find_existing_entry = ChannelSegment.find(key_channel_seg);

                if(find_existing_entry == ChannelSegment.end()){ // no entry found
                    
                    
                     // compute A* delay for each arc:
                    for(auto& entry : arc_map[net_info]){

						arc_key arc = entry.arc;

                        auto& curr_aStar_obj = net2arc2AStar[net_info][arc];
               
                        net_info->users[arc.user_idx];

                        auto dst_wire = ctx->getNetinfoSinkWire(net_info, net_info->users[arc.user_idx], arc.phys_idx);

						net2Dsts[net_info].push_back(dst_wire); // preparation work for clause 10


                        int dst_x = ctx->chip_info->wire_data[dst_wire.index].x;
                        int dst_y = ctx->chip_info->wire_data[dst_wire.index].y;

                        float delay_est_x = std::abs(dst_x - loc.x);
                        float delay_est_y = std::abs(dst_y - loc.y);
                        float delay_est = delay_est_x + delay_est_y;

               			if(delay_est == 0 && true){delay_est = 0.75;}

                        float weighted_delay = (1 - alpha) * real_delay  + alpha * delay_est;

                        // create a new path:
                        Path* path = new Path(); // is a pointer

                        path->channel_segment_loc.push_back(channel_seg_coord);

                        path->real_delay = real_delay;

                        auto find_key = curr_aStar_obj->delay2Paths.find(weighted_delay);

                        if(find_key == curr_aStar_obj->delay2Paths.end()){ // key not found

                            curr_aStar_obj->min_heap.push(weighted_delay);
                        }

                        curr_aStar_obj->delay2Paths[weighted_delay].push_back(path); // map path to its delay
                    }
                }


                ChannelSegment[key_channel_seg].push_back(pip); // the end pip will be the start of the next A* channel round search
            }

            if(!extended_src_wire){

                // are all arcs' dst wires of a net the src_wire? Then still possible that net is 'routable' without follow-up wire:

                for(auto& entry : arc_map[net_info]){

                	arc_key arc = entry.arc;

                    auto dst_wire = ctx->getNetinfoSinkWire(net_info, net_info->users[arc.user_idx], arc.phys_idx);
                    if(dst_wire != src_wire){

                        return false;
                    }
                }
            } // one net could not reach further than its source wire
        }
        return true;
    }


    std::tuple<Path*, float> my_global_router(Router1 router, arc_key arc, int round_number){ //, std::vector<arc_key*> arc_vec){

        NetInfo *net_info = arc.net_info;

        // check if we have constant net value here:
        bool constant_router = false;
        if(net_info->constant_value != IdString()){

            constant_router = true;
        }
        log_info("my_global_router start with net %s and constant value %s\n", ctx->nameOf(net_info), ctx->nameOf(net_info->constant_value));

        auto dst_wire = ctx->getNetinfoSinkWire(net_info, net_info->users[arc.user_idx], arc.phys_idx);

        /*
        // find out wire no of dst_wire:
        int type_dst = my_intra_coord_finder(dst_wire, PipId());
        int dst_wire_no = -1;
        if(type_dst >= 20){
            
            dst_wire_no = my_wire_no(dst_wire, true);
            log_info("dst wire %s with wire no %d\n", ctx->nameOfWire(dst_wire), dst_wire_no);
        }
        */


        int dst_x = ctx->chip_info->wire_data[dst_wire.index].x;
        int dst_y = ctx->chip_info->wire_data[dst_wire.index].y;

        float alpha = 0.6; // should be 0.6 but we want integer and work instead with the base of 10

        auto src_wire = ctx->getNetinfoSourceWire(net_info);

        log_info("name of source wire %s\n destination wire %s \n", ctx->nameOfWire(src_wire), ctx->nameOfWire(dst_wire));

        if(src_wire == dst_wire && round_number == 1){ // Remark: even in the original route_arc does it happen several times for the blinky example that the source wire is the same as the destination wire

            log_info("source wire is destination wire -> No routing needed\n");

            //
            NetInfo* bound_net = ctx->getBoundWireNet(src_wire);
             
            if(bound_net == nullptr){   
                
                log_info("%s bind wire src %s\n", ctx->nameOf(net_info), ctx->nameOfWire(src_wire));        
                ctx->bindWire(src_wire, net_info, STRENGTH_STRONG);            
            } // already bound by the net
            else{log_info("source wire ist bereits an net gebunden: %s | aktuelles net: %s \n", ctx->nameOf(bound_net), ctx->nameOf(net_info));} // not happening
            //

            Path* path = new Path(); // is a pointer
            float delay = 0;
            return std::make_tuple(path, delay);
        }
        else if(src_wire == dst_wire && round_number > 1){ // we are in at least second round of path finding. But for arc with only one wire as path one path exists. Therefore, ignore this arc

            Path* path = new Path(); // is a pointer
            float delay = -1;
            return std::make_tuple(path, delay);
        }

        auto& curr_aStar_obj = net2arc2AStar[net_info][arc];
        auto& heap = curr_aStar_obj->min_heap;


        if(heap.empty()){log_info("heap empty\n");}

        // first test for dst wire such that no extended path of an already completed path is created. That would make no sense. :
        std::map<arc_key, boolSetter> dst_in_pip_start;
        dst_in_pip_start.clear();

        while(!heap.empty()){

        	//std::map<std::tuple<int, int, int, int, int, int>, std::vector<PipId>>  ChannelSegmentLocal;
        	std::set<std::tuple<int, int, int, int, int, int>>  ChannelSegmentLocal;
			ChannelSegmentLocal.clear();

            float heap_delay = heap.top();

            // It can happen that there is a heap entry which should not exist as the path was deleted before, therefore simply ignore unneeded delay and jump to next heap entry:
            auto find_delay = curr_aStar_obj->delay2Paths.find(heap_delay);
            if(find_delay == curr_aStar_obj->delay2Paths.end()){continue;}
     	
            auto& shortest_paths = curr_aStar_obj->delay2Paths[heap_delay];
            auto& shortest_path = shortest_paths.back(); // the auto& leads to the fact that changes in shortest_path lead to changes in shortest_paths!
            // it gives reference/address to object


			if(round_number > 1){

		    	for(auto& past_visited_coord : shortest_path->channel_segment_loc){ // this coordinate was already visited before by path

					ChannelSegmentLocal.insert(past_visited_coord);
		       	}
			}

            shortest_paths.pop_back(); // it got extended and was in no other destination wires envolved    	

            // shortest path should still exist
            if(shortest_paths.size() == 0){

                heap.pop();
            }

            // extend the current path by its follow pips:

            std::tuple<int, int, int, int, int, int> last_channel_seg_loc = *shortest_path->channel_segment_loc.rbegin(); // * enables to get element instead of iterator

            auto pips_end = ChannelSegment[std::tuple_cat(std::make_tuple(net_info), last_channel_seg_loc)];

            int start_x = std::get<2>(last_channel_seg_loc);
            int start_y = std::get<3>(last_channel_seg_loc);


            std::set<PipId> pips_found;
            pips_found.clear();

        	for(PipId pip_start : pips_end){

                WireId next_wire = ctx->getPipDstWire(pip_start);

                if(dst_wire == next_wire){
         
                    // all complete paths which are not part of the arc but of an other one are stored in the intermed_delay2Paths.
                    // Transfer them back to the overall delay2Paths:


                    // FINAL PATH FOUND

                    // store path in intermediate storage
                    // translate the path into SAT CNF:
                    return std::make_tuple(shortest_path, heap_delay);
                    // we also need to delete this path after it was SAT encoded! Therefore, return a tuple with the needed information
                    }


                for(auto& entry_other : arc_map[net_info]){


					arc_key arc_other = entry_other.arc;

                    auto dst_wire_other = ctx->getNetinfoSinkWire(net_info, net_info->users[arc_other.user_idx], arc_other.phys_idx);


                    if(dst_wire_other == next_wire){
                     
                        dst_in_pip_start[arc_other].val = true;
                    }
                }


            }


            bool first_time_expanded = true;

            for(PipId pip_start : pips_end){

                WireId next_wire = ctx->getPipDstWire(pip_start);
      
                // jump to next pip in the journey downstream:

				int intra_coord1 = my_intra_coord_finder(next_wire, pip_start);

				//if(intra_coord1 > 19){continue;} // after wire type13 we have an impasse. Indeed there are pips which overcome this by connecting LUT input wire type13 with the LUT output wire
				// but this doesn't make any sense.
				// Besides that it was already tested if the next_wire is the destination wire.

                // in case of an impasse the following loop doesn't get ever activated. Therefore, the path isn't extended and therefore not farther followed. In the code above it is already checked for
                // destination wire and the path is deleted.

                /*
                if(intra_coord1 >= 0 && intra_coord1 < 20 && dst_wire_no > -1){ 
                    // then we have wire type 12 whose wire no. needs to be aligned with the destination wire wire no. when it is of type 13

                    int next_wire_no = my_wire_no(next_wire, false);
                    
                    if(next_wire_no != dst_wire_no){ // then ignore this option. The no. has to be identical

                        continue;
                    }
                }
                */


                for (auto pip_end : ctx->getPipsDownhill(next_wire)){

                     
                    WireId next_next_wire = ctx->getPipDstWire(pip_end);

                    

                    if(constant_router){

                        WireId next_next_wire = ctx->getPipDstWire(pip_end);

                        // when the upcoming wire doesn't fulfill the constant value needed for a constant net then ignore it:
                        if(ctx->getWireConstantValue(next_next_wire) != net_info->constant_value){continue;}
                    }

                    // const_go_on = true;

                    auto find_pip = pips_found.find(pip_end);
                    if(find_pip != pips_found.end()){continue;} // pip was already found, therefore ignore it
                    else{pips_found.insert(pips_found.end(), pip_end);}

                    Loc loc = ctx->getPipLocation(pip_end);

					int intra_coord2 = my_intra_coord_finder(next_next_wire, pip_end);

                    /*
                    if(intra_coord2 >= 0 && intra_coord2 < 20 && dst_wire_no > -1){ 
                        // then we have wire type 12 whose wire no. needs to be aligned with the destination wire wire no. when it is of type 13
    
                        int next_next_wire_no = my_wire_no(next_next_wire, false);
                        
                        if(next_next_wire_no != dst_wire_no){ // then ignore this option. The no. has to be identical
    
                            log_info("no no. alignment dst wire %s with no. %d | next next wire %s with no. %d\n", ctx->nameOfWire(dst_wire), dst_wire_no, ctx->nameOfWire(next_next_wire), next_next_wire_no);
                            continue;
                        }
                    }
                    */
				
                    // check if ChannelSegement for this coordinates already exists, when so create NO new path:
                    auto coord_tuple = std::make_tuple(start_x, start_y, loc.x, loc.y, intra_coord1, intra_coord2);
                    auto net_coord_tuple = std::tuple_cat(std::make_tuple(net_info), coord_tuple);

                    auto find_existing_entry = ChannelSegment.find(net_coord_tuple);
                    auto find_existing_entry_local = ChannelSegmentLocal.find(coord_tuple);
                    
                    // Check if we already added the pip_end to the ChannelSegment:
                    bool overjump_pip_end = false;

                    for(auto& pip : ChannelSegment[net_coord_tuple]){ // already searching for it makes a new entry!
                        if(pip == pip_end){overjump_pip_end = true;}
                    }
                    if(overjump_pip_end && round_number == 1){continue;} // we processed pip before and therefore don't need to look at it twice


                    // create/append a ChannelSegement entry:

                    if(!overjump_pip_end){

                    	ChannelSegment[net_coord_tuple].push_back(pip_end);
					}


                    float delay_x = std::abs(loc.x - start_x);
                    float delay_y = std::abs(loc.y - start_y);
                    // -> as channels are vertical or horizontal one of them should be 0
                    float delay_channel = delay_x + delay_y;
                    
                    if(delay_channel == 0){delay_channel = 0.75;}

                    bool path_extension = false;
                    if (round_number == 1){ // first time that each arc calls the my_global_router method

                    	path_extension = find_existing_entry == ChannelSegment.end();
                    }
                    else{

                      	path_extension = find_existing_entry_local == ChannelSegmentLocal.end();
                    }


                    if(path_extension){ // no entry found -> create extended_path

						if(round_number > 1){

		                   ChannelSegmentLocal.insert(coord_tuple);
						}
 
                        // create a new path with that end:
                        auto& old_path_obj = *shortest_path; // get object


                        /////////////////////////////////////////////////////
                        // for each arc of same net do it. Expand them every time the current path is expanded.

                        for(auto& entry_other : arc_map[net_info]){

                        	arc_key arc_other = entry_other.arc;

                            // check if this path is allowed for other arcs:
                            // we already found that this path is a solution for an other arc, thus it makes no sense to expand the path for that arc.

                            // only consider arcs of the same net:
                            NetInfo *net_info_other = arc_other.net_info;
                            if(net_info != net_info_other){continue;}


                            auto dst_wire_other = ctx->getNetinfoSinkWire(net_info, net_info->users[arc_other.user_idx], arc_other.phys_idx);

                            // check out if old path contains destination. When yes, keep it. When it's the other arc's turn it will first check if current path is dst wire:
                            if(dst_in_pip_start[arc_other].val){continue;}

                            auto& other_aStar_obj = net2arc2AStar[net_info][arc_other];

                            int dst_x_other = ctx->chip_info->wire_data[dst_wire_other.index].x;
                            int dst_y_other = ctx->chip_info->wire_data[dst_wire_other.index].y;

                            //// delete old path entry of other arc:
                            if(arc_other.net_info != arc.net_info && arc_other.user_idx != arc.user_idx && arc_other.phys_idx != arc.phys_idx && first_time_expanded){

                                //// remove that old path: //////////////////////////////////////////////////

                                // compute old delay:
                                float delay_est_x_other_old_path = std::abs(dst_x_other - start_x);
                                float delay_est_y_other_old_path = std::abs(dst_y_other - start_y);
                                float delay_est_other_old_path = delay_est_x_other_old_path + delay_est_y_other_old_path;
                                if(delay_est_other_old_path == 0 && true){delay_est_other_old_path = 0.75;}
                                // ???????? delete old path when it contains destination???

                                float weighted_delay_other_old_path = (1 - alpha) * shortest_path->real_delay + alpha * delay_est_other_old_path;

                                auto it_other_old_path = std::find_if(other_aStar_obj->delay2Paths[weighted_delay_other_old_path].begin(), other_aStar_obj->delay2Paths[weighted_delay_other_old_path].end(), [&old_path_obj](Path* p)
                                { return p == &old_path_obj; });
                 
                                if(it_other_old_path != other_aStar_obj->delay2Paths[weighted_delay_other_old_path].end()) { // != doesn't compare pointers themselves but the reference addresses they are pointing to
                                    other_aStar_obj->delay2Paths[weighted_delay_other_old_path].erase(it_other_old_path);
                                }
                                else{log_info("ERROR old path other arc not found!!!!!!!!!!!!!!\n");}
                            }

                            //////////////////////////////////////////////////


                            //// create extended path entry for all arcs of the same net which haven't met their destination wire already: //////////////////////////////////////////////////

                            Path* extended_path = new Path();
                            for(auto ele: shortest_path->channel_segment_loc){

                                extended_path->channel_segment_loc.push_back(ele);
                            }

                            extended_path->channel_segment_loc.push_back(coord_tuple);

                            // compute new real delay:
                            //Loc loc = ctx->getPipLocation(pip_end);
                            float delay_x = std::abs(loc.x - start_x); // pip end x - pip start x
                            float delay_y = std::abs(loc.y - start_y);
                            // -> as channels are vertical or horizontal one of them should be 0
                            float delay_channel = delay_x + delay_y;

                            // if the delay_channel is 0 we need to add extra penalty such that the wire is not stuck forever in a 0-wire loop:
                            if(delay_channel == 0){delay_channel = 0.75;}

                            extended_path->real_delay = shortest_path->real_delay + delay_channel; // the old path travelled so far from source up to the new ending pip

                            // estimate the distance between the current node and the destination wire of the other arc:
                            float delay_est_x_other = std::abs(dst_x_other - loc.x);
                            float delay_est_y_other = std::abs(dst_y_other - loc.y);
                            float delay_est_other = delay_est_x_other + delay_est_y_other;
                            ////if(delay_est_other == 0 && find_dst_wire2 == set_dst_wires.end()){delay_est_other = 0.25;}
                            if(delay_est_other == 0 && next_next_wire != dst_wire_other){delay_est_other = 0.75;}

                            float weighted_delay_other = (1 - alpha) * extended_path->real_delay  + alpha * delay_est_other;


                            auto find_key_other = other_aStar_obj->delay2Paths.find(weighted_delay_other);

                            if(find_key_other == other_aStar_obj->delay2Paths.end()){ // key/delay not found

                                other_aStar_obj->min_heap.push(weighted_delay_other);
                            }

                            if(intra_coord1 > -1 && intra_coord2 == -7){ // switch from input to output of LUT, to avoid endless loops this path is pushed to the beginning

	                            other_aStar_obj->delay2Paths[weighted_delay_other].insert((other_aStar_obj->delay2Paths[weighted_delay_other]).begin(), extended_path);
                            }
                            else{
                            	other_aStar_obj->delay2Paths[weighted_delay_other].push_back(extended_path);
                            }
                            // when pushing to the last position this path is always watched first from the shortest_paths. Therefore, we can focus to one instead of several paths

                            //////////////////////////////////////////////////
                        }
                        //////////////////////////////////////////////////////

                        first_time_expanded = false; // the old path is erased only once.

                    }
                }
            } // pip_start end loop
        } // -> looping over heap done

        // When heap is empty and so far nothing was returned the global router didn't find a solution. There don't exist new paths for the current arc:
        Path* empty_path = new Path();
        return std::make_tuple(empty_path, -1); // the empty path and integer of -1 mark that there exists no routing solution
    }



    bool my_master(Router1 router){ // connect all my methods to global and local routing, SAT conflict formulation, and SAT solver with each other
        log_info("Start my_master\n");
        // Remark - Routing to constant nets: The global router methods only collect pips which lead to the constant wires fitting to the constant net.
        // It follows that the local router automatically only considers the fitting constant wires.

        //Cadical:Solver solver;
        //CaDiCaL::Solver solver;
        //CaDiCaL::Solver * solver = new CaDiCaL::Solver;


        // 1. Global routing
        // 1.1. Start the global router prepartion where not the arcs but each net searches for the first channel segement. All arcs of one net have this channel segment in common:
        log_info("- Start my_global_router_start all arcs\n");
        bool routable1 = my_global_router_start(router);
        // 5. Routability check:
        // 5.1. When for source wire of one net no pips found, then not routable:
        if(!routable1){return false;}
        log_info("- End my_global_router_start\n");

        int number_failed_arcs = -1;

        int round_number = 0;

        pow_collector.push_back(0); // at index/round 0 of pow_collector are 0 arcs carrying an assumption number of 0

        while(!arc_queue.empty()){

            ++round_number;

            if(round_number==2){

                log_info("size priority queue %ld \n", arc_queue.size());
            }

            log_info("\n\nWHILE Round %d\n", round_number);
            pow_collector.push_back(0); // every round open new possible slot

            int counter_failing_arcs = 0;

            int arc_count = 0;
            while(!arc_queue.empty()){ // consider the arcs individually

                //ctx->yield(); // original calls it with every arc after few times

                arc_key arc = arc_queue_pop();

                ++arc_count;
                log_info("ARC COUNT %d\n", arc_count);
   
                // 1.2. global router to find for each arc channel segements:
                log_info("-- Start my_global_router one arc\n");
                std::tuple<Path*, float> return_tuple = my_global_router(router, arc, round_number);
                log_info("-- End my_global_router\n");

                float delay = std::get<1>(return_tuple);
                log_info("-- Returned delay %f\n", delay); // %f as float

                // 5.2. collect info whether global routing was successful:
                if(delay == -1 && round_number == 1){ // one arc could not be routed, nontheless it's the first round. It follows that the whole problem is unroutable

                    log_info("-- Return false as first round and delay is -1 \n");

                    return false;
                }
                else if(delay == -1){ // no routing solution exists; the channel segement path wasn't expanded
                    log_info("-- Delay -1, failed arc, continue\n");

                    ++counter_failing_arcs;
                    continue;
                }

                Path* path_ptr = std::get<0>(return_tuple);
                //NetInfo* net_info = std::get<2>(return_tuple);
                log_info("-- global tuples read out\n");

                // 2. SAT encoding:
                // 2.1. local router to SAT-encode for each arc the individual wire choice within the globally chosen wire segments:
                if(delay != -1){

                log_info("-- Start my_local_router one arc\n");
                my_local_router(router, path_ptr, arc, delay);
                log_info("-- End my_local_router\n");
                }
            }

            // consider the arcs all together:

            // 5.3. return false when no new global routings were added:
            if(counter_failing_arcs == number_failed_arcs){return false;} // we could not add any new paths

            // 2.2. at the end of each round create conflict clauses such that each arc gets only one path and one wire can only be assigned to one net:
            // ???? first one not needed when we work with assumption which switch on and off each other, but needed when assumption are simple inputs independent of each other
            log_info("- Start my_local_conflict_clauses_creator all arcs\n");
            my_local_conflict_clauses_creator(router);
            log_info("- End my_local_conflict_clauses_creator\n");

            // 2.3. Modify the first line of the CNF file by creating a new file, writing the header line, and finally adding the already created SAT formula encodings of the other file:
            std::ifstream file("CNF.csv", std::ios_base::app);

            std::ofstream file_with_header("CNF_withHeader.csv", std::ios_base::trunc | std::ios_base::out); // trunc to discard contents; std::ios_base::out gives writing mode

            file_with_header << "p cnf " << std::to_string(max_dimacs_var) << " " << std::to_string(cnf_clause_counter) << "\n" << file.rdbuf();
            // the first line is the header of the DIMACS file which is modified later; p cnf #variables #clauses\n

            // 3. SAT solver taking the assumptions and returning failing assump set when UNSAT:
            // assumpVec
            // if sat solver is sat then return true and bind all wires
            //number_failed_arcs =  // number of failed assumptions
            //solver.attach(file_with_header);
            log_info("- files created, next start SAT_runner\n");

            // std::tuple<bool, int> return_SAT_failures = SAT_runner(round_number, active_assump);
            std::tuple<bool, int> return_SAT_failures = SAT_runner(round_number, solver);


            log_info("- End SAT_runner\n");

            bool return_SAT = std::get<0>(return_SAT_failures);
            if(return_SAT){return true;}

            //solver.~Solver(); // remove the old solver by destructor
            //delete solver;
        	//CaDiCaL::Solver * solver = new CaDiCaL::Solver;
    		//solver->reset_constraint(); // reset the clauses
       		//solver->reset_assumptions();
       		//solver->reset_observed_vars();
       		//solver->clear();
            //solver->copy(*unsolve_solver);

            int number_failed_arcs = std::get<1>(return_SAT_failures);
        }

        log_info("End my_master\n");
        return false;
    }



    std::tuple<bool, int> SAT_runner(int round_number, CaDiCaL::Solver* not_used_solver){


        std::vector<Assump_l*> active_assump;

        log_info("-- sat runner: start\n");
        int number_failed_arcs = 0;
        bool return_marker = false;
        bool first_time_UNSAT = true;

        // find out all assumption combinations which are possible:
        container_combi_vec.clear();
        container_combi_str.clear();

        std::vector<int> curr_assump_combi; // method local variable which is set up every time newly
        int curr_ind = 0;
        std::string parallel_string = "";

        // collect all possible combinations:
        log_info("-- sat runner: start combi creator\n");

        int overall_number_combis = 1;
        int number_slots = 0;
        for(auto number_arcs : pow_collector){

            if(number_slots > 1){

                overall_number_combis = overall_number_combis * std::pow(number_slots, number_arcs);
            }

            ++number_slots;
        }

        recursive_combi_creator(curr_assump_combi, curr_ind, parallel_string, round_number, overall_number_combis); 

        // loop over the nested_assump_vec, when the current assumption is part of chosen_assump set it to false else to true:
        // also store highest delay for the current combi:
        int combi_counter = 0;

        float best_SAT_delay = std::numeric_limits<float>::infinity();
        std::vector<int> best_SAT_model;
        log_info("-- sat runner: start container combi loop\n");

        // for the case that we have a timeout:
        int ind_counter = 0;

        // initialize sat solver instance:
        //CaDiCaL::Solver * active_solver = new CaDiCaL::Solver;
        //active_solver->copy(*solver);

        //end_container_combi_vec:
        //unsolve_solver.~Solver(); // remove the old solver by destructor
        //delete unsolve_solver;
        //CaDiCaL::Solver * unsolve_solver = new CaDiCaL::Solver(local_solver);

        //unsolve_solver->reset_constraint();
        //unsolve_solver->reset_assumptions();
   		//unsolve_solver->reset_observed_vars();
   		//unsolve_solver->clear();
        //unsolve_solver->copy(*local_solver);

        bool UNSAT_marker = false;

        CaDiCaL::Solver * local_solver = new CaDiCaL::Solver;


		//local_solver->set("time", 0);  // Disable time limit
		//local_solver->set("conflicts", 0);  // Disable conflict limit

        local_solver->read_dimacs("CNF_withHeader.csv", max_dimacs_var, 0);

        for(auto& active_combi_vec : container_combi_vec){ // container_combi_vec contains all possible combinations for the current paths created by recursive_combi_creator

            local_solver->reset_assumptions();
     
            float highest_delay = -1;


            // check if chosen_assump was already used before:
            auto active_combi_str = *(container_combi_str.begin() + combi_counter);
            ++combi_counter;
            if(used_assump_combis.count(active_combi_str) > 0){continue;} // this combi was already checked in an earlier round before by the SAT solver
            else{used_assump_combis.insert(active_combi_str);}

            active_assump.clear();

            // add assumptions to solver:
            int curr_arc = 0;
            for(int& active_dimacs : active_combi_vec){

                // go over all arcs and deactivate all assumptions which are not active (that are not member of active_combi_vec):
                // arc1 is the position one of active_combi_vec.

                auto it = nested_assump_map.begin();
                std::advance(it, curr_arc); // for std::vector it would be enough to define: auto it = vec.begin() + ind;. This not accounts for maps.
                auto all_assump_of_curr_arc = (*it).second;

                for(Assump_l* assump : all_assump_of_curr_arc){ // take care of all assumptions, those who are active (in active_combi_vec, set to false) and
                    //those who are deactivated this sat solver round (set to true)

                    // expand solver by assumptions:
                    int assump_dimacs = assump->dimacs_var;

                    if(assump_dimacs == active_dimacs){ // this assumption is activated and therefore set to false


                        // find the longest delay of the current combination:
                        float assump_delay = assump->delay;
                        if(assump_delay > highest_delay){highest_delay = assump_delay;}
                  
                        local_solver->assume(-assump_dimacs);
                        local_solver->freeze(-assump_dimacs);

                        // store the assumption's arc in a container. When it leads to error, in the next global router round a new path is found for it:
                        active_assump.push_back(assump);

                    }
                    else{ // the arc is currently not used and therefore set to true
                        
                        local_solver->assume(assump_dimacs);
                        local_solver->freeze(assump_dimacs);
                    }
                }

                ++curr_arc;
            }

            if(best_SAT_delay < highest_delay){continue;} // we already have a satisfying model which performs better in delay, therefore, ignore this combination

            // run the solver:
            log_info("-- sat runner: start solving process\n");

            bool lim = local_solver->limit("preprocessing", -1); // default 0, negative int gives non-limited space
            bool lim_search = local_solver->limit("localSearch", -1); // default 0, negative int gives non-limited space

            int result = local_solver->solve();

            // print solver information:
            local_solver->resources();
            local_solver->statistics();

            if(result == 10){ // SAT

                log_info("SATISFIED\n");

                best_SAT_delay = highest_delay;
                best_SAT_model.clear();

                for(auto& pair_ : wireId2activeArcSet){pair_.second.clear();}

                wireId2activeArcSet.clear();

                // get model:

                for(int var=1; var <= local_solver->vars(); ++var){ // we loop over all vars // .nvars excludes assumptions

                    int var_bool = local_solver->val(var);
                    
                    if(var_bool > 0){ // dimacs variable is true


                        // is each assumption related to this variable deactivated? It may happen that all assumptions relating to this variable are deactivated, then the variable can be true, nontheless
                        // it should be ignored:
                        std::pair<NetInfo*, WireId> net_wire = dimacs2NetinfoWireId[var];
                        NetInfo* found_net = net_wire.first;
                      
                        if(found_net == nullptr){ // the found variable is then a helper variable or an assumption, we can ignore both

                            continue;
                        }

                        WireId found_wire = net_wire.second;

                        std::vector<Assump_l*> assumpVec_ = wireId2assumpVec[found_wire];

                        int net_member_number = 0;
                        int counted_deactivations = 0;

                        for(Assump_l* assump_l : assumpVec_){

                            NetInfo* assump_net = assump_l->arc.net_info;
                            
                            if(assump_net == found_net){ // only when nets are identical, the same DIMACS variable is used

                                ++net_member_number;

                                int assump_dimacs = assump_l->dimacs_var;
                                int assump_dimacs_truth_state = local_solver->val(assump_dimacs);
		                   		
                                if(assump_dimacs_truth_state > 0){ // then the assumption is set to true and therefore is deactivated

                                    ++counted_deactivations;
                                }
                                else{
                                
                                    wireId2activeArcSet[found_net][found_wire].insert(assump_l->arc);
                                }
                            }
                        }

                       if(net_member_number == counted_deactivations && found_wire != ctx->getNetinfoSourceWire(found_net)){
                            // the dimacs variable is set to true but all related assumptions are deactivated. So the variable has to be ignored

                            continue;
                        }

                        // done, var is indeed activated and true:
                        best_SAT_model.push_back(var);                       
                    }                   
                }
                
                // routing process finished:
                return_marker = true;
            }

            else if(result == 20 && !return_marker && first_time_UNSAT){ // UNSAT

                log_info("UNSAT1\n");

                UNSAT_marker = true;
                first_time_UNSAT = false;

                // find out which assumptions caused conflict:
                // when UNSAT run Cardical again with changed assumption

                // collect assumptions by looping over all active paths:
                log_info("active assump %ld \n", active_assump.size());

                std::set<arc_key> arc_already_added;
                arc_already_added.clear();

                for(Assump_l* assump : active_assump){

                    if(arc_already_added.count(assump->arc)==0){ // add arc only to arc_queue when it wasn't added before through an other assumption


                        int active_assump_var = -assump->dimacs_var;

                        bool encounters_failure = local_solver->failed(active_assump_var);

                        if(encounters_failure){ // The assumption leads to a failure and therefore in the next round a new global path is added

                            bool contender_pushed = false;

                            // find out which wires are involved:
                            for(const WireId& wire : assump2wireSet[assump]){

                            	// find out which assumptions use these wires. These shall be considered in the next routing round:
                            	for(Assump_l* otherOrSame_assump : wireId2assumpVec[wire]){

                            		// ignore assumptions which are of the same net as the congested assumption:
                            		if(otherOrSame_assump != assump && otherOrSame_assump->arc.net_info == assump->arc.net_info){continue;}

                            		// make sure the arc hasn't already been added to the next round list and that the conflict doesn't appear at its wire (when the source is affected
                            		// it can't be modified to avoid a congestion):
                            		if(arc_already_added.count(otherOrSame_assump->arc)==0 && wire != ctx->getNetinfoSourceWire((otherOrSame_assump->arc).net_info)){

                            			// check if the assump is active:
                            			bool is_active = false;
                            			for(Assump_l* find_assump : active_assump){

                            				if(otherOrSame_assump == find_assump){

                            					is_active = true;
                            					break;
                            				}
                            			}
                            			
                            			if(!is_active){continue;}


                            			// when the assump is active and part of the congestion append the arc_queue to treat it again in the next routing round:

						                arc_entry e;
						                e.arc = otherOrSame_assump->arc;

						                arc_queue.push(e);
						                ++number_failed_arcs;

						                arc_already_added.insert(otherOrSame_assump->arc);

						                contender_pushed = true;
                                        log_info("push arc of net %s of dimacs %d\n", ctx->nameOf(otherOrSame_assump->arc.net_info), otherOrSame_assump->dimacs_var);
                            		}
                            	}
                            }                        
                        }
                    }
                }
            }

            else{ // exhausted resources: timeout, memory, CPU, Search -> 0 as UNKNOWN | Second time UNSAT in one routing round

                if(result == 20){log_info("UNSAT\n");}
                else{log_info("UNKNOWN\n");}

                if(!return_marker && first_time_UNSAT && ind_counter == (static_cast<int>(container_combi_vec.size()) - 1)){ // its the last combination and still no solution was found

                    log_info("SAT solver can't find out if SAT or UNSAT!\n");

                    return std::make_tuple(false, -1);
                }
            }

            ++ind_counter;
        } // end of the combination loop


        if(return_marker){ // SAT solution found

            std::map<std::tuple<WireId, WireId, NetInfo*>, PipId> src_pip_storage;

            std::map<NetInfo*, std::set<int>> active_map_set;    

            for(int var : best_SAT_model){

                // translate it back to its net_info, WireID, and pip:
                auto it = dimacs2NetinfoWireId.find(var); // check if DIMACS variable belongs to an assumption
                if(it != dimacs2NetinfoWireId.end()){ // we have no assumption var

                    std::pair<NetInfo*, WireId> net_wire = dimacs2NetinfoWireId[var];
                    NetInfo* found_net = net_wire.first;
                    WireId found_wire = net_wire.second;
				
                    // bind the belonging net to the pip and WireID:
                    bool bound_pip = false;
                    bool dst_wire_activated;

                    for(int other_var : active_map_set[found_net]){

                        std::pair<int, int> hypo_dimacs_pair;
                        if(other_var < var){

                            hypo_dimacs_pair.first = other_var;
                            hypo_dimacs_pair.second = var;
                            dst_wire_activated = true; // when the current wire is a dst wire of the pip, then we can bind pip and don't need to bind this wire in an extra step as it is already included
                            // in the binding pip process.
                        }
                        else{

                            hypo_dimacs_pair.first = var;
                            hypo_dimacs_pair.second = other_var;
                            dst_wire_activated = false;

                        }

                        auto it2 = netinfo2DimacsPair2PipId[found_net].find(hypo_dimacs_pair);
                        if(it2 != netinfo2DimacsPair2PipId[found_net].end()){

                            PipId found_pip = (*it2).second;

                            // when the input and output wire of a net comes from the same arc then the pip can be bound:
                            std::set<arc_key> found_arcs =  wireId2activeArcSet[found_net][found_wire];

                            std::pair<NetInfo*, WireId> other_net_wire = dimacs2NetinfoWireId[other_var];
                            NetInfo* other_net = other_net_wire.first;
                            WireId other_wire = other_net_wire.second;

                            if(found_wire == ctx->getNetinfoSourceWire(found_net) && found_wire == ctx->getPipSrcWire(found_pip)){
                                // the variable is a src wire and therefore the pip connection is stored until the end and then activate such that no conlficts happen

                                src_pip_storage[std::make_tuple(found_wire, other_wire, found_net)] = found_pip;
                                continue;
                            }
                            else if(other_wire == ctx->getNetinfoSourceWire(found_net) && other_wire == ctx->getPipSrcWire(found_pip)){
                                // the variable is a src wire and therefore the pip connection is stored until the end and then activate such that no conlficts happen

                                src_pip_storage[std::make_tuple(other_wire, found_wire, found_net)] = found_pip;
                                continue;
                            }
                            else if(found_wire == ctx->getNetinfoSourceWire(found_net) || other_wire == ctx->getNetinfoSourceWire(found_net)){ // a source wire is involved but it is the output wire of the pip.
                                // This should never happen as router1.cc only watches at single arcs independent of the inter-net-connections and the source wire marks always the start of a net
                                log_info("third pip source case happens but should not\n");
                                continue;
                            }

                            std::set<arc_key> other_arcs = wireId2activeArcSet[other_net][other_wire];

                            bool intersection = false;
                            for(auto& found_arc : found_arcs){

                            	auto it = other_arcs.find(found_arc);
                            	if(it != other_arcs.end()){

                            		intersection = true;
                            		break;
                            	}
                            }

                           if(intersection){ // at least one arc has the wires found_wire and other_wire in common.


                                auto it_finder = src_pip_storage.find(std::make_tuple(ctx->getNetinfoSourceWire(found_net), ctx->getPipDstWire(found_pip), found_net));

                                if(it_finder != src_pip_storage.end()){ // when src pip entry exists but is used now, delete this entry:

                                    src_pip_storage.erase(std::make_tuple(ctx->getNetinfoSourceWire(found_net), ctx->getPipDstWire(found_pip), found_net));
                                }

                                ctx->unlock();
                                ctx->bindPip(found_pip, found_net, STRENGTH_STRONG);
                                ctx->lock(); 
                                log_info("----- %s bind pip/switch1 %s\n", ctx->nameOf(found_net), ctx->nameOfPip(found_pip));

                                if(dst_wire_activated){

                                    bound_pip = true;
                                }
                            }
                        }                       
                    }  
                                   
                    active_map_set[found_net].insert(var);
                }
            }


            // loop over all unpiped source wires:
            for(auto& item : src_pip_storage){

                std::tuple<WireId, WireId, NetInfo*> keys = item.first;
                NetInfo* net = std::get<2>(keys);

                PipId pip = item.second;

                NetInfo* bound_net = ctx->getBoundWireNet(ctx->getPipDstWire(pip));

                if(bound_net != nullptr){
                    
                    log_info("Dst wire of pip is already bound to net %s | current net: %s\n", ctx->nameOf(bound_net), ctx->nameOf(net));
                    
                    continue;}
                    
                ctx->unlock();

                ctx->bindPip(pip, net, STRENGTH_STRONG); 
                ctx->lock();
                log_info("----- %s bind pip/switch2 %s\n", ctx->nameOf(net), ctx->nameOfPip(pip));
            }


            for(int var : best_SAT_model){

			    // translate it back to its net_info, WireID, and pip:
			    auto it = dimacs2NetinfoWireId.find(var); // check if DIMACS variable belongs to an assumption
			    if(it != dimacs2NetinfoWireId.end()){ // we have no assumption var

			        std::pair<NetInfo*, WireId> net_wire = dimacs2NetinfoWireId[var];
			        NetInfo* found_net = net_wire.first;
			        WireId found_wire = net_wire.second;
			  
			        NetInfo* bound_net = ctx->getBoundWireNet(found_wire);
			     
			        if(bound_net != nullptr){continue;} // already bound by the net			      
                }
            }


            for(int var : best_SAT_model){

            // translate it back to its net_info, WireID, and pip:
            auto it = dimacs2NetinfoWireId.find(var); // check if DIMACS variable belongs to an assumption
            if(it != dimacs2NetinfoWireId.end()){ // we have no assumption var


                std::pair<NetInfo*, WireId> net_wire = dimacs2NetinfoWireId[var];
                NetInfo* found_net = net_wire.first;
                WireId found_wire = net_wire.second;
 
                NetInfo* bound_net = ctx->getBoundWireNet(found_wire);
             
                if(bound_net == found_net){continue;} // already bound by the net
          
                ctx->unlock();

                ctx->bindWire(found_wire, found_net, STRENGTH_STRONG);
                ctx->lock();
                log_info("----- %s bind wire %s \n",ctx->nameOf(found_net), ctx->nameOfWire(found_wire));
                }
            }

            return std::make_tuple(true, 0);
        } // end case that return_marker is true

    return std::make_tuple(false, number_failed_arcs);
    }



    void recursive_combi_creator(std::vector<int> curr_assump_combi, int curr_ind, std::string parallel_string, int round_number, int overall_number_combis){ // the vectors use int for DIMACS variables
        // creates one possible combination

        // check if end reached:
        int number_arcs = static_cast<int>(nested_assump_map.size());

        if(curr_ind == number_arcs){ // we already looped over all arcs and therefore can now return the final combination

            container_combi_vec.push_back(curr_assump_combi);
            container_combi_str.push_back(parallel_string);

            // check if number_combis = round_number^number_arcs already fulfilled by container_combi_vec.size():
            // we assume taht each global router round the paths of ALL arcs are expanded (excluded the arcs where src_wire == dst_wire).
            int reached_number_combis = static_cast<int>(container_combi_vec.size());

            if(reached_number_combis == overall_number_combis){return;}
        }


        // create combinations:
        if(curr_ind < static_cast<int>(nested_assump_map.size())){

            auto it = nested_assump_map.begin();
            std::advance(it, curr_ind); // for std::vector it would be enough to define: auto it = vec.begin() + ind;. This not accounts for maps.

            for(auto assump : it->second){

                int assump_dimacs = assump->dimacs_var;

                int delay = assump->delay;

                curr_assump_combi.push_back(assump_dimacs);

                std::string parallel_string_expanded = parallel_string + " " + std::to_string(assump_dimacs);
               
                recursive_combi_creator(curr_assump_combi, curr_ind + 1, parallel_string_expanded, round_number, overall_number_combis);

                curr_assump_combi.pop_back(); // remove added assumption assump_l again such that next assump of the same arc can be considered                
            }
        }
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



};

} // namespace

NEXTPNR_NAMESPACE_BEGIN

Router1Cfg::Router1Cfg(Context *ctx)
{
    maxIterCnt = ctx->setting<int>("router1/maxIterCnt", 200);
    cleanupReroute = ctx->setting<bool>("router1/cleanupReroute", false); // new
    fullCleanupReroute = ctx->setting<bool>("router1/fullCleanupReroute", false); // new
    useEstimate = ctx->setting<bool>("router1/useEstimate", true);

    wireRipupPenalty = ctx->getRipupDelayPenalty();
    netRipupPenalty = 10 * ctx->getRipupDelayPenalty();
    reuseBonus = wireRipupPenalty / 2;

    estimatePrecision = 100 * ctx->getRipupDelayPenalty();
}

bool router1(Context *ctx, const Router1Cfg &cfg)
{
    try {
        log_break();
        log_info("Routing..\n");
        ScopeLock<Context> lock(ctx);
        //ctx->unlock();
        auto rstart = std::chrono::high_resolution_clock::now();

        log_info("Setting up routing queue.\n");

        Router1 router(ctx, cfg);
        router.setup();
#ifndef NDEBUG
        router.check();
#endif

        log_info("Routing %d arcs.\n", int(router.arc_queue.size()));

        ////////////// mine: ///////////////
        // clean CNF file from previous nextpnr runs:
 //std::ofstream file_with_header("CNF.csv", std::ios_base::trunc);
//file_with_header("CNF_withHeader.csv"
        std::ofstream file("CNF.csv", std::ios_base::trunc);
        //ctx->unlock();
      //  ctx->yield();
        log_info("Checksum before routing: 0x%08x\n", ctx->checksum());


        if(router.my_master(router)){

          //  ctx->yield();
        	//router.check();
        //	ctx->check();
            log_info("Checksum after routing: 0x%08x\n", ctx->checksum());

        	log_info("Routing complete.\n");
            //ctx->unlock(); 
       		ctx->yield(); // common/kernel/basectx.h // Lock to be taken by UI when wanting to access context - the yield()
               // method will lock/unlock it when its' released the main mutex to make
               // sure the UI is not starved.
       // log_info("Router1 time %.02fs\n", std::chrono::duration<float>(rend - rstart).count());
       
            log_info("mark0\n");

            ctx->debug = true;
            log_info("mark1\n");
            //ctx->lock();
            log_info("mark2\n");

            //ctx->yield(); // original runs it after routing
            log_info("mark3\n");

            ctx->check();
            log_info("mark4\n");


#ifndef NDEBUG
       		//router.check();
        	//ctx->check();
        //	log_assert(ctx->checkRoutedDesign());
#endif
            log_assert(ctx->checkRoutedDesign());
            log_info("Checksum after routing and yield: 0x%08x\n", ctx->checksum());
        	timing_analysis(ctx, true /* slack_histogram */, true /* print_fmax */, true /* print_path */,
                        true /* warn_on_failure */, true /* update_results */);



        	log_info("SATISFIABLE ROUTED\n");
        	return true;
    	}

        return false;


        ///////////////////////////////////

    } catch (log_execution_error_exception) {
#ifndef NDEBUG
        ctx->lock();
        ctx->check();
        ctx->unlock();
#endif
        return false;
    }
}

bool Context::checkRoutedDesign() const
{
    const Context *ctx = getCtx();

    for (auto &net_it : ctx->nets) {
        NetInfo *net_info = net_it.second.get();

#ifdef ARCH_ECP5
        if (net_info->is_global)
            continue;
#endif

        if (ctx->debug)
            log("checking net %s\n", ctx->nameOf(net_info));

        if (net_info->users.empty()) {
            if (ctx->debug)
                log("  net without sinks\n");
            log_assert(net_info->wires.empty());
            continue;
        }

        bool found_unrouted = false;
        bool found_loop = false;
        bool found_stub = false;

        struct ExtraWireInfo
        {
            int order_num = 0;
            pool<WireId> children;
        };

        dict<WireId, std::unique_ptr<ExtraWireInfo>> db;

        for (auto &it : net_info->wires) {
            WireId w = it.first;
            PipId p = it.second.pip;

            if (p != PipId()) {
                log_assert(ctx->getPipDstWire(p) == w);
                db.emplace(ctx->getPipSrcWire(p), std::make_unique<ExtraWireInfo>()).first->second->children.insert(w);
            }
        }

        auto src_wire = ctx->getNetinfoSourceWire(net_info);
        if (net_info->constant_value == IdString()) {
            if (src_wire == WireId()) {
                log_assert(net_info->driver.cell == nullptr);
                if (ctx->debug)
                    log("  undriven and unrouted\n");
                continue;
            }

            if (net_info->wires.count(src_wire) == 0) {
                if (ctx->debug)
                    log("  source (%s) not bound to net\n", ctx->nameOfWire(src_wire));
                found_unrouted = true;
            }
        }

        dict<WireId, store_index<PortRef>> dest_wires;
        for (auto user : net_info->users.enumerate()) {
            for (auto dst_wire : ctx->getNetinfoSinkWires(net_info, user.value)) {
                log_assert(dst_wire != WireId());
                dest_wires[dst_wire] = user.index;

                if (net_info->wires.count(dst_wire) == 0) {
                    if (ctx->debug)
                        log("  sink %d (%s) not bound to net\n", user.index.idx(), ctx->nameOfWire(dst_wire));
                    found_unrouted = true;
                }
            }
        }

        std::function<void(WireId, int)> setOrderNum;
        pool<WireId> logged_wires;

        setOrderNum = [&](WireId w, int num) {
            auto &db_entry = *db.emplace(w, std::make_unique<ExtraWireInfo>()).first->second;
            if (db_entry.order_num != 0) {
                found_loop = true;
                log("  %*s=> loop\n", 2 * num, "");
                return;
            }
            db_entry.order_num = num;
            for (WireId child : db_entry.children) {
                if (ctx->debug) {
                    log("  %*s-> %s\n", 2 * num, "", ctx->nameOfWire(child));
                    logged_wires.insert(child);
                }
                setOrderNum(child, num + 1);
            }
            if (db_entry.children.empty()) {
                if (dest_wires.count(w) != 0) {
                    if (ctx->debug)
                        log("  %*s=> sink %d\n", 2 * num, "", dest_wires.at(w).idx());
                } else {
                    if (ctx->debug)
                        log("  %*s=> stub\n", 2 * num, "");
                    found_stub = true;
                }
            }
        };

        /**/
        for (const auto &wire : net_info->wires) {
           
            if(wire.second.pip != PipId())
                log_info("Wire pip value of net %s, wire %s, pip %s\n", ctx->nameOf(net_info), nameOfWire(wire.first), nameOfPip(wire.second.pip));

            NetInfo* found_net = ctx->getBoundWireNet(wire.first);
            if(found_net != net_info){log_info("ALARM1, nets wire bound else by net  %s of wire %s as net member %s |\n", ctx->nameOf(found_net), ctx->nameOfWire(wire.first), ctx->nameOf(net_info));}
            else if(found_net == nullptr){log_info("ALARM2, nets wire bound else by net null by wire %s  as net member %s |\n", ctx->nameOfWire(wire.first), ctx->nameOf(net_info));}

        }
        

        if (ctx->debug) {
            log("  driver: %s\n", ctx->nameOfWire(src_wire));
            logged_wires.insert(src_wire);
        }
        if (net_info->constant_value != IdString()) {
            for (const auto &wire : net_info->wires) {
                if (wire.second.pip == PipId() && ctx->getWireConstantValue(wire.first) == net_info->constant_value)
                    setOrderNum(wire.first, 1);
            }
        } else {
            setOrderNum(src_wire, 1);
        }

        pool<WireId> dangling_wires;

        for (auto &it : db) {
            auto &db_entry = *it.second;
            if (db_entry.order_num == 0)
                dangling_wires.insert(it.first);
        }

        if (ctx->debug) {
            if (dangling_wires.empty()) {
                log("  no dangling wires.\n");
            } else {
                pool<WireId> root_wires = dangling_wires;

                for (WireId w : dangling_wires) {
                    for (WireId c : db[w]->children)
                        root_wires.erase(c);
                }

                for (WireId w : root_wires) {
                    log("  dangling wire: %s\n", ctx->nameOfWire(w));
                    logged_wires.insert(w);
                    setOrderNum(w, 1);
                }

                for (WireId w : dangling_wires) {
                    if (logged_wires.count(w) == 0)
                        log("  loop: %s -> %s\n", ctx->nameOfWire(ctx->getPipSrcWire(net_info->wires.at(w).pip)),
                            ctx->nameOfWire(w));
                }
            }
        }

        bool fail = false;

        if (found_unrouted) {
            if (ctx->debug)
                log("check failed: found unrouted arcs\n");
            fail = true;
        }

        if (found_loop) {
            if (ctx->debug)
                log("check failed: found loops\n");
            fail = true;
        }

        if (found_stub) {
            if (ctx->debug)
                log("check failed: found stubs\n");
            fail = true;
        }

        if (!dangling_wires.empty()) {
            if (ctx->debug)
                log("check failed: found dangling wires\n");
            fail = true;
        }

        if (fail)
            return false;
    }

    return true;
}

bool Context::getActualRouteDelay(WireId src_wire, WireId dst_wire, delay_t *delay, dict<WireId, PipId> *route,
                                  bool useEstimate)
{
    // FIXME
    return false;
}

NEXTPNR_NAMESPACE_END
