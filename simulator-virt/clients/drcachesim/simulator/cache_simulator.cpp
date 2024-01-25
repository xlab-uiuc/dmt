/* **********************************************************
 * Copyright (c) 2015-2018 Google, Inc.  All rights reserved.
 * **********************************************************/

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Google, Inc. nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL VMWARE, INC. OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#define NUM_PWC 3
#define PWC_ENTRY_SIZE 8
const unsigned int PWC_ASSOC[] = { 1, 4, 4};
const unsigned int PWC_SIZE[] = { PWC_ENTRY_SIZE * 2, PWC_ENTRY_SIZE * 4, PWC_ENTRY_SIZE * 32};

#define NUM_PAGE_TABLE_LEVELS 4
#define PAGE_TABLE_ENTRY_SIZE 8 
#define PAGE_OFFSET_SIZE 12
#define PAGE_INDEX_SIZE 9
#define SIMULATOR_HEARTBEAT_FREQ 22 //Log of number of meminsts to process between two simulator hearbeats

#include <iostream>
#include <iterator>
#include <string>
#include <assert.h>
#include <limits.h>
#include <stdint.h> /* for supporting 64-bit integers*/
#include "../common/memref.h"
#include "../common/options.h"
#include "../common/utils.h"
#include "../reader/config_reader.h"
#include "../reader/file_reader.h"
#include "../reader/ipc_reader.h"
#include "cache_stats.h"
#include "cache.h"
#include "cache_lru.h"
#include "cache_fifo.h"
#include "cache_simulator.h"
#include "droption.h"

#include <cstdio>
#include <inttypes.h>
#include <stdlib.h>

trace_type_t TRACE_TYPE[][5] = { { TRACE_TYPE_PE1_PA, TRACE_TYPE_PE1_PE1, TRACE_TYPE_PE1_PE2, TRACE_TYPE_PE1_PE3, TRACE_TYPE_PE1_PE4 }, 
                                 { TRACE_TYPE_PE2_PA, TRACE_TYPE_PE2_PE1, TRACE_TYPE_PE2_PE2, TRACE_TYPE_PE2_PE3, TRACE_TYPE_PE2_PE4 }, 
                                 { TRACE_TYPE_PE3_PA, TRACE_TYPE_PE3_PE1, TRACE_TYPE_PE3_PE2, TRACE_TYPE_PE3_PE3, TRACE_TYPE_PE3_PE4 }, 
                                 { TRACE_TYPE_PE4_PA, TRACE_TYPE_PE4_PE1, TRACE_TYPE_PE4_PE2, TRACE_TYPE_PE4_PE3, TRACE_TYPE_PE4_PE4 } };



analysis_tool_t *
cache_simulator_create(const cache_simulator_knobs_t &knobs, const tlb_simulator_knobs_t &tlb_knobs)
{
    return new cache_simulator_t(knobs, tlb_knobs);
}

analysis_tool_t *
cache_simulator_create(const std::string &config_file)
{
    return new cache_simulator_t(config_file);
}

cache_simulator_t::cache_simulator_t(const cache_simulator_knobs_t &knobs_, const tlb_simulator_knobs_t &tlb_knobs_)
    : simulator_t(knobs_.num_cores, knobs_.skip_refs, knobs_.warmup_refs,
                  knobs_.warmup_fraction, knobs_.sim_refs, knobs_.cpu_scheduling,
                  knobs_.verbose)
    , knobs(knobs_)
    , l1_icaches(NULL)
    , l1_dcaches(NULL)
    , l2_caches(NULL)
    , pw_caches(NULL)
    , gpw_caches(NULL)
    , is_warmed_up(false)
