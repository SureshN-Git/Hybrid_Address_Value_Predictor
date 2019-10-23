#include "pipeline.h"

// constructor
mpt::mpt(unsigned int size) {

	// Initialize the Memory Prefetch Table
	mp_table = new mpt_table[size];
	this->size = size;

}

bool mpt::read_entry(unsigned int pc) {

	unsigned int mask = size - 1;
	unsigned int mpt_index = ((pc >> 2) & mask);
	mp_table[mpt_index].pred_count++;
	if (mp_table[mpt_index].valid) {
		//PAY.buf[index].mpt_hit = true;
		return true;
	}
	else {
		//PAY.buf[index].mpt_hit = false;
		return false;
	}
}

void mpt::create_entry(reg_t current_addr, unsigned int pc) {

	unsigned int mask = size - 1;
	unsigned int mpt_index = ((pc >> 2) & mask);

	mp_table[mpt_index].valid = 1;
	mp_table[mpt_index].effective_address = current_addr;
	mp_table[mpt_index].stride = 0;
	mp_table[mpt_index].shb = 0;
	mp_table[mpt_index].next_predicted_address = 0;

	if (mp_table[mpt_index].pred_count == 0)
		mp_table[mpt_index].pred_count = 0;
	else
		mp_table[mpt_index].pred_count--;

}


bool mpt::is_addr_predictable(unsigned int pc) {

	unsigned int mask = size - 1;
	unsigned int mpt_index = ((pc >> 2) & mask);



	if (mp_table[mpt_index].shb >= 15 && mp_table[mpt_index].dispatch_conf >=2) {


		//printf("Number of Predicted Instructions :  %d \n", predictionCount);
		return true;


	}
	else {
		return false;
	}


}

unsigned int mpt::predict_address(unsigned int pc) {

	unsigned int mask = size - 1;
	unsigned int mpt_index = ((pc >> 2) & mask);


		return mp_table[mpt_index].effective_address + ((mp_table[mpt_index].pred_count) * mp_table[mpt_index].stride);


}


unsigned int mpt::predict_next_addr(unsigned int pc) {

	unsigned int mask = size - 1;
	unsigned int mpt_index = ((pc >> 2) & mask);

	if (mp_table[mpt_index].shb >= 2) {

			return mp_table[mpt_index].effective_address + (2 * mp_table[mpt_index].stride);

	}

}


void mpt::update_entry(reg_t current_addr, unsigned int pc, int inst_issue_delay) {

	unsigned int mask = size - 1;
	unsigned int mpt_index = ((pc >> 2) & mask);

	// Updating Pred Count
	if (mp_table[mpt_index].pred_count == 0)
		mp_table[mpt_index].pred_count = 0;
	else
		mp_table[mpt_index].pred_count--;



	//if (current_addr >= mp_table[mpt_index].effective_address) {
	// Updating SHB
	if (current_addr - mp_table[mpt_index].effective_address == mp_table[mpt_index].stride) {
		if (mp_table[mpt_index].shb == 15)
			mp_table[mpt_index].shb = 15;
		else
			mp_table[mpt_index].shb++;
	}
	else {
		if (mp_table[mpt_index].shb == 0)
			mp_table[mpt_index].shb = 0;
		else
			mp_table[mpt_index].shb = 0;
	}


	// Updating Stride
	mp_table[mpt_index].stride = current_addr - mp_table[mpt_index].effective_address;


	// Updaing Effective Address
	mp_table[mpt_index].effective_address = current_addr;

	// Updaing Dispatch Confidence

	if(inst_issue_delay < 3)
	{
		mp_table[mpt_index].dispatch_conf = 0;
	}

	else
	{
	if (mp_table[mpt_index].dispatch_conf == 2)
			mp_table[mpt_index].dispatch_conf = 2;
		else
			mp_table[mpt_index].dispatch_conf++;
	}

	//}
	/*else {

		if (mp_table[mpt_index].effective_address - current_addr == mp_table[mpt_index].stride) {
			if (mp_table[mpt_index].shb == 100)
				mp_table[mpt_index].shb = 100;
			else
				mp_table[mpt_index].shb++;
		}
		else {
			if (mp_table[mpt_index].shb == 0)
				mp_table[mpt_index].shb = 0;
			else
				mp_table[mpt_index].shb = 0;
		}
		//printf("SHB = %d \n",mp_table[mpt_index].shb);


			mp_table[mpt_index].stride = mp_table[mpt_index].effective_address - current_addr;


	}*/






}


void mpt::set_confidence_low(unsigned int pc) {

	unsigned int mask = size - 1;
	unsigned int mpt_index = ((pc >> 2) & mask);

	mp_table[mpt_index].shb = 0;

}


void mpt::squash_pred_count() {

	for (int i = 0; i <= size - 1; i++) {

		mp_table[i].pred_count = 0;
	}





}

void mpt::dec_pred_count(unsigned int pc) {


	unsigned int mask = size - 1;
	unsigned int mpt_index = ((pc >> 2) & mask);

		mp_table[mpt_index].pred_count--;

}

void mpt::inc_pred_count(unsigned int pc) {


	unsigned int mask = size - 1;
	unsigned int mpt_index = ((pc >> 2) & mask);

		mp_table[mpt_index].pred_count++;

}
