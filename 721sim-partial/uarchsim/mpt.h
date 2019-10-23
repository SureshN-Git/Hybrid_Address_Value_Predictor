#include "pipeline.h"

typedef struct {

	bool valid;
	unsigned int effective_address;
	int stride;
	int shb;
	union64_t value;
	bool valid_value;
	unsigned int next_predicted_address;
	int pred_count;   // Count for multiple outstanding prediction
   	int dispatch_conf; // #cycles for getting ready and issued

} mpt_table;

//Forward declaring classes
class pipeline_t;
class payload;
class stats_t;

class mpt {
private:
	pipeline_t* proc;
	mpt_table* mp_table;
	int size; // always powers of 2


public:

       int confident_and_correct= 0;
       int confident_and_incorrect= 0;
       int not_confident_and_correct= 0;
       int not_confident_and_incorrect= 0;
       
	int delay_one_to_five = 0;
	int delay_five_to_ten = 0;
	int delay_ten_to_fifty = 0;
	int delay_fifty_to_seventy_five = 0;
	int delay_seventy_five_to_hundred = 0;
	int delay_more_than_hundred = 0;

	// Constructor
	mpt(unsigned int size);

	// Read the mpt for matching Load/Store entry in the Fetch stage
	bool read_entry(unsigned int pc);

	// Create a new entry in the mpt after AGEN/RETIRE stage
	void create_entry(reg_t addr, unsigned int pc);

	// Is Address Predictable
	bool is_addr_predictable(unsigned int pc);

	// Predict Address
	unsigned int predict_address(unsigned int pc);

	// Predict Next Address
	unsigned int predict_next_addr (unsigned int pc);

	// Update an existing entry in MPT after RETIRE
	void update_entry(reg_t current_addr, unsigned int pc, int inst_issue_delay);

	// Set confidence to 0 on misprediction
	void set_confidence_low(unsigned int pc);

	// Resect all prediction Counts to zer on squash
	void squash_pred_count();

	void dec_pred_count(unsigned int pc);

	void inc_pred_count(unsigned int pc);

};