//    , tlb_knobs(tlb_knobs_)
{
    // XXX i#1703: get defaults from hardware being run on.

    //Artemiy - add TLB
    tlb_sim = tlb_simulator_create(tlb_knobs_);

    /* initialize random seed: */
    //srand (time(NULL));
    srand (42);

    std::cout << "Initialising PT from file: " << knobs.pt_dump_filename.c_str() << std::endl;
// load page table
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result" 
    //load page_table
    FILE* page_table_file = fopen(knobs.pt_dump_filename.c_str(),"r");
    //read page_table size (in records)
    int page_table_record_num = 0;
    fscanf(page_table_file, "%d\n", &page_table_record_num);
    std::cerr << "Loading page table with " <<  page_table_record_num << " PT entries...\n";
    for (int i = 0; i < page_table_record_num; i++) {
      page_table_info_t tmp;
      fscanf(page_table_file, "%llx,%llx,%llx,%llx,%llx,%llx\n", &(tmp.VA), &(tmp.PE1), &(tmp.PE2), &(tmp.PE3), &(tmp.PE4), &(tmp.PA));
      page_table.insert(std::make_pair(tmp.VA, tmp));
    }
    std::cerr << "Loaded " << page_table.size() << " unique PT entries.\n";
    fclose(page_table_file);
#pragma GCC diagnostic pop 

// load ranges
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result" 
    if (knobs.pt_ranges_file != "") {
      FILE* range_file = fopen(knobs.pt_ranges_file.c_str(),"r");
      int range_record_num = 0;
      fscanf(range_file, "%d\n", &range_record_num);
      std::cerr << "Loading range with " <<  range_record_num << "total range entries...\n";
      for (int i = 0; i < range_record_num; i++) {
        range_info_t tmp;
        fscanf(range_file, "%llx,%llx\n", &(tmp.l_bound), &(tmp.h_bound));
        range_table.push_back(tmp);
        std::cerr << "Added " << tmp.l_bound << " " << tmp.h_bound << std::endl;
      }
      std::cerr << "Loaded " << range_table.size() << " unique ranges.\n";
      fclose(range_file);
    }
#pragma GCC diagnostic pop 

    std::cout << "Initialising VT_PT from file: " << knobs.vt_pt_dump_filename.c_str() << std::endl;
// load host_page_table table
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result" 
    //load host_page_table
    FILE* host_page_table_file = fopen(knobs.vt_pt_dump_filename.c_str(),"r");
    //read host_page_table size (in records)
    int host_page_table_record_num = 0;
    fscanf(host_page_table_file, "%d\n", &host_page_table_record_num);
    std::cerr << "Loading host_page table with " <<  host_page_table_record_num << " PT entries...\n";
    for (int i = 0; i < host_page_table_record_num; i++) {
      page_table_info_t tmp;
      fscanf(host_page_table_file, "%llx,%llx,%llx,%llx,%llx,%llx\n", &(tmp.VA), &(tmp.PE1), &(tmp.PE2), &(tmp.PE3), &(tmp.PE4), &(tmp.PA));
      host_page_table.insert(std::make_pair(tmp.VA, tmp));
    }
    std::cerr << "Loaded " << host_page_table.size() << " unique PT entries.\n";
    fclose(host_page_table_file);
#pragma GCC diagnostic pop 


//    for(page_table_t::const_iterator it = host_page_table.begin();
//    it != host_page_table.end(); ++it)
//    {
//      std::cout << it->first << " " << it->second.VA << " " << it->second.PA << "\n";
//    }

    //*********************************************************

    // This configuration allows for one shared LLC only.
    cache_t *llc = create_cache(knobs.replace_policy);
    llc1 = llc;
    if (llc == NULL) {
        success = false;
        return;
    }

    if (knobs.data_prefetcher != PREFETCH_POLICY_NEXTLINE &&
        knobs.data_prefetcher != PREFETCH_POLICY_NONE) {
        // Unknown value.
        success = false;
        return;
    }

    bool warmup_enabled = ((knobs.warmup_refs > 0) || (knobs.warmup_fraction > 0.0));

    if (!llc->init(knobs.LL_assoc, (int)knobs.line_size, (int)knobs.LL_size, NULL,
                   new cache_stats_t(knobs.LL_miss_file, warmup_enabled))) {
        error_string = "Usage error: failed to initialize LL cache.  Ensure sizes and "
                       "associativity are powers of 2, that the total size is a multiple "
                       "of the line size, and that any miss file path is writable.";
        success = false;
        return;
    }

    std::string cache_name = "LLC";
    all_caches[cache_name] = llc;
    llcaches[cache_name] = llc;

    l1_icaches = new cache_t *[knobs.num_cores];
    l1_dcaches = new cache_t *[knobs.num_cores];
    l2_caches =  new cache_t *[knobs.num_cores];
    for (unsigned int i = 0; i < knobs.num_cores; i++) {
        l2_caches[i] = create_cache(knobs.replace_policy);
        if (l2_caches[i] == NULL) {
            success = false;
            return;
        }
        l1_icaches[i] = create_cache(knobs.replace_policy);
        if (l1_icaches[i] == NULL) {
            success = false;
            return;
        }
        l1_dcaches[i] = create_cache(knobs.replace_policy);
        if (l1_dcaches[i] == NULL) {
            success = false;
            return;
        }

        if (!l2_caches[i]->init (knobs.L2_assoc, (int)knobs.line_size,
                                 (int)knobs.L2_size, llc,
                                 new cache_stats_t("", warmup_enabled)) || 
            !l1_icaches[i]->init(knobs.L1I_assoc, (int)knobs.line_size,
                                 (int)knobs.L1I_size, l2_caches[i],
                                 new cache_stats_t("", warmup_enabled)) ||
            !l1_dcaches[i]->init(knobs.L1D_assoc, (int)knobs.line_size,
                                 (int)knobs.L1D_size, l2_caches[i],
                                 new cache_stats_t("", warmup_enabled),
                                 knobs.data_prefetcher == PREFETCH_POLICY_NEXTLINE
                                     ? new prefetcher_t((int)knobs.line_size)
                                     : nullptr)) {
            error_string = "Usage error: failed to initialize L1 and L2 caches.  Ensure sizes "
                           "and associativity are powers of 2 "
                           "and that the total sizes are multiples of the line size.";
            success = false;
            return;
        }

        cache_name = "L2_Cache_" + std::to_string(i);
        all_caches[cache_name] = l2_caches[i];
        cache_name = "L1_I_Cache_" + std::to_string(i);
        all_caches[cache_name] = l1_icaches[i];
        cache_name = "L1_D_Cache_" + std::to_string(i);
        all_caches[cache_name] = l1_dcaches[i];
    }
    // host PWC
    pw_caches =  new cache_t *[NUM_PWC];
    for (unsigned int i = 0; i < NUM_PWC; i++) {
        pw_caches[i] = create_cache(knobs.replace_policy);
        if (pw_caches[i] == NULL) {
            success = false;
            return;
        }
        if (!pw_caches[i]->init (PWC_ASSOC[i], PWC_ENTRY_SIZE,
                                 PWC_SIZE[i], NULL,
                                 new cache_stats_t("", warmup_enabled))) {
            error_string = "Usage error: failed to initialize PW caches.  Ensure sizes "
                           "and associativity are powers of 2 "
                           "and that the total sizes are multiples of the line size.";
            success = false;
            return;
        }
    }
    // guest PWC
    gpw_caches =  new cache_t *[NUM_PWC];
    for (unsigned int i = 0; i < NUM_PWC; i++) {
        gpw_caches[i] = create_cache(knobs.replace_policy);
        if (gpw_caches[i] == NULL) {
            success = false;
            return;
        }
        if (!gpw_caches[i]->init (PWC_ASSOC[i], PWC_ENTRY_SIZE,
                                 PWC_SIZE[i], NULL,
                                 new cache_stats_t("", warmup_enabled))) {
            error_string = "Usage error: failed to initialize PW caches.  Ensure sizes "
                           "and associativity are powers of 2 "
                           "and that the total sizes are multiples of the line size.";
            success = false;
            return;
        }
    }
}

