#include <stdio.h>
#include <math.h>
#include <cmath>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include "predictor.h"

using namespace std;

predictor::predictor(uint64_t AL_size, uint64_t type, uint64_t ORD)
{
	uint64_t count = pow(2,15);
	uint64_t count1 = pow(2,25);
	predict = new pred[count];
	hystride = new hyb_pred[count];
	vpt = new VPT[count1];
	chooser = new uint64_t[count];
	for (uint64_t i = 0; i < count; i++)
	{
		predict[i].last_val = 0;
		predict[i].last_val2 = 0;
		predict[i].last_val3 = 0;
		predict[i].last_val4 = 0;
		predict[i].del = 0;
		predict[i].conf = 0;
		predict[i].iter_count = 0;
		chooser[i] = 1;
	}

	for (uint64_t i = 0; i < count1; i++)
	{
		vpt[i].value = 0;
		vpt[i].conf = 0;
	}
	al_size = AL_size;
	update_queue = new uint64_t[al_size];
	order = ORD;
	uq_tail = 0;
	uq_head = 0;
}

uint64_t predictor::stride(uint64_t tag)
{

	uint64_t value;
	predict[tag].iter_count++;
	value = predict[tag].last_val + (predict[tag].del * predict[tag].iter_count);
	return value;
}

uint64_t predictor::last_value(uint64_t tag)
{
	return predict[tag].last_val;
}

uint64_t predictor::get_val(uint64_t type, uint64_t PC)
{
	uint64_t value;
	uint64_t tag = PC>>2;
	tag = tag & 32767;

	if (type == 1)
	{
		counter = 1;
		value = stride(tag);
	}
	else if (type == 2)
	{
		value = last_value(tag);
		counter = 2;
	}

	return value;
}

uint64_t predictor::get_context_val(uint64_t LV1, uint64_t LV2,uint64_t LV3, uint64_t LV4, uint64_t found, uint64_t PC)
{
	uint64_t value, tag;
	uint64_t tag1;
	tag1 = PC >> 2;
	tag1 = tag1 & 32767;

	counter = 3;
	if (order == 4) {
		if(found >= 4)
		{
			tag = hash(PC,LV4, LV3, LV2, LV1);
			value = vpt[tag].value;
		}
		else if (found == 3)
		{
			tag = hash(PC, predict[tag1].last_val4,LV3,LV2, LV1);
			value = vpt[tag].value;
		}
		else if (found == 2)
		{
			tag = hash(PC, predict[tag1].last_val3, predict[tag1].last_val4,LV2,LV1);
			value = vpt[tag].value;
		}
		else if (found == 1)
		{
			tag = hash(PC, predict[tag1].last_val2, predict[tag1].last_val3,predict[tag1].last_val4,LV1);
			value = vpt[tag].value;
		}
		else if (found == 0)
		{
			tag = hash(PC,predict[tag1].last_val, predict[tag1].last_val2, predict[tag1].last_val3, predict[tag1].last_val4);
			value = vpt[tag].value;
		}
	}
	else if (order == 3) {
		if (found == 3) {
			tag = hash(PC, LV3, LV2, LV1, 0);
			value = vpt[tag].value;
		}
		else if (found == 2) {
			tag = hash(PC, predict[tag1].last_val3, LV2, LV1, 0);
			value = vpt[tag].value;
		}
		else if (found == 1) {
			tag = hash(PC, predict[tag1].last_val2, predict[tag1].last_val3, LV1, 0);
			value = vpt[tag].value;
		}
		else if (found == 0) {
			tag = hash(PC, predict[tag1].last_val, predict[tag1].last_val2, predict[tag1].last_val, 0);
			value = vpt[tag].value;
		}
	}
	else if (order == 2) {
		if (found == 2) {
			tag = hash(PC, LV2, LV1, 0, 0);
			value = vpt[tag].value;
		}
		else if (found == 1) {
			tag = hash(PC, predict[tag1].last_val2, LV1, 0, 0);
			value = vpt[tag].value;
		}
		else if (found == 0) {
			tag = hash(PC, predict[tag1].last_val, predict[tag1].last_val2, 0, 0);
			value = vpt[tag].value;
		}
	}
	else if (order == 1) {
		if (found == 1) {
			tag = hash(PC, LV1, 0, 0, 0);
			value = vpt[tag].value;
		}
		else if (found == 0) {
			tag = hash(PC, predict[tag1].last_val, 0, 0, 0);
			value = vpt[tag].value;
		}
	}
	return value;
}

