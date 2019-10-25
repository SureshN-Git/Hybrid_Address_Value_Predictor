#include <stdio.h>
#include <inttypes.h>

class predictor
{
private:
	struct pred
	{
		uint64_t last_val, last_val2, last_val3,last_val4, del, conf, iter_count;
	};
	struct hyb_pred
	{
		uint64_t last_val, del, conf;
	};
	struct VPT	{
		uint64_t value, conf;
	};

	uint64_t *update_queue;
	uint64_t *chooser;
	bool choose;
	VPT *vpt;
	hyb_pred *hystride;
	pred *predict;
public:
	int correct_predicted_count = 0;
	int misp_count = 0;
	int not_predicted_but_correct = 0;
	int not_predicted_not_correct = 0;
	int predicted =0;
	int counter = 0;
	int order = 0;
	int al_size;
	bool hybrid;
	uint64_t HASH;
	uint64_t uq_tail;
	uint64_t uq_head;
	predictor(uint64_t AL_size, uint64_t type, uint64_t ORD);
	uint64_t stride(uint64_t tag);
	uint64_t last_value(uint64_t tag);
	uint64_t get_val(uint64_t type, uint64_t PC);
	uint64_t hash(uint64_t PC, uint64_t value1, uint64_t value2, uint64_t value3, uint64_t value4);
	void enqueue (uint64_t tag);
	uint64_t get_context_val(uint64_t LV1, uint64_t LV2,uint64_t LV3,uint64_t LV4, uint64_t found, uint64_t PC);
	uint64_t get_hybrid_val(uint64_t LV1, uint64_t LV2,uint64_t LV3,uint64_t LV4, uint64_t found, uint64_t PC);
	void update_predict(uint64_t value, uint64_t PC, uint64_t index);
	void dec_iter(uint64_t PC);
	void inc_iter(uint64_t PC);
	void clear_predictor();
	void checkpoint_context(uint64_t tail);
	bool get_conf(uint64_t PC);
	bool get_context_conf(uint64_t LV1, uint64_t LV2,uint64_t LV3,uint64_t LV4, uint64_t found, uint64_t PC);
	bool get_hybrid_conf(uint64_t LV1, uint64_t LV2,uint64_t LV3,uint64_t LV4, uint64_t found, uint64_t PC);
	~predictor();
};