cache_simulator_t::cache_simulator_t(const std::string &config_file)
    : simulator_t()
    , l1_icaches(NULL)
    , l1_dcaches(NULL)
    , pw_caches(NULL)
    , gpw_caches(NULL)
    , is_warmed_up(false)
{
    std::map<std::string, cache_params_t> cache_params;
    config_reader_t config_reader;
    if (!config_reader.configure(config_file, knobs, cache_params)) {
        error_string =
            "Usage error: Failed to read/parse configuration file " + config_file;
        success = false;
        return;
    }

    init_knobs(knobs.num_cores, knobs.skip_refs, knobs.warmup_refs, knobs.warmup_fraction,
               knobs.sim_refs, knobs.cpu_scheduling, knobs.verbose);

    if (knobs.data_prefetcher != PREFETCH_POLICY_NEXTLINE &&
        knobs.data_prefetcher != PREFETCH_POLICY_NONE) {
        // Unknown prefetcher type.
        success = false;
        return;
    }

    bool warmup_enabled = ((knobs.warmup_refs > 0) || (knobs.warmup_fraction > 0.0));

    l1_icaches = new cache_t *[knobs.num_cores];
    l1_dcaches = new cache_t *[knobs.num_cores];

    // Create all the caches in the hierarchy.
    for (auto &cache_params_it : cache_params) {
        std::string cache_name = cache_params_it.first;
        auto &cache_config = cache_params_it.second;

        cache_t *cache = create_cache(cache_config.replace_policy);
        if (cache == NULL) {
            success = false;
            return;
        }

        all_caches[cache_name] = cache;
    }

    // Initialize all the caches in the hierarchy and identify both
    // the L1 caches and LLC(s).
    for (auto &cache_it : all_caches) {
        std::string cache_name = cache_it.first;
        cache_t *cache = cache_it.second;

        // Locate the cache's configuration.
        const auto &cache_config_it = cache_params.find(cache_name);
        if (cache_config_it == cache_params.end()) {
            error_string = "Error locating the configuration of the cache: " + cache_name;
            success = false;
            return;
        }
        auto &cache_config = cache_config_it->second;

        // Locate the cache's parent.
        cache_t *parent = NULL;
        if (cache_config.parent != CACHE_PARENT_MEMORY) {
            const auto &parent_it = all_caches.find(cache_config.parent);
            if (parent_it == all_caches.end()) {
                error_string = "Error locating the configuration of the parent cache: " +
                    cache_config.parent;
                success = false;
                return;
            }
            parent = parent_it->second;
        }

        // Locate the cache's children.
        std::vector<caching_device_t *> children;
        children.clear();
        if (cache_config.inclusive) {
            for (std::string &child_name : cache_config.children) {
                const auto &child_it = all_caches.find(child_name);
                if (child_it == all_caches.end()) {
                    error_string =
                        "Error locating the configuration of the cache: " + child_name;
                    success = false;
                    return;
                }
                children.push_back(child_it->second);
            }
        }

        if (!cache->init((int)cache_config.assoc, (int)knobs.line_size,
                         (int)cache_config.size, parent,
                         new cache_stats_t(cache_config.miss_file, warmup_enabled),
                         cache_config.prefetcher == PREFETCH_POLICY_NEXTLINE
                             ? new prefetcher_t((int)knobs.line_size)
                             : nullptr,
                         cache_config.inclusive, children)) {
            error_string = "Usage error: failed to initialize the cache " + cache_name;
            success = false;
            return;
        }

        bool is_l1_or_llc = false;

        // Assign the pointers to the L1 instruction and data caches.
        if (cache_config.core >= 0 && cache_config.core < (int)knobs.num_cores) {
            is_l1_or_llc = true;
            if (cache_config.type == CACHE_TYPE_INSTRUCTION ||
                cache_config.type == CACHE_TYPE_UNIFIED) {
                l1_icaches[cache_config.core] = cache;
            }
            if (cache_config.type == CACHE_TYPE_DATA ||
                cache_config.type == CACHE_TYPE_UNIFIED) {
                l1_dcaches[cache_config.core] = cache;
            }
        }

        // Assign the pointer(s) to the LLC(s).
        if (cache_config.parent == CACHE_PARENT_MEMORY) {
            is_l1_or_llc = true;
            llcaches[cache_name] = cache;
        }

        // Keep track of non-L1 and non-LLC caches.
        if (!is_l1_or_llc) {
            other_caches[cache_name] = cache;
        }
    }
}

cache_simulator_t::~cache_simulator_t()
{
    for (auto &caches_it : all_caches) {
        cache_t *cache = caches_it.second;
        delete cache->get_stats();
        delete cache->get_prefetcher();
        delete cache;
    }

    if (l1_icaches != NULL) {
        delete[] l1_icaches;
    }
    if (l1_dcaches != NULL) {
        delete[] l1_dcaches;
    }
    if (l2_caches != NULL) {
        delete[] l2_caches;
    }
    if (pw_caches != NULL) {
        delete[] pw_caches;
    }
    if (gpw_caches != NULL) {
        delete[] gpw_caches;
    }
}

uint64_t
cache_simulator_t::remaining_sim_refs() const
{
    return knobs.sim_refs;
}

std::pair<bool,bool>
cache_simulator_t::process_memref(const memref_t &memref, bool changed) {
return std::pair<bool,bool>(true,true);
}

static uint64_t num_request = 0;
static uint64_t num_not_found = 0;
static uint64_t vt_num_not_found = 0;
static uint64_t num_request_shifted = 0;