uint64_t predictor::get_hybrid_val(uint64_t LV1, uint64_t LV2,uint64_t LV3,uint64_t LV4, uint64_t found, uint64_t PC)
{
	uint64_t value_con, value_str, tag;
	uint64_t tag1;
	tag1 = PC >> 2;
	hybrid = 1;
	tag1 = tag1 & 32767;
	bool choose;
	if (chooser[tag]>=2){
		choose = 1;
	}
	else{
		choose = 0;
	}
	value_con = get_context_val(LV1, LV2, LV3, LV4, found, PC);

	value_str = hystride[tag].last_val + (hystride[tag].del * found);
	if (choose) {
		return value_con;
	}
	else{
		return value_str;
	}

}


// uint64_t predictor::hash(uint64_t PC, uint64_t value1, uint64_t value2, uint64_t value3, uint64_t value4)
// {
// 	uint64_t tag,tag1,x1,x2,x3;
// 	if (order == 4) {
// 		PC = PC >>2;
// 		value3 = value3 ^ PC;
// 		//value3 = value3 << 8;
// 		value2 = value2 ^ PC;
// 		//value2 = value2 << 16;
// 		value1 = value1 ^ PC;
// 		//value1 = value1 << 24;
// 		value4 = value4 ^ PC;
//
// 		tag = value1+value2+value3+value4;
// 		tag1 = pow(2,25);
// 		tag1 = tag1 - 1;
// 		tag = tag & tag1;
// 	}
// 	else if (order == 3) {
// 		value1 = value1 ^ PC;
// 		value2 = value2 ^ PC;
// 		value3 = value3 ^ PC;
// 		// value1 = value1 << 20;
// 		// value2 = value2 << 10;
// 		// tag = value1+value2+value3+value4;
// 		PC = PC >>2;
// 		tag = value1+value2+value3;
// 		tag1 = pow(2,25);
// 		tag1 = tag1 - 1;
// 		tag = tag & tag1;
// 	}
// 	else if (order == 2) {
// 		// PC = PC << 20;
// 		// value1 = value1 & 1023;
// 		// value2 = value2 & 1023;
// 		// value1 = value1 << 10;
// 		PC = PC >> 2;
// 		tag = PC^value1^value2;
// 		tag1 = pow(2,25);
// 		tag1 = tag1 - 1;
// 		tag = tag & tag1;
// 	}
// 	else if (order == 1){
// 		// value1 = value1 & 32767;
// 		// PC = PC & 32767;
// 		// PC = PC << 15;
// 		PC = PC >>2;
// 		tag = PC^value1;
// 		tag1 = pow(2,25);
// 		tag1 = tag1 - 1;
// 		tag = tag & tag1;
// 	}
// 	enqueue(tag);
// 	HASH = tag;
// 	return tag;
// }

