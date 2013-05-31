#pragma once

#include "Character.h"

//This class performs actual calculations

class Evaluator
{
public:
	//The only public function does more or less what it should
	Characters evaluate(const Characters & input) const;

protected:
	//Evaluation stages
	Characters do_evaluate(const Characters & input) const;
	Characters do_evaluate(Characters & firstOperand, Characters & secondOperand, Character & oper) const;
	bool parse(const Characters & input, Characters & firstOperand, Characters & secondOperand, Character & oper) const;
	bool is_atom(const Characters & input) const;

	//Main operations
	Characters sum(Characters & firstOperand, Characters & secondOperand) const;
	Characters sub(Characters & firstOperand, Characters & secondOperand) const;
	Characters mul(Characters & firstOperand, Characters & secondOperand) const;
	Characters div(Characters & firstOperand, Characters & secondOperand) const;

	//Basic operators: everything else depends on them
	bool dec(Character & ch) const;
	void inc(Character & digit, Character & transfer) const;
	bool inc(Character & digit) const;

	//Addition and substraction stages
	void sum(Character first, Character second, Character third, Character & result, Character & transfer) const; 
	void sub(Character first, Character second, Character borrow, Character & result, Character & next_borrow) const; 
	Characters sum_aligned(Characters & firstOperand, Characters & secondOperand) const;
	Characters sub_aligned(Characters & firstOperand, Characters & secondOperand) const;

	//Multiplication and division stages
	Characters mul_ints(Characters & firstOperand, Characters & secondOperand) const;
	Characters mul(Characters & number, Character & digit) const;
	Characters div_by_int(Characters & firstOperand, Characters & secondOperand) const;
	bool div_int(Characters & firstOperand, Characters & secondOperand, Characters & result) const;

	//Cuts off everything but the last expression
	std::vector<Character> significant_part(const Characters & input) const;

	//Check if the input is a complete expression
	bool must_evaluate(const Characters & input) const;

	//Sometimes we need to align operands
	void align_dots(Characters & firstOperand, Characters & secondOperand) const;
	void align_lengths(Characters & firstOperand, Characters & secondOperand) const;

	//Functions that don't change the numeric value of parameter
	void chop(Characters & result) const;
	void rpad(Characters & result, int num) const;
	void lpad(Characters & result, int num) const;

	//Tests
	bool is_less(Characters firstOperand, Characters secondOperand) const;
	bool is_less(Character first, Character second) const;
	bool is_zero(Characters val) const;
	bool is_zero(Character val) const;
	bool is_negative(Characters & input) const;
	int  dot_pos(Characters & input) const;

	//Basic modifications
	void strip_dot(Characters & input) const;
	void insert_dot(Characters & input, int pos1, int pos2) const;
	void shl(Characters & input, int pow) const; // input *= 10 ^^ pow
	void shr(Characters & input, int pow) const; // input /= 10 ^^ pow
	void negate(Characters & result) const;

	//Basic strings
	const Characters & operators() const;
	const Characters & err() const;
	const Characters & zeros() const;
	const Characters & ones() const;

	Character one() const;
	Character zero() const;
	Character last() const;
	Character dot() const;
};