static uint64_t num_range_found = 0;
static uint64_t num_range_not_found = 0;

bool
cache_simulator_t::process_memref(const memref_t &memref)
{
    num_request++;
    num_request_shifted++;

    if ((num_request_shifted >> SIMULATOR_HEARTBEAT_FREQ) > 0) {
      num_request_shifted = 0;
      std::cerr << "Heartbeat. " << num_request << " references processed.\n";
      print_results();
      //std::cerr << std::endl;
      //std::cerr << std::endl;
      //std::cerr << std::endl;
      //std::cerr << std::endl;
    }

      

    if (knobs.skip_refs > 0) {
        knobs.skip_refs--;
        return true;
    }

    // If no warmup is specified and we have simulated sim_refs then
    // we are done.
    if ((knobs.warmup_refs == 0 && knobs.warmup_fraction == 0.0) && knobs.sim_refs == 0) {
        if (knobs.verbose >= 3) {
            std::cerr << "No warmup, but have simulation refs -> Done " << memref.data.pid << "." << memref.data.tid << ":: "
                      << "marker type " << memref.marker.marker_type << " value "
                      << memref.marker.marker_value << "\n";
        }
        return true;
    }

    // The references after warmup and simulated ones are dropped.
    if (check_warmed_up() && knobs.sim_refs == 0) {
        if (knobs.verbose >= 3) {
            std::cerr << "The references after warmup and simulated ones are dropped " << memref.data.pid << "." << memref.data.tid << ":: "
                      << "marker type " << memref.marker.marker_type << " value "
                      << memref.marker.marker_value << "\n";
        }
        return true;
    }

    // Both warmup and simulated references are simulated.

    if (!simulator_t::process_memref(memref)) {
        if (knobs.verbose >= 3) {
            std::cerr << "Both warmup and simulated references are simulated " << memref.data.pid << "." << memref.data.tid << ":: "
                      << "marker type " << memref.marker.marker_type << " value "
                      << memref.marker.marker_value << "\n";
        }
        return false;
    }

    if (memref.marker.type == TRACE_TYPE_MARKER) {
        // We ignore markers before we ask core_for_thread, to avoid asking
        // too early on a timestamp marker.
        if (knobs.verbose >= 3) {
            std::cerr << "::" << memref.data.pid << "." << memref.data.tid << ":: "
                      << "marker type " << memref.marker.marker_type << " value "
                      << memref.marker.marker_value << "\n";
        }
        return true;
    }

    // We use a static scheduling of threads to cores, as it is
    // not practical to measure which core each thread actually
    // ran on for each memref.
    int core;
    if (memref.data.tid == last_thread)
        core = last_core;
    else {
        core = core_for_thread(memref.data.tid);
        last_thread = memref.data.tid;
        last_core = core;
    }

    //Artemiy: add TLB
    uint64_t addr;

    uint64_t virtual_page_addr = 0;
    uint64_t page_offset = 0;
    uint64_t physical_page_addr = 0;
    uint64_t instrs_type = 0;

    if (type_is_instr(memref.instr.type) || memref.instr.type == TRACE_TYPE_PREFETCH_INSTR) {
      addr              = memref.instr.addr;
      virtual_page_addr = memref.instr.addr >> 12;
      page_offset       = memref.instr.addr & ((1 << 12) - 1);
      instrs_type       = 1;
      //std::cerr << __FUNCTION__ << " should be instr request" << std::endl;
    } else if (memref.data.type == TRACE_TYPE_READ || memref.data.type == TRACE_TYPE_WRITE || type_is_prefetch(memref.data.type)) {
      addr              = memref.data.addr;
      virtual_page_addr = memref.data.addr >> 12;
      page_offset       = memref.data.addr & ((1 << 12) - 1);
      instrs_type       = 2;
      //std::cerr << __FUNCTION__ << " should be data request" << std::endl;
    }

    uint64_t virtual_full_page_addr = virtual_page_addr << 12;

    //TLB request
    std::pair<bool, bool> res = tlb_sim->process_memref(memref, true /*changeByArtemiy*/);
    bool is_TLB_hit = res.second;
    if (knobs.verbose >= 2) {
      std::cerr << __FUNCTION__ << " Received TLB result: " << is_TLB_hit << std::endl;
    }

    memref_t new_memref; 
    new_memref = memref;
    new_memref.marker.type = memref.marker.type;
    new_memref.marker.pid = memref.marker.pid;
    new_memref.marker.tid = memref.marker.tid;

    page_table_t::iterator guest_it = page_table.find(virtual_full_page_addr);
    if (guest_it != page_table.end()) {


	    page_table_t::iterator it = host_page_table.find((guest_it->second.PA >> 12) << 12);
	    page_table_t::iterator last_it = it;
	    if (it != host_page_table.end()) {
	      
	      physical_page_addr = it->second.PA;

	      if (type_is_instr(memref.instr.type) || memref.instr.type == TRACE_TYPE_PREFETCH_INSTR) {
          new_memref.instr.addr = physical_page_addr + page_offset;
	      } else if (memref.data.type == TRACE_TYPE_READ || memref.data.type == TRACE_TYPE_WRITE || type_is_prefetch(memref.data.type)) {
          new_memref.data.addr  = physical_page_addr + page_offset;
	      }

	      //std::cerr << "Request \n"
	      //              << std::hex 
	      //              << "Type " << ((instrs_type == 1) ? "instr" : "data") << std::endl 
	      //              << "Vddr " << addr <<  std::endl
	      //              << "VAddr >> 12 : " << virtual_page_addr << std::endl
	      //              << "VAddr page_addr : " << (virtual_page_addr << 12) << std::endl
	      //              << "PAddr page_addr : " << physical_page_addr << std::endl
	      //              << "PAddr page_addr : " << physical_page_addr + page_offset << std::endl
	      //              << std::dec
	      //              << "num_requests : " << num_request << "\n" << std::hex
        //              << "guest PE1: " << guest_it->second.PE1 << std::endl
        //              << "guest PE2: " << guest_it->second.PE2 << std::endl
        //              << "guest PE3: " << guest_it->second.PE3 << std::endl
        //              << "guest PE4: " << guest_it->second.PE4 << std::endl
        //              << "guest PA: " << guest_it->second.PA << std::endl
	      //            ;


	      // process TLB miss
	      if (!is_TLB_hit) {
          if (knobs.verbose >= 2) {
            std::cerr << "TLB miss \n" << std::flush;
          }

          bool range_found = true;
          // reset page walk results
          page_walk_res.clear();

          cache_result_t gpwc_search_res = NOT_FOUND;
          unsigned int gpwc_hit_level = 0;
          memref_t gpwc_check_memref; 
          gpwc_check_memref.data.type = TRACE_TYPE_READ;
          gpwc_check_memref.data.size = 1;
          // search PWC starting from highest level
          for(unsigned int i = NUM_PWC; i >= 1; i--) {
            gpwc_check_memref.data.addr = virtual_full_page_addr >> (12 + (4 - i) * 9);

            gpwc_search_res = gpw_caches[i-1]->request(gpwc_check_memref, true /*Artemiy*/);
            // if found, memorize and stop searching 
            if (gpwc_search_res == FOUND_L1) {
              if (gpwc_hit_level == 0) {
                gpwc_hit_level = i;
              }
            }
          }

          for (unsigned int level_guest = 1; level_guest <= NUM_PAGE_TABLE_LEVELS; level_guest++) {
            long long unsigned int page_offset_in_vpage = 8 * ((virtual_full_page_addr >> (12 + (4 - level_guest) * 9)) & ((1 << 9) - 1));
            if (gpwc_hit_level < level_guest) {
              // if not found in the PWC, then make a memory req
              //one_pw_at_host(page_walk_res, *(guest_it->second.all[level_guest]) + 8 * ((virtual_full_page_addr >> (12 + (4 - level_guest) * 9)) & ((1 << 9) - 1)), level_guest, core);
              one_pw_at_host(page_walk_res, *(guest_it->second.all[level_guest]), page_offset_in_vpage, level_guest, core);

            } else if (gpwc_hit_level == level_guest) {
              // if found in the PWC, indicate PWC_LAT
              for (unsigned int i = 0; i < NUM_PAGE_TABLE_LEVELS; i++) {
                page_walk_res.push_back(ZERO);
              }
              page_walk_res.push_back(PWC);

            } else if (gpwc_hit_level > level_guest) {
              // if skipped due to a PWC hit, indicate ZERO_LAT
              for (unsigned int i = 0; i < NUM_PAGE_TABLE_LEVELS+1; i++) {
                page_walk_res.push_back(ZERO);
              }
            }
          }
          it = last_it;
          make_request(page_walk_res, TRACE_TYPE_PA_PE1, it->second.PE1, guest_it->second.PA + page_offset, 1, core);
          make_request(page_walk_res, TRACE_TYPE_PA_PE2, it->second.PE2, guest_it->second.PA + page_offset, 2, core);
          make_request(page_walk_res, TRACE_TYPE_PA_PE3, it->second.PE3, guest_it->second.PA + page_offset, 3, core);
          make_request(page_walk_res, TRACE_TYPE_PA_PE4, it->second.PE4, guest_it->second.PA + page_offset, 4, core);


          if (range_found) {
            page_walk_res.push_back(RANGE_HIT);
              num_range_found++;
          } else {
            page_walk_res.push_back(RANGE_MISS);
              num_range_not_found++;
          }

          hm_full_statistic_t::iterator it = hm_full_statistic.find(page_walk_res);
          if (it != hm_full_statistic.end()) {
            it->second++;
          } else {
            hm_full_statistic.insert(std::make_pair(page_walk_res, 1));
          }
       } // end processing TLB miss
     } else { // virtual not found
          vt_num_not_found++;
          if (knobs.verbose >= 2) {
      std::cerr << "Error: cannot find translation for host page table" << std::endl
              << std::hex 
              << "Type " << ((instrs_type == 1) ? "instr" : "data") << std::endl 
              << "Vddr " << addr <<  std::endl
              << "VAddr >> 12 : " << virtual_page_addr << std::endl
              << "VAddr page_addr : " << (virtual_page_addr << 12) << std::endl
              << "guest PE addr : " << ((guest_it->second.PA >> 12) << 12) << std::endl
              << std::dec
              << "VAddr page_offset : " << page_offset << std::endl
              << "num_requests : " << num_request << std::endl
              << "num_not_found : " << num_not_found <<  std::endl
              << "host num_not_found : " << vt_num_not_found <<  std::endl
              ;
          }
          return true;
      }
    } else { // physical not found
      num_not_found++;
      if (knobs.verbose >= 2) {
  std::cerr << "Error: cannot find translation for actual page table" << std::endl
          << std::hex 
          << "Type " << ((instrs_type == 1) ? "instr" : "data") << std::endl 
          << "Vddr " << addr <<  std::endl
          << "VAddr >> 12 : " << virtual_page_addr << std::endl
          << "VAddr page_addr : " << (virtual_page_addr << 12) << std::endl
          << std::dec
          << "VAddr page_offset : " << page_offset << std::endl
          << "num_requests : " << num_request << std::endl
          << "num_not_found : " << num_not_found <<  std::endl
          << "host num_not_found : " << vt_num_not_found <<  std::endl
          ;
      }
      return true;
    }

    cache_result_t search_res;
    if (type_is_instr(new_memref.instr.type) ||
        new_memref.instr.type == TRACE_TYPE_PREFETCH_INSTR) {
        if (knobs.verbose >= 2) {
            std::cerr << "Go to L1I\n";
        }
        if (knobs.verbose >= 3) {
            std::cerr << "Go to L1I\n";
            std::cerr << "::" << new_memref.data.pid << "." << new_memref.data.tid << ":: "
                      << " @" << (void *)new_memref.instr.addr << " instr x"
                      << new_memref.instr.size << "\n";
        }
        l1_icaches[core]->request(new_memref);
    } else if (new_memref.data.type == TRACE_TYPE_READ ||
               new_memref.data.type == TRACE_TYPE_WRITE ||
               // We may potentially handle prefetches differently.
               // TRACE_TYPE_PREFETCH_INSTR is handled above.
               type_is_prefetch(new_memref.data.type)) {
        if (knobs.verbose >= 2) {
            std::cerr << "Go to L1D\n";
        }
        if (knobs.verbose >= 3) {
            std::cerr << "::" << new_memref.data.pid << "." << new_memref.data.tid << ":: "
                      << " @" << (void *)new_memref.data.pc << " "
                      << trace_type_names[new_memref.data.type] << " "
                      << (void *)new_memref.data.addr << " x" << new_memref.data.size << "\n";
        }
        search_res = l1_dcaches[core]->request(new_memref, true /*Artemiy*/);
    } else if (new_memref.flush.type == TRACE_TYPE_INSTR_FLUSH) {
        if (knobs.verbose >= 3) {
            std::cerr << "::" << new_memref.data.pid << "." << new_memref.data.tid << ":: "
                      << " @" << (void *)new_memref.data.pc << " iflush "
                      << (void *)new_memref.data.addr << " x" << new_memref.data.size << "\n";
        }
        l1_icaches[core]->flush(new_memref);
    } else if (new_memref.flush.type == TRACE_TYPE_DATA_FLUSH) {
        if (knobs.verbose >= 3) {
            std::cerr << "::" << new_memref.data.pid << "." << new_memref.data.tid << ":: "
                      << " @" << (void *)new_memref.data.pc << " dflush "
                      << (void *)new_memref.data.addr << " x" << new_memref.data.size << "\n";
        }
        l1_dcaches[core]->flush(new_memref);
    } else if (new_memref.exit.type == TRACE_TYPE_THREAD_EXIT) {
        handle_thread_exit(new_memref.exit.tid);
        last_thread = 0;
    } else if (new_memref.marker.type == TRACE_TYPE_INSTR_NO_FETCH) {
        // Just ignore.
        if (knobs.verbose >= 3) {
            std::cerr << "::" << new_memref.data.pid << "." << new_memref.data.tid << ":: "
                      << " @" << (void *)new_memref.instr.addr << " non-fetched instr x"
                      << new_memref.instr.size << "\n";
        }
    } else {
        std::cout << __FILE__ << " " << __func__ << std::endl;
        error_string = "Unhandled memref type " + std::to_string(new_memref.data.type);
        return false;
    }

    if (op_contention_L1.get_value() != 0) {

      //L1 contention 
      if (op_contention_L1.get_value() >= 100) {
        unsigned int i = 100;
        for(; i <= op_contention_L1.get_value(); i+=100) {
          addr_t raddr = rand() & ((1L << 48) - 1);
          memref_t page_walk_memref; 
          page_walk_memref.data.type = TRACE_TYPE_CONT_L1;
          page_walk_memref.data.addr = raddr;
          page_walk_memref.data.size = 1; 
          cache_result_t res = l1_dcaches[core]->request(page_walk_memref, true /* Artemiy -- get the source */);
          if (knobs.verbose >= 2) {
            std::cerr << "Contention L1: res" << res << std::endl;
          }
        }
        int fraction_part = 100 - (i - op_contention_L1.get_value());
        if (fraction_part > 0) {
          int roll = rand() % 100; 
          if (fraction_part >= roll) {
            addr_t raddr = rand() & ((1L << 48) - 1);
            memref_t page_walk_memref; 
            page_walk_memref.data.type = TRACE_TYPE_CONT_L1;
            page_walk_memref.data.addr = raddr;
            page_walk_memref.data.size = 1; 
            cache_result_t res = l1_dcaches[core]->request(page_walk_memref, true /* Artemiy -- get the source */);
            if (knobs.verbose >= 2) {
              std::cerr << "Contention L1: res" << res << std::endl;
            }
          }
        }
      } else { //op_contention_L1.get_value() < 100
        int fraction_part = op_contention_L1.get_value();
        if (fraction_part > 0) {
          int roll = rand() % 100; 
          if (fraction_part >= roll) {
            addr_t raddr = rand() & ((1L << 48) - 1);
            memref_t page_walk_memref; 
            page_walk_memref.data.type = TRACE_TYPE_CONT_L1;
            page_walk_memref.data.addr = raddr;
            page_walk_memref.data.size = 1; 
            cache_result_t res = l1_dcaches[core]->request(page_walk_memref, true /* Artemiy -- get the source */);
            if (knobs.verbose >= 2) {
              std::cerr << "Contention L1: res" << res << std::endl;
            }
          }
        }
      }
    } //end if L1 contention

    if ((op_contention_LLC.get_value() != 0) && ((search_res == FOUND_LLC) || (search_res == NOT_FOUND))) {

      //LLC contention 
      if (op_contention_LLC.get_value() >= 100) {
        unsigned int i = 100;
        for(; i <= op_contention_LLC.get_value(); i+=100) {
          addr_t raddr = rand() & ((1L << 48) - 1);
          memref_t page_walk_memref; 
          page_walk_memref.data.type = TRACE_TYPE_CONT_LLC;
          page_walk_memref.data.addr = raddr;
          page_walk_memref.data.size = 1; 
          cache_result_t res = llc1->request(page_walk_memref, true /* Artemiy -- get the source */);
          if (knobs.verbose >= 2) {
            std::cerr << "Contention LLC fraction: res " << res << std::endl;
          }
        }
        int fraction_part = 100 - (i - op_contention_LLC.get_value());
        if (fraction_part > 0) {
          int roll = rand() % 100; 
          if (fraction_part >= roll) {
            addr_t raddr = rand() & ((1L << 48) - 1);
            memref_t page_walk_memref; 
            page_walk_memref.data.type = TRACE_TYPE_CONT_LLC;
            page_walk_memref.data.addr = raddr;
            page_walk_memref.data.size = 1; 
            cache_result_t res = llc1->request(page_walk_memref, true /* Artemiy -- get the source */);
            if (knobs.verbose >= 2) {
              std::cerr << "Contention LLC fraction: res " << res << std::endl;
            }
          }
        }
      } else { //op_contention_LLC.get_value() < 100
        int fraction_part = op_contention_LLC.get_value();
        if (fraction_part > 0) {
          int roll = rand() % 100; 
          if (fraction_part >= roll) {
            addr_t raddr = rand() & ((1L << 48) - 1);
            memref_t page_walk_memref; 
            page_walk_memref.data.type = TRACE_TYPE_CONT_LLC;
            page_walk_memref.data.addr = raddr;
            page_walk_memref.data.size = 1; 
            cache_result_t res = llc1->request(page_walk_memref, true /* Artemiy -- get the source */);
            if (knobs.verbose >= 2) {
              std::cerr << "Contention LLC fraction: res " << res << std::endl;
            }
          }
        }
      }
    } //end if LLC contention
  

    // reset cache stats when warming up is completed
    if (!is_warmed_up && check_warmed_up()) {
        for (auto &cache_it : all_caches) {
            cache_t *cache = cache_it.second;
            cache->get_stats()->reset();
        }
        if (knobs.verbose >= 0) {
            std::cerr << "Cache simulation warmed up\n";
        }
        //clear the hm_statistic_map
        hm_full_statistic.clear(); 
    } else {
        knobs.sim_refs--;
    }

    return true;
}


    typedef std::vector<cache_result_t> page_walk_hm_result_t;
    typedef std::map< page_walk_hm_result_t, uint64_t> hm_full_statistic_t;