// uint64_t predictor::hash(uint64_t PC, uint64_t value1, uint64_t value2, uint64_t value3, uint64_t value4)
// {
// 	uint64_t tag,tag1,x1,x2,x3;
// 	if (order == 4) {
// 		value3 = value3 ^ PC;
// 		value3 = value3 << 8;
// 		value2 = value2 ^ PC;
// 		value2 = value2 << 16;
// 		value1 = value1 ^ PC;
// 		value1 = value1 << 24;
// 		value4 = value4 ^ PC;
// 		tag = value1+value2+value3+value4;
// 		tag1 = pow(2,25);
// 		tag1 = tag1 - 1;
// 		tag = tag & tag1;
// 	}
// 	else if (order == 3) {
// 		value1 = value1 ^ PC;
// 		value2 = value2 ^ PC;
// 		value3 = value3 ^ PC;
// 		value1 = value1 << 20;
// 		value2 = value2 << 10;
// 		tag = value1+value2+value3+value4;
// 		tag1 = pow(2,25);
// 		tag1 = tag1 - 1;
// 		tag = tag & tag1;
// 	}
// 	else if (order == 2) {
// 		PC = PC << 25;
// 		value1 = value1 & 1023;
// 		value2 = value2 & 1023;
// 		value1 = value1 << 10;
// 		tag = PC+value1+value2;
// 		tag1 = pow(2,25);
// 		tag1 = tag1 - 1;
// 		tag = tag & tag1;
// 	}
// 	else if (order == 1){
// 		value1 = value1 & 32767;
// 		PC = PC & 32767;
// 		PC = PC << 15;
// 		tag = PC + value1;
// 		tag1 = pow(2,25);
// 		tag1 = tag1 - 1;
// 		tag = tag & tag1;
// 		//tag = value1;
// 	}
// 	enqueue(tag);
// 	HASH = tag;
// 	return tag;
// }

uint64_t predictor::hash(uint64_t PC, uint64_t value1, uint64_t value2, uint64_t value3, uint64_t value4)
{
	uint64_t tag,tag1,x1,x2,x3;
	if (order == 4) {
		PC = PC >>2;
		// value3 = value3 ^ PC;
		// //value3 = value3 << 8;
		// value2 = value2 ^ PC;
		// //value2 = value2 << 16;
		// value1 = value1 ^ PC;
		// //value1 = value1 << 24;
		// value4 = value4 ^ PC;

		tag = value1+value2+value3+value4;
		tag1 = pow(2,25);
		tag1 = tag1 - 1;
		tag = tag & tag1;
	}
	else if (order == 3) {
		// value1 = value1 ^ PC;
		// value2 = value2 ^ PC;
		// value3 = value3 ^ PC;
		// value1 = value1 << 20;
		// value2 = value2 << 10;
		// tag = value1+value2+value3+value4;
		PC = PC >>2;
		tag = value1+value2+value3;
		tag1 = pow(2,25);
		tag1 = tag1 - 1;
		tag = tag & tag1;
	}
	else if (order == 2) {
		// PC = PC << 20;
		// value1 = value1 & 1023;
		// value2 = value2 & 1023;
		// value1 = value1 << 10;
		PC = PC >> 2;
		tag = value1+value2;
		tag1 = pow(2,25);
		tag1 = tag1 - 1;
		tag = tag & tag1;
	}
	else if (order == 1){
		// value1 = value1 & 32767;
		// PC = PC & 32767;
		// PC = PC << 15;
		PC = PC >>2;
		tag = PC+value1;
		tag1 = pow(2,25);
		tag1 = tag1 - 1;
		tag = tag & tag1;
	}
	enqueue(tag);
	HASH = tag;
	return tag;
}

void predictor::enqueue(uint64_t tag)
{
	update_queue[uq_tail] = tag;
	if(uq_tail == (al_size-1))
	{
		uq_tail = 0;
	}
	else
	{
		uq_tail++;
	}
}

