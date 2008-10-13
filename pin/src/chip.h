// Jonathan Eastep and Harshad Kasture
//

#ifndef CHIP_H
#define CHIP_H

#include <iostream>
#include <fstream>
#include <map>

#include "pin.H"
#include "capi.h"
#include "core.h"
#include "ocache.h"
#include "dram_directory_entry.h"
#include "cache_state.h"
#include "address_home_lookup.h"
#include "perfmdl.h"
#include "syscall_model.h"
#include "syscall_server.h"

// external variables

// JME: not entirely sure why these are needed...
class Chip;
class Core;

extern Chip *g_chip;
extern SyscallServer *g_syscall_server;

extern LEVEL_BASE::KNOB<string> g_knob_output_file;

// prototypes

// FIXME: if possible, these shouldn't be globals. Pin callbacks may need them to be. 

CAPI_return_t chipInit(int *rank);

CAPI_return_t chipRank(int *rank);

CAPI_return_t commRank(int *rank);

CAPI_return_t chipSendW(CAPI_endpoint_t sender, CAPI_endpoint_t receiver,
                        char *buffer, int size);

CAPI_return_t chipRecvW(CAPI_endpoint_t sender, CAPI_endpoint_t receiver,
                        char *buffer, int size);
//harshad should replace this -cpc
CAPI_return_t chipHackFinish(int my_rank);

CAPI_return_t chipDebugSetMemState(ADDRINT address, INT32 dram_address_home_id, DramDirectoryEntry::dstate_t dstate, CacheState::cstate_t cstate0, CacheState::cstate_t cstate1, vector<UINT32> sharers_list);

CAPI_return_t chipDebugAssertMemState(ADDRINT address, INT32 dram_address_home_id, DramDirectoryEntry::dstate_t dstate, CacheState::cstate_t cstate0, CacheState::cstate_t cstate1, vector<UINT32> sharers_list, string test_code, string error_code);


CAPI_return_t chipSetDramBoundaries(vector< pair<ADDRINT, ADDRINT> > addr_boundaries);

// performance model wrappers

void perfModelRun(PerfModelIntervalStat *interval_stats);

void perfModelRun(PerfModelIntervalStat *interval_stats, REG *reads, 
                  UINT32 num_reads);

void perfModelRun(PerfModelIntervalStat *interval_stats, bool dcache_load_hit, 
                  REG *writes, UINT32 num_writes);

PerfModelIntervalStat* perfModelAnalyzeInterval(const string& parent_routine, 
                                                const INS& start_ins, const INS& end_ins);

void perfModelLogICacheLoadAccess(PerfModelIntervalStat *stats, bool hit);
     
void perfModelLogDCacheStoreAccess(PerfModelIntervalStat *stats, bool hit);

void perfModelLogBranchPrediction(PerfModelIntervalStat *stats, bool correct);


// organic cache model wrappers

bool icacheRunLoadModel(ADDRINT i_addr, UINT32 size);

bool dcacheRunLoadModel(ADDRINT d_addr, UINT32 size);

bool dcacheRunStoreModel(ADDRINT d_addr, UINT32 size);

// syscall model wrappers
void syscallEnterRunModel(CONTEXT *ctx, SYSCALL_STANDARD syscall_standard);
void syscallExitRunModel(CONTEXT *ctx, SYSCALL_STANDARD syscall_standard);

// syscall server wrappers
void syscallServerRun();


// chip class

class Chip 
{

   // wrappers
   public:

      // messaging wrappers

      friend CAPI_return_t chipInit(int *rank); 
      friend CAPI_return_t chipRank(int *rank);
      
	  //FIXME: this is a hack function 
	  friend CAPI_return_t chipHackFinish(int my_rank);
      
      friend CAPI_return_t commRank(int *rank);
      friend CAPI_return_t chipSendW(CAPI_endpoint_t sender, 
                                     CAPI_endpoint_t receiver, char *buffer, 
                                     int size);
      friend CAPI_return_t chipRecvW(CAPI_endpoint_t sender, 
                                     CAPI_endpoint_t receiver, char *buffer, 
                                     int size);

      // performance modeling wrappers
 
      friend void perfModelRun(PerfModelIntervalStat *interval_stats);
      friend void perfModelRun(PerfModelIntervalStat *interval_stats, REG *reads, 
                               UINT32 num_reads);
      friend void perfModelRun(PerfModelIntervalStat *interval_stats, bool dcache_load_hit, 
                               REG *writes, UINT32 num_writes);
      friend PerfModelIntervalStat* perfModelAnalyzeInterval(const string& parent_routine, 
                                                             const INS& start_ins, 
                                                             const INS& end_ins);
      friend void perfModelLogICacheLoadAccess(PerfModelIntervalStat *stats, bool hit);
      friend void perfModelLogDCacheStoreAccess(PerfModelIntervalStat *stats, bool hit);
      friend void perfModelLogBranchPrediction(PerfModelIntervalStat *stats, bool correct);      

      // syscall modeling wrapper
      friend void syscallEnterRunModel(CONTEXT *ctx, SYSCALL_STANDARD syscall_standard);
      friend void syscallExitRunModel(CONTEXT *ctx, SYSCALL_STANDARD syscall_standard);

      
      // organic cache modeling wrappers
      friend bool icacheRunLoadModel(ADDRINT i_addr, UINT32 size);
      friend bool dcacheRunLoadModel(ADDRINT d_addr, UINT32 size);
      friend bool dcacheRunStoreModel(ADDRINT d_addr, UINT32 size);      

   private:

      int num_modules;

      UINT64 *proc_time;

      // tid_map takes core # to pin thread id
      // core_map takes pin thread id to core # (it's the reverse map)
      THREADID *tid_map;
      map<THREADID, int> core_map;
      int prev_rank;
      PIN_LOCK maps_lock;
      PIN_LOCK dcache_lock;

      Core *core;


   public:

      UINT64 getProcTime(int module) { assert(module < num_modules); return proc_time[module]; }

      void setProcTime(int module, unsigned long long new_time) 
      { assert(module < num_modules); proc_time[module] = new_time; }

      int getNumModules() { return num_modules; }

      Chip(int num_mods);

      void fini(int code, void *v);

		//input parameters: 
		//an address to set the conditions for
		//a dram vector, with a pair for the id's of the dram directories to set, and the value to set it to
		//a cache vector, with a pair for the id's of the caches to set, and the value to set it to
		void debugSetInitialMemConditions(ADDRINT address, vector< pair<INT32, DramDirectoryEntry::dstate_t> > dram_vector, vector< pair<INT32, CacheState::cstate_t> > cache_vector, vector<UINT32> sharers_list);
		bool debugAssertMemConditions(ADDRINT address, vector< pair<INT32, DramDirectoryEntry::dstate_t> > dram_vector, vector< pair<INT32, CacheState::cstate_t> > cache_vector, vector<UINT32> sharers_list, string test_code, string error_string);
		
		/*
		void setDramBoundaries(vector< pair<ADDRINT, ADDRINT> > addr_boundaries);
		*/

		void getDCacheModelLock(int rank);
		void releaseDCacheModelLock(int rank);
};

#endif