//    typedef std::unordered_map<long long unsigned int, page_table_info_t> page_table_t;
void cache_simulator_t::make_request(page_walk_hm_result_t& page_walk_res, 
                                     trace_type_t type, 
                                     long long unsigned int base_addr, 
                                     long long unsigned int addr_to_find, 
                                     int level, 
                                     int core)
{
  if (knobs.verbose >= 2) {
    std::cerr << "Start walk Type: " << type 
    << " Level: " << level 
    << std::hex
    << " BaseAddr: " << base_addr 
    << " addr_to_find: " << addr_to_find 
    << std::dec
    << std::endl;
  }
  memref_t page_walk_memref; 

  page_walk_memref.data.type = type;
  page_walk_memref.data.addr = base_addr + 8 * ( ( addr_to_find >> (12 + (( 4 - level) * 9))) & ((1 << 9) - 1) );
  page_walk_memref.data.size = 1; 
  page_walk_res.push_back(l1_dcaches[core]->request(page_walk_memref, true /* Artemiy -- get the source */));
  if (knobs.verbose >= 2) {
    std::cerr << "Done walk Type: " << type 
    << " Level: " << level 
    << std::hex
    << " BaseAddr: " << base_addr 
    << " addr_to_find: " << addr_to_find 
    << " offset in cur table: " << (( addr_to_find >> (12 + (( 4 - level) * 9))) & ((1 << 9) - 1))
    << " final addr : " << page_walk_memref.data.addr
    << std::dec
    << std::endl;
  }
  return;
}