void predictor::update_predict(uint64_t value, uint64_t PC, uint64_t index)
{
	uint64_t delta;
	uint64_t tag = PC>>2;
	tag = tag & 32767;

	delta = value - predict[tag].last_val;

	if (counter == 1)
	{


	if (delta == predict[tag].del)
	{
		if (predict[tag].conf < 31)
		{
			predict[tag].conf++;
		}
	}
	else
	{
		if (predict[tag].conf > 0)
		{
			predict[tag].conf = 0;
		}
	}

	predict[tag].del = delta;
	predict[tag].last_val = value;

	}

	if (counter == 2)
	{
		if (value == predict[tag].last_val)
		{
			if (predict[tag].conf < 31)
			{
			predict[tag].conf++;
			}
		}
		else
		{
			if (predict[tag].conf > 0)
			{
			predict[tag].conf = 0;
			}
		}
		predict[tag].last_val = value;
	}
	if (counter == 3 && !hybrid)
	{
		tag = index;

		if(value == vpt[tag].value)
		{
			if(vpt[tag].conf < 31)
			{
				vpt[tag].conf++;
			}
		}
		else
		{
			if(vpt[tag].conf >0)
			{
				vpt[tag].conf=0;
				// vpt[tag].value = value;
			}
			vpt[tag].value = value;
		}

		if(uq_head == (al_size-1))
		{
			uq_head = 0;
		}
		else
		{
			uq_head++;
		}

		tag = PC>>2;
		tag = tag & 32767;
		if (order == 4) {
			predict[tag].last_val = predict[tag].last_val2;
			predict[tag].last_val2 = predict[tag].last_val3;
			predict[tag].last_val3 = predict[tag].last_val4;
			predict[tag].last_val4 = value;
		}
		else if (order == 3) {
			predict[tag].last_val = predict[tag].last_val2;
			predict[tag].last_val2 = predict[tag].last_val3;
			predict[tag].last_val3 = value;
		}
		else if(order == 2){
			predict[tag].last_val = predict[tag].last_val2;
			predict[tag].last_val2 = value;
		}
		else if(order == 1){
			predict[tag].last_val = value;
		}
	}
	if (hybrid) {
		//bool choose;
		// if (chooser[tag] >= 2) {
		// 	choose = 1;
		// }
		// else{
		// 	choose = 0;
		// }
		if (value == vpt[index].value) {
			if (chooser[tag] < 3) {
				chooser[tag]++;
			}
		}
		else if (value == hystride[tag].last_val) {
			if (chooser[tag] > 0) {
				chooser[tag]--;
			}
		}
		 if (choose) {
			tag = index;
			if(value == vpt[tag].value)
			{
				if(vpt[tag].conf < 31)
				{
					vpt[tag].conf++;
				}
			}
			else
			{
				if(vpt[tag].conf >0)
				{
					vpt[tag].conf=0;
					// vpt[tag].value = value;
				}
				vpt[tag].value = value;
			}

			if(uq_head == (al_size-1))
			{
				uq_head = 0;
			}
			else
			{
				uq_head++;
			}

			tag = PC>>2;
			tag = tag & 32767;
			if (order == 4) {
				predict[tag].last_val = predict[tag].last_val2;
				predict[tag].last_val2 = predict[tag].last_val3;
				predict[tag].last_val3 = predict[tag].last_val4;
				predict[tag].last_val4 = value;
			}
			else if (order == 3) {
				predict[tag].last_val = predict[tag].last_val2;
				predict[tag].last_val2 = predict[tag].last_val3;
				predict[tag].last_val3 = value;
			}
			else if(order == 2){
				predict[tag].last_val = predict[tag].last_val2;
				predict[tag].last_val2 = value;
			}
			else if(order == 1){
				predict[tag].last_val = value;
			}
		 }
		 else
		{
			delta = value - hystride[tag].last_val;
			if (delta == hystride[tag].del)
			{
				if (hystride[tag].conf < 31)
				{
					hystride[tag].conf++;
				}
			}
			else
			{
				if (hystride[tag].conf > 0)
				{
					hystride[tag].conf = 0;
				}
			}
			hystride[tag].del = delta;
			hystride[tag].last_val = value;
		}
	}
}


void predictor::dec_iter(uint64_t PC)
{
	uint64_t tag = PC>>2;
	tag = tag & 32767;
	if (predict[tag].iter_count > 0)
	{
		predict[tag].iter_count--;
	}
}

void predictor::checkpoint_context(uint64_t tail)
{
    uq_tail = tail;
}

void predictor::inc_iter(uint64_t PC)
{
	uint64_t tag = PC>>2;
	tag = tag & 32767;
	predict[tag].iter_count++;
}

