#include <iostream>
#include "renamer.h"
#include <assert.h>
#include <bitset>


////////////////////////////////////////
// Public functions.
////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// This is the constructor function.
// When a renamer object is instantiated, the caller indicates:
// 1. The number of logical registers (e.g., 32).
// 2. The number of physical registers (e.g., 128).
// 3. The maximum number of unresolved branches.
//    Requirement: 1 <= n_branches <= 64.
//
// Tips:
//
// Assert the number of physical registers > number logical registers.
// Assert 1 <= n_branches <= 64.
// Then, allocate space for the primary data structures.
// Then, initialize the data structures based on the knowledge
// that the pipeline is intially empty (no in-flight instructions yet).
/////////////////////////////////////////////////////////////////////
  using namespace std;
	
  renamer::renamer(uint64_t n_log_regs, uint64_t n_phys_regs, uint64_t n_branches)
	{
   std::cout << "Start ----------------------------------------------------" << std::endl;
   cout << "Number of Physical Registers : " << n_phys_regs << endl;
   cout << "Number of logical Registers : " << n_log_regs << endl;
   cout << "Number of Branch Checkpoints : " << n_branches<< endl;
		assert((n_phys_regs > n_log_regs) && (n_branches >= 1) && (n_branches <= 64));
			rmt = new uint64_t[n_log_regs];
			amt = new uint64_t[n_log_regs];
			fl = new freeList[n_phys_regs - n_log_regs];
			flHead = 0;
			flTail = 0;
			actList = new activeList[n_phys_regs - n_log_regs];
			alHead = 0;
			alTail = 0;
			phyList = new uint64_t[n_phys_regs];
			
			// Initializing Branch Checkpoint
			bCheckPoint = new branchCheckpoint[n_branches];
			for (int i = 0; i <= n_branches - 1; i++) {
				bCheckPoint[i].shadowMapTable = new uint64_t[n_log_regs];
				bCheckPoint[i].GBM = 0;
			}
			
			phyRegisterReady = new uint64_t[n_phys_regs];

			for (int i = 0; i <= n_log_regs-1; i++) {
				rmt[i] = i;
			}

			for (int i = 0; i <= n_log_regs-1; i++) {
				amt[i] = i;
			}

			for (int i = 0; i <= n_phys_regs-1; i++) {
				phyList[i] = 0;
			}

			for (int i = 0; i <= (n_phys_regs - n_log_regs - 1); i++) {
				fl[i].physicalReg = i+n_log_regs;
				fl[i].valid = 1;
			}

			for (int i = 0; i <= (n_phys_regs - n_log_regs - 1); i++) {
				actList[i].valid = 0;
        //actList[i].ready = 0;
			}

			for (int i = 0; i <= (n_phys_regs - 1); i++) { ////////----------------------
				phyRegisterReady[i] = 1;
			}

			GBM = 0;
      
      phyRegSize = n_phys_regs;
			logRegSize = n_log_regs;
			numOfBranch = n_branches;
      
      

	}



	//////////////////////////////////////////
		// Functions related to Rename Stage.   //
		//////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////
		// The Rename Stage must stall if there aren't enough free physical
		// registers available for renaming all logical destination registers
		// in the current rename bundle.
		//
		// Inputs:
		// 1. bundle_dst: number of logical destination registers in
		//    current rename bundle
		//
		// Return value:there
		// Return "true" (stall) if there aren't enough free physical
		// registers to allocate to all of the logical destination registers
		// in the current rename bundle.
		/////////////////////////////////////////////////////////////////////
	// To find the number of free registers
	/*uint64_t renamer::numOfFreeRegister() {
		uint64_t freeRegCount = 0;
		for (int i = 0; i <= (phyRegSize - logRegSize - 1); i++) {
			if (fl[i].valid == 1) {
				freeRegCount++;
			}
		}
		return freeRegCount;
	}
*/
	bool renamer::stall_reg(uint64_t bundle_dst) {
   uint64_t freeRegCount = 0;
		for (int i = 0; i <= (phyRegSize - logRegSize - 1); i++) {
			if (fl[i].valid == 1) {
				freeRegCount++;
			}
		}
		if (freeRegCount < bundle_dst) {
			//printf("Rename Stalled \n");
			return true;
		}
		else {
			return false;
		}
		
	}

	/////////////////////////////////////////////////////////////////////
	// The Rename Stage must stall if there aren't enough free
	// checkpoints for all branches in the current rename bundle.
	//
	// Inputs:
	// 1. bundle_branch: number of branches in current rename bundle
	//
	// Return value:
	// Return "true" (stall) if there aren't enough free checkpoints
	// for all branches in the current rename bundle.
	/////////////////////////////////////////////////////////////////////
	//To find the number of empty branch checkpoints

	
	bool renamer::stall_branch(uint64_t bundle_branch) {
 		std::bitset <64> gbmBinary(GBM);
   uint64_t freeBranchCount = 0;
		for (int i = 0; i <= numOfBranch - 1; i++) {
			if (gbmBinary[i] == 0) {
				freeBranchCount++;
			}
		}
		if (freeBranchCount < bundle_branch) {
			//printf("Rename Stalled Branch \n");
			return true;
		}
		else {
			return false;
		}
	}
	/////////////////////////////////////////////////////////////////////
	// This function is used to get the branch mask for an instruction.
	/////////////////////////////////////////////////////////////////////
	uint64_t renamer::get_branch_mask() {
		return GBM;
	}


	/////////////////////////////////////////////////////////////////////
	// This function is used to rename a single source re/gister.
	//
	// Inputs:
	// 1. log_reg: the logical register to rename
	//
	// Return value: physical register name
	/////////////////////////////////////////////////////////////////////
	uint64_t renamer::rename_rsrc(uint64_t log_reg) {
		return rmt[log_reg];
	}


	/////////////////////////////////////////////////////////////////////
	// This function is used to rename a single destination register.
	//
	// Inputs:
	// 1. log_reg: the logical register to rename
	//
	// Return value: physical register name
	/////////////////////////////////////////////////////////////////////
	uint64_t renamer::rename_rdst(uint64_t log_reg) {
   assert(!stall_reg(1));
  		uint64_t phyReg;
      phyReg = fl[flHead].physicalReg;
  		fl[flHead].valid = 0;
  		rmt[log_reg] = phyReg;
  		phyRegisterReady[phyReg] = 0;
  		if (flHead == (phyRegSize - logRegSize - 1))
  		{
  			flHead = 0;
  		}
  		else {
  			flHead++;
  		}
     return phyReg;
	}
	/////////////////////////////////////////////////////////////////////
	// This function creates a new branch checkpoint.
	//
	// Inputs: none.
	//
	// Output:
	// 1. The function returns the branch's ID. When the branch resolves,
	//    its ID is passed back to the renamer via "resolve()" below.
	//
	// Tips:
	//
	// Allocating resources for the branch (a GBM bit and a checkpoint):
	// * Find a free bit -- i.e., a '0' bit -- in the GBM. Assert that
	//   a free bit exists: it is the user's responsibility to avoid
	//   a structural hazard by calling stall_branch() in advance.
	// * Set the bit to '1' since it is now in use by the new branch.
	// * The position of this bit in the GBM is the branch's ID.
	// * Use the branch checkpoint that corresponds to this bit.
	// 
	// The branch checkpoint should contain the following:
	// 1. Shadow Map Table (checkpointed Rename Map Table)
	// 2. checkpointed Free List head index
	// 3. checkpointed GBM
	/////////////////////////////////////////////////////////////////////
	uint64_t renamer::checkpoint() {
   	assert(!stall_branch(1));
		std::bitset	<64> gbmBinary(GBM);
		uint64_t branchId;                         // TO BE CHECKED
		for (int i = 0; i <= numOfBranch - 1; i++) {
			if (gbmBinary[i] == 0) {
				branchId = i;
				break;
			}
		}
		gbmBinary[branchId] = 1;
		GBM = (uint64_t)(gbmBinary.to_ulong());
		bCheckPoint[branchId].freeListHead = flHead;
		bCheckPoint[branchId].GBM = GBM;
    for(int i =0; i<= logRegSize-1; i++)
		bCheckPoint[branchId].shadowMapTable[i] = rmt[i];
		return branchId;
	}

	//////////////////////////////////////////
	// Functions related to Dispatch Stage. //
	//////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	// The Dispatch Stage must stall if there are not enough free
	// entries in the Active List for all instructions in the current
	// dispatch bundle.
	//
	// Inputs:
	// 1. bundle_inst: number of instructions in current dispatch bundlethere
	//
	// Return value:
	// Return "true" (stall) if the Active List does not have enough
	// space for all instructions in the dispatch bundle.
	/////////////////////////////////////////////////////////////////////
	// To find the number of free Active List registers



	bool renamer::stall_dispatch(uint64_t bundle_inst) {
   uint64_t freeRegCount = 0;
		  for (int i = 0; i <= (phyRegSize - logRegSize - 1); i++) {
			  if (actList[i].valid == 0) {
				freeRegCount++;
			}
		}
		if (freeRegCount < bundle_inst) {
			//printf("Dispatch Stalled \n");
			return true;
		}
		else {
			return false;
		}
	}


	/////////////////////////////////////////////////////////////////////
	// This function dispatches a single instruction into the Active
	// List.
	//
	// Inputs:
	// 1. dest_valid: If 'true', the instr. has a destination register,
	//    otherwise it does not. If it does not, then the log_reg and
	//    phys_reg inputs should be ignored.
	// 2. log_reg: Logical register number of the instruction's
	//    destination.
	// 3. phys_reg: Physical register number of the instruction's
	//    destination.
	// 4. load: If 'true', the instr. is a load, otherwise it isn't.
	// 5. store: If 'true', the instr. is a store, otherwise it isn't.
	// 6. branch: If 'true', the instr. is a branch, otherwise it isn't.
	// 7. amo: If 'true', this is an atomic memory operation.
	// 8. csr: If 'true', this is a system instruction.
	// 9. PC: Program counter of the instruction.
	//
	// Return value:
	// Return the instruction's index in the Active List.
	//
	// Tips:
	//
	// Before dispatching the instruction into the Active List, assert
	// that the Active List isn't full: it is the user's responsibility
	// to avoid a structural hazard by calling stall_dispatch()
	// in advance.
	/////////////////////////////////////////////////////////////////////
  uint64_t renamer::dispatch_inst(bool dest_valid,
		uint64_t log_reg,
		uint64_t phys_reg,
		bool load,
		bool store,
		bool branch,
		bool amo,
		bool csr,
		uint64_t PC) {
		uint64_t activeIndex;
		assert(!stall_dispatch(1));
		if (dest_valid) {
			actList[alTail].logicalRegister = log_reg;
			actList[alTail].physicalRegister = phys_reg;
     //phyRegisterReady[phys_reg] = 0;
    }
    else{
    }
    actList[alTail].destinationFlag = dest_valid;
		actList[alTail].loadFlag = load;
		actList[alTail].storeFlag = store;
		actList[alTail].branchFlag = branch;
		actList[alTail].amoFlag = amo;
		actList[alTail].csrFlag = csr;
		actList[alTail].pc = PC;
		actList[alTail].valid = 1;
    actList[alTail].ready = 0;
    actList[alTail].exception = 0;
    actList[alTail].loadViolationBit = 0;
    actList[alTail].branchMispredictionBit = 0;
    actList[alTail].valueMispredictionBit = 0;
   
		activeIndex = alTail;
		if (alTail == phyRegSize - logRegSize - 1) {
			alTail = 0;
		}
		else {
			alTail++;
		}
		return activeIndex;
	}



	//////////////////////////////////////////
	// Functions related to Schedule Stage. //
	//////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	// Test the ready bit of the indicated physical register.
	// Returns 'true' if ready.
	/////////////////////////////////////////////////////////////////////
	bool renamer::is_ready(uint64_t phys_reg) {
		if (phyRegisterReady[phys_reg] == 1) {
			return true;
		}
		else {
			return false;
		}
	}

	/////////////////////////////////////////////////////////////////////
	// Clear the ready bit of the indicated physical register.
	/////////////////////////////////////////////////////////////////////
	void renamer::clear_ready(uint64_t phys_reg) {
		phyRegisterReady[phys_reg] = 0;
	}

	/////////////////////////////////////////////////////////////////////
	// Set the ready bit of the indicated physical register.
	/////////////////////////////////////////////////////////////////////
	void renamer::set_ready(uint64_t phys_reg){
		phyRegisterReady[phys_reg] = 1;
		//if(phys_reg == 197)
			//printf("Destination Ready bit set for phy mapping 197 of inst 154 \n");
	}


	//////////////////////////////////////////
	// Functions related to Reg. Read Stage.//
	//////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	// Return the contents (value) of the indicated physical register.
	/////////////////////////////////////////////////////////////////////
	uint64_t renamer::read(uint64_t phys_reg) {
		return phyList[phys_reg];
	}


	//////////////////////////////////////////
	// Functions related to Writeback Stage.//
	//////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	// Write a value into the indicated physical register.
	/////////////////////////////////////////////////////////////////////
	void renamer::write(uint64_t phys_reg, uint64_t value) {
		if (phys_reg == 266) {
			//printf("Destination Value : %d \n", value);
		}
		phyList[phys_reg] = value;
	}

	/////////////////////////////////////////////////////////////////////
	// Set the completed bit of the indicated entry in the Active List.
	/////////////////////////////////////////////////////////////////////
	void renamer::set_complete(uint64_t AL_index) {
		actList[AL_index].ready = 1;
	}


	/////////////////////////////////////////////////////////////////////
	// This function is for handling branch resolution.
	//
	// Inputs:
	// 1. AL_index: Index of the branch in the Active List.
	// 2. branch_ID: This uniquely identifies the branch and the
	//    checkpoint in question.  It was originally provided
	//    by the checkpoint function.
	// 3. correct: 'true' indicates the branch was correctly
	//    predicted, 'false' indicates it was mispredicted
	//    and recovery is required.
	//
	// Outputs: none.
	//
	// Tips:
	//
	// While recovery is not needed in the case of a correct branch,
	// some actions are still required with respect to the GBM and
	// all checkpointed GBMs:
	// * Remember to clear the branch's bit in the GBM.
	// * Remember to clear the branch's bit in all checkpointed GBMs.
	//
	// In the case of a misprediction:
	// * Restore the GBM from the checkpoint. Also make sure the
	//   mispredicted branch's bit is cleared in the restored GBM,
	//   since it is now resolved and its bit and checkpoint are freed.
	// * You don't have to worry about explicitly freeing the GBM bits
	//   and checkpoints of branches that are after the mispredicted
	//   branch in program order. The mere act of restoring the GBM
	//   from the checkpoint achieves this feat.
	// * Restore other state using the branch's checkpoint.
	//   In addition to the obvious state ...  *if* you maintain a
	//   freelist length variable (you may or may not), you must
	//   recompute the freelist length. It depends on your
	//   implementation how to recompute the length.
	//   (Note: you cannot checkpoint the length like you did with
	//   the head, because the tail can change in the meantime;
	//   you must recompute the length in this function.)
	// * Do NOT set the branch misprediction bit in the active list.
	//   (Doing so would cause a second, full squash when the branch
	//   reaches the head of the Active List. We don’t want or need
	//   that because we immediately recover within this function.)
	/////////////////////////////////////////////////////////////////////
	void renamer::resolve(uint64_t AL_index,
		uint64_t branch_ID,
		bool correct) {
		if (correct) {
			
			//to clear the branch's bit in the GBM.
			std::bitset	<64> gbmBinary(GBM);
			gbmBinary[branch_ID] = 0;
			GBM = (uint64_t)(gbmBinary.to_ulong());
			

			//to clear the branch's bit in all checkpointed GBMs.
			for (int i = 0; i <= numOfBranch - 1; i++) {
				std::bitset	<64> gbmCheckpointBinary(bCheckPoint[i].GBM);
				gbmCheckpointBinary[branch_ID] = 0;
				bCheckPoint[i].GBM = (uint64_t)(gbmCheckpointBinary.to_ulong());
			}

		}
		else {
      //assert(0);
			GBM = bCheckPoint[branch_ID].GBM;
			std::bitset	<64> gbmBinary(GBM);
			gbmBinary[branch_ID] = 0;
			GBM = (uint64_t)(gbmBinary.to_ulong());
			
			// restoring rmt
      for(int i=0; i<=logRegSize-1; i++)
			rmt[i] = bCheckPoint[branch_ID].shadowMapTable[i];

	

      if(flHead > flTail){
        for(int i = bCheckPoint[branch_ID].freeListHead ;i<flHead; i++)
          fl[i].valid = 1;
      }
      else{
	      if(bCheckPoint[branch_ID].freeListHead > flHead){
          for(int i =bCheckPoint[branch_ID].freeListHead; i<=(phyRegSize - logRegSize - 1); i++)
            fl[i].valid = 1;
          
          for(int i =0; i<flHead; i++)
            fl[i].valid = 1;
           }
      	else{
		      for(int i = bCheckPoint[branch_ID].freeListHead ;i<flHead; i++)
          	fl[i].valid = 1;
      		}
	    } 
          flHead = bCheckPoint[branch_ID].freeListHead;
       


			

			// restoring tail pointer of active list

			if (alTail > alHead) {
				for (int i = AL_index + 1; i < alTail; i++) {
					actList[i].valid = 0;
				}
				alTail = AL_index + 1;
			}
			else {
				if(AL_index < alTail){
					for (int i = AL_index + 1; i < alTail; i++) {
						actList[i].valid = 0;
					}
					alTail = AL_index + 1;
				}
				else {
          if(AL_index == phyRegSize-logRegSize-1){
					  for (int i = 0; i < alTail; i++) {
						  actList[i].valid = 0;
					  }
					  alTail = 0;
           }
           else{
             for(int i= AL_index+1; i<=phyRegSize-logRegSize-1; i++){
               actList[i].valid = 0;
             }
             for(int i=0; i<alTail; i++){
               actList[i].valid = 0;
             }
             alTail = AL_index + 1;
           }
				}
			}


		}

	}



	//////////////////////////////////////////
	// Functions related to Retire Stage.   //
	//////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	// This function allows the caller to examine the instruction at the head
	// of the Active List.
	//
	// Input arguments: none.
	//
	// Return value:
	// * Return "true" if the Active List is NOT empty, i.e., there
	//   is an instruction at the head of the Active List.
	// * Return "false" if the Active List is empty, i.e., there is
	//   no instruction at the head of the Active List.
	//
	// Output arguments:
	// Simply return the following contents of the head entry of
	// the Active List.  These are don't-cares if the Active List
	// is empty (you may either return the contents of the head
	// entry anyway, or not set these at all).
	// * completed bit
	// * exception bit
	// * load violation bit
	// * branch misprediction bit
	// * value misprediction bit
	// * load flag (indicates whether or not the instr. is a load)
	// * store flag (indicates whether or not the instr. is a store)
	// * branch flag (indicates whether or not the instr. is a branch)
	// * amo flag (whether or not instr. is an atomic memory operation)
	// * csr flag (whether or not instr. is a system instruction)
	// * program counter of the instruction
	/////////////////////////////////////////////////////////////////////
	bool renamer::precommit(bool &completed,
		bool &exception, bool &load_viol, bool &br_misp, bool &val_misp,
		bool &load, bool &store, bool &branch, bool &amo, bool &csr,
		uint64_t &PC) {
  
    if (actList[alHead].valid == 1) {
			completed = actList[alHead].ready;
			exception = actList[alHead].exception;
			load_viol = actList[alHead].loadViolationBit;
		        br_misp = actList[alHead].branchMispredictionBit;
			val_misp = actList[alHead].valueMispredictionBit;
			load = actList[alHead].loadFlag;
			store = actList[alHead].storeFlag;
			branch = actList[alHead].branchFlag;
			amo = actList[alHead].amoFlag;
			csr = actList[alHead].csrFlag;
			PC = actList[alHead].pc;
			return true;
		}
		else {
			return false;
		}
   
	}


	/////////////////////////////////////////////////////////////////////
	// This function commits the instruction at the head of the Active List.
	//
	// Tip (optional but helps catch bugs):
	// Before committing the head instruction, assert that it is valid to
	// do so (use assert() from standard library). Specifically, assert
	// that all of the following are true:
	// - there is a head instruction (the active list isn't empty)
	// - the head instruction is completed
	// - the head instruction is not marked as an exception
	// - the head instruction is not marked as a load violation
	// It is the caller's (pipeline's) duty to ensure that it is valid
	// to commit the head instruction BEFORE calling this function
	// (by examining the flags returned by "precommit()" above).
	// This is why you should assert() that it is valid to commit the
	// head instruction and otherwise cause the simulator to exit.
	/////////////////////////////////////////////////////////////////////
	void renamer::commit() {
		assert((actList[alHead].valid == 1));
    assert((actList[alHead].ready == 1));
    assert((actList[alHead].exception == 0) );
    assert((actList[alHead].loadViolationBit == 0));
    if(actList[alHead].destinationFlag == true){
  		
      // Copying existing phy Mapping from AMT to Free List
  		fl[flTail].physicalReg = amt[actList[alHead].logicalRegister];
  		fl[flTail].valid = 1;	
      phyRegisterReady[amt[actList[alHead].logicalRegister]] = 0;
  		
      if (flTail == (phyRegSize - logRegSize - 1)) {
  			flTail = 0;
  		}
  		else {
  			flTail++;
  		}
     
      
     
  		// Copying new committed mapping from active list head to amt
  		amt[actList[alHead].logicalRegister] = actList[alHead].physicalRegister;
     
   }
   else{
   }
   	actList[alHead].valid = 0;
		if (alHead == (phyRegSize - logRegSize -1)) {
			alHead = 0;
		}
		else {
			alHead++;
		}
  
	}

	//////////////////////////////////////////////////////////////////////
	// Squash the renamer class.
	//
	// Squash all instructions in the Active List and think about which
	// sructures in your renamer class need to be restored, and how.
	//
	// After this function is called, the renamer should be rolled-back
	// to the committed state of the machine and all renamer state
	// should be consistent with an empty pipeline.
	/////////////////////////////////////////////////////////////////////
	void renamer::squash() {
   //assert(0);
		// Squashing All Instructions in Active List
		alTail = alHead;
		for (int i = 0; i <= phyRegSize - logRegSize - 1; i++) {
			actList[i].valid = 0;
		}

		// Restoring rmt from amt
   for(int i=0; i<=(logRegSize-1); i++)
		rmt[i] = amt[i];

		// Restoring branch checkpoints
		GBM = 0;

		// Restoring Free List
		flHead = flTail;
		for (int i = 0; i <= (phyRegSize - logRegSize - 1); i++) {
			fl[i].valid = 1;
		}

		// Restoring physical Ready Bits
		for (int i = 0; i <= phyRegSize - logRegSize - 1; i++) {
			//phyRegisterReady[fl[i].physicalReg] = 0;
			phyRegisterReady[i] = 0;


		}

		for (int i = 0; i <= (logRegSize - 1); i++)
		phyRegisterReady[amt[i]] = 1;

	}

	//////////////////////////////////////////
	// Functions not tied to specific stage.//
	//////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	// Functions for individually setting the exception bit,
	// load violation bit, branch misprediction bit, and
	// value misprediction bit, of the indicated entry in the Active List.
	/////////////////////////////////////////////////////////////////////
	void renamer::set_exception(uint64_t AL_index) {
		actList[AL_index].exception = 1;
	}
	void renamer::set_load_violation(uint64_t AL_index) {
		actList[AL_index].loadViolationBit = 1;
	}
	void renamer::set_branch_misprediction(uint64_t AL_index) {
		actList[AL_index].branchMispredictionBit = 1;
	}
	void renamer::set_value_misprediction(uint64_t AL_index) {
		actList[AL_index].valueMispredictionBit = 1;
	}

	/////////////////////////////////////////////////////////////////////
	// Query the exception bit of the indicated entry in the Active List.
	/////////////////////////////////////////////////////////////////////
	bool renamer::get_exception(uint64_t AL_index) {
		return actList[AL_index].exception;
	}