void cache_simulator_t::make_request_simple(trace_type_t type, long long unsigned int addr, int core)
{
  if (knobs.verbose >= 2) {
    std::cerr << "Start make_request_simple" << type 
    << " addr_to_find: " << addr
    << std::endl;
  }
  memref_t page_walk_memref; 

  page_walk_memref.data.type = type;
  page_walk_memref.data.addr = addr ;
  page_walk_memref.data.size = 1; 
  page_walk_res.push_back(l1_dcaches[core]->request(page_walk_memref, true /* Artemiy -- get the source */));
  if (knobs.verbose >= 2) {
    std::cerr << "Done make_request_simple" << type 
    << " final_addr: " << addr
    << std::endl;
  }
  return;
}

void cache_simulator_t::one_pw_at_host(page_walk_hm_result_t& page_walk_res, 
                                       long long unsigned int guest_addr, 
                                       long long unsigned int page_offset_in_vpage, 
                                       uint64_t level_guest, 
                                       int core) {

  cache_result_t pwc_search_res = NOT_FOUND;
  unsigned int pwc_hit_level = 0;
  memref_t pwc_check_memref; 
  pwc_check_memref.data.type = TRACE_TYPE_READ;
  pwc_check_memref.data.size = 1;
  // search PWC starting from highest level
  for(unsigned int i = NUM_PWC; i >= 1; i--) {
    pwc_check_memref.data.addr = guest_addr >> (PAGE_OFFSET_SIZE + (4 - i) * 9);
    pwc_search_res = pw_caches[i-1]->request(pwc_check_memref, true /*Artemiy*/);
    // if found, memorize and stop searching 
    if (pwc_search_res == FOUND_L1) {
      if (pwc_hit_level == 0) {
        pwc_hit_level = i;
      }
    }
  }
  long long unsigned int page_offset_guest_addr_to_find = 0;
  // find a record in the host PT corresponding to the given guest address
  page_table_t::iterator host_it = host_page_table.find((guest_addr >> PAGE_OFFSET_SIZE) << PAGE_OFFSET_SIZE);
  page_offset_guest_addr_to_find = page_offset_in_vpage;
                             

  //long long unsigned int guest_addr_to_find = guest_addr + page_offset_guest_addr_to_find;
  long long unsigned int guest_addr_to_find = guest_addr;

  for (unsigned int level_host = 1; level_host <= NUM_PAGE_TABLE_LEVELS; level_host++) {
    if (pwc_hit_level < level_host) {
      // if not found in the PWC, then make a memory req
      make_request(page_walk_res, 
                   TRACE_TYPE[level_guest][level_host], 
                   *(host_it->second.all[level_host]), 
                   guest_addr_to_find, 
                   level_host, 
                   core); 

    } else if (pwc_hit_level == level_host) {
      // if found in the PWC, indicate PWC_LAT
      page_walk_res.push_back(PWC);

    } else if (pwc_hit_level > level_host) {
      // if skipped due to a PWC hit, indicate ZERO_LAT
      page_walk_res.push_back(ZERO);
    }
  }
//  make_request_simple(TRACE_TYPE[level_guest][0], host_it->second.PA + page_offset_guest_addr_to_find, core);                                    // A5
// Artemiy: reuse make_request for fetching 5,10,15,20 from memory 
// 0 is passed as a stub
  make_request(page_walk_res, TRACE_TYPE[level_guest][0], host_it->second.PA + page_offset_guest_addr_to_find, 0, 1, core);                                    // A5
}