void predictor::clear_predictor()
{

	uint64_t count = pow(2,15);
	for (uint64_t i = 0; i < count; i++)
	{
		predict[i].iter_count = 0;
	}

	uq_head = 0;
	uq_tail = 0;
}

bool predictor::get_conf(uint64_t PC)
{
	uint64_t tag = PC>>2;
	tag = tag & 32767;
	if(predict[tag].conf == 31)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool predictor::get_context_conf(uint64_t LV1, uint64_t LV2,uint64_t LV3, uint64_t LV4, uint64_t found, uint64_t PC)
{
	uint64_t value, tag;
	uint64_t tag1;
	tag1 = PC >> 2;
	tag1 = tag1 & 32767;
	counter = 3;
	if (order == 4) {
		if(found >= 4)
		{
			tag = hash(PC,LV4, LV3, LV2, LV1);
			value = vpt[tag].conf;
		}
		else if (found == 3)
		{
			tag = hash(PC, predict[tag1].last_val4,LV3,LV2, LV1);
			value = vpt[tag].conf;
		}
		else if (found == 2)
		{
			tag = hash(PC, predict[tag1].last_val3, predict[tag1].last_val4,LV2,LV1);
			value = vpt[tag].conf;
		}
		else if (found == 1)
		{
			tag = hash(PC, predict[tag1].last_val2, predict[tag1].last_val3,predict[tag1].last_val4,LV1);
			value = vpt[tag].conf;
		}
		else if (found == 0)
		{
			tag = hash(PC,predict[tag1].last_val, predict[tag1].last_val2, predict[tag1].last_val3, predict[tag1].last_val4);
			value = vpt[tag].conf;
		}
	}
	else if (order == 3) {
		if (found == 3) {
			tag = hash(PC, LV3, LV2, LV1, 0);
			value = vpt[tag].conf;
		}
		else if (found == 2) {
			tag = hash(PC, predict[tag1].last_val3, LV2, LV1, 0);
			value = vpt[tag].conf;
		}
		else if (found == 1) {
			tag = hash(PC, predict[tag1].last_val2, predict[tag1].last_val3, LV1, 0);
			value = vpt[tag].conf;
		}
		else if (found == 0) {
			tag = hash(PC, predict[tag1].last_val, predict[tag1].last_val2, predict[tag1].last_val, 0);
			value = vpt[tag].conf;
		}
	}
	else if (order == 2) {
		if (found == 2) {
			tag = hash(PC, LV2, LV1, 0, 0);
			value = vpt[tag].conf;
		}
		else if (found == 1) {
			tag = hash(PC, predict[tag1].last_val2, LV1, 0, 0);
			value = vpt[tag].conf;
		}
		else if (found == 0) {
			tag = hash(PC, predict[tag1].last_val, predict[tag1].last_val2, 0, 0);
			value = vpt[tag].conf;
		}
	}
	else if (order == 1) {
		if (found == 1) {
			tag = hash(PC, LV1, 0, 0, 0);
			value = vpt[tag].conf;
		}
		else if (found == 0) {
			tag = hash(PC, predict[tag1].last_val, 0, 0, 0);
			value = vpt[tag].conf;
		}
	}
	if (value == 31) {
		return 1;
	}
	else{
		return 0;
	}
}

bool predictor::get_hybrid_conf(uint64_t LV1, uint64_t LV2,uint64_t LV3,uint64_t LV4, uint64_t found, uint64_t PC)
{
	uint64_t value, tag, conf;
	uint64_t tag1;
	tag1 = PC >> 2;
	tag1 = tag1 & 32767;

	hybrid = 1;
	if (chooser[tag] >= 2) {
		choose = 1;
	}
	else{
		choose = 0;
	}
	if (choose) {
		conf = get_context_conf(LV1, LV2, LV3, LV4, found, PC);
		return conf;
	}
	else{
		if(hystride[tag].conf == 31)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}

predictor::~predictor()
{

}