// Return true if the number of warmup references have been executed or if
// specified fraction of the llcaches has been loaded. Also return true if the
// cache has already been warmed up. When there are multiple last level caches
// this function only returns true when all of them have been warmed up.
bool
cache_simulator_t::check_warmed_up()
{
    // If the cache has already been warmed up return true
    if (is_warmed_up)
        return true;

    // If the warmup_fraction option is set then check if the last level has
    // loaded enough data to be warmed up.
    if (knobs.warmup_fraction > 0.0) {
        is_warmed_up = true;
        for (auto &cache : llcaches) {
            if (cache.second->get_loaded_fraction() < knobs.warmup_fraction) {
                is_warmed_up = false;
                break;
            }
        }

        if (is_warmed_up) {
            return true;
        }
    }

    // If warmup_refs is set then decrement and indicate warmup done when
    // counter hits zero.
    if (knobs.warmup_refs > 0) {
        knobs.warmup_refs--;
        if (knobs.warmup_refs == 0) {
            is_warmed_up = true;
            return true;
        }
    }

    // If we reach here then warmup is not done.
    return false;
}

bool
cache_simulator_t::print_results()
{
    tlb_sim->print_results();
    std::cerr << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cerr << "Cache simulation results:" << std::endl;
    // Print core and associated L1 cache stats first.
    for (unsigned int i = 0; i < knobs.num_cores; i++) {
        print_core(i);
        if (thread_ever_counts[i] > 0) {
            if (l1_icaches[i] != l1_dcaches[i]) {
                std::cerr << "  L1I stats:" << std::endl;
                l1_icaches[i]->get_stats()->print_stats("    ");
                std::cerr << "  L1D stats:" << std::endl;
                l1_dcaches[i]->get_stats()->print_stats("    ");
                std::cerr << "  L2 stats:" << std::endl;
                l2_caches[i]->get_stats()->print_stats("    ");
            } else {
                std::cerr << "  unified L1 stats:" << std::endl;
                l1_icaches[i]->get_stats()->print_stats("    ");
            }
        }
    }
    // Print non-L1, non-LLC cache stats.
    for (auto &caches_it : other_caches) {
        std::cerr << caches_it.first << " stats:" << std::endl;
        caches_it.second->get_stats()->print_stats("    ");
    }

    // Print LLC stats.
    for (auto &caches_it : llcaches) {
        std::cerr << caches_it.first << " stats:" << std::endl;
        caches_it.second->get_stats()->print_stats("    ");
    }
    std::cerr << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cerr << "num_requests : " << num_request << std::endl 
              << "num_not_found : " << num_not_found << std::endl;

    std::cerr << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cerr << "num_range_found : "     << num_range_found << std::endl 
              << "num_range_not_found : " << num_range_not_found << std::endl;
    std::cerr << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings" 
    char *print_hm_stats[] =
    {
        "MEMORY"
        , "L1"
        , "L2"
        , "LLC"
        , "WRONG"
        , "RANGE_HIT"
        , "RANGE_MISS"
        , "PWC"
        , "ZERO"
    };
#pragma GCC diagnostic pop 
    for (hm_full_statistic_t::iterator it = hm_full_statistic.begin(); it != hm_full_statistic.end(); it++) {
      for(unsigned int i = 0; i < it->first.size(); i++) {
        std::cerr << print_hm_stats[it->first[i]] << ",";
      }
      std::cerr << "\t" << it->second << std::endl;
    }

    return true;
}

cache_t *
cache_simulator_t::create_cache(const std::string &policy)
{
    if (policy == REPLACE_POLICY_NON_SPECIFIED || // default LRU
        policy == REPLACE_POLICY_LRU)             // set to LRU
        return new cache_lru_t;
    if (policy == REPLACE_POLICY_LFU) // set to LFU
        return new cache_t;
    if (policy == REPLACE_POLICY_FIFO) // set to FIFO
        return new cache_fifo_t;

    // undefined replacement policy
    ERRMSG("Usage error: undefined replacement policy. "
           "Please choose " REPLACE_POLICY_LRU " or " REPLACE_POLICY_LFU ".\n");
    return NULL;
}
